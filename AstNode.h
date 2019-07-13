#pragma once
#include "SpinLock.h"
#include "Pool.h"
#include "Utf8Crc.h"
#include "Tokenizer.h"
#include "SyntaxJob.h"
#include "Register.h"
#include "AstFlags.h"
#include "RegisterList.h"
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
enum class Property;

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
    ConstDecl,
	LetDecl,
    TypeDecl,
    IdentifierRef,
    Identifier,
    Type,
    Namespace,
    If,
    Else,
    While,
    Loop,
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
    SizeOf,
    IntrinsicProp,
    Index,
    Cast,
    TypeList,
    SingleOp,
    MakePointer,
    BinaryOp,
    FactorOp,
    ExpressionList,
    AffectOp,
    ArrayPointerRef,
    ArrayPointerDeRef,
    PointerRef,
    CompilerAssert,
    CompilerPrint,
    CompilerRun,
    CompilerImport,
    CompilerVersion,
};

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
        resultRegisterRC.clear();
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

    bool isDisabled()
    {
        return flags & AST_DISABLED;
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
        {
            flags |= AST_CONST_EXPR;
            computedValue = move(from->computedValue);
        }
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

    void inheritOwnersAndFlags(SyntaxJob* job)
    {
        ownerScope     = job->currentScope;
        ownerBreakable = job->currentBreakable;
        ownerFct       = job->currentFct;
        flags |= job->currentFlags;
    }

    bool isConstantTrue()
    {
        return (flags & AST_VALUE_COMPUTED) && computedValue.reg.b == true;
    }

    bool isConstantFalse()
    {
        return (flags & AST_VALUE_COMPUTED) && computedValue.reg.b == false;
    }

    bool isConstantInt0()
    {
        return (flags & AST_VALUE_COMPUTED) && computedValue.reg.u64 == 0;
    }

    bool isConstantInt1()
    {
        return (flags & AST_VALUE_COMPUTED) && computedValue.reg.u64 == 1;
    }

    Scope*        ownerScope;
    AstBreakable* ownerBreakable;
    AstFuncDecl*  ownerFct;
    uint64_t      ownerFlags;

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
    RegisterList        resultRegisterRC;
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
        stackSize  = 0;
        parameters = nullptr;
        returnType = nullptr;
        content    = nullptr;
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
        values.clear();
        AstNode::reset();
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
        boolExpression = nullptr;
        ifBlock        = nullptr;
        elseBlock      = nullptr;
        AstNode::reset();
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
        needIndex       = false;
        registerIndex   = 0;
        parentBreakable = nullptr;
		breakList.clear();
        continueList.clear();
        AstNode::reset();
    }

    bool                      needIndex;
    uint32_t                  registerIndex;
    AstNode*                  parentBreakable;
    vector<AstBreakContinue*> breakList;
    vector<AstBreakContinue*> continueList;
};

struct AstWhile : public AstBreakable
{
    void reset() override
    {
        boolExpression = nullptr;
        block          = nullptr;
        AstBreakable::reset();
    }

    AstNode* boolExpression;
    AstNode* block;

    int seekJumpBeforeExpression;
    int seekJumpExpression;
    int seekJumpAfterBlock;
};

struct AstLoop : public AstBreakable
{
    void reset() override
    {
        expression = nullptr;
        block      = nullptr;
        AstBreakable::reset();
    }

    AstNode* expression;
    AstNode* block;

    int seekJumpBeforeExpression;
    int seekJumpExpression;
    int seekJumpAfterBlock;
};

struct AstType : public AstNode
{
    void reset() override
    {
        typeExpression = nullptr;
        ptrCount       = 0;
        arrayDim       = 0;
        isSlice        = false;
        isConst        = false;
        AstNode::reset();
    }

    AstNode* typeExpression;
    int      ptrCount;
    int      arrayDim;
    bool     isSlice;
    bool     isConst;
};

struct AstPointerDeRef : public AstNode
{
    void reset() override
    {
        array  = nullptr;
        access = nullptr;
        AstNode::reset();
    }

    AstNode* array;
    AstNode* access;
};

struct AstProperty : public AstNode
{
    void reset() override
    {
        expression = nullptr;
        AstNode::reset();
    }

    AstNode* expression;
    Property prop;
};

struct AstExpressionList : public AstNode
{
    uint32_t storageOffset;
};

extern Pool<AstNode>           g_Pool_astNode;
extern Pool<AstAttrDecl>       g_Pool_astAttrDecl;
extern Pool<AstAttrUse>        g_Pool_astAttrUse;
extern Pool<AstVarDecl>        g_Pool_astVarDecl;
extern Pool<AstFuncDecl>       g_Pool_astFuncDecl;
extern Pool<AstIdentifier>     g_Pool_astIdentifier;
extern Pool<AstIdentifierRef>  g_Pool_astIdentifierRef;
extern Pool<AstFuncCallParam>  g_Pool_astFuncCallParam;
extern Pool<AstIf>             g_Pool_astIf;
extern Pool<AstWhile>          g_Pool_astWhile;
extern Pool<AstLoop>           g_Pool_astLoop;
extern Pool<AstBreakContinue>  g_Pool_astBreakContinue;
extern Pool<AstType>           g_Pool_astType;
extern Pool<AstPointerDeRef>   g_Pool_astPointerDeref;
extern Pool<AstProperty>       g_Pool_astProperty;
extern Pool<AstExpressionList> g_Pool_astExpressionList;
