#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"

bool SemanticJob::execute()
{
    while (!m_nodes.empty())
    {
        auto node = m_nodes.back();
        switch (node->semanticState)
        {
        case AstNodeSemanticState::Enter:
            node->semanticState = AstNodeSemanticState::ProcessingChilds;
			if (!node->childs.empty())
			{
				m_nodes.insert(m_nodes.end(), node->childs.begin(), node->childs.end());
				break;
			}

		case AstNodeSemanticState::ProcessingChilds:
            m_nodes.pop_back();
            break;
        }
    }

    return true;
}
