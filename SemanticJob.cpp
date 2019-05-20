#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"

bool SemanticJob::resolveType(SemanticContext* context)
{
    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveVarDecl(SemanticContext* context)
{
    context->result = SemanticResult::Done;
    return true;
}

bool SemanticJob::execute()
{
    SemanticContext context;
	context.job = this;

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
