#pragma once
#include "Pool.h"
#include "SpinLock.h"

enum class AstNodeType
{
    RootModule,
    RootFile,
    VarDecl,
};

struct AstNode : public PoolElement
{
    AstNodeType      type;
    AstNode*         parent = nullptr;
    vector<AstNode*> childs;
    SpinLock         mutex;

    void reset() override
    {
        parent = nullptr;
        childs.clear();
    }

    void lock()
    {
        mutex.lock();
    }

    void unlock()
    {
        mutex.unlock();
    }
};

struct AstVarDecl : public AstNode
{
	string name;
};

struct PoolFactory;
namespace Ast
{
    template<typename T>
    T* newNode(Pool<T>* pool, AstNodeType type, AstNode* parent = nullptr, bool lockParent = true)
    {
        auto node    = pool->alloc();
        node->type   = type;
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

    extern void addChild(AstNode* parent, AstNode* child, bool lockParent = true);
}; // namespace Ast