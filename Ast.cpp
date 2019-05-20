#include "pch.h"
#include "Ast.h"
#include "PoolFactory.h"

namespace Ast
{
    AstNode* newNode(PoolFactory* factory, AstNodeType type, AstNode* parent, bool lockParent)
    {
        auto node    = factory->m_astNode.alloc();
        node->type   = AstNodeType::RootFile;
        node->parent = parent;

        if (parent)
        {
            if (lockParent)
                parent->lock();
            parent->childs.push_back(node);
            if (lockParent)
                parent->unlock();
        }

        return node;
    }
}; // namespace Ast