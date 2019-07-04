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

bool SemanticJob::resolveTypeExpression(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto node       = CastAst<AstType>(context->node, AstNodeKind::Type);

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
        node->typeInfo       = g_TypeMgr.registerType(ptrType);
    }

    // In fact, this is an array
    if (node->arrayDim)
    {
        for (int i = node->arrayDim - 1; i >= 0; i--)
        {
            auto child = node->childs[i];
            SWAG_VERIFY(child->flags & AST_VALUE_COMPUTED, sourceFile->report({sourceFile, child, "can't evaluate array dimension at compile time"}));
            SWAG_VERIFY(child->typeInfo->isNativeInteger(), sourceFile->report({sourceFile, child, format("array dimension is '%s' and should be integer", child->typeInfo->name.c_str())}));
            SWAG_VERIFY(child->typeInfo->sizeOf <= 4, sourceFile->report({sourceFile, child, format("array dimension overflow, cannot be more than a 32 bits integer, and is '%s'", child->typeInfo->name.c_str())}));
            SWAG_VERIFY(child->computedValue.reg.u32 <= g_CommandLine.maxStaticArraySize, sourceFile->report({sourceFile, child, format("array dimension overflow, maximum size is %I64u, and requested size is %I64u", g_CommandLine.maxStaticArraySize, child->computedValue.reg.u32)}));

            auto ptrArray         = g_Pool_typeInfoArray.alloc();
            ptrArray->size        = child->computedValue.reg.u32;
            ptrArray->pointedType = node->typeInfo;
            ptrArray->name        = format("[%d] %s", node->arrayDim + 1, node->typeInfo->name.c_str());
            ptrArray->sizeOf      = ptrArray->size * ptrArray->pointedType->sizeOf;
            node->typeInfo        = ptrArray;
        }
    }

    return true;
}

bool SemanticJob::resolveTypeDecl(SemanticContext* context)
{
    auto node      = context->node;
    node->typeInfo = node->childs[0]->typeInfo;

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
