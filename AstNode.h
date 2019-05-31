#pragma once
#include "SpinLock.h"
#include "Pool.h"
#include "Tokenizer.h"
#include "Utf8Crc.h"
#include "SourceLocation.h"
#include "Tokenizer.h"
struct SemanticContext;
struct Scope;
struct Attribute;
struct TypeInfo;
struct SymbolOverload;
struct SymbolName;

typedef bool (*SemanticFct)(SemanticContext* context);

enum class AstNodeSemanticState
{
    Enter,
    ProcessingChilds,
};

enum class AstNodeKind
{
    Module,
    File,
    VarDecl,
    TypeDecl,
    IdentifierRef,
    Identifier,
    Type,
    Namespace,
    Statement,
    EnumDecl,
    FuncDecl,
    FuncCall,
    AttrDecl,
    AttrUse,
    FuncDeclParams,
    FuncDeclType,
    FuncCallParams,
    EnumType,
    EnumValue,
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
        semanticState      = AstNodeSemanticState::Enter;
        scope              = nullptr;
        parent             = nullptr;
        semanticFct        = nullptr;
        typeInfo           = nullptr;
        resolvedSymbolName = nullptr;
        attributes         = nullptr;
        sourceFileIdx      = UINT32_MAX;
        flags              = 0;
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

    void inheritComputedValue(AstNode* from)
    {
        inheritAndFlag(from, AST_VALUE_COMPUTED);
        if (flags & AST_VALUE_COMPUTED)
            computedValue = move(from->computedValue);
    }

    void inheritLocation()
    {
        if (childs.empty())
            return;
        token.startLocation = childs.front()->token.startLocation;
        token.endLocation   = childs.back()->token.endLocation;
    }

    void inheritToken(Token& tkn)
    {
        name = move(tkn.text);
        name.computeCrc();
        token = move(tkn);
    }

    Scope*          scope;
    TypeInfo*       typeInfo;
    SymbolName*     resolvedSymbolName;
    SymbolOverload* resolvedSymbolOverload;

    AstNode*             parent;
    AstNode*             attributes;
    Token                token;
    SemanticFct          semanticFct;
    AstNodeKind          kind;
    AstNodeSemanticState semanticState;
    uint64_t             flags;
    vector<AstNode*>     childs;
    SpinLock             mutex;
    ComputedValue        computedValue;
    Utf8Crc              name;
    uint32_t             sourceFileIdx;
    uint32_t             childParentIdx;
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

    Scope* startScope;
};

struct AstIdentifier : public AstNode
{
    void reset() override
    {
        callParameters = nullptr;
    }

    AstNode* callParameters;
};

struct AstFuncDecl : public AstNode
{
    void reset() override
    {
        parameters = nullptr;
        returnType = nullptr;
        content    = nullptr;
    }

    AstNode* parameters;
    AstNode* returnType;
    AstNode* content;
};

struct AstAttrDecl : public AstNode
{
    void reset() override
    {
        parameters = nullptr;
    }

    AstNode* parameters;
};
