#pragma once
#include "SpinLock.h"
#include "SymTable.h"
#include "Tokenizer.h"
#include "Utf8Crc.h"
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
    Namespace,
};

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

struct AstScope : public AstNode
{
    AstScope* parentScope = nullptr;
    SymTable* symTable    = nullptr;
    utf8crc   name;

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
        scope   = nullptr;
        astType = nullptr;
    }

    AstScope*       scope;
    utf8crc         name;
    struct AstType* astType;
};

struct AstType : public AstNode
{
};
