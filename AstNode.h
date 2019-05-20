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
    void reset() override
    {
        semanticState = AstNodeSemanticState::Enter;
        parent        = nullptr;
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

    AstNodeType          type;
    AstNodeSemanticState semanticState = AstNodeSemanticState::Enter;
    AstNode*             parent        = nullptr;
    vector<AstNode*>     childs;
    SpinLock             mutex;
};

struct AstVarDecl : public AstNode
{
    void reset() override
    {
        name.clear();
        astType = nullptr;
    }

    string          name;
    struct AstType* astType;
};

struct AstType : public AstNode
{
};
