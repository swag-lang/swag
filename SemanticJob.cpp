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

SymTable::SymTable()
{
    memset(mapNames, 0, sizeof(mapNames));
}

bool SemanticJob::resolveType(SemanticContext* context)
{
    auto node = static_cast<AstType*>(context->node);
    SWAG_VERIFY(node->token.literalType, context->job->error(context, "invalid type"));
    node->typeInfo  = node->token.literalType;
    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveVarDecl(SemanticContext* context)
{
    auto node      = static_cast<AstVarDecl*>(context->node);
    node->typeInfo = node->astType->typeInfo;
    assert(node->typeInfo);
    assert(node->scope);

	SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, node->astType, node->astAssignment));

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

bool SemanticJob::error(SemanticContext* context, const Utf8& msg)
{
    context->sourceFile->report({context->sourceFile, context->node->token, msg});
    return false;
}

bool SemanticJob::execute()
{
    SemanticContext context;
    context.job        = this;
    context.sourceFile = sourceFile;

    while (!nodes.empty())
    {
        auto node = nodes.back();
        switch (node->semanticState)
        {
        case AstNodeSemanticState::Enter:
            node->semanticState = AstNodeSemanticState::ProcessingChilds;
            if (!node->childs.empty())
            {
                nodes.insert(nodes.end(), node->childs.begin(), node->childs.end());
                break;
            }

        case AstNodeSemanticState::ProcessingChilds:
            context.node = node;
            SWAG_CHECK(node->semanticFct(&context));
            if (context.result == SemanticResult::Pending)
                break;
            nodes.pop_back();
            break;
        }
    }

    return true;
}
