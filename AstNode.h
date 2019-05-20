#pragma once
#include "SpinLock.h"

enum class AstNodeSemanticState
{
    Enter,
    ProcessingChilds,
};

enum class AstNodeType
{
    RootModule,
    RootFile,
    VarDecl,
    Type,
};

struct AstNode : public PoolElement
{
    AstNodeType          type;
    AstNodeSemanticState semanticState = AstNodeSemanticState::Enter;
    AstNode*             parent        = nullptr;
    vector<AstNode*>     childs;
    SpinLock             mutex;

    void reset() override
    {
		semanticState = AstNodeSemanticState::Enter;
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
    string          name;
    struct AstType* astType;

    void reset() override
    {
        name.clear();
        astType = nullptr;
    }
};

struct AstType : public AstNode
{
};
