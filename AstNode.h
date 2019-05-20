#pragma once
#include "SpinLock.h"
#include "SymTable.h"
#include "Tokenizer.h"
struct SemanticContext;

typedef bool (*SemanticFct)(SemanticContext* context);

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

const uint64_t AST_IS_TOPLEVEL = 0x00000000'00000001;

struct AstNode : public PoolElement
{
    void reset() override
    {
        semanticState = AstNodeSemanticState::Enter;
        parent        = nullptr;
        semanticFct   = nullptr;
        typeInfo      = nullptr;
        flags         = 0;
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

    TypeInfo*            typeInfo;
    Token                token;
    SemanticFct          semanticFct;
    AstNodeType          type;
    AstNodeSemanticState semanticState;
    AstNode*             parent;
    uint64_t             flags;
    vector<AstNode*>     childs;
    SpinLock             mutex;
};

struct AstScopeNode : public AstNode
{
    SymTable* symTable = nullptr;

    void allocateSymTable()
    {
        if (symTable)
            return;
        symTable = new SymTable();
    }
};

struct AstVarDecl : public AstNode
{
    void reset() override
    {
        astType = nullptr;
    }

    struct AstType* astType;
};

struct AstType : public AstNode
{
};
