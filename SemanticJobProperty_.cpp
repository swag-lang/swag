#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"

bool SemanticJob::resolveSliceOfProperty(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    auto first  = node->childs.front();
    auto second = node->childs.back();

    // Must start with a pointer of the same type as the slice
    if (first->typeInfo->kind != TypeInfoKind::Pointer)
        return context->report({node, "'@sliceof' must have a pointer as a first parameter"});

    auto ptrPointer = CastTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    if (ptrPointer->ptrCount != 1)
        return context->report({node, "'@sliceof' must have a one dimension pointer as a first parameter"});

    // Must end with an U32, which is the slice count
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoU32, second->typeInfo, nullptr, second));

    // Create slice type
    auto ptrSlice         = g_Allocator.alloc<TypeInfoSlice>();
    ptrSlice->pointedType = ptrPointer->finalType;
    if (ptrPointer->isConst())
        ptrSlice->flags |= TYPEINFO_CONST;
    ptrSlice->computeName();
    node->typeInfo = ptrSlice;

    node->byteCodeFct = ByteCodeGenJob::emitSliceOfProperty;
    return true;
}

bool SemanticJob::resolveDataOfProperty(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
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
        node->byteCodeFct = ByteCodeGenJob::emitDataOfProperty;
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
        node->byteCodeFct = ByteCodeGenJob::emitDataOfProperty;
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
        node->byteCodeFct = ByteCodeGenJob::emitDataOfProperty;
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
        node->byteCodeFct = ByteCodeGenJob::emitDataOfProperty;
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
            node->byteCodeFct = ByteCodeGenJob::emitDataOfProperty;
    }
    else
    {
        return context->report({node, format("'@dataof' cannot be applied to expression of type '%s'", node->typeInfo->name.c_str())});
    }

    return true;
}

bool SemanticJob::resolveCountOfProperty(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
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
            node->byteCodeFct = ByteCodeGenJob::emitCountOfProperty;
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
        node->byteCodeFct = ByteCodeGenJob::emitCountOfProperty;
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
        node->byteCodeFct = ByteCodeGenJob::emitCountOfProperty;
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
            node->byteCodeFct = ByteCodeGenJob::emitCountOfProperty;
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

bool SemanticJob::resolveKindOfProperty(SemanticContext* context)
{
    auto  node       = CastAst<AstProperty>(context->node, AstNodeKind::IntrinsicProp);
    auto  expr       = node->childs.front();
    auto  sourceFile = context->sourceFile;
    auto& typeTable  = sourceFile->module->typeTable;

    SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));

    // Will be runtime for an 'any' type
    if (expr->typeInfo->isNative(NativeTypeKind::Any))
    {
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, expr->typeInfo, &node->typeInfo, &node->computedValue.reg.u32));
        typeTable.waitForTypeTableJobs(context->job);
        if (context->result != ContextResult::Done)
            return true;
        node->byteCodeFct = ByteCodeGenJob::emitKindOfProperty;
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

    SWAG_CHECK(resolveTypeOfProperty(context));
    return true;
}

bool SemanticJob::makeTypeOfProperty(SemanticContext* context)
{
    auto node     = CastAst<AstProperty>(context->node, AstNodeKind::IntrinsicProp);
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

bool SemanticJob::resolveTypeOfProperty(SemanticContext* context)
{
    auto node = CastAst<AstProperty>(context->node, AstNodeKind::IntrinsicProp);
    auto expr = node->childs.front();

    SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));
    expr->flags |= AST_NO_BYTECODE;
    SWAG_CHECK(makeTypeOfProperty(context));
    return true;
}

bool SemanticJob::resolveIntrinsicProperty(SemanticContext* context)
{
    auto node = CastAst<AstProperty>(context->node, AstNodeKind::IntrinsicProp);
    auto expr = node->childs.front();

    switch (node->token.id)
    {
    case TokenId::IntrinsicSizeOf:
        SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));
        node->computedValue.reg.u64 = expr->typeInfo->sizeOf;
        node->setFlagsValueIsComputed();
        node->typeInfo = g_TypeMgr.typeInfoU32;
        break;

    case TokenId::IntrinsicTypeOf:
        SWAG_CHECK(resolveTypeOfProperty(context));
        return true;

    case TokenId::IntrinsicKindOf:
        SWAG_CHECK(resolveKindOfProperty(context));
        return true;

    case TokenId::IntrinsicCountOf:
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
        SWAG_CHECK(resolveCountOfProperty(context, node, expr->typeInfo));
        break;

    case TokenId::IntrinsicDataOf:
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveDataOfProperty(context, node, expr->typeInfo));
        break;

    case TokenId::IntrinsicSliceOf:
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveSliceOfProperty(context, node, expr->typeInfo));
        break;
    }

    return true;
}
