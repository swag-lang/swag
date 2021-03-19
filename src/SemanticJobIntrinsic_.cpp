#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"

bool SemanticJob::resolveIntrinsicMakeForeign(SemanticContext* context)
{
    auto node  = context->node;
    auto first = node->childs.front();
    auto expr  = node->childs.back();

    // Check first parameter
    if (first->typeInfo->kind != TypeInfoKind::Lambda)
        return context->report({first, "'@mkforeign' must have a lambda type as a first parameter"});
    first->flags |= AST_NO_BYTECODE;

    // Check expression
    SWAG_CHECK(checkIsConcrete(context, expr));
    if (!expr->typeInfo->isSame(g_TypeMgr.typeInfoConstPVoid, ISSAME_CAST))
        return context->report({expr, "'@mkforeign' must have a 'const *void' as a second parameter"});

    node->typeInfo    = first->typeInfo;
    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicMakeForeign;
    return true;
}

bool SemanticJob::resolveIntrinsicMakeCallback(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    auto first = node->childs.front();

    // Check first parameter
    if (first->typeInfo->kind != TypeInfoKind::Lambda)
        return context->report({node, "'@mkcallback' must have a lambda value as a first parameter"});

    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(first->typeInfo, TypeInfoKind::Lambda);
    if (typeFunc->parameters.size() > SWAG_LIMIT_CB_MAX_PARAMS)
        return context->report({node, format("callback type not supported, too many parameters (maximum is '%d')", SWAG_LIMIT_CB_MAX_PARAMS)});
    if (typeFunc->numReturnRegisters() > 1)
        return context->report({node, format("callback return type '%s' not supported", typeFunc->returnType->name.c_str())});

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
        return context->report({first, format("'%s' must have a pointer as a first parameter", name)});

    auto ptrPointer = CastTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    if (!ptrPointer->pointedType)
        return context->report({first, format("'%s' cannot have 'null' as first parameter", name)});

    // Slice count
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, second->typeInfo, nullptr, second, CASTFLAG_COERCE_FULL));

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
        return context->report({first, "'@mkany' must have a pointer as a first parameter"});

    auto ptrPointer = CastTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    if (!ptrPointer->pointedType)
        return context->report({first, "'@mkany' cannot have 'null' as first parameter"});

    // Check second parameter
    if (second->flags & AST_VALUE_IS_TYPEINFO)
    {
        if (!TypeManager::makeCompatibles(context, ptrPointer->pointedType, second->typeInfo, nullptr, second, CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
            return context->report({node, format("'pointer to value and type are not related (first parameter is a pointer to type '%s' but second parameter is type '%s')", ptrPointer->pointedType->name.c_str(), second->typeInfo->name.c_str())});
    }

    SWAG_CHECK(checkIsConcreteOrType(context, second));
    if (context->result != ContextResult::Done)
        return true;
    if (!(second->typeInfo->isPointerToTypeInfo()))
        return context->report({node, format("'@mkany' must have a 'typeinfo' or a type value as a second parameter ('%s' provided)", second->typeInfo->name.c_str())});

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
    SWAG_VERIFY(firstTypeInfo->kind == TypeInfoKind::Pointer || firstTypeInfo->kind == TypeInfoKind::Struct, context->report({first, "'@mkinterface' must have a one dimension pointer or a struct as a first parameter"}));
    SWAG_VERIFY(second->typeInfo->isPointerToTypeInfo(), context->report({second, "'@mkinterface' must have a typeinfo as a second parameter"}));
    auto thirdTypeInfo = TypeManager::concreteReferenceType(third->typeInfo, CONCRETE_ALIAS);
    SWAG_VERIFY(thirdTypeInfo->kind == TypeInfoKind::Interface, context->report({third, "'@mkinterface' must have an interface as a third parameter"}));

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
        return context->report({node, format("'@dataof' cannot be applied to expression of type '%s'", typeInfo->name.c_str())});
    }

    return true;
}

bool SemanticJob::resolveIntrinsicCountOf(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
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
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
        node->typeInfo    = g_TypeMgr.typeInfoUInt;
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
            return context->report({node, "'@countof' cannot be used on a tuple type"});
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
        SWAG_VERIFY(typeInfo->flags & TYPEINFO_INTEGER, context->report({node, format("expression should be of type integer, but is '%s'", typeInfo->name.c_str())}));
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
                case NativeTypeKind::S64:
                case NativeTypeKind::Int:
                    if (node->computedValue.reg.s64 < 0)
                        return context->report({node, format("constant value should be unsigned, but is '%I64d'", node->computedValue.reg.s64)});
                    break;
                }
            }
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoUInt, typeInfo, nullptr, node, CASTFLAG_COERCE_FULL));
    }

    return true;
}

bool SemanticJob::resolveIntrinsicSpread(SemanticContext* context)
{
    auto node         = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto expr         = node->childs.front();
    auto typeInfo     = TypeManager::concreteReferenceType(expr->typeInfo);
    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicSpread;

    SWAG_VERIFY(node->parent && node->parent->parent && node->parent->parent->kind == AstNodeKind::FuncCallParam, context->report({node, "'@spread' can only be called as a function parameter"}));

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
        return context->report({expr, format("expression of type '%s' cannot be spreaded", typeInfo->name.c_str())});
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

    SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));

    // Will be runtime for an 'any' type, or a typeset
    if (expr->typeInfo->isNative(NativeTypeKind::Any) ||
        expr->typeInfo->kind == TypeInfoKind::TypeSet ||
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

    SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));
    SWAG_VERIFY(expr->typeInfo->kind != TypeInfoKind::Generic, context->report({expr, "cannot evaluate type in that context because it's generic"}));

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
        SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));
        SWAG_VERIFY(expr->typeInfo->kind != TypeInfoKind::Generic, context->report({expr, "size cannot be computed because expression is generic"}));
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
        SWAG_VERIFY(expr->typeInfo, context->report({expr, "expression cannot be evaluated at compile time"}));
        SWAG_VERIFY(expr->typeInfo->kind != TypeInfoKind::Generic, context->report({expr, "alignement cannot be computed because expression is generic"}));
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
        SWAG_VERIFY(expr->resolvedSymbolOverload, context->report({expr, "expression cannot be evaluated at compile time"}));
        node->computedValue.reg.u64 = expr->resolvedSymbolOverload->storageOffset;
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

    case TokenId::IntrinsicCountOf:
    {
        auto expr = node->childs.front();
        if (expr->typeInfo->kind == TypeInfoKind::Enum)
        {
            auto typeEnum               = CastTypeInfo<TypeInfoEnum>(expr->typeInfo, TypeInfoKind::Enum);
            node->computedValue.reg.u64 = typeEnum->values.size();
            node->setFlagsValueIsComputed();
            node->typeInfo = g_TypeMgr.typeInfoUInt;
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
