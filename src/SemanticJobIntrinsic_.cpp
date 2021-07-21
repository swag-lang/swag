#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "ErrorIds.h"

bool SemanticJob::resolveIntrinsicMakeForeign(SemanticContext* context)
{
    auto node  = context->node;
    auto first = node->childs.front();
    auto expr  = node->childs.back();

    // Check first parameter
    if (first->typeInfo->kind != TypeInfoKind::Lambda)
        return context->report({first, Msg0782});
    first->flags |= AST_NO_BYTECODE;

    // Check expression
    SWAG_CHECK(checkIsConcrete(context, expr));
    if (!expr->typeInfo->isSame(g_TypeMgr.typeInfoConstPVoid, ISSAME_CAST))
        return context->report({expr, Msg0783});

    node->typeInfo    = first->typeInfo;
    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicMakeForeign;
    return true;
}

bool SemanticJob::resolveIntrinsicMakeCallback(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    auto first = node->childs.front();

    // Check first parameter
    if (first->typeInfo->kind != TypeInfoKind::Lambda)
        return context->report({node, Msg0784});

    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(first->typeInfo, TypeInfoKind::Lambda);
    if (typeFunc->parameters.size() > SWAG_LIMIT_CB_MAX_PARAMS)
        return context->report({node, format(Msg0785, SWAG_LIMIT_CB_MAX_PARAMS)});
    if (typeFunc->numReturnRegisters() > 1)
        return context->report({node, format(Msg0786, typeFunc->returnType->getDisplayName().c_str())});

    node->typeInfo    = g_TypeMgr.typeInfoPVoid;
    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicMakeCallback;
    return true;
}

bool SemanticJob::resolveIntrinsicMakeSlice(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, const char* name)
{
    auto first  = node->childs.front();
    auto second = node->childs.back();

    // Must start with a pointer of the same type as the slice
    if (first->typeInfo->kind != TypeInfoKind::Pointer)
        return context->report({first, format(Msg0787, name)});

    auto ptrPointer = CastTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    if (!ptrPointer->pointedType)
        return context->report({first, format(Msg0788, name)});

    // Slice count
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, second->typeInfo, nullptr, second, CASTFLAG_TRY_COERCE));

    // Create slice type
    auto ptrSlice         = allocType<TypeInfoSlice>();
    ptrSlice->pointedType = ptrPointer->pointedType;
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

    // Check first parameter
    if (first->typeInfo->kind != TypeInfoKind::Pointer)
        return context->report({first, Msg0789});

    auto ptrPointer = CastTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    if (!ptrPointer->pointedType)
        return context->report({first, Msg0790});

    // Check second parameter
    if (second->flags & AST_VALUE_IS_TYPEINFO)
    {
        if (!TypeManager::makeCompatibles(context, ptrPointer->pointedType, second->typeInfo, nullptr, second, CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
            return context->report({node, format(Msg0791, ptrPointer->pointedType->getDisplayName().c_str(), second->typeInfo->getDisplayName().c_str())});
    }

    SWAG_CHECK(checkIsConcreteOrType(context, second));
    if (context->result != ContextResult::Done)
        return true;
    if (!(second->typeInfo->isPointerToTypeInfo()))
        return context->report({node, format(Msg0792, second->typeInfo->getDisplayName().c_str())});

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
    auto  module     = sourceFile->module;
    auto& typeTable  = module->typeTable;

    SWAG_CHECK(checkIsConcrete(context, first));

    third->concreteTypeInfoSegment = typeTable.getSegmentStorage(module, CONCRETE_SHOULD_WAIT);
    SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, third->typeInfo, nullptr, &third->concreteTypeInfoStorage, CONCRETE_SHOULD_WAIT));
    if (context->result != ContextResult::Done)
        return true;

    auto firstTypeInfo = TypeManager::concreteReferenceType(first->typeInfo, CONCRETE_ALIAS);
    SWAG_VERIFY(firstTypeInfo->kind == TypeInfoKind::Pointer || firstTypeInfo->kind == TypeInfoKind::Struct, context->report({first, Msg0793}));
    SWAG_VERIFY(second->typeInfo->isPointerToTypeInfo(), context->report({second, Msg0794}));
    auto thirdTypeInfo = TypeManager::concreteReferenceType(third->typeInfo, CONCRETE_ALIAS);
    SWAG_VERIFY(thirdTypeInfo->kind == TypeInfoKind::Interface, context->report({third, Msg0795}));

    node->typeInfo = third->typeInfo;
    third->flags |= AST_NO_BYTECODE;

    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicMakeInterface;
    return true;
}

bool SemanticJob::resolveIntrinsicDataOf(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    typeInfo = TypeManager::concreteReferenceType(typeInfo);
    if (typeInfo->isNative(NativeTypeKind::String))
    {
        auto ptrType         = allocType<TypeInfoPointer>();
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
        auto ptrType         = allocType<TypeInfoPointer>();
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
        auto ptrType         = allocType<TypeInfoPointer>();
        ptrType->pointedType = ptrArray->pointedType;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->computeName();
        if (ptrArray->isConst())
            ptrType->setConst();
        node->typeInfo    = ptrType;
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else if (typeInfo->isNative(NativeTypeKind::Any) || typeInfo->kind == TypeInfoKind::Interface)
    {
        auto ptrType         = allocType<TypeInfoPointer>();
        ptrType->pointedType = g_TypeMgr.typeInfoVoid;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->computeName();
        node->typeInfo    = ptrType;
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else if (typeInfo->kind == TypeInfoKind::Struct)
    {
        if (typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
            return context->report({node, Msg0796});
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
        return context->report({node, format(Msg0797, typeInfo->getDisplayName().c_str())});
    }

    return true;
}

bool SemanticJob::resolveIntrinsicStringOf(SemanticContext* context)
{
    auto node = context->node;
    auto expr = node->childs.front();

    if (!(expr->flags & AST_VALUE_COMPUTED))
    {
        SWAG_CHECK(resolveTypeAsExpression(context, expr, nullptr, CONCRETE_FORCE_NO_SCOPE));
        if (context->result != ContextResult::Done)
            return true;
    }

    auto typeInfo = expr->typeInfo;
    SWAG_VERIFY(typeInfo, context->report({expr, Msg0798}));
    SWAG_VERIFY(expr->flags & AST_VALUE_COMPUTED, context->report({expr, Msg0798}));

    if (expr->flags & AST_VALUE_IS_TYPEINFO)
        node->computedValue.text = typeInfo->name;
    else if (typeInfo->isNative(NativeTypeKind::String))
        node->computedValue.text = expr->computedValue.text;
    else if (typeInfo->kind == TypeInfoKind::Native)
        node->computedValue.text = Ast::literalToString(typeInfo, expr->computedValue);
    else if (typeInfo->kind == TypeInfoKind::Enum)
        node->computedValue.text = Ast::enumToString(typeInfo, expr->computedValue.text, expr->computedValue.reg);
    else
        node->computedValue.text = typeInfo->name;

    node->setFlagsValueIsComputed();
    node->typeInfo = g_TypeMgr.typeInfoString;
    return true;
}

bool SemanticJob::resolveIntrinsicCountOf(SemanticContext* context, AstNode* node, AstNode* expression)
{
    auto typeInfo = expression->typeInfo;
    if (typeInfo->kind == TypeInfoKind::Enum)
    {
        auto typeEnum               = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        node->computedValue.reg.u64 = typeEnum->values.size();
        node->setFlagsValueIsComputed();
        node->typeInfo = g_TypeMgr.typeInfoUInt;
        return true;
    }

    typeInfo = TypeManager::concreteReferenceType(typeInfo);
    if (typeInfo->isNative(NativeTypeKind::String))
    {
        node->typeInfo = g_TypeMgr.typeInfoUInt;
        if (node->flags & AST_VALUE_COMPUTED)
        {
            node->setFlagsValueIsComputed();
            node->computedValue.reg.u64 = node->computedValue.text.length();
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
        node->typeInfo              = g_TypeMgr.typeInfoUInt;
    }
    else if (typeInfo->kind == TypeInfoKind::Slice)
    {
        // :SliceLiteral
        // Slice literal. This can happen for enum values
        if (node->flags & AST_VALUE_COMPUTED)
        {
            node->computedValue.reg.u64 = node->computedValue.reg.u64;
            node->typeInfo              = g_TypeMgr.typeInfoUInt;
        }
        else
        {
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
            node->typeInfo    = g_TypeMgr.typeInfoUInt;
        }
    }
    else if (typeInfo->kind == TypeInfoKind::TypeListTuple || typeInfo->kind == TypeInfoKind::TypeListArray)
    {
        auto typeList = CastTypeInfo<TypeInfoList>(typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);
        node->setFlagsValueIsComputed();
        node->computedValue.reg.u64 = typeList->subTypes.size();
        node->typeInfo              = g_TypeMgr.typeInfoUInt;
    }
    else if (typeInfo->kind == TypeInfoKind::Variadic || typeInfo->kind == TypeInfoKind::TypedVariadic)
    {
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
        node->typeInfo    = g_TypeMgr.typeInfoUInt;
    }
    else if (typeInfo->kind == TypeInfoKind::Struct)
    {
        if (typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE)
            return context->report({expression, Msg0800});
        node->typeInfo = typeInfo;
        SWAG_CHECK(resolveUserOp(context, "opCount", nullptr, nullptr, node, nullptr, false));
        if (context->result == ContextResult::Pending)
            return true;
        node->typeInfo = g_TypeMgr.typeInfoUInt;
        if (!node->byteCodeFct)
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
    }
    else
    {
        SWAG_VERIFY(typeInfo->flags & TYPEINFO_INTEGER, context->report({expression, format(Msg0801, typeInfo->getDisplayName().c_str())}));
        if (node->flags & AST_VALUE_COMPUTED)
        {
            if (!(typeInfo->flags & TYPEINFO_UNSIGNED))
            {
                switch (typeInfo->nativeType)
                {
                case NativeTypeKind::S8:
                    if (node->computedValue.reg.s8 < 0)
                        return context->report({expression, format(Msg0802, node->computedValue.reg.s8)});
                    break;
                case NativeTypeKind::S16:
                    if (node->computedValue.reg.s16 < 0)
                        return context->report({expression, format(Msg0802, node->computedValue.reg.s16)});
                    break;
                case NativeTypeKind::S32:
                    if (node->computedValue.reg.s32 < 0)
                        return context->report({expression, format(Msg0802, node->computedValue.reg.s32)});
                    break;
                case NativeTypeKind::S64:
                case NativeTypeKind::Int:
                    if (node->computedValue.reg.s64 < 0)
                        return context->report({expression, format(Msg0805, node->computedValue.reg.s64)});
                    break;
                }
            }
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, typeInfo, nullptr, node, CASTFLAG_TRY_COERCE));
    }

    return true;
}

bool SemanticJob::resolveIntrinsicSpread(SemanticContext* context)
{
    auto node         = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto expr         = node->childs.front();
    auto typeInfo     = TypeManager::concreteReferenceType(expr->typeInfo);
    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicSpread;

    SWAG_VERIFY(node->parent && node->parent->parent && node->parent->parent->kind == AstNodeKind::FuncCallParam, context->report({node, Msg0806}));

    if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto typeArr   = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        node->typeInfo = typeArr->pointedType;
    }
    else if (typeInfo->kind == TypeInfoKind::Slice)
    {
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        node->typeInfo = typeSlice->pointedType;
    }
    else if (typeInfo->kind == TypeInfoKind::TypeListArray)
    {
        auto typeList  = CastTypeInfo<TypeInfoList>(typeInfo, TypeInfoKind::TypeListArray);
        node->typeInfo = typeList->subTypes[0]->typeInfo;

        // Need to be sure that the expression list can be casted to the equivalent array
        auto typeArr         = allocType<TypeInfoArray>();
        typeArr->count       = (uint32_t) typeList->subTypes.size();
        typeArr->pointedType = typeList->subTypes[0]->typeInfo;
        typeArr->finalType   = typeArr->pointedType;
        typeArr->sizeOf      = typeArr->count * typeArr->finalType->sizeOf;
        typeArr->totalCount  = typeArr->count;
        typeArr->computeName();

        SWAG_CHECK(TypeManager::makeCompatibles(context, typeArr, typeList, nullptr, expr));

        g_Allocator.free(typeArr, sizeof(TypeInfoArray));
    }
    else
    {
        return context->report({expr, format(Msg0807, typeInfo->getDisplayName().c_str())});
    }

    auto typeVar     = allocType<TypeInfoVariadic>();
    typeVar->kind    = TypeInfoKind::TypedVariadic;
    typeVar->rawType = node->typeInfo;
    typeVar->computeName();
    node->typeInfo = typeVar;
    node->typeInfo->flags |= TYPEINFO_SPREAD;

    return true;
}

bool SemanticJob::resolveIntrinsicKindOf(SemanticContext* context)
{
    auto  node       = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto  expr       = node->childs.front();
    auto  sourceFile = context->sourceFile;
    auto& typeTable  = sourceFile->module->typeTable;

    SWAG_VERIFY(expr->typeInfo, context->report({expr, Msg0798}));

    // Will be runtime for an 'any' type, or a typeset
    if (expr->typeInfo->isNative(NativeTypeKind::Any) ||
        expr->typeInfo->kind == TypeInfoKind::Interface)
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

    // For a function, this is the unscoped type
    if (expr->typeInfo->kind == TypeInfoKind::FuncAttr)
    {
        SWAG_CHECK(resolveTypeAsExpression(context, expr, &node->typeInfo, CONCRETE_FORCE_NO_SCOPE));
        if (context->result != ContextResult::Done)
            return true;
        node->inheritComputedValue(expr);
        SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));
        return true;
    }

    // For an enum, this is the raw type
    if (expr->typeInfo->kind == TypeInfoKind::Enum)
    {
        auto typeEnum = CastTypeInfo<TypeInfoEnum>(expr->typeInfo, TypeInfoKind::Enum);
        SWAG_CHECK(resolveTypeAsExpression(context, expr, typeEnum->rawType, &node->typeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->inheritComputedValue(expr);
        SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));
        return true;
    }

    SWAG_CHECK(resolveIntrinsicTypeOf(context));
    return true;
}

bool SemanticJob::makeIntrinsicTypeOf(SemanticContext* context)
{
    auto node     = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto expr     = node->childs.front();
    auto typeInfo = expr->typeInfo;

    // A @typeof/@kindof as a type in a declaration
    if (node->specFlags & AST_SPEC_INTRINSIC_TYPEOF_AS_TYPE)
    {
        // @kindof on a typeinfo will give back the original compiler type
        if (node->token.id == TokenId::IntrinsicKindOf &&
            typeInfo->isPointerToTypeInfo() &&
            expr->computedValue.storageOffset != UINT32_MAX &&
            expr->computedValue.storageSegment != nullptr)
        {
            auto addr        = expr->computedValue.storageSegment->address(expr->computedValue.storageOffset);
            auto newTypeInfo = context->sourceFile->module->typeTable.getRealType((ConcreteTypeInfo*) addr);
            if (newTypeInfo)
                typeInfo = newTypeInfo;
        }

        node->typeInfo = typeInfo;
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

    SWAG_VERIFY(expr->typeInfo, context->report({expr, Msg0798}));
    SWAG_VERIFY(expr->typeInfo->kind != TypeInfoKind::Generic, context->report({expr, Msg0810}));

    // If we have a function, then we transform it to a lambda type, as this makes no sens to
    // get the real function type with @typeof
    // i.e.
    // x := @typeof(func) is equivalent to x := @typeof(&func)
    if (expr->typeInfo->kind == TypeInfoKind::FuncAttr)
    {
        expr->typeInfo         = expr->typeInfo->clone();
        expr->typeInfo->kind   = TypeInfoKind::Lambda;
        expr->typeInfo->sizeOf = sizeof(void*);
    }

    expr->flags |= AST_NO_BYTECODE;
    SWAG_CHECK(makeIntrinsicTypeOf(context));
    return true;
}

bool SemanticJob::resolveIntrinsicProperty(SemanticContext* context)
{
    auto node = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);

    switch (node->token.id)
    {
    case TokenId::IntrinsicSpread:
        SWAG_CHECK(resolveIntrinsicSpread(context));
        return true;

    case TokenId::IntrinsicIsConstExpr:
    {
        auto expr      = node->childs.front();
        node->typeInfo = g_TypeMgr.typeInfoBool;
        expr->flags |= AST_NO_BYTECODE;

        // Special case for a function parameter in a selectif block, should be done at runtime
        if (expr->isSelectIfParam(expr->resolvedSymbolOverload))
        {
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicIsConstExpr;
            break;
        }

        node->computedValue.reg.b = (expr->flags & AST_VALUE_COMPUTED);
        node->setFlagsValueIsComputed();
        break;
    }

    case TokenId::IntrinsicSizeOf:
    {
        auto expr = node->childs.front();
        SWAG_VERIFY(expr->typeInfo, context->report({expr, Msg0798}));
        SWAG_VERIFY(expr->typeInfo->kind != TypeInfoKind::Generic, context->report({expr, Msg0812}));
        node->computedValue.reg.u64 = expr->typeInfo->sizeOf;
        node->setFlagsValueIsComputed();
        if (node->computedValue.reg.u64 > 0xFFFFFFFF)
            node->typeInfo = g_TypeMgr.typeInfoUInt;
        else
            node->typeInfo = g_TypeMgr.typeInfoUntypedInt;
        break;
    }

    case TokenId::IntrinsicAlignOf:
    {
        auto expr = node->childs.front();
        SWAG_VERIFY(expr->typeInfo, context->report({expr, Msg0798}));
        SWAG_VERIFY(expr->typeInfo->kind != TypeInfoKind::Generic, context->report({expr, Msg0814}));
        node->computedValue.reg.u64 = TypeManager::alignOf(expr->typeInfo);
        node->setFlagsValueIsComputed();
        if (node->computedValue.reg.u64 > 0xFFFFFFFF)
            node->typeInfo = g_TypeMgr.typeInfoUInt;
        else
            node->typeInfo = g_TypeMgr.typeInfoUntypedInt;
        break;
    }

    case TokenId::IntrinsicOffsetOf:
    {
        auto expr = node->childs.front();
        SWAG_VERIFY(expr->resolvedSymbolOverload, context->report({expr, Msg0798}));
        node->computedValue.reg.u64 = expr->resolvedSymbolOverload->computedValue.storageOffset;
        node->setFlagsValueIsComputed();
        if (node->computedValue.reg.u64 > 0xFFFFFFFF)
            node->typeInfo = g_TypeMgr.typeInfoUInt;
        else
            node->typeInfo = g_TypeMgr.typeInfoUntypedInt;
        break;
    }

    case TokenId::IntrinsicTypeOf:
        SWAG_CHECK(resolveIntrinsicTypeOf(context));
        return true;

    case TokenId::IntrinsicKindOf:
        SWAG_CHECK(resolveIntrinsicKindOf(context));
        return true;

    case TokenId::IntrinsicStringOf:
        SWAG_CHECK(resolveIntrinsicStringOf(context));
        return true;

    case TokenId::IntrinsicCountOf:
    {
        auto expr = node->childs.front();
        if (expr->typeInfo->kind != TypeInfoKind::Enum)
            SWAG_CHECK(checkIsConcrete(context, expr));
        node->inheritComputedValue(expr);
        SWAG_CHECK(resolveIntrinsicCountOf(context, node, expr));
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
        SWAG_CHECK(resolveIntrinsicMakeSlice(context, node, expr->typeInfo, "@mkslice"));
        break;
    }

    case TokenId::IntrinsicMakeString:
    {
        auto expr = node->childs.front();
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveIntrinsicMakeSlice(context, node, expr->typeInfo, "@mkstring"));
        node->typeInfo = g_TypeMgr.typeInfoString;
        break;
    }

    case TokenId::IntrinsicMakeCallback:
    {
        auto expr = node->childs.front();
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveIntrinsicMakeCallback(context, node, expr->typeInfo));
        break;
    }

    case TokenId::IntrinsicMakeForeign:
    {
        SWAG_CHECK(resolveIntrinsicMakeForeign(context));
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
