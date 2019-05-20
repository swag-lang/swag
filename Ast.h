#pragma once
#include "Pool.h"
#include "SpinLock.h"

enum class AstNodeType
{
    RootModule,
    RootFile,
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

struct PoolFactory;
namespace Ast
{
    extern AstNode* newNode(PoolFactory* factory, AstNodeType type, AstNode* parent = nullptr, bool lockParent = true);
};