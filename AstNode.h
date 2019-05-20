#pragma once
#include "SpinLock.h"

enum class AstNodeType
{
    RootModule,
    RootFile,
    VarDecl,
	Type,
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
