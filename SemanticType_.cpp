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

bool SemanticJob::resolveTypeExpression(SemanticContext* context)
{
    auto node = static_cast<AstType*>(context->node);
    if (node->token.literalType)
        node->typeInfo = node->token.literalType;
    else if (!node->childs.empty())
        node->typeInfo = node->childs[0]->typeInfo;
    else
        context->job->error(context, "invalid type yet !");
    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveTypeDecl(SemanticContext* context)
{
    auto node      = context->node;
    node->typeInfo = node->childs[0]->typeInfo;

    // Register symbol with its type
    SWAG_CHECK(node->scope->symTable->addSymbol(context->sourceFile, node->token, node->name, node->typeInfo, SymbolType::TypeDecl));

    // We need to check the scope hierarchy for symbol ghosting
    auto scope = node->scope->parentScope;
    while (scope)
    {
        SWAG_CHECK(scope->symTable->checkHiddenSymbol(context->sourceFile, node->token, node->name, node->typeInfo, SymbolType::TypeDecl));
        scope = scope->parentScope;
    }

    context->result = SemanticResult::Done;
    return true;
}
