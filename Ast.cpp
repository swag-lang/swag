#include "pch.h"
#include "Ast.h"
#include "PoolFactory.h"

namespace Ast
{
    void addChild(AstNode* parent, AstNode* child, bool lockParent)
    {
        assert(parent && child);
        if (lockParent)
            parent->lock();
        parent->childs.push_back(child);
        if (lockParent)
            parent->unlock();
        child->parent = parent;
    }
}; // namespace Ast