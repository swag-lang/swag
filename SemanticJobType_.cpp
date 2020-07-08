#include "pch.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Module.h"
#include "TypeManager.h"
#include "Workspace.h"
#include "Diagnostic.h"

bool SemanticJob::checkIsConcrete(SemanticContext* context, AstNode* node)
{
    if (node->flags & AST_R_VALUE)
        return true;

    if (node->kind == AstNodeKind::TypeExpression)
        return context->report({node, "cannot reference a type expression"});
    if (node->resolvedSymbolName)
        return context->report({node, format("cannot reference %s", SymTable::getArticleKindName(node->resolvedSymbolName->kind))});

    return true;
}

bool SemanticJob::resolveTypeLambda(SemanticContext* context)
{
    auto node          = CastAst<AstTypeLambda>(context->node, AstNodeKind::TypeLambda);
    auto typeInfo      = g_Allocator.alloc<TypeInfoFuncAttr>();
    typeInfo->kind     = TypeInfoKind::Lambda;
    typeInfo->declNode = node;

    if (node->returnType)
    {
        typeInfo->returnType = node->returnType->typeInfo;
        if (typeInfo->returnType->flags & TYPEINFO_GENERIC)
            typeInfo->flags |= TYPEINFO_GENERIC;
    }

    if (node->parameters)
    {
        for (auto param : node->parameters->childs)
        {
            auto typeParam      = g_Allocator.alloc<TypeInfoParam>();
            typeParam->typeInfo = param->typeInfo;
            typeParam->node     = param;
            if (typeParam->typeInfo->flags & TYPEINFO_GENERIC)
                typeInfo->flags |= TYPEINFO_GENERIC;
            typeInfo->parameters.push_back(typeParam);
        }
    }

    typeInfo->computeName();
    typeInfo->sizeOf = sizeof(void*);
    node->typeInfo   = typeInfo;

    return true;
}

void SemanticJob::forceConstNode(SemanticContext* context, AstNode* node)
{
    auto typeInfo = node->typeInfo;
    SWAG_ASSERT(typeInfo->flags & TYPEINFO_RETURN_BY_COPY);

    unique_lock lk(typeInfo->mutex);
    if (!typeInfo->isConst())
    {
        if (!typeInfo->constCopy)
        {
            auto copyType = typeInfo->clone();
            copyType->computeName();
            copyType->setConst();
            typeInfo->constCopy = copyType;
        }

        node->typeInfo = typeInfo->constCopy;
    }
}

void SemanticJob::forceConstType(SemanticContext* context, AstTypeExpression* node)
{
    if (node->typeInfo->flags & TYPEINFO_RETURN_BY_COPY)
    {
        if (node->forceConstType)
            node->isConst = true;

        if (node->isConst)
            forceConstNode(context, node);
    }
}

bool SemanticJob::resolveTypeExpression(SemanticContext* context)
{
    auto typeNode = CastAst<AstTypeExpression>(context->node, AstNodeKind::TypeExpression);

    // Already solved
    if ((typeNode->flags & AST_FROM_GENERIC) && typeNode->typeInfo)
    {
        forceConstType(context, typeNode);
        return true;
    }

    // Code
    if (typeNode->isCode)
    {
        typeNode->typeInfo = g_TypeMgr.typeInfoCode;
        return true;
    }

    if (typeNode->identifier)
    {
        typeNode->typeInfo = typeNode->identifier->typeInfo;
        typeNode->inheritOrFlag(typeNode->identifier, AST_IS_GENERIC);
    }
    else
    {
        typeNode->typeInfo = typeNode->token.literalType;

        // Typed variadic ?
        if (typeNode->typeInfo->kind == TypeInfoKind::Variadic && !typeNode->childs.empty())
        {
            auto typeVariadic     = (TypeInfoVariadic*) typeNode->typeInfo->clone();
            typeVariadic->kind    = TypeInfoKind::TypedVariadic;
            typeVariadic->rawType = typeNode->childs.front()->typeInfo;
            typeNode->typeInfo    = typeVariadic;
        }
    }

    // This is a generic type, not yet known
    if (!typeNode->typeInfo && typeNode->identifier && typeNode->identifier->resolvedSymbolOverload->flags & OVERLOAD_GENERIC)
    {
        typeNode->resolvedSymbolName     = typeNode->identifier->resolvedSymbolName;
        typeNode->resolvedSymbolOverload = typeNode->identifier->resolvedSymbolOverload;
        typeNode->typeInfo               = g_Allocator.alloc<TypeInfoGeneric>();
        typeNode->typeInfo->name         = typeNode->resolvedSymbolName->name;
        typeNode->typeInfo->nakedName    = typeNode->resolvedSymbolName->name;
        typeNode->typeInfo               = typeNode->typeInfo;
    }

    // Otherwise, this is strange, we should have a type
    SWAG_VERIFY(typeNode->typeInfo, internalError(context, "resolveTypeExpression, null type !"));

    // If type comes from an identifier, be sure it's a type
    if (typeNode->identifier)
    {
        auto child = typeNode->childs.back();
        if (child->resolvedSymbolName)
        {
            auto symName = child->resolvedSymbolName;
            auto symOver = child->resolvedSymbolOverload;
            if (symName->kind != SymbolKind::Enum &&
                symName->kind != SymbolKind::TypeAlias &&
                symName->kind != SymbolKind::GenericType &&
                symName->kind != SymbolKind::Struct &&
                symName->kind != SymbolKind::Interface)
            {
                Diagnostic diag{child->sourceFile, child->token.startLocation, child->token.endLocation, format("symbol '%s' is not a type (it's %s)", child->name.c_str(), SymTable::getArticleKindName(symName->kind))};
                Diagnostic note{symOver->node, symOver->node->token, format("this is the definition of '%s'", symName->name.c_str()), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }
        }
    }

    // In fact, this is a pointer
    if (typeNode->ptrCount)
    {
        auto ptrPointer       = g_Allocator.alloc<TypeInfoPointer>();
        ptrPointer->ptrCount  = typeNode->ptrCount;
        ptrPointer->finalType = typeNode->typeInfo;
        ptrPointer->sizeOf    = sizeof(void*);
        if (typeNode->isConst)
            ptrPointer->flags |= TYPEINFO_CONST;
        if (typeNode->isSelf)
            ptrPointer->flags |= TYPEINFO_SELF;
        ptrPointer->flags |= (ptrPointer->finalType->flags & TYPEINFO_GENERIC);
        ptrPointer->computeName();
        ptrPointer->computePointedType();
        typeNode->typeInfo = ptrPointer;
    }

    // In fact, this is a reference
    else if (typeNode->isRef)
    {
        auto ptrRef         = g_Allocator.alloc<TypeInfoReference>();
        ptrRef->pointedType = typeNode->typeInfo;
        ptrRef->sizeOf      = typeNode->typeInfo->sizeOf;
        SWAG_VERIFY(typeNode->isConst, context->report({typeNode, "a reference must be declared as 'const'"}));
        if (typeNode->isConst)
            ptrRef->flags |= TYPEINFO_CONST;
        ptrRef->flags |= (typeNode->typeInfo->flags & TYPEINFO_GENERIC);
        ptrRef->computeName();
        typeNode->typeInfo = ptrRef;
    }

    // A struct function parameter is const
    else
    {
        forceConstType(context, typeNode);
    }

    // In fact, this is an array
    if (typeNode->arrayDim)
    {
        // Array without a specified size
        if (typeNode->arrayDim == UINT32_MAX)
        {
            auto ptrArray         = g_Allocator.alloc<TypeInfoArray>();
            ptrArray->count       = UINT32_MAX;
            ptrArray->totalCount  = UINT32_MAX;
            ptrArray->pointedType = typeNode->typeInfo;
            ptrArray->finalType   = typeNode->typeInfo;
            if (typeNode->isConst)
                ptrArray->flags |= TYPEINFO_CONST;
            ptrArray->flags |= (ptrArray->finalType->flags & TYPEINFO_GENERIC);
            ptrArray->sizeOf = 0;
            ptrArray->computeName();
            typeNode->typeInfo = ptrArray;
        }
        else
        {
            auto rawType    = typeNode->typeInfo;
            auto totalCount = 1;
            for (int i = typeNode->arrayDim - 1; i >= 0; i--)
            {
                auto child = typeNode->childs[i];
                SWAG_VERIFY(child->flags & AST_VALUE_COMPUTED, context->report({child, "array dimension cannot be evaluted at compile time"}));
                SWAG_VERIFY(child->typeInfo->isNativeInteger(), context->report({child, format("array dimension is '%s' and should be integer", child->typeInfo->name.c_str())}));
                SWAG_VERIFY(child->typeInfo->sizeOf <= 4, context->report({child, format("array dimension overflow, cannot be more than a 32 bits integer, and is '%s'", child->typeInfo->name.c_str())}));
                SWAG_VERIFY(child->computedValue.reg.u32 <= g_CommandLine.staticArrayMaxSize, context->report({child, format("array dimension overflow, maximum size is %I64u, and requested size is %I64u", g_CommandLine.staticArrayMaxSize, child->computedValue.reg.u32)}));

                auto ptrArray   = g_Allocator.alloc<TypeInfoArray>();
                ptrArray->count = child->computedValue.reg.u32;
                totalCount *= ptrArray->count;
                ptrArray->totalCount  = totalCount;
                ptrArray->pointedType = typeNode->typeInfo;
                ptrArray->finalType   = rawType;
                ptrArray->sizeOf      = ptrArray->count * ptrArray->pointedType->sizeOf;
                if (typeNode->isConst)
                    ptrArray->flags |= TYPEINFO_CONST;
                ptrArray->flags |= (ptrArray->finalType->flags & TYPEINFO_GENERIC);
                ptrArray->computeName();
                typeNode->typeInfo = ptrArray;
            }
        }
    }
    else if (typeNode->isSlice)
    {
        auto ptrSlice         = g_Allocator.alloc<TypeInfoSlice>();
        ptrSlice->pointedType = typeNode->typeInfo;
        ptrSlice->sizeOf      = 2 * sizeof(void*);
        if (typeNode->isConst)
            ptrSlice->flags |= TYPEINFO_CONST;
        ptrSlice->flags |= (ptrSlice->pointedType->flags & TYPEINFO_GENERIC);
        ptrSlice->computeName();
        typeNode->typeInfo = ptrSlice;
    }

    typeNode->computedValue.reg.pointer = (uint8_t*) typeNode->typeInfo;
    if (!(typeNode->flags & AST_HAS_STRUCT_PARAMETERS))
        typeNode->flags |= AST_VALUE_COMPUTED | AST_NO_BYTECODE;

    return true;
}

bool SemanticJob::resolveTypeAlias(SemanticContext* context)
{
    auto node = context->node;

    auto typeInfo      = g_Allocator.alloc<TypeInfoAlias>();
    typeInfo->declNode = node;
    typeInfo->rawType  = node->childs.front()->typeInfo;
    typeInfo->name     = node->name;
    typeInfo->sizeOf   = typeInfo->rawType->sizeOf;
    typeInfo->flags |= (typeInfo->rawType->flags & TYPEINFO_RETURN_BY_COPY);
    typeInfo->flags |= (typeInfo->rawType->flags & TYPEINFO_GENERIC);
    typeInfo->flags |= (typeInfo->rawType->flags & TYPEINFO_CONST);
    typeInfo->computeName();
    node->typeInfo = typeInfo;

    uint32_t symbolFlags = 0;
    if (node->typeInfo->flags & TYPEINFO_GENERIC)
        symbolFlags |= OVERLOAD_GENERIC;

    SWAG_VERIFY(!(node->typeInfo->flags & TYPEINFO_GENERIC), context->report({node, "type alias cannot be generic"}));
    SWAG_CHECK(node->ownerScope->symTable.addSymbolTypeInfo(context, node, node->typeInfo, SymbolKind::TypeAlias, nullptr, symbolFlags));
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node, SymbolKind::TypeAlias));

    // Check public
    if (node->attributeFlags & ATTRIBUTE_PUBLIC)
    {
        if (node->ownerScope->isGlobal() || node->ownerScope->kind == ScopeKind::Struct)
            node->ownerScope->addPublicTypeAlias(node);
    }

    return true;
}

bool SemanticJob::resolveExplicitCast(SemanticContext* context)
{
    auto node     = context->node;
    auto typeNode = node->childs[0];
    auto exprNode = node->childs[1];

    SWAG_CHECK(checkIsConcrete(context, exprNode));

    SWAG_CHECK(TypeManager::makeCompatibles(context, typeNode->typeInfo, nullptr, exprNode, CASTFLAG_EXPLICIT));

    if (typeNode->typeInfo->kind == TypeInfoKind::Struct || typeNode->typeInfo->kind == TypeInfoKind::Interface)
    {
        auto typeRef         = g_Allocator.alloc<TypeInfoReference>();
        typeRef->flags       = typeNode->typeInfo->flags | TYPEINFO_CONST;
        typeRef->pointedType = typeNode->typeInfo;
        typeRef->computeName();
        typeRef->sizeOf    = typeNode->typeInfo->sizeOf;
        typeNode->typeInfo = typeRef;
    }

    node->typeInfo = typeNode->typeInfo;

    // Revert the implicit cast informations
    // Requested type if store in typeInfo of node, and previous type is stored in typeInfo of exprNode
    // We cannot use castedTypeInfo from node, because an explicit cast result could be casted itself with an implicit cast
    if (exprNode->castedTypeInfo)
    {
        exprNode->typeInfo       = exprNode->castedTypeInfo;
        exprNode->castedTypeInfo = nullptr;
    }

    node->byteCodeFct = ByteCodeGenJob::emitExplicitCast;
    node->inheritOrFlag(exprNode, AST_CONST_EXPR | AST_VALUE_IS_TYPEINFO | AST_VALUE_COMPUTED);
    node->inheritComputedValue(exprNode);
    return true;
}

bool SemanticJob::resolveExplicitAutoCast(SemanticContext* context)
{
    auto node      = context->node;
    auto exprNode  = node->childs[0];
    auto cloneType = exprNode->typeInfo->clone();
    cloneType->flags |= TYPEINFO_AUTO_CAST;
    node->typeInfo = cloneType;

    node->byteCodeFct = ByteCodeGenJob::emitExplicitAutoCast;
    node->inheritOrFlag(exprNode, AST_CONST_EXPR | AST_VALUE_IS_TYPEINFO | AST_VALUE_COMPUTED);
    node->inheritComputedValue(exprNode);
    return true;
}

bool SemanticJob::resolveTypeList(SemanticContext* context)
{
    auto node = context->node;
    if (node->childs.size() == 1)
        node->typeInfo = node->childs.front()->typeInfo;
    return true;
}

bool SemanticJob::resolveIsExpression(SemanticContext* context)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto left       = node->childs[0];
    auto right      = node->childs[1];

    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);
    SWAG_ASSERT(leftTypeInfo);
    SWAG_ASSERT(rightTypeInfo);

    // Keep it generic if it's generic on one side
    if (leftTypeInfo->kind == TypeInfoKind::Generic)
    {
        node->typeInfo = leftTypeInfo;
        return true;
    }
    if (rightTypeInfo->kind == TypeInfoKind::Generic)
    {
        node->typeInfo = rightTypeInfo;
        return true;
    }

    SWAG_CHECK(checkIsConcrete(context, left));
    node->typeInfo = g_TypeMgr.typeInfoBool;
    right->flags |= AST_NO_BYTECODE;

    if (leftTypeInfo->isNative(NativeTypeKind::Any))
    {
        node->byteCodeFct = ByteCodeGenJob::emitIs;
        auto& typeTable   = sourceFile->module->typeTable;
        SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, right->typeInfo, &right->typeInfo, &right->computedValue.reg.u32));
        right->flags |= AST_VALUE_IS_TYPEINFO;
    }
    else if (leftTypeInfo->flags & TYPEINFO_UNTYPED_FLOAT)
    {
        node->computedValue.reg.b = rightTypeInfo == g_TypeMgr.typeInfoF32;
        node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
    }
    else if (leftTypeInfo->flags & TYPEINFO_UNTYPED_INTEGER)
    {
        node->computedValue.reg.b = rightTypeInfo == g_TypeMgr.typeInfoS32;
        node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
    }
    else
    {
        node->computedValue.reg.b = leftTypeInfo->isSame(rightTypeInfo, ISSAME_EXACT);
        node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
    }

    return true;
}

bool SemanticJob::resolveUserCast(SemanticContext* context)
{
    auto node = context->node;
    SWAG_CHECK(context->job->resolveUserOp(context, "opCast", nullptr, node->typeInfo, node->parent, nullptr, false));
    if (context->result == ContextResult::Pending)
        return true;

    node->parent->castedTypeInfo               = node->castedTypeInfo;
    node->parent->typeInfo                     = node->typeInfo;
    node->parent->resolvedUserOpSymbolName     = node->resolvedUserOpSymbolName;
    node->parent->resolvedUserOpSymbolOverload = node->resolvedUserOpSymbolOverload;
    return true;
}