#pragma once
#include "SpinLock.h"
#include "Pool.h"
#include "Utf8Crc.h"
#include "Tokenizer.h"
#include "SyntaxJob.h"
#include "Register.h"
struct SemanticContext;
struct ByteCodeGenContext;
struct Scope;
struct Attribute;
struct TypeInfo;
struct SymbolOverload;
struct SymbolName;
struct ByteCodeGenJob;
struct ByteCode;
struct Job;
struct AstFuncDecl;
struct AstAttrUse;
struct TypeInfoFuncAttrParam;

typedef bool (*SemanticFct)(SemanticContext* context);
typedef bool (*ByteCodeFct)(ByteCodeGenContext* context);
typedef bool (*ByteCodeNotifyFct)(ByteCodeGenContext* context);

enum class AstNodeResolveState
{
    Enter,
    ProcessingChilds,
    PostChilds,
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
    If,
    Else,
    While,
    Break,
    Continue,
    Statement,
    EnumDecl,
    FuncDecl,
    AttrDecl,
    AttrUse,
    FuncDeclParams,
    FuncDeclParam,
    FuncDeclType,
    FuncCallParams,
    FuncCallParam,
    FuncCall,
    FuncContent,
    Return,
    EnumType,
    EnumValue,
    Literal,
    Cast,
    SingleOp,
    BinaryOp,
    AffectOp,
    CompilerAssert,
    CompilerPrint,
    CompilerRun,
    CompilerImport,
};

static const uint64_t AST_CONST_EXPR         = 0x00000000'00000001;
static const uint64_t AST_VALUE_COMPUTED     = 0x00000000'00000002;
static const uint64_t AST_BYTECODE_GENERATED = 0x00000000'00000004;
static const uint64_t AST_FULL_RESOLVE       = 0x00000000'00000008;
static const uint64_t AST_SCOPE_HAS_RETURN   = 0x00000000'00000010;
static const uint64_t AST_FCT_HAS_RETURN     = 0x00000000'00000020;
static const uint64_t AST_LEFT_EXPRESSION    = 0x00000000'00000040;
static const uint64_t AST_NO_BYTECODE_CHILDS = 0x00000000'00000080;
static const uint64_t AST_BYTECODE_RESOLVED  = 0x00000000'00000100;

struct AstNode : public PoolElement
{
    void reset() override
    {
        semanticState      = AstNodeResolveState::Enter;
        bytecodeState      = AstNodeResolveState::Enter;
        ownerScope         = nullptr;
        ownerBreakable     = nullptr;
        ownerFct           = nullptr;
        parent             = nullptr;
        semanticFct        = nullptr;
        semanticBeforeFct  = nullptr;
        semanticAfterFct   = nullptr;
        byteCodeFct        = nullptr;
        byteCodeBeforeFct  = nullptr;
        byteCodeAfterFct   = nullptr;
        typeInfo           = nullptr;
        castedTypeInfo     = nullptr;
        resolvedSymbolName = nullptr;
        parentAttributes   = nullptr;
        bc                 = nullptr;
        sourceFileIdx      = UINT32_MAX;
        attributeFlags     = 0;
        flags              = 0;
        byteCodeJob        = nullptr;
        resultRegisterRC   = UINT32_MAX;
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

    void inheritOwners(SyntaxJob* job)
    {
        ownerScope     = job->currentScope;
        ownerBreakable = job->currentBreakable;
        ownerFct       = job->currentFct;
    }

    Scope*        ownerScope;
    AstBreakable* ownerBreakable;
    AstFuncDecl*  ownerFct;

    TypeInfo*       typeInfo;
    TypeInfo*       castedTypeInfo;
    SymbolName*     resolvedSymbolName;
    SymbolOverload* resolvedSymbolOverload;
    ByteCodeGenJob* byteCodeJob;

    AstNode*            parent;
    AstAttrUse*         parentAttributes;
    uint64_t            attributeFlags;
    Token               token;
    SemanticFct         semanticFct;
    SemanticFct         semanticBeforeFct;
    SemanticFct         semanticAfterFct;
    ByteCodeFct         byteCodeFct;
    ByteCodeNotifyFct   byteCodeBeforeFct;
    ByteCodeNotifyFct   byteCodeAfterFct;
    AstNodeKind         kind;
    AstNodeResolveState semanticState;
    AstNodeResolveState bytecodeState;
    uint64_t            flags;
    vector<AstNode*>    childs;
    SpinLock            mutex;
    ComputedValue       computedValue;
    Utf8Crc             name;
    uint32_t            sourceFileIdx;
    uint32_t            childParentIdx;
    ByteCode*           bc;
    uint32_t            resultRegisterRC;
};

struct AstVarDecl : public AstNode
{
    void reset() override
    {
        astType       = nullptr;
        astAssignment = nullptr;
        AstNode::reset();
    }

    struct AstNode* astType;
    struct AstNode* astAssignment;
};

struct AstIdentifierRef : public AstNode
{
    void reset() override
    {
        startScope = nullptr;
        AstNode::reset();
    }

    Scope* startScope;
};

struct AstIdentifier : public AstNode
{
    void reset() override
    {
        callParameters = nullptr;
        AstNode::reset();
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
        stackSize  = 0;
        dependentJobs.clear();
        AstNode::reset();
    }

    int          stackSize;
    AstNode*     parameters;
    AstNode*     returnType;
    AstNode*     content;
    vector<Job*> dependentJobs;
};

struct AstAttrDecl : public AstNode
{
    void reset() override
    {
        parameters = nullptr;
        AstNode::reset();
    }

    AstNode* parameters;
};

struct AstAttrUse : public AstNode
{
    void reset() override
    {
        AstNode::reset();
        values.clear();
    }

    map<string, ComputedValue> values;
};

struct AstFuncCallParam : public AstNode
{
    Utf8                   namedParam;
    TypeInfoFuncAttrParam* resolvedParameter;
};

struct AstIf : public AstNode
{
    void reset() override
    {
        AstNode::reset();
        boolExpression = nullptr;
        ifBlock        = nullptr;
        elseBlock      = nullptr;
    }

    AstNode* boolExpression;
    AstNode* ifBlock;
    AstNode* elseBlock;

    int seekJumpExpression;
    int seekJumpAfterIf;
};

struct AstBreakContinue : public AstNode
{
    int jumpInstruction;
};

struct AstBreakable : public AstNode
{
    void reset() override
    {
        AstNode::reset();
        parentBreakable = nullptr;
    }

    AstNode*                  parentBreakable;
    vector<AstBreakContinue*> breakList;
    vector<AstBreakContinue*> continueList;
};

struct AstWhile : public AstBreakable
{
    void reset() override
    {
        AstBreakable::reset();
        boolExpression = nullptr;
        block          = nullptr;
    }

    AstNode* boolExpression;
    AstNode* block;

    int seekJumpBeforeExpression;
    int seekJumpExpression;
    int seekJumpAfterBlock;
};

extern Pool<AstNode>          g_Pool_astNode;
extern Pool<AstAttrDecl>      g_Pool_astAttrDecl;
extern Pool<AstAttrUse>       g_Pool_astAttrUse;
extern Pool<AstVarDecl>       g_Pool_astVarDecl;
extern Pool<AstFuncDecl>      g_Pool_astFuncDecl;
extern Pool<AstIdentifier>    g_Pool_astIdentifier;
extern Pool<AstIdentifierRef> g_Pool_astIdentifierRef;
extern Pool<AstFuncCallParam> g_Pool_astFuncCallParam;
extern Pool<AstIf>            g_Pool_astIf;
extern Pool<AstWhile>         g_Pool_astWhile;
extern Pool<AstBreakContinue> g_Pool_astBreakContinue;
