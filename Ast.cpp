#include "pch.h"
#include "Ast.h"
#include "PoolFactory.h"

namespace Ast
{
    void addChild(AstNode* parent, AstNode* child, bool lockParent)
    {
        if (parent)
        {
            assert(parent && child);
            if (lockParent)
                parent->lock();
            parent->childs.push_back(child);
            if (lockParent)
                parent->unlock();
        }

        child->parent = parent;
    }

    Scope* newScope(SourceFile* sourceFile, Utf8Crc& name, ScopeKind kind, Scope* parentScope)
    {
        auto fullname         = parentScope->fullname + "." + name;
        auto newScope         = sourceFile->poolFactory->scope.alloc();
        newScope->kind        = kind;
        newScope->parentScope = parentScope;
        newScope->name        = name;
        newScope->fullname    = move(fullname);
        return newScope;
    }

}; // namespace Ast
