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

bool SemanticJob::resolveTypeExpression(SemanticContext* context)
{
    auto node = CastAst<AstType>(context->node, AstNodeKind::Type);

    if (node->token.literalType)
    {
        if (node->ptrCount)
        {
            auto ptrType         = g_Pool_typeInfoPointer.alloc();
            ptrType->ptrCount    = node->ptrCount;
            ptrType->pointedType = node->token.literalType;
            ptrType->sizeOf      = sizeof(void*);
            ptrType->name        = "*" + node->token.literalType->name;
            node->typeInfo       = g_TypeMgr.registerType(ptrType);
        }
        else
            node->typeInfo = node->token.literalType;
        return true;
    }

    if (node->childs.empty())
    {
        return context->job->error(context, "invalid type (yet) !");
    }

    auto child     = node->childs[0];
    node->typeInfo = child->typeInfo;

    // If type comes from an identifier, be sure it's a type
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
