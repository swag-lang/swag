#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "Scope.h"

bool SemanticJob::resolveAttrDecl(SemanticContext* context)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;

    SWAG_CHECK(node->scope->symTable->addSymbolTypeInfo(sourceFile, node->token, node->name, node->typeInfo, SymbolKind::Attribute));
    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveAttrUse(SemanticContext* context)
{
    context->result = SemanticResult::Done;
    return true;
}
