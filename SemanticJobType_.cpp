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
    auto sourceFile = context->sourceFile;
    if (node->flags & AST_R_VALUE)
        return true;

    if (node->kind == AstNodeKind::TypeExpression)
        return context->errorContext.report({sourceFile, node, "cannot reference a type expression"});
    if (node->resolvedSymbolName)
        return context->errorContext.report({sourceFile, node, format("cannot reference %s", SymTable::getArticleKindName(node->resolvedSymbolName->kind))});

    return true;
}

bool SemanticJob::resolveTypeLambda(SemanticContext* context)
{
    auto  sourceFile = context->sourceFile;
    auto& typeTable  = sourceFile->module->typeTable;
    auto  node       = CastAst<AstTypeLambda>(context->node, AstNodeKind::TypeLambda);
    auto  typeInfo   = g_Pool_typeInfoFuncAttr.alloc();
    typeInfo->kind   = TypeInfoKind::Lambda;

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
            auto typeParam      = g_Pool_typeInfoParam.alloc();
            typeParam->typeInfo = param->typeInfo;
            if (typeParam->typeInfo->flags & TYPEINFO_GENERIC)
                typeInfo->flags |= TYPEINFO_GENERIC;
            typeInfo->parameters.push_back(typeParam);
        }
    }

    typeInfo->computeName();
    typeInfo->sizeOf = sizeof(void*);
    node->typeInfo   = typeTable.registerType(typeInfo);

    return true;
}

bool SemanticJob::resolveTypeExpression(SemanticContext* context)
{
    auto  sourceFile = context->sourceFile;
    auto& typeTable  = sourceFile->module->typeTable;
    auto  node       = CastAst<AstTypeExpression>(context->node, AstNodeKind::TypeExpression);

    // Already solved
    if ((node->flags & AST_FROM_GENERIC) && node->typeInfo)
        return true;

    if (node->identifier)
    {
        node->typeInfo = node->identifier->typeInfo;
        node->inheritOrFlag(node->identifier, AST_IS_GENERIC);
    }
    else
    {
        node->typeInfo = node->token.literalType;

        // Typed variadic ?
        if (node->typeInfo->kind == TypeInfoKind::Variadic && !node->childs.empty())
        {
            auto typeVariadic     = (TypeInfoVariadic*) node->typeInfo->clone();
            typeVariadic->kind    = TypeInfoKind::TypedVariadic;
            typeVariadic->rawType = node->childs.front()->typeInfo;
            node->typeInfo        = typeTable.registerType(typeVariadic);
        }
    }

    // This is a generic type, not yet known
    if (!node->typeInfo && node->identifier && node->identifier->resolvedSymbolOverload->flags & OVERLOAD_GENERIC)
    {
        node->resolvedSymbolName     = node->identifier->resolvedSymbolName;
        node->resolvedSymbolOverload = node->identifier->resolvedSymbolOverload;
        node->typeInfo               = g_Pool_typeInfoGeneric.alloc();
        node->typeInfo->name         = node->resolvedSymbolName->name;
        node->typeInfo               = typeTable.registerType(node->typeInfo);
    }

    // Otherwise, this is strange, we should have a type
    SWAG_VERIFY(node->typeInfo, internalError(context, "resolveTypeExpression, null type !"));

    // If type comes from an identifier, be sure it's a type
    if (node->identifier)
    {
        auto child = node->childs.back();
        if (child->resolvedSymbolName)
        {
            auto symName = child->resolvedSymbolName;
            auto symOver = child->resolvedSymbolOverload;
            if (symName->kind != SymbolKind::Enum &&
                symName->kind != SymbolKind::TypeAlias &&
                symName->kind != SymbolKind::GenericType &&
                symName->kind != SymbolKind::Struct)
            {
                Diagnostic diag{context->sourceFile, child->token.startLocation, child->token.endLocation, format("symbol '%s' is not a type (it's %s)", child->name.c_str(), SymTable::getArticleKindName(symName->kind))};
                Diagnostic note{symOver->sourceFile, symOver->node->token, format("this is the definition of '%s'", symName->name.c_str()), DiagnosticLevel::Note};
                return context->errorContext.report(diag, &note);
            }
        }
    }

    // In fact, this is a pointer
    if (node->ptrCount)
    {
        auto ptrPointer         = g_Pool_typeInfoPointer.alloc();
        ptrPointer->ptrCount    = node->ptrCount;
        ptrPointer->pointedType = node->typeInfo;
        ptrPointer->sizeOf      = sizeof(void*);
        if (node->isConst)
            ptrPointer->flags |= TYPEINFO_CONST;
        ptrPointer->flags |= (ptrPointer->pointedType->flags & TYPEINFO_GENERIC);
        ptrPointer->computeName();
        node->typeInfo = typeTable.registerType(ptrPointer);
    }

    // Const struct
    else
    {
        // A struct function parameter is const
        if (node->forFuncParameter && node->typeInfo->kind == TypeInfoKind::Struct)
            node->isConst = true;

        if (node->isConst && node->typeInfo->kind == TypeInfoKind::Struct)
        {
            if (!node->typeInfo->isConst())
            {
                auto copyType = node->typeInfo->clone();
                copyType->setConst();
                node->typeInfo = typeTable.registerType(copyType);
            }
        }
    }

    // In fact, this is an array
    if (node->arrayDim)
    {
        // Array without a specified size
        if (node->arrayDim == UINT32_MAX)
        {
            auto ptrArray         = g_Pool_typeInfoArray.alloc();
            ptrArray->count       = UINT32_MAX;
            ptrArray->totalCount  = UINT32_MAX;
            ptrArray->pointedType = node->typeInfo;
            ptrArray->rawType     = node->typeInfo;
            if (node->isConst)
                ptrArray->flags |= TYPEINFO_CONST;
            ptrArray->flags |= (ptrArray->rawType->flags & TYPEINFO_GENERIC);
            ptrArray->sizeOf = 0;
            ptrArray->computeName();
            node->typeInfo = typeTable.registerType(ptrArray);
        }
        else
        {
            auto rawType    = node->typeInfo;
            auto totalCount = 1;
            for (int i = node->arrayDim - 1; i >= 0; i--)
            {
                auto child = node->childs[i];
                SWAG_VERIFY(child->flags & AST_VALUE_COMPUTED, context->errorContext.report({sourceFile, child, "array dimension cannot be evaluted at compile time"}));
                SWAG_VERIFY(child->typeInfo->isNativeInteger(), context->errorContext.report({sourceFile, child, format("array dimension is '%s' and should be integer", child->typeInfo->name.c_str())}));
                SWAG_VERIFY(child->typeInfo->sizeOf <= 4, context->errorContext.report({sourceFile, child, format("array dimension overflow, cannot be more than a 32 bits integer, and is '%s'", child->typeInfo->name.c_str())}));
                SWAG_VERIFY(child->computedValue.reg.u32 <= g_CommandLine.staticArrayMaxSize, context->errorContext.report({sourceFile, child, format("array dimension overflow, maximum size is %I64u, and requested size is %I64u", g_CommandLine.staticArrayMaxSize, child->computedValue.reg.u32)}));

                auto ptrArray   = g_Pool_typeInfoArray.alloc();
                ptrArray->count = child->computedValue.reg.u32;
                totalCount *= ptrArray->count;
                ptrArray->totalCount  = totalCount;
                ptrArray->pointedType = node->typeInfo;
                ptrArray->rawType     = rawType;
                ptrArray->sizeOf      = ptrArray->count * ptrArray->pointedType->sizeOf;
                if (node->isConst)
                    ptrArray->flags |= TYPEINFO_CONST;
                ptrArray->flags |= (ptrArray->rawType->flags & TYPEINFO_GENERIC);
                ptrArray->computeName();
                node->typeInfo = typeTable.registerType(ptrArray);
            }
        }
    }
    else if (node->isSlice)
    {
        auto ptrSlice         = g_Pool_typeInfoSlice.alloc();
        ptrSlice->pointedType = node->typeInfo;
        ptrSlice->sizeOf      = 2 * sizeof(void*);
        if (node->isConst)
            ptrSlice->flags |= TYPEINFO_CONST;
        ptrSlice->flags |= (ptrSlice->pointedType->flags & TYPEINFO_GENERIC);
        ptrSlice->computeName();
        node->typeInfo = typeTable.registerType(ptrSlice);
    }

    node->computedValue.reg.pointer = (uint8_t*) node->typeInfo;
    if (!(node->flags & AST_HAS_STRUCT_PARAMETERS))
        node->flags |= AST_VALUE_COMPUTED | AST_NO_BYTECODE;

    return true;
}

bool SemanticJob::resolveTypeAlias(SemanticContext* context)
{
    auto  sourceFile = context->sourceFile;
    auto& typeTable  = sourceFile->module->typeTable;
    auto  node       = context->node;

    auto typeInfo     = g_Pool_typeInfoAlias.alloc();
    typeInfo->rawType = node->childs.front()->typeInfo;
    typeInfo->name    = node->name;
    typeInfo->sizeOf  = typeInfo->rawType->sizeOf;
    typeInfo->flags |= (typeInfo->rawType->flags & TYPEINFO_RETURN_BY_COPY);
    typeInfo->flags |= (typeInfo->rawType->flags & TYPEINFO_GENERIC);
    typeInfo->flags |= (typeInfo->rawType->flags & TYPEINFO_CONST);
    typeInfo->computeName();
    node->typeInfo = typeTable.registerType(typeInfo);

    uint32_t symbolFlags = 0;
    if (node->typeInfo->flags & TYPEINFO_GENERIC)
        symbolFlags |= OVERLOAD_GENERIC;

    SWAG_VERIFY(!(node->typeInfo->flags & TYPEINFO_GENERIC), context->errorContext.report({sourceFile, node, "type alias cannot be generic"}));
    SWAG_CHECK(node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, node, node->typeInfo, SymbolKind::TypeAlias, nullptr, symbolFlags));
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node->ownerScope, node, SymbolKind::TypeAlias));

    return true;
}

bool SemanticJob::resolveExplicitCast(SemanticContext* context)
{
    auto node     = context->node;
    auto typeNode = node->childs[0];
    auto exprNode = node->childs[1];

    SWAG_CHECK(TypeManager::makeCompatibles(context, typeNode->typeInfo, nullptr, exprNode, CASTFLAG_EXPLICIT));
    node->typeInfo = typeNode->typeInfo;

    node->byteCodeFct = &ByteCodeGenJob::emitExplicitCast;
    node->inheritOrFlag(exprNode, AST_CONST_EXPR | AST_VALUE_IS_TYPEINFO | AST_VALUE_COMPUTED);
    node->inheritComputedValue(exprNode);
    return true;
}

bool SemanticJob::resolveExplicitAutoCast(SemanticContext* context)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;

    auto exprNode  = node->childs[0];
    auto cloneType = TypeManager::concreteType(exprNode->typeInfo)->clone();
    cloneType->flags |= TYPEINFO_AUTO_CAST;
    node->typeInfo = sourceFile->module->typeTable.registerType(cloneType);

    node->byteCodeFct = &ByteCodeGenJob::emitExplicitAutoCast;
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
    SWAG_ASSERT(leftTypeInfo && rightTypeInfo);

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
        node->byteCodeFct = &ByteCodeGenJob::emitIs;
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