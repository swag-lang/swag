#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"
#include "TypeInfo.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "Scope.h"

bool SemanticJob::resolveTypeExpression(SemanticContext* context)
{
    auto node = context->node;

    if (node->token.literalType)
    {
        node->typeInfo  = node->token.literalType;
        context->result = SemanticResult::Done;
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

    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveTypeDecl(SemanticContext* context)
{
    auto node      = context->node;
    node->typeInfo = node->childs[0]->typeInfo;

    // Register symbol with its type
    SWAG_CHECK(node->ownerScope->symTable->addSymbolTypeInfo(context->sourceFile, node, node->typeInfo, SymbolKind::Type));
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(context->sourceFile, node->ownerScope->parentScope, node, SymbolKind::Type));

    context->result = SemanticResult::Done;
    return true;
}
