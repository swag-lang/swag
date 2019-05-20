#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"

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
            nodes.pop_back();
            break;
        }
    }

    return true;
}
