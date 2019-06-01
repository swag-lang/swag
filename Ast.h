#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "AstNode.h"
struct Scope;
enum class ScopeKind;

struct PoolFactory;
namespace Ast
{
    template<typename T>
    T* newNode(Pool<T>* pool, AstNodeKind kind, Scope* scope, uint32_t sourceFileIdx, AstNode* parent = nullptr, bool lockParent = true)
    {
        auto node           = pool->alloc();
        node->kind          = kind;
        node->parent        = parent;
        node->scope         = scope;
        node->sourceFileIdx = sourceFileIdx;

        if (parent)
        {
            if (lockParent)
                parent->lock();
            node->childParentIdx = (uint32_t) parent->childs.size();
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

    extern Scope*      newScope(SourceFile* sourceFile, const Utf8Crc& name, ScopeKind kind, Scope* parentScope);
    extern void        addChild(AstNode* parent, AstNode* child, bool lockParent = true);
    extern const char* getKindName(AstNode* node);
}; // namespace Ast

template<typename T>
inline T* CastAst(AstNode* ptr, AstNodeKind kind)
{
    T* casted = static_cast<T*>(ptr);
    assert(casted->kind == kind);
    return casted;
}