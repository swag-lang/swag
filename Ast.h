#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "AstNode.h"
#include "Scope.h"

struct PoolFactory;
namespace Ast
{
    template<typename T>
    T* newNode(Pool<T>* pool, AstNodeKind kind, Scope* scope, AstNode* parent = nullptr, bool lockParent = true)
    {
        auto node    = pool->alloc();
        node->kind   = kind;
        node->parent = parent;
        node->scope  = scope;

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

    inline void assignToken(AstNode* node, Token& token)
    {
        node->name = move(token.text);
        node->name.computeCrc();
        node->token = move(token);
    }

    extern Scope* newScope(SourceFile* sourceFile, Utf8Crc& name, ScopeKind kind, Scope* parentScope);
    extern void   addChild(AstNode* parent, AstNode* child, bool lockParent = true);
}; // namespace Ast

template<typename T>
inline T* CastAst(AstNode* ptr, AstNodeKind kind)
{
    T* casted = static_cast<T*>(ptr);
    assert(casted->kind == kind);
    return casted;
}