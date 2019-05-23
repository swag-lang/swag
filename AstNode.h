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
    Type,
    Namespace,
    Literal,
    SingleOp,
    BinaryOp,
	CompilerAssert,
};

static const uint64_t AST_CONST_EXPR     = 0x00000000'00000001;
static const uint64_t AST_VALUE_COMPUTED = 0x00000000'00000002;

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
};

struct AstVarDecl : public AstNode
{
    void reset() override
    {
        scope   = nullptr;
        astType = nullptr;
    }

    Scope*          scope;
    Utf8Crc         name;
    struct AstType* astType;
    struct AstNode* astAssignment;
};

struct AstType : public AstNode
{
};
