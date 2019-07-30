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
enum class TypeInfoListKind;

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
    TypeExpression,
    TypeLambda,
    Namespace,
    If,
    Else,
    While,
    For,
    Loop,
    Switch,
    SwitchCase,
    Break,
    Continue,
    Statement,
    EnumDecl,
    StructDecl,
    FuncDecl,
    AttrDecl,
    AttrUse,
    FuncDeclParams,
    FuncDeclParam,
    FuncDeclType,
    FuncCallParameters,
    FuncCallOneParam,
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

    AstNode*         findChildRef(AstNode* ref, AstNode* fromChild);
    virtual AstNode* clone();
    void             copyFrom(AstNode* from);

    AstNodeKind   kind;
    Scope*        ownerScope;
    AstBreakable* ownerBreakable;
    AstFuncDecl*  ownerFct;
    uint64_t      ownerFlags;

    TypeInfo*       typeInfo;
    TypeInfo*       castedTypeInfo;
    SymbolName*     resolvedSymbolName;
    SymbolOverload* resolvedSymbolOverload;
    ByteCodeGenJob* byteCodeJob;

    AstNode*    parent;
    uint32_t    childParentIdx;
    AstAttrUse* parentAttributes;
    uint64_t    attributeFlags;
    Token       token;

    SemanticFct       semanticFct;
    SemanticFct       semanticBeforeFct;
    SemanticFct       semanticAfterFct;
    ByteCodeFct       byteCodeFct;
    ByteCodeNotifyFct byteCodeBeforeFct;
    ByteCodeNotifyFct byteCodeAfterFct;

    AstNodeResolveState semanticState;
    AstNodeResolveState bytecodeState;

    uint64_t         flags;
    vector<AstNode*> childs;
    SpinLock         mutex;
    ComputedValue    computedValue;
    Utf8Crc          name;
    uint32_t         sourceFileIdx;
    ByteCode*        bc;
    RegisterList     resultRegisterRC;
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

    AstNode* clone() override;

    Scope* startScope;
};

struct AstIdentifier : public AstNode
{
    void reset() override
    {
        callParameters       = nullptr;
        fctCallStorageOffset = 0;
        AstNode::reset();
    }

    AstNode* clone() override;

    AstNode* callParameters;
    uint32_t fctCallStorageOffset;
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

    AstNode* clone() override;

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

    AstNode* clone() override;

    AstNode* parameters;
};

struct AstAttrUse : public AstNode
{
    void reset() override
    {
        values.clear();
        AstNode::reset();
    }

    AstNode* clone() override;

    map<string, ComputedValue> values;
};

struct AstFuncCallParam : public AstNode
{
    void reset() override
    {
        namedParam.clear();
        namedParamNode     = nullptr;
        resolvedParameter  = nullptr;
        index              = 0;
        mustSortParameters = false;
        AstNode::reset();
    }

    AstNode* clone() override;

    Utf8                   namedParam;
    AstNode*               namedParamNode;
    TypeInfoFuncAttrParam* resolvedParameter;
    int                    index;
    bool                   mustSortParameters;
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

    AstNode* clone() override;

    AstNode* boolExpression;
    AstNode* ifBlock;
    AstNode* elseBlock;

    int seekJumpExpression;
    int seekJumpAfterIf;
};

struct AstBreakContinue : public AstNode
{
    AstNode* clone() override;

    int jumpInstruction;
};

const uint32_t BREAKABLE_CAN_HAVE_INDEX    = 0x00000001;
const uint32_t BREAKABLE_CAN_HAVE_CONTINUE = 0x00000002;
const uint32_t BREAKABLE_NEED_INDEX        = 0x00000002;

struct AstBreakable : public AstNode
{
    void reset() override
    {
        breakableFlags  = BREAKABLE_CAN_HAVE_INDEX | BREAKABLE_CAN_HAVE_CONTINUE;
        registerIndex   = 0;
        parentBreakable = nullptr;
        breakList.clear();
        continueList.clear();
        AstNode::reset();
    }

    bool needIndex()
    {
        return breakableFlags & BREAKABLE_NEED_INDEX;
    }

    void copyFrom(AstNode* from);

    uint32_t                  breakableFlags;
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

    AstNode* clone() override;

    AstNode* boolExpression;
    AstNode* block;

    int seekJumpBeforeExpression;
    int seekJumpExpression;
    int seekJumpAfterBlock;
};

struct AstFor : public AstBreakable
{
    void reset() override
    {
        preExpression  = nullptr;
        boolExpression = nullptr;
        postExpression = nullptr;
        block          = nullptr;
        AstBreakable::reset();
    }

    AstNode* clone() override;

    AstNode* preExpression;
    AstNode* boolExpression;
    AstNode* postExpression;
    AstNode* block;

    int seekJumpBeforeExpression;
    int seekJumpExpression;
    int seekJumpAfterBlock;
    int seekJumpToBlock;
    int seekJumpBeforePost;
};

struct AstLoop : public AstBreakable
{
    void reset() override
    {
        expression = nullptr;
        block      = nullptr;
        AstBreakable::reset();
    }

    AstNode* clone() override;

    AstNode* expression;
    AstNode* block;

    int seekJumpBeforeExpression;
    int seekJumpExpression;
    int seekJumpAfterBlock;
};

struct AstSwitch : public AstBreakable
{
    void reset() override
    {
        expression = nullptr;
        block      = nullptr;
        cases.clear();
        AstBreakable::reset();
        breakableFlags &= ~BREAKABLE_CAN_HAVE_INDEX;
        breakableFlags &= ~BREAKABLE_CAN_HAVE_CONTINUE;
    }

    AstNode* clone() override;

    AstNode*                      expression;
    AstNode*                      block;
    vector<struct AstSwitchCase*> cases;

    int seekJumpBeforeExpression;
    int seekJumpExpression;
    int seekJumpAfterBlock;
};

struct AstSwitchCase : public AstNode
{
    void reset() override
    {
        expressions.clear();
        block       = nullptr;
        ownerSwitch = nullptr;
        isDefault   = false;
        AstNode::reset();
    }

    AstNode* clone() override;

    vector<AstNode*> expressions;
    AstNode*         block;
    AstSwitch*       ownerSwitch;
    bool             isDefault;

    int seekJumpBeforeExpression;
    int seekJumpExpression;
    int seekJumpAfterBlock;
};

struct AstSwitchCaseBlock : public AstNode
{
    void reset() override
    {
        isDefault = false;
        ownerCase = nullptr;
        AstNode::reset();
    }

    AstNode* clone() override;

    bool           isDefault;
    AstSwitchCase* ownerCase;

    int seekJumpNextCase;
};

struct AstTypeExpression : public AstNode
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

    AstNode* clone() override;

    AstNode* typeExpression;
    int      ptrCount;
    int      arrayDim;
    bool     isSlice;
    bool     isConst;
};

struct AstTypeLambda : public AstNode
{
    void reset() override
    {
        parameters = nullptr;
        returnType = nullptr;
        AstNode::reset();
    }

    AstNode* clone() override;

    AstNode* parameters;
    AstNode* returnType;
};

struct AstPointerDeRef : public AstNode
{
    void reset() override
    {
        array  = nullptr;
        access = nullptr;
        AstNode::reset();
    }

    AstNode* clone() override;

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

    AstNode* clone() override;

    AstNode* expression;
    Property prop;
};

struct AstExpressionList : public AstNode
{
    AstNode* clone() override;

    uint32_t         storageOffset;
    TypeInfoListKind listKind;
};

struct AstStruct : public AstNode
{
    AstNode* clone() override;
};

extern Pool<AstNode>            g_Pool_astNode;
extern Pool<AstAttrDecl>        g_Pool_astAttrDecl;
extern Pool<AstAttrUse>         g_Pool_astAttrUse;
extern Pool<AstVarDecl>         g_Pool_astVarDecl;
extern Pool<AstFuncDecl>        g_Pool_astFuncDecl;
extern Pool<AstIdentifier>      g_Pool_astIdentifier;
extern Pool<AstIdentifierRef>   g_Pool_astIdentifierRef;
extern Pool<AstFuncCallParam>   g_Pool_astFuncCallParam;
extern Pool<AstIf>              g_Pool_astIf;
extern Pool<AstWhile>           g_Pool_astWhile;
extern Pool<AstFor>             g_Pool_astFor;
extern Pool<AstLoop>            g_Pool_astLoop;
extern Pool<AstSwitch>          g_Pool_astSwitch;
extern Pool<AstSwitchCase>      g_Pool_astSwitchCase;
extern Pool<AstSwitchCaseBlock> g_Pool_astSwitchCaseBlock;
extern Pool<AstBreakContinue>   g_Pool_astBreakContinue;
extern Pool<AstTypeExpression>  g_Pool_astTypeExpression;
extern Pool<AstTypeLambda>      g_Pool_astTypeLambda;
extern Pool<AstPointerDeRef>    g_Pool_astPointerDeref;
extern Pool<AstProperty>        g_Pool_astProperty;
extern Pool<AstExpressionList>  g_Pool_astExpressionList;
extern Pool<AstStruct>          g_Pool_astStruct;
