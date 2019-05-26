#pragma once
#include "SpinLock.h"
#include "SymTable.h"
#include "Tokenizer.h"
#include "Utf8Crc.h"
struct SemanticContext;
struct Scope;

typedef bool (*SemanticFct)(SemanticContext* context);

enum class AstNodeSemanticState
{
    Enter,
    ProcessingChilds,
};

enum class AstNodeType
{
    Module,
    File,
    VarDecl,
    TypeDecl,
    IdentifierRef,
    Identifier,
    Type,
    Namespace,
	Enum,
    Literal,
    SingleOp,
    BinaryOp,
    CompilerAssert,
    CompilerPrint,
    CompilerRun,
};

static const uint64_t AST_CONST_EXPR     = 0x00000000'00000001;
static const uint64_t AST_VALUE_COMPUTED = 0x00000000'00000002;

struct AstNode : public PoolElement
{
    void reset() override
    {
        semanticState = AstNodeSemanticState::Enter;
        scope         = nullptr;
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

    void inheritAndFlag(AstNode* op, uint64_t flag)
    {
        flags |= op->flags & flag;
    }

    void inheritAndFlag(AstNode* left, AstNode* right, uint64_t flag)
    {
        if ((left->flags & flag) && (right->flags & flag))
            flags |= flag;
    }

    void inherhitComputedValue(AstNode* from)
    {
        inheritAndFlag(from, AST_VALUE_COMPUTED);
        if (flags & AST_VALUE_COMPUTED)
            computedValue = move(from->computedValue);
    }

    Scope*               scope;
    TypeInfo*            typeInfo;
    Token                token;
    SemanticFct          semanticFct;
    AstNodeType          type;
    AstNodeSemanticState semanticState;
    AstNode*             parent;
    uint64_t             flags;
    vector<AstNode*>     childs;
    SpinLock             mutex;
    ComputedValue        computedValue;
    Utf8Crc              name;
};

struct AstVarDecl : public AstNode
{
    void reset() override
    {
        scope   = nullptr;
        astType = nullptr;
    }

    struct AstNode* astType;
    struct AstNode* astAssignment;
};

struct AstIdentifierRef : public AstNode
{
    void reset() override
    {
        startScope = nullptr;
    }

    SymbolKind symbolKind;
    Scope*     startScope;
};

struct AstIdentifier : public AstNode
{
    void reset() override
    {
        resolvedSymbolName = nullptr;
        matchScope         = nullptr;
    }

    SymbolName*     resolvedSymbolName;
    SymbolOverload* resolvedSymbolOverload;
    Scope*          matchScope;
};
