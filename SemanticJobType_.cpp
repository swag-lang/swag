#include "pch.h"
#include "SemanticJob.h"
#include "Global.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "Scope.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "Ast.h"
#include "CommandLine.h"
#include "TypeManager.h"

bool SemanticJob::checkIsConcrete(SemanticContext* context, AstNode* node)
{
    auto sourceFile = context->sourceFile;
    if (node->kind == AstNodeKind::TypeExpression)
        return context->errorContext.report({sourceFile, node, "cannot reference a type expression"});
    if (node->resolvedSymbolName)
    {
        switch (node->resolvedSymbolName->kind)
        {
        case SymbolKind::Namespace:
        case SymbolKind::Enum:
        case SymbolKind::TypeAlias:
            return context->errorContext.report({sourceFile, node, format("cannot reference %s", SymTable::getArticleKindName(node->resolvedSymbolName->kind))});
        }
    }

    return true;
}

bool SemanticJob::resolveTypeTuple(SemanticContext* context)
{
    auto node = context->node;
    SWAG_VERIFY(node->childs.size(), context->job->error(context, "empty tuple type"));

    auto typeInfoList = g_Pool_typeInfoList.alloc();
    typeInfoList->flags |= TYPEINFO_CONST;
    typeInfoList->scope    = node->childs.front()->ownerScope;
    typeInfoList->listKind = TypeInfoListKind::Tuple;

    typeInfoList->name = "{";
    for (auto child : node->childs)
    {
        if (!typeInfoList->childs.empty())
            typeInfoList->name += ", ";
        typeInfoList->childs.push_back(child->typeInfo);

        if (!child->name.empty())
        {
            typeInfoList->name += child->name;
            typeInfoList->name += ": ";
            typeInfoList->names.push_back(child->name);
        }

        typeInfoList->name += child->typeInfo->name;
        typeInfoList->sizeOf += child->typeInfo->sizeOf;
    }

    typeInfoList->name += "}";
    node->typeInfo = g_TypeMgr.registerType(typeInfoList);
    return true;
}

bool SemanticJob::resolveTypeLambda(SemanticContext* context)
{
    auto node      = CastAst<AstTypeLambda>(context->node, AstNodeKind::TypeLambda);
    auto typeInfo  = g_Pool_typeInfoFuncAttr.alloc();
    typeInfo->kind = TypeInfoKind::Lambda;
    if (node->returnType)
        typeInfo->returnType = node->returnType->typeInfo;

    if (node->parameters)
    {
        for (auto param : node->parameters->childs)
        {
            auto typeParam      = g_Pool_typeInfoParam.alloc();
            typeParam->typeInfo = param->typeInfo;
            typeInfo->parameters.push_back(typeParam);
        }
    }

    typeInfo->computeName();
    typeInfo->sizeOf = sizeof(void*);
    node->typeInfo   = g_TypeMgr.registerType(typeInfo);

    return true;
}

bool SemanticJob::resolveTypeExpression(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto node       = CastAst<AstTypeExpression>(context->node, AstNodeKind::TypeExpression);

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
    }

    // This is a generic type, not yet known
    if (!node->typeInfo && node->identifier && node->identifier->resolvedSymbolOverload->flags & OVERLOAD_GENERIC)
    {
        node->resolvedSymbolName     = node->identifier->resolvedSymbolName;
        node->resolvedSymbolOverload = node->identifier->resolvedSymbolOverload;
        node->typeInfo               = g_Pool_typeInfoGeneric.alloc();
        node->typeInfo->name         = node->resolvedSymbolName->name;
        node->typeInfo               = g_TypeMgr.registerType(node->typeInfo);
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

            /*if ((node->typeInfo->kind == TypeInfoKind::Struct) && (node->typeInfo->flags & TYPEINFO_GENERIC))
            {
                if (node->identifier->flags & AST_GENERIC_MATCH_WAS_PARTIAL)
                {
                    Diagnostic diag{context->sourceFile, child->token.startLocation, child->token.endLocation, "missing generic parameters"};
                    Diagnostic note{symOver->sourceFile, symOver->node->token, format("this is the definition of '%s'", symName->name.c_str()), DiagnosticLevel::Note};
                    return context->errorContext.report(diag, &note);
                }
            }*/
        }
    }

    // In fact, this is a pointer
    if (node->ptrCount)
    {
        auto ptrType         = g_Pool_typeInfoPointer.alloc();
        ptrType->ptrCount    = node->ptrCount;
        ptrType->pointedType = node->typeInfo;
        ptrType->sizeOf      = sizeof(void*);
        ptrType->name        = "*" + node->typeInfo->name;
        if (node->isConst)
            ptrType->setConst();
        node->typeInfo = g_TypeMgr.registerType(ptrType);
    }

    // In fact, this is an array
    if (node->arrayDim)
    {
        // If no childs, then this is an array without a specified size
        if (node->childs.empty())
        {
            SWAG_ASSERT(node->arrayDim == UINT32_MAX);
            auto ptrArray         = g_Pool_typeInfoArray.alloc();
            ptrArray->count       = UINT32_MAX;
            ptrArray->totalCount  = UINT32_MAX;
            ptrArray->pointedType = node->typeInfo;
            ptrArray->rawType     = node->typeInfo;
            ptrArray->name        = format("[] %s", node->typeInfo->name.c_str());
            ptrArray->sizeOf      = 0;
            if (node->isConst)
                ptrArray->setConst();
            node->typeInfo = g_TypeMgr.registerType(ptrArray);
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
                SWAG_VERIFY(child->computedValue.reg.u32 <= g_CommandLine.maxStaticArraySize, context->errorContext.report({sourceFile, child, format("array dimension overflow, maximum size is %I64u, and requested size is %I64u", g_CommandLine.maxStaticArraySize, child->computedValue.reg.u32)}));

                auto ptrArray   = g_Pool_typeInfoArray.alloc();
                ptrArray->count = child->computedValue.reg.u32;
                totalCount *= ptrArray->count;
                ptrArray->totalCount  = totalCount;
                ptrArray->pointedType = node->typeInfo;
                ptrArray->rawType     = rawType;
                ptrArray->name        = format("[%d] %s", child->computedValue.reg.u32, node->typeInfo->name.c_str());
                ptrArray->sizeOf      = ptrArray->count * ptrArray->pointedType->sizeOf;
                if (node->isConst)
                    ptrArray->setConst();
                node->typeInfo = g_TypeMgr.registerType(ptrArray);
            }
        }
    }
    else if (node->isSlice)
    {
        auto ptrSlice         = g_Pool_typeInfoSlice.alloc();
        ptrSlice->pointedType = node->typeInfo;
        ptrSlice->name        = format("[..] %s", node->typeInfo->name.c_str());
        ptrSlice->sizeOf      = 2 * sizeof(void*);
        if (node->isConst)
            ptrSlice->setConst();
        node->typeInfo = g_TypeMgr.registerType(ptrSlice);
    }

    node->computedValue.reg.pointer = (uint8_t*) node->typeInfo;
    if (!(node->flags & AST_HAS_STRUCT_PARAMETERS))
        node->flags |= AST_VALUE_COMPUTED | AST_NO_BYTECODE;

    return true;
}

bool SemanticJob::resolveTypeAlias(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto node       = context->node;

    auto typeInfo     = g_Pool_typeInfoAlias.alloc();
    typeInfo->rawType = node->childs.front()->typeInfo;
    typeInfo->name    = node->name;
    typeInfo->sizeOf  = typeInfo->rawType->sizeOf;
    typeInfo->flags |= (typeInfo->rawType->flags & TYPEINFO_RETURN_BY_COPY);
    typeInfo->flags |= (typeInfo->rawType->flags & TYPEINFO_GENERIC);
    typeInfo->flags |= (typeInfo->rawType->flags & TYPEINFO_CONST);
    node->typeInfo = g_TypeMgr.registerType(typeInfo);

    uint32_t symbolFlags = 0;
    if (node->typeInfo->flags & TYPEINFO_GENERIC)
        symbolFlags |= OVERLOAD_GENERIC;

    SWAG_VERIFY(!(node->typeInfo->flags & TYPEINFO_GENERIC), context->errorContext.report({sourceFile, node, "type alias cannot be generic"}));
    SWAG_CHECK(node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, node, node->typeInfo, SymbolKind::TypeAlias, nullptr, symbolFlags));
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node->ownerScope, node, SymbolKind::TypeAlias));

    return true;
}

bool SemanticJob::resolveCast(SemanticContext* context)
{
    auto node     = context->node;
    auto typeNode = node->childs[0];
    auto exprNode = node->childs[1];

    SWAG_CHECK(TypeManager::makeCompatibles(&context->errorContext, typeNode->typeInfo, exprNode, CASTFLAG_FORCE));
    node->typeInfo    = typeNode->typeInfo;
    node->byteCodeFct = &ByteCodeGenJob::emitCast;

    node->inheritOrFlag(exprNode, AST_CONST_EXPR);
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