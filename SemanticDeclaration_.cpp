#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "AstNode.h"
#include "Utf8.h"
#include "Global.h"
#include "TypeInfo.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "Scope.h"
#include "TypeManager.h"

bool SemanticJob::resolveVarDecl(SemanticContext* context)
{
    auto node = static_cast<AstVarDecl*>(context->node);
    if (node->astType && node->astAssignment)
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, node->astType->typeInfo, node->astAssignment));
        node->typeInfo = node->astType->typeInfo;
    }
    else if (node->astAssignment)
    {
        node->typeInfo = node->astAssignment->typeInfo;
    }
    else
    {
        node->typeInfo = node->astType->typeInfo;
    }

    // Register symbol with its type
    SWAG_CHECK(node->scope->symTable->addSymbol(context->sourceFile, node->token, node->name, node->typeInfo, SymbolType::Variable));

    // We need to check the scope hierarchy for symbol ghosting
    auto scope = node->scope->parentScope;
    while (scope)
    {
        SWAG_CHECK(scope->symTable->checkHiddenSymbol(context->sourceFile, node->token, node->name, node->typeInfo, SymbolType::Variable));
        scope = scope->parentScope;
    }

    context->result = SemanticResult::Done;
    return true;
}
