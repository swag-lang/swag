#include "pch.h"
#include "SemanticJob.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "Global.h"
#include "AstNode.h"
#include "Scope.h"

bool SemanticJob::resolveNamespace(SemanticContext* context)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    SWAG_CHECK(SemanticJob::checkSymbolGhosting(sourceFile, node->ownerScope->parentScope, node, SymbolKind::Namespace));
    return true;
}

bool SemanticJob::resolveUsing(SemanticContext* context)
{
    return true;
}
