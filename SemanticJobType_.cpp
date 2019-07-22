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

bool SemanticJob::resolveTypeTuple(SemanticContext* context)
{
    auto node = context->node;
    SWAG_VERIFY(node->childs.size(), context->job->error(context, "empty tuple type"));

    auto typeInfoList = g_Pool_typeInfoList.alloc();
	typeInfoList->flags |= TYPEINFO_CONST;

	typeInfoList->name = "{";
	for (auto child : node->childs)
	{
		if (!typeInfoList->childs.empty())
			typeInfoList->name += ", ";
		typeInfoList->childs.push_back(child->typeInfo);
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
            auto typeParam      = g_Pool_typeInfoFuncAttrParam.alloc();
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

    node->typeInfo = node->typeExpression ? node->typeExpression->typeInfo : node->token.literalType;
    SWAG_VERIFY(node->typeInfo, context->job->error(context, "invalid type (yet) !"));

    // If type comes from an identifier, be sure it's a type
    if (node->typeExpression)
    {
        auto child = node->childs.back();
        if (child->resolvedSymbolName)
        {
            auto symName = child->resolvedSymbolName;
            auto symOver = child->resolvedSymbolOverload;
            if (symName->kind != SymbolKind::Enum && symName->kind != SymbolKind::Type)
            {
                Diagnostic diag{context->sourceFile, child->token.startLocation, child->token.endLocation, format("symbol '%s' is not a type", child->name.c_str())};
                Diagnostic note{symOver->sourceFile, symOver->node->token, format("this is the definition of '%s'", symName->name.c_str()), DiagnosticLevel::Note};
                return context->sourceFile->report(diag, &note);
            }
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
            ptrArray->pointedType = node->typeInfo;
            ptrArray->name        = format("[] %s", node->typeInfo->name.c_str());
            ptrArray->sizeOf      = 0;
            if (node->isConst)
                ptrArray->setConst();
            node->typeInfo = g_TypeMgr.registerType(ptrArray);
        }
        else
        {
            for (int i = node->arrayDim - 1; i >= 0; i--)
            {
                auto child = node->childs[i];
                SWAG_VERIFY(child->flags & AST_VALUE_COMPUTED, sourceFile->report({sourceFile, child, "can't evaluate array dimension at compile time"}));
                SWAG_VERIFY(child->typeInfo->isNativeInteger(), sourceFile->report({sourceFile, child, format("array dimension is '%s' and should be integer", child->typeInfo->name.c_str())}));
                SWAG_VERIFY(child->typeInfo->sizeOf <= 4, sourceFile->report({sourceFile, child, format("array dimension overflow, cannot be more than a 32 bits integer, and is '%s'", child->typeInfo->name.c_str())}));
                SWAG_VERIFY(child->computedValue.reg.u32 <= g_CommandLine.maxStaticArraySize, sourceFile->report({sourceFile, child, format("array dimension overflow, maximum size is %I64u, and requested size is %I64u", g_CommandLine.maxStaticArraySize, child->computedValue.reg.u32)}));

                auto ptrArray         = g_Pool_typeInfoArray.alloc();
                ptrArray->count       = child->computedValue.reg.u32;
                ptrArray->pointedType = node->typeInfo;
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

    return true;
}

bool SemanticJob::resolveTypeDecl(SemanticContext* context)
{
    auto node      = context->node;
    node->typeInfo = node->childs.front()->typeInfo;

    // Register symbol with its type
    SWAG_CHECK(node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, node, node->typeInfo, SymbolKind::Type));
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context, node->ownerScope, node, SymbolKind::Type));

    return true;
}

bool SemanticJob::resolveCast(SemanticContext* context)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto typeNode   = node->childs[0];
    auto exprNode   = node->childs[1];

    SWAG_CHECK(TypeManager::makeCompatibles(sourceFile, typeNode->typeInfo, exprNode, CASTFLAG_FORCE));
    node->typeInfo    = typeNode->typeInfo;
    node->byteCodeFct = &ByteCodeGenJob::emitCast;

    node->inheritAndFlag(exprNode, AST_CONST_EXPR);
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