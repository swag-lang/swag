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

const uint64_t AST_IS_TOPLEVEL = 0x00000000'00000001;
const uint64_t AST_IS_NAMED    = 0x00000000'00000002;

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
        astType = nullptr;
    }

    utf8crc         name;
    struct AstType* astType;
};

struct AstType : public AstNode
{
};
