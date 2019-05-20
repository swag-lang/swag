#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"

bool SemanticJob::resolveVarDecl(SemanticJob*, AstNode*, SemanticResult& result)
{
    result = SemanticResult::Done;
    return true;
}

bool SemanticJob::execute()
{
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
			SemanticResult result;
            SWAG_CHECK(fct(this, node, result));
			if (result == SemanticResult::Pending)
				break;
            nodes.pop_back();
            break;
        }
    }

    return true;
}
