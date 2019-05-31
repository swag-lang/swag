#include "pch.h"
#include "Ast.h"
#include "PoolFactory.h"
#include "SourceFile.h"

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

    const char* getKindName(AstNode* node)
    {
        switch (node->kind)
        {
        case AstNodeKind::VarDecl:
            return "a variable";
        case AstNodeKind::FuncDecl:
            return "a function";
        case AstNodeKind::EnumDecl:
            return "an enum";
        case AstNodeKind::EnumValue:
            return "an enum value";
        case AstNodeKind::Namespace:
            return "a namespace";
        case AstNodeKind::TypeDecl:
            return "a type";
        }

		return "something else";
    }

}; // namespace Ast
