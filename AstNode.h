#pragma once
#include "DependentJobs.h"
#include "SpinLock.h"
#include "Pool.h"
#include "Utf8Crc.h"
#include "Tokenizer.h"
#include "SyntaxJob.h"
#include "Register.h"
#include "AstFlags.h"
#include "RegisterList.h"
#include "SymTable.h"
#include "Attribute.h"
#include "Scope.h"

struct SemanticContext;
struct ByteCodeGenContext;
struct TypeInfo;
struct SymbolOverload;
struct SymbolName;
struct ByteCodeGenJob;
struct ByteCode;
struct Job;
struct AstFuncDecl;
struct AstAttrUse;
struct AstSwitch;
struct TypeInfoParam;
struct AstBreakable;
struct AstInline;
struct AstStruct;
struct TypeInfoStruct;
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
	Invalid,
    Module,
    File,
    VarDecl,
    ConstDecl,
    LetDecl,
    TypeAlias,
    IdentifierRef,
    Identifier,
    TypeExpression,
    TypeLambda,
    Namespace,
    If,
    While,
    For,
    Loop,
    Switch,
    SwitchCase,
    Break,
    Continue,
    Statement,
    LabelBreakable,
    EnumDecl,
    StructDecl,
    InterfaceDecl,
    Inline,
    StructContent,
    TupleContent,
    Impl,
    FuncDecl,
    AttrDecl,
    AttrUse,
    FuncDeclParams,
    FuncDeclParam,
    FuncDeclType,
    FuncDeclGenericParams,
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
    ExplicitNoInit,
    MultiIdentifier,
    MultiIdentifierTuple,
    AffectOp,
    ArrayPointerIndex,
    PointerRef,
    CompilerIf,
    CompilerIfBlock,
    CompilerAssert,
    CompilerMixin,
    CompilerInline,
    CompilerPrint,
    CompilerRun,
    CompilerCode,
    CompilerImport,
    CompilerSpecialFunction,
    QuestionExpression,
    Defer,
    Init,
    Drop,
};

struct CloneContext
{
    AstInline*                ownerInline      = nullptr;
    AstBreakable*             ownerBreakable   = nullptr;
    AstFuncDecl*              ownerFct         = nullptr;
    AstNode*                  parent           = nullptr;
    Scope*                    parentScope      = nullptr;
    Scope*                    ownerStructScope = nullptr;
    AstNode*                  ownerMainNode    = nullptr;
    map<TypeInfo*, TypeInfo*> replaceTypes;
};

struct AstNode : public PoolElement
{
    void reset() override
    {
        semanticState                = AstNodeResolveState::Enter;
        bytecodeState                = AstNodeResolveState::Enter;
        ownerScope                   = nullptr;
        ownerBreakable               = nullptr;
        ownerInline                  = nullptr;
        ownerFct                     = nullptr;
        ownerStructScope             = nullptr;
        ownerMainNode                = nullptr;
        ownerCompilerIfBlock         = nullptr;
        parent                       = nullptr;
        semanticFct                  = nullptr;
        semanticBeforeFct            = nullptr;
        semanticAfterFct             = nullptr;
        byteCodeFct                  = nullptr;
        byteCodeBeforeFct            = nullptr;
        byteCodeAfterFct             = nullptr;
        typeInfo                     = nullptr;
        castedTypeInfo               = nullptr;
        resolvedSymbolName           = nullptr;
        resolvedSymbolOverload       = nullptr;
        resolvedUserOpSymbolName     = nullptr;
        resolvedUserOpSymbolOverload = nullptr;
        parentAttributes             = nullptr;
        bc                           = nullptr;
        sourceFile                   = nullptr;
        attributeFlags               = 0;
        flags                        = 0;
        doneFlags                    = 0;
        fctCallStorageOffset         = 0;
        byteCodeJob                  = nullptr;
        concreteTypeInfo             = nullptr;
        concreteTypeInfoStorage      = UINT32_MAX;
        resultRegisterRC.clear();
        childs.clear();
        computedValue.reg.u64 = 0;
        computedValue.text.clear();
        alternativeScopes.clear();
        alternativeScopesVars.clear();
        doneLeaveScopeDefer.clear();
        doneLeaveScopeDrop.clear();
    }

    void lock()
    {
        mutex.lock();
    }

    void unlock()
    {
        mutex.unlock();
    }

    void inheritOrFlag(uint64_t flag)
    {
        for (auto child : childs)
            flags |= child->flags & flag;
    }

    void inheritOrFlag(AstNode* op, uint64_t flag)
    {
        if (!op)
            return;
        flags |= op->flags & flag;
    }

    void inheritAndFlag(uint64_t flag)
    {
        inheritAndFlag(this, flag);
    }

    void inheritAndFlag(AstNode* who, uint64_t flag)
    {
        for (auto child : who->childs)
        {
            if (!(child->flags & flag))
                return;
        }

        flags |= flag;
    }

    void inheritComputedValue(AstNode* from)
    {
        if (!from)
            return;
        inheritOrFlag(from, AST_VALUE_COMPUTED);
        if (flags & AST_VALUE_COMPUTED)
        {
            flags |= AST_CONST_EXPR;
            computedValue = move(from->computedValue);
        }
    }

    void inheritTokenName(Token& tkn)
    {
        SWAG_ASSERT(!tkn.text.empty());
        name = move(tkn.text);
    }

    void inheritTokenLocation(Token& tkn)
    {
        token.startLocation = tkn.startLocation;
        token.endLocation   = tkn.endLocation;
    }

    void inheritOwners(AstNode* op)
    {
        if (!op)
            return;
        ownerStructScope     = op->ownerStructScope;
        ownerMainNode        = op->ownerMainNode;
        ownerScope           = op->ownerScope;
        ownerFct             = op->ownerFct;
        ownerBreakable       = op->ownerBreakable;
        ownerInline          = op->ownerInline;
        ownerCompilerIfBlock = op->ownerCompilerIfBlock;
    }

    void inheritOwnersAndFlags(SyntaxJob* job)
    {
        ownerStructScope     = job->currentStructScope;
        ownerMainNode        = job->currentMainNode;
        ownerScope           = job->currentScope;
        ownerFct             = job->currentFct;
        ownerBreakable       = job->currentBreakable;
        ownerCompilerIfBlock = job->currentCompilerIfBlock;

        flags |= job->currentFlags;
        attributeFlags |= job->currentAttributesFlags;
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

    void             setPassThrough();
    void             inheritLocationFromChilds();
    static Utf8      getKindName(AstNode* node);
    static Utf8      getNakedKindName(AstNode* node);
    AstNode*         findChildRef(AstNode* ref, AstNode* fromChild);
    virtual AstNode* clone(CloneContext& context);
    void             copyFrom(CloneContext& context, AstNode* from, bool cloneChilds = true);
    void             computeFullName();
    void             releaseRec();

    AstNodeKind         kind;
    Scope*              ownerScope;
    AstBreakable*       ownerBreakable;
    AstInline*          ownerInline;
    AstFuncDecl*        ownerFct;
    uint64_t            ownerFlags;
    Scope*              ownerStructScope;
    AstNode*            ownerMainNode;
    AstCompilerIfBlock* ownerCompilerIfBlock;

    vector<Scope*>           alternativeScopes;
    vector<AlternativeScope> alternativeScopesVars;

    TypeInfo*       typeInfo;
    TypeInfo*       castedTypeInfo;
    SymbolName*     resolvedSymbolName;
    SymbolOverload* resolvedSymbolOverload;
    SymbolName*     resolvedUserOpSymbolName;
    SymbolOverload* resolvedUserOpSymbolOverload;
    ByteCodeGenJob* byteCodeJob;

    TypeInfo* concreteTypeInfo;
    uint32_t  concreteTypeInfoStorage;

    AstNode*    parent;
    uint32_t    childParentIdx;
    uint32_t    attributeFlags;
    AstAttrUse* parentAttributes;
    Token       token;

    SemanticFct         semanticFct;
    SemanticFct         semanticBeforeFct;
    SemanticFct         semanticAfterFct;
    ByteCodeFct         byteCodeFct;
    ByteCodeNotifyFct   byteCodeBeforeFct;
    ByteCodeNotifyFct   byteCodeAfterFct;
    AstNodeResolveState semanticState;
    AstNodeResolveState bytecodeState;

    vector<AstNode*> childs;
    set<Scope*>      doneLeaveScopeDefer;
    set<Scope*>      doneLeaveScopeDrop;

    uint64_t      flags;
    uint64_t      doneFlags;
    SpinLock      mutex;
    ComputedValue computedValue;
    Utf8Crc       name;
    Utf8          fullnameDot;
    Utf8          fullnameForeign;
    SourceFile*   sourceFile;
    ByteCode*     bc;
    RegisterList  resultRegisterRC;
    RegisterList  additionalRegisterRC;
    RegisterList  contiguousRegisterRC;
    uint32_t      fctCallStorageOffset;
};

struct AstVarDecl : public AstNode
{
    void reset() override
    {
        type       = nullptr;
        assignment = nullptr;
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    struct AstNode* type;
    struct AstNode* assignment;
};

struct AstIdentifierRef : public AstNode
{
    void reset() override
    {
        startScope           = nullptr;
        previousResolvedNode = nullptr;
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;
    void     computeName();

    Scope*   startScope;
    AstNode* previousResolvedNode;
};

struct AstIdentifier : public AstNode
{
    void reset() override
    {
        identifierRef     = nullptr;
        callParameters    = nullptr;
        genericParameters = nullptr;
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    AstIdentifierRef* identifierRef;
    AstNode*          genericParameters;
    AstNode*          callParameters;
};

struct AstFuncDecl : public AstNode
{
    void reset() override
    {
        stackSize         = 0;
        parameters        = nullptr;
        genericParameters = nullptr;
        returnType        = nullptr;
        content           = nullptr;
        scope             = nullptr;
        dependentJobs.clear();
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    uint32_t         stackSize;
    AstNode*         parameters;
    AstNode*         genericParameters;
    AstNode*         returnType;
    AstNode*         content;
    DependentJobs    dependentJobs;
    Scope*           scope;
    SymbolAttributes collectAttributes;
};

struct AstAttrDecl : public AstNode
{
    void reset() override
    {
        parameters = nullptr;
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    AstNode* parameters;
};

struct AstAttrUse : public AstNode
{
    void reset() override
    {
        values.clear();
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    map<Utf8, pair<TypeInfo*, ComputedValue>> values;
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

    AstNode* clone(CloneContext& context) override;

    Utf8           namedParam;
    AstNode*       namedParamNode;
    TypeInfoParam* resolvedParameter;
    int            index;
    bool           mustSortParameters;
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

    AstNode* clone(CloneContext& context) override;

    AstNode* boolExpression;
    AstNode* ifBlock;
    AstNode* elseBlock;

    int seekJumpExpression;
    int seekJumpAfterIf;
};

struct AstBreakContinue : public AstNode
{
    void reset() override
    {
        label.clear();
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    int  jumpInstruction;
    Utf8 label;
};

const uint32_t BREAKABLE_CAN_HAVE_INDEX    = 0x00000001;
const uint32_t BREAKABLE_CAN_HAVE_CONTINUE = 0x00000002;
const uint32_t BREAKABLE_NEED_INDEX        = 0x00000004;

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

    void copyFrom(CloneContext& context, AstBreakable* curParentBreakable, AstBreakable* from);

    int seekJumpBeforeContinue;
    int seekJumpBeforeExpression;
    int seekJumpExpression;
    int seekJumpAfterBlock;

    uint32_t                  breakableFlags;
    uint32_t                  registerIndex;
    AstNode*                  parentBreakable;
    vector<AstBreakContinue*> breakList;
    vector<AstBreakContinue*> continueList;
};

struct AstLabelBreakable : public AstBreakable
{
    void reset() override
    {
        block = nullptr;
        AstBreakable::reset();
    }

    AstNode* clone(CloneContext& context) override;

    AstNode* block;
};

struct AstWhile : public AstBreakable
{
    void reset() override
    {
        boolExpression = nullptr;
        block          = nullptr;
        AstBreakable::reset();
    }

    AstNode* clone(CloneContext& context) override;

    AstNode* boolExpression;
    AstNode* block;
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

    AstNode* clone(CloneContext& context) override;

    AstNode* preExpression;
    AstNode* boolExpression;
    AstNode* postExpression;
    AstNode* block;

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

    AstNode* clone(CloneContext& context) override;

    AstNode* expression;
    AstNode* block;
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

    AstNode* clone(CloneContext& context) override;

    AstNode*                      expression;
    AstNode*                      block;
    vector<struct AstSwitchCase*> cases;
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

    AstNode* clone(CloneContext& context) override;

    vector<AstNode*> expressions;
    AstNode*         block;
    AstSwitch*       ownerSwitch;
    bool             isDefault;
};

struct AstSwitchCaseBlock : public AstNode
{
    void reset() override
    {
        isDefault = false;
        ownerCase = nullptr;
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    bool           isDefault;
    AstSwitchCase* ownerCase;

    int seekJumpNextCase;
};

struct AstTypeExpression : public AstNode
{
    void reset() override
    {
        identifier     = nullptr;
        ptrCount       = 0;
        arrayDim       = 0;
        isSlice        = false;
        isConst        = false;
        isCode         = false;
        forceConstType = false;
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    AstNode* identifier;
    int      ptrCount;
    int      arrayDim;
    bool     isSlice;
    bool     isConst;
    bool     isCode;
    bool     forceConstType;
};

struct AstTypeLambda : public AstNode
{
    void reset() override
    {
        parameters = nullptr;
        returnType = nullptr;
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    AstNode* parameters;
    AstNode* returnType;
};

struct AstPointerDeRef : public AstNode
{
    void reset() override
    {
        array  = nullptr;
        access = nullptr;
        structFlatParams.clear();
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    AstNode*         array;
    AstNode*         access;
    vector<AstNode*> structFlatParams;
};

struct AstProperty : public AstNode
{
    void reset() override
    {
        expression = nullptr;
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    AstNode* expression;
    Property prop;
};

struct AstExpressionList : public AstNode
{
    void reset() override
    {
        storageOffset        = 0;
        storageOffsetSegment = UINT32_MAX;
        isConst              = false;
        forceConstType       = false;
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    bool             isConst;
    bool             forceConstType;
    uint32_t         storageOffset;
    uint32_t         storageOffsetSegment;
    TypeInfoListKind listKind;
};

struct AstStruct : public AstNode
{
    void reset() override
    {
        genericParameters = nullptr;
        content           = nullptr;
        scope             = nullptr;
        packing           = sizeof(uint64_t);
        dependentJobs.clear();
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    uint32_t      packing;
    AstNode*      genericParameters;
    AstNode*      content;
    Scope*        scope;
    DependentJobs dependentJobs;
};

struct AstImpl : public AstNode
{
    void reset() override
    {
        structScope   = nullptr;
        identifier    = nullptr;
        identifierFor = nullptr;
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    Scope*   structScope;
    AstNode* identifier;
    AstNode* identifierFor;
};

struct AstInit : public AstNode
{
    void reset() override
    {
        expression = nullptr;
        count      = nullptr;
        parameters = nullptr;
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    AstNode* expression;
    AstNode* count;
    AstNode* parameters;
};

struct AstDrop : public AstNode
{
    void reset() override
    {
        expression = nullptr;
        count      = nullptr;
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    AstNode* expression;
    AstNode* count;
};

struct AstReturn : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    int seekJump;
};

struct AstCompilerInline : public AstNode
{
    AstNode* clone(CloneContext& context) override;
};

struct AstInline : public AstNode
{
    void reset() override
    {
        returnList.clear();
        scope = nullptr;
        func  = nullptr;
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    AstFuncDecl*       func;
    Scope*             scope;
    vector<AstReturn*> returnList;
};

struct AstCompilerIfBlock : public AstNode
{
    void reset() override
    {
        symbols.clear();
        blocks.clear();
        interfacesCount.clear();
        AstNode::reset();
    }

    AstNode* clone(CloneContext& context) override;

    void addSymbol(SymbolName* symbolName)
    {
        scoped_lock lk(mutex);
        symbols.push_back(symbolName);
    }

    vector<AstCompilerIfBlock*> blocks;
    vector<SymbolName*>         symbols;
    vector<TypeInfoStruct*>     interfacesCount;
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
extern Pool<AstImpl>            g_Pool_astImpl;
extern Pool<AstInit>            g_Pool_astInit;
extern Pool<AstDrop>            g_Pool_astDrop;
extern Pool<AstInline>          g_Pool_astInline;
extern Pool<AstReturn>          g_Pool_astReturn;
extern Pool<AstCompilerIfBlock> g_Pool_astCompilerIfBlock;
extern Pool<AstLabelBreakable>  g_Pool_astLabelBreakable;
extern Pool<AstCompilerInline>  g_Pool_astCompilerInline;
