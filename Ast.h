#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "AstNode.h"
struct Utf8Crc;
struct Scope;
enum class ScopeKind;

namespace Ast
{
    template<typename T>
    T* newNode(Pool<T>* pool, AstNodeKind kind, uint32_t sourceFileIdx, AstNode* parent = nullptr)
    {
        auto node           = pool->alloc();
        node->kind          = kind;
        node->parent        = parent;
        node->ownerScope    = nullptr;
        node->ownerFct      = nullptr;
        node->sourceFileIdx = sourceFileIdx;

        if (parent)
        {
            parent->lock();
            node->childParentIdx = (uint32_t) parent->childs.size();
            parent->childs.push_back(node);
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

    extern Scope*   findOrCreateScopeByName(Scope* parentScope, const string& name);
    extern void     setupScope(Scope* newScope, AstNode* owner, const string& name, ScopeKind kind, Scope* parentScope);
    extern Scope*   newScope(AstNode* owner, const string& name, ScopeKind kind, Scope* parentScope, bool matchName = false);
    extern void     removeFromParent(AstNode* child);
    extern void     addChild(AstNode* parent, AstNode* child);
    extern AstNode* createIdentifierRef(SyntaxJob* job, const Utf8Crc& name, const Token& token, AstNode* parent);
    extern void     visit(AstNode* root, const function<void(AstNode*)>& fctor);
}; // namespace Ast

template<typename T>
inline T* CastAst(AstNode* ptr, AstNodeKind kind)
{
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted->kind == kind);
    return casted;
}

template<typename T>
inline T* CastAst(AstNode* ptr, AstNodeKind kind1, AstNodeKind kind2)
{
    T* casted = static_cast<T*>(ptr);
    SWAG_ASSERT(casted->kind == kind1 || casted->kind == kind2);
    return casted;
}