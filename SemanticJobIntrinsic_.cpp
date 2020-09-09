#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"

bool SemanticJob::resolveIntrinsicMakeSlice(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    auto first  = node->childs.front();
    auto second = node->childs.back();

    // Must start with a pointer of the same type as the slice
    if (first->typeInfo->kind != TypeInfoKind::Pointer)
        return context->report({node, "'@mkslice' must have a pointer as a first parameter"});

    auto ptrPointer = CastTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    if (ptrPointer->ptrCount != 1)
        return context->report({node, "'@mkslice' must have a one dimension pointer as a first parameter"});

    // Must end with an U32, which is the slice count
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoU32, second->typeInfo, nullptr, second));

    // Create slice type
    auto ptrSlice         = g_Allocator.alloc<TypeInfoSlice>();
    ptrSlice->pointedType = ptrPointer->finalType;
    if (ptrPointer->isConst())
        ptrSlice->flags |= TYPEINFO_CONST;
    ptrSlice->computeName();
    node->typeInfo = ptrSlice;

    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicMakeSlice;
    return true;
}

bool SemanticJob::resolveIntrinsicMakeAny(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    auto first  = node->childs.front();
    auto second = node->childs.back();

    if (first->typeInfo->kind != TypeInfoKind::Pointer)
        return context->report({node, "'@mkany' must have a pointer as a first parameter"});
    auto ptrPointer = CastTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    if (ptrPointer->ptrCount != 1)
        return context->report({node, "'@mkany' must have a one dimension pointer as a first parameter"});

    if (second->flags & AST_VALUE_IS_TYPEINFO)
    {
        if (!TypeManager::makeCompatibles(context, ptrPointer->pointedType, second->typeInfo, nullptr, second, CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
            return context->report({node, format("'pointer to value and type are not related (first parameter is a pointer to type '%s' but second parameter is type '%s')", ptrPointer->pointedType->name.c_str(), second->typeInfo->name.c_str())});
    }

    SWAG_CHECK(checkIsConcreteOrType(context, second));
    if (!(second->typeInfo->isPointerToTypeInfo()))
        return context->report({node, "'@mkany' must have a 'const *swag.TypeInfo' or a type value as a second parameter"});

    node->typeInfo    = g_TypeMgr.typeInfoAny;
    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicMakeAny;
    return true;
}

bool SemanticJob::resolveIntrinsicMakeInterface(SemanticContext* context)
{
    auto node   = context->node;
    auto params = node->childs.front();

    auto  first      = params->childs[0];
    auto  second     = params->childs[1];
    auto  third      = params->childs[2];
    auto  sourceFile = context->sourceFile;
    auto& typeTable  = sourceFile->module->typeTable;

    SWAG_CHECK(checkIsConcrete(context, first));

    SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, third->typeInfo, nullptr, &third->computedValue.reg.u32, CONCRETE_SHOULD_WAIT));
    if (context->result != ContextResult::Done)
        return true;

    auto firstTypeInfo = TypeManager::concreteReferenceType(first->typeInfo, CONCRETE_ALIAS);
    SWAG_VERIFY(firstTypeInfo->isPointer1() || firstTypeInfo->kind == TypeInfoKind::Struct, context->report({node, "'@mkinterface' must have a one dimension pointer or a struct as a first parameter"}));
    SWAG_VERIFY(second->typeInfo->isPointerToTypeInfo(), context->report({node, "'@mkinterface' must have a typeinfo as a second parameter"}));
    auto thirdTypeInfo = TypeManager::concreteReferenceType(third->typeInfo, CONCRETE_ALIAS);
    SWAG_VERIFY(thirdTypeInfo->kind == TypeInfoKind::Interface, context->report({node, "'@mkinterface' must have an interface as a third parameter"}));

    node->typeInfo = third->typeInfo;
    third->flags |= AST_NO_BYTECODE;

    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicMakeInterface;
    return true;
}

bool SemanticJob::resolveIntrinsicDataOf(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    if (typeInfo->isNative(NativeTypeKind::String))
    {
        auto ptrType         = g_Allocator.alloc<TypeInfoPointer>();
        ptrType->ptrCount    = 1;
        ptrType->finalType   = g_TypeMgr.typeInfoU8;
        ptrType->pointedType = g_TypeMgr.typeInfoU8;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->computeName();
        ptrType->setConst();
        node->typeInfo    = ptrType;
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else if (typeInfo->kind == TypeInfoKind::Slice)
    {
        auto ptrSlice        = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto ptrType         = g_Allocator.alloc<TypeInfoPointer>();
        ptrType->ptrCount    = 1;
        ptrType->finalType   = ptrSlice->pointedType;
        ptrType->pointedType = ptrSlice->pointedType;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->computeName();
        if (ptrSlice->isConst())
            ptrType->setConst();
        node->typeInfo    = ptrType;
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto ptrArray        = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto ptrType         = g_Allocator.alloc<TypeInfoPointer>();
        ptrType->ptrCount    = 1;
        ptrType->finalType   = ptrArray->pointedType;
        ptrType->pointedType = ptrArray->pointedType;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->computeName();
        if (ptrArray->isConst())
            ptrType->setConst();
        node->typeInfo    = ptrType;
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else if (typeInfo->isNative(NativeTypeKind::Any))
    {
        auto ptrType         = g_Allocator.alloc<TypeInfoPointer>();
        ptrType->ptrCount    = 1;
        ptrType->finalType   = g_TypeMgr.typeInfoVoid;
        ptrType->pointedType = g_TypeMgr.typeInfoVoid;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->computeName();
        node->typeInfo    = ptrType;
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else if (typeInfo->kind == TypeInfoKind::Struct)
    {
        if (typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
            return context->report({node, "'@dataof' cannot be used on a tuple type"});
        node->typeInfo = typeInfo;
        SWAG_CHECK(resolveUserOp(context, "opData", nullptr, nullptr, node, nullptr, false));
        if (context->result == ContextResult::Pending)
            return true;
        node->typeInfo = g_TypeMgr.typeInfoPVoid;
        if (!node->byteCodeFct)
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else
    {
        return context->report({node, format("'@dataof' cannot be applied to expression of type '%s'", node->typeInfo->name.c_str())});
    }

    return true;
}

bool SemanticJob::resolveIntrinsicCountOf(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    if (typeInfo->isNative(NativeTypeKind::String))
    {
        if (node->flags & AST_VALUE_COMPUTED)
        {
            node->setFlagsValueIsComputed();
            node->computedValue.reg.u64 = node->computedValue.text.length();
            node->typeInfo              = g_TypeMgr.typeInfoU32;
        }
        else
        {
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        node->setFlagsValueIsComputed();
        auto typeArray              = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        node->computedValue.reg.u64 = typeArray->count;
        node->typeInfo              = g_TypeMgr.typeInfoU32;
    }
    else if (typeInfo->kind == TypeInfoKind::Slice)
    {
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
    }
    else if (typeInfo->kind == TypeInfoKind::TypeListTuple || typeInfo->kind == TypeInfoKind::TypeListArray)
    {
        auto typeList = CastTypeInfo<TypeInfoList>(typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);
        node->setFlagsValueIsComputed();
        node->computedValue.reg.u64 = (uint32_t) typeList->subTypes.size();
        node->typeInfo              = g_TypeMgr.typeInfoU32;
    }
    else if (typeInfo->kind == TypeInfoKind::Variadic || typeInfo->kind == TypeInfoKind::TypedVariadic)
    {
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
    }
    else if (typeInfo->kind == TypeInfoKind::Struct)
    {
        if (typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
            return context->report({node, "'@countof' cannot be used on a tuple type"});
        node->typeInfo = typeInfo;
        SWAG_CHECK(resolveUserOp(context, "opCount", nullptr, nullptr, node, nullptr, false));
        if (context->result == ContextResult::Pending)
            return true;
        node->typeInfo = g_TypeMgr.typeInfoU32;
        if (!node->byteCodeFct)
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
    }
    else
    {
        typeInfo = TypeManager::concreteType(typeInfo);
        SWAG_VERIFY(typeInfo->flags & TYPEINFO_INTEGER, context->report({node, format("expression should be of type integer, but is '%s'", typeInfo->name.c_str())}));
        SWAG_VERIFY(typeInfo->sizeOf <= 4, context->report({node, format("expression should be a 32 bit integer, but is '%s'", typeInfo->name.c_str())}));
        if (node->flags & AST_VALUE_COMPUTED)
        {
            if (!(typeInfo->flags & TYPEINFO_UNSIGNED))
            {
                switch (typeInfo->nativeType)
                {
                case NativeTypeKind::S8:
                    if (node->computedValue.reg.s8 < 0)
                        return context->report({node, format("constant value should be unsigned, but is '%d'", node->computedValue.reg.s8)});
                    break;
                case NativeTypeKind::S16:
                    if (node->computedValue.reg.s16 < 0)
                        return context->report({node, format("constant value should be unsigned, but is '%d'", node->computedValue.reg.s16)});
                    break;
                case NativeTypeKind::S32:
                    if (node->computedValue.reg.s32 < 0)
                        return context->report({node, format("constant value should be unsigned, but is '%d'", node->computedValue.reg.s32)});
                    break;
                }
            }
        }
        else
        {
            SWAG_VERIFY(typeInfo->flags & TYPEINFO_UNSIGNED, context->report({node, format("expression should be of type unsigned integer, but is '%s'", typeInfo->name.c_str())}));
        }
    }

    node->typeInfo = g_TypeMgr.typeInfoU32;
    return true;
}

bool SemanticJob::resolveIntrinsicKindOf(SemanticContext* context)
{
    auto  node       = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto  expr       = node->childs.front();
    auto  sourceFile = context->sourceFile;
    auto& typeTable  = sourceFile->module->typeTable;

    SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));

    // Will be runtime for an 'any' type
    if (expr->typeInfo->isNative(NativeTypeKind::Any))
    {
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, expr->typeInfo, &node->typeInfo, &node->computedValue.reg.u32, CONCRETE_SHOULD_WAIT));
        if (context->result != ContextResult::Done)
            return true;
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicKindOf;
        node->flags |= AST_R_VALUE;
        SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));
        return true;
    }

    // For a function, this is the return type
    if (expr->typeInfo->kind == TypeInfoKind::FuncAttr)
    {
        auto typeFunc  = CastTypeInfo<TypeInfoFuncAttr>(expr->typeInfo, TypeInfoKind::FuncAttr);
        expr->typeInfo = typeFunc->returnType;
    }

    SWAG_CHECK(resolveIntrinsicTypeOf(context));
    return true;
}

bool SemanticJob::makeIntrinsicTypeOf(SemanticContext* context)
{
    auto node     = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto expr     = node->childs.front();
    auto typeInfo = expr->typeInfo;

    // A @typeof as a type in a declaration
    if (node->typeOfAsType)
    {
        if (node->typeOfAsConst && !typeInfo->isConst())
        {
            node->typeInfo = typeInfo->clone();
            node->typeInfo->setConst();
        }
        else
        {
            node->typeInfo = typeInfo;
        }
    }

    // A @typeof to get a typeinfo
    else
    {
        SWAG_CHECK(resolveTypeAsExpression(context, expr, &node->typeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->inheritComputedValue(expr);
        SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));
    }

    return true;
}

bool SemanticJob::resolveIntrinsicTypeOf(SemanticContext* context)
{
    auto node = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto expr = node->childs.front();

    SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));
    expr->flags |= AST_NO_BYTECODE;
    SWAG_CHECK(makeIntrinsicTypeOf(context));
    return true;
}

bool SemanticJob::resolveIntrinsicProperty(SemanticContext* context)
{
    auto node = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);

    switch (node->token.id)
    {
    case TokenId::IntrinsicSizeOf:
    {
        auto expr = node->childs.front();
        SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));
        node->computedValue.reg.u64 = expr->typeInfo->sizeOf;
        node->setFlagsValueIsComputed();
        node->typeInfo = g_TypeMgr.typeInfoU32;
        break;
    }

    case TokenId::IntrinsicTypeOf:
        SWAG_CHECK(resolveIntrinsicTypeOf(context));
        return true;

    case TokenId::IntrinsicKindOf:
        SWAG_CHECK(resolveIntrinsicKindOf(context));
        return true;

    case TokenId::IntrinsicCountOf:
    {
        auto expr = node->childs.front();
        if (expr->typeInfo->kind == TypeInfoKind::Enum)
        {
            auto typeEnum               = CastTypeInfo<TypeInfoEnum>(expr->typeInfo, TypeInfoKind::Enum);
            node->computedValue.reg.u32 = (uint32_t) typeEnum->values.size();
            node->setFlagsValueIsComputed();
            node->typeInfo = g_TypeMgr.typeInfoU32;
            break;
        }

        SWAG_CHECK(checkIsConcrete(context, expr));
        node->inheritComputedValue(expr);
        SWAG_CHECK(resolveIntrinsicCountOf(context, node, expr->typeInfo));
        break;
    }

    case TokenId::IntrinsicDataOf:
    {
        auto expr = node->childs.front();
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveIntrinsicDataOf(context, node, expr->typeInfo));
        break;
    }

    case TokenId::IntrinsicMakeAny:
    {
        auto expr = node->childs.front();
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveIntrinsicMakeAny(context, node, expr->typeInfo));
        break;
    }

    case TokenId::IntrinsicMakeSlice:
    {
        auto expr = node->childs.front();
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveIntrinsicMakeSlice(context, node, expr->typeInfo));
        break;
    }

    case TokenId::IntrinsicMakeInterface:
    {
        SWAG_CHECK(resolveIntrinsicMakeInterface(context));
        break;
    }
    }

    return true;
}
