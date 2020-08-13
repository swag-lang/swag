#pragma once
#include "DependentJobs.h"
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
#include "RaceCondition.h"
#include "VectorNative.h"

struct DocContent;
struct AstSwitchCase;
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
enum class TypeInfoListKind;

typedef bool (*SemanticFct)(SemanticContext* context);
typedef bool (*ByteCodeFct)(ByteCodeGenContext* context);
typedef bool (*ByteCodeNotifyFct)(ByteCodeGenContext* context);

enum class AstNodeResolveState : uint8_t
{
    Enter,
    ProcessingChilds,
    PostChilds,
    Done,
};

enum class AstNodeKind : uint8_t
{
    Invalid,
    Module,
    File,
    VarDecl,
    ConstDecl,
    LetDecl,
    IdentifierRef,
    Identifier,
    TypeExpression,
    TypeLambda,
    Namespace,
    Using,
    Alias,
    If,
    While,
    For,
    Loop,
    Visit,
    Switch,
    SwitchCase,
    Break,
    FallThrough,
    Continue,
    Statement,
    StatementNoScope,
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
    DocComment,
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
    AutoCast,
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
    ArrayPointerSlicing,
    RawMove,
    PointerRef,
    CompilerIf,
    CompilerIfBlock,
    CompilerAssert,
    CompilerMixin,
    CompilerMacro,
    CompilerInline,
    CompilerPrint,
    CompilerRun,
    CompilerAst,
    CompilerCode,
    CompilerImport,
    CompilerSpecialFunction,
    CompilerForeignLib,
    ConditionalExpression,
    NullConditionalExpression,
    Defer,
    Init,
    Drop,
};

struct CloneContext
{
    map<Utf8Crc, TypeInfo*> replaceTypes;
    map<TokenId, AstNode*>  replaceTokens;
    map<Utf8Crc, Utf8>      replaceNames;

    AstInline*    ownerInline      = nullptr;
    AstBreakable* ownerBreakable   = nullptr;
    AstFuncDecl*  ownerFct         = nullptr;
    AstNode*      parent           = nullptr;
    Scope*        parentScope      = nullptr;
    Scope*        ownerStructScope = nullptr;
    AstNode*      ownerMainNode    = nullptr;
    uint64_t      forceFlags       = 0;
};

struct AstNode
{
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

    void inheritAndFlag1(uint64_t flag)
    {
        inheritAndFlag1(this, flag);
    }

    void inheritAndFlag2(uint64_t flag1, uint64_t flag2)
    {
        inheritAndFlag2(this, flag1, flag2);
    }

    void inheritAndFlag3(uint64_t flag1, uint64_t flag2, uint64_t flag3)
    {
        inheritAndFlag3(this, flag1, flag2, flag3);
    }

    void inheritAndFlag1(AstNode* who, uint64_t flag)
    {
        for (auto child : who->childs)
        {
            if (!(child->flags & flag))
                return;
        }

        flags |= flag;
    }

    void inheritAndFlag2(AstNode* who, uint64_t flag1, uint64_t flag2)
    {
        flags |= flag1;
        flags |= flag2;

        for (auto child : who->childs)
        {
            if (!(child->flags & flag1))
                flags &= ~flag1;
            if (!(child->flags & flag2))
                flags &= ~flag2;
            if (!(flags & (flag1 | flag2)))
                return;
        }
    }

    void inheritAndFlag3(AstNode* who, uint64_t flag1, uint64_t flag2, uint64_t flag3)
    {
        flags |= flag1;
        flags |= flag2;
        flags |= flag3;

        for (auto child : who->childs)
        {
            if (!(child->flags & flag1))
                flags &= ~flag1;
            if (!(child->flags & flag2))
                flags &= ~flag2;
            if (!(child->flags & flag3))
                flags &= ~flag3;
            if (!(flags & (flag1 | flag2 | flag3)))
                return;
        }
    }

    void setFlagsValueIsComputed()
    {
        flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED | AST_PURE | AST_R_VALUE;
    }

    void inheritComputedValue(AstNode* from)
    {
        if (!from)
            return;
        inheritOrFlag(from, AST_VALUE_COMPUTED | AST_VALUE_IS_TYPEINFO);
        if (flags & AST_VALUE_COMPUTED)
        {
            flags |= AST_CONST_EXPR | AST_PURE | AST_R_VALUE;
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
        attributeFlags |= job->currentAttributeFlags;
    }

    bool hasComputedValue()
    {
        return (flags & AST_VALUE_COMPUTED);
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
    void             cloneChilds(CloneContext& context, AstNode* from);
    void             copyFrom(CloneContext& context, AstNode* from, bool cloneHie = true);
    void             computeScopedNameNoLock();
    Utf8             computeScopedName();

    SWAG_RACE_CONDITION_INSTANCE(raceConditionAlternativeScopes);

    VectorNative<Scope*>     alternativeScopes;
    vector<AlternativeScope> alternativeScopesVars;
    VectorNative<AstNode*>   childs;
    set<Scope*>              doneLeaveScopeDefer;
    set<Scope*>              doneLeaveScopeDrop;

    Token         token;
    Utf8Crc       name;
    Utf8          scopedName1;
    shared_mutex  mutex;
    ComputedValue computedValue;
    RegisterList  resultRegisterRC;
    RegisterList  additionalRegisterRC;

    DocContent*         docContent                   = nullptr;
    Scope*              ownerScope                   = nullptr;
    AstBreakable*       ownerBreakable               = nullptr;
    AstInline*          ownerInline                  = nullptr;
    AstFuncDecl*        ownerFct                     = nullptr;
    Scope*              ownerStructScope             = nullptr;
    AstNode*            ownerMainNode                = nullptr;
    AstCompilerIfBlock* ownerCompilerIfBlock         = nullptr;
    TypeInfo*           typeInfo                     = nullptr;
    TypeInfo*           castedTypeInfo               = nullptr;
    SymbolName*         resolvedSymbolName           = nullptr;
    SymbolOverload*     resolvedSymbolOverload       = nullptr;
    SymbolName*         resolvedUserOpSymbolName     = nullptr;
    SymbolOverload*     resolvedUserOpSymbolOverload = nullptr;
    ByteCodeGenJob*     byteCodeJob                  = nullptr;
    TypeInfo*           concreteTypeInfo             = nullptr;
    AstNode*            parent                       = nullptr;
    AstAttrUse*         parentAttributes             = nullptr;
    SemanticFct         semanticFct                  = nullptr;
    SemanticFct         semanticBeforeFct            = nullptr;
    SemanticFct         semanticAfterFct             = nullptr;
    ByteCodeFct         byteCodeFct                  = nullptr;
    ByteCodeNotifyFct   byteCodeBeforeFct            = nullptr;
    ByteCodeNotifyFct   byteCodeAfterFct             = nullptr;
    SourceFile*         sourceFile                   = nullptr;
    ByteCode*           bc                           = nullptr;

    uint64_t flags = 0;

    uint32_t doneFlags               = 0;
    uint32_t fctCallStorageOffset    = 0;
    uint32_t castOffset              = 0;
    uint32_t concreteTypeInfoStorage = UINT32_MAX;
    uint32_t childParentIdx          = 0;
    uint32_t attributeFlags          = 0;

    AstNodeResolveState semanticState = AstNodeResolveState::Enter;
    AstNodeResolveState bytecodeState = AstNodeResolveState::Enter;
    AstNodeKind         kind          = AstNodeKind::Invalid;
};

struct AstVarDecl : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstNode* type       = nullptr;
    AstNode* assignment = nullptr;
};

struct AstIdentifierRef : public AstNode
{
    AstNode* clone(CloneContext& context) override;
    void     computeName();

    Scope*   startScope           = nullptr;
    AstNode* previousResolvedNode = nullptr;
};

struct AstIdentifier : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    vector<Utf8> aliasNames;

    AstIdentifierRef* identifierRef     = nullptr;
    AstNode*          genericParameters = nullptr;
    AstNode*          callParameters    = nullptr;
};

struct AstFuncDecl : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    SymbolAttributes collectAttributes;
    DependentJobs    dependentJobs;

    Token          endToken;
    AstNode*       parameters        = nullptr;
    AstNode*       genericParameters = nullptr;
    AstNode*       returnType        = nullptr;
    AstNode*       content           = nullptr;
    Scope*         scope             = nullptr;
    TypeInfoParam* methodParam       = nullptr;
    Job*           pendingLambdaJob  = nullptr;

    uint32_t stackSize = 0;

    Utf8 fullnameForeign;
    void computeFullNameForeign(bool forExport);
};

struct AstAttrDecl : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstNode* parameters = nullptr;
};

struct AstAttrUse : public AstNode
{
    AstNode*         clone(CloneContext& context) override;
    SymbolAttributes attributes;
};

struct AstFuncCallParam : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    Utf8 namedParam;

    AstNode*       namedParamNode    = nullptr;
    TypeInfoParam* resolvedParameter = nullptr;

    int  index              = 0;
    bool mustSortParameters = false;
};

struct AstIf : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstNode* boolExpression = nullptr;
    AstNode* ifBlock        = nullptr;
    AstNode* elseBlock      = nullptr;

    int seekJumpExpression = 0;
    int seekJumpAfterIf    = 0;
};

struct AstBreakContinue : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    Utf8 label;

    AstSwitchCase* switchCase = nullptr; // The corresponding case, if inside a switch

    int jumpInstruction = 0;
};

const uint32_t BREAKABLE_CAN_HAVE_INDEX    = 0x00000001;
const uint32_t BREAKABLE_CAN_HAVE_CONTINUE = 0x00000002;
const uint32_t BREAKABLE_NEED_INDEX        = 0x00000004;

struct AstBreakable : public AstNode
{
    bool needIndex()
    {
        return breakableFlags & BREAKABLE_NEED_INDEX;
    }

    void copyFrom(CloneContext& context, AstBreakable* from);

    VectorNative<AstBreakContinue*> breakList;
    VectorNative<AstBreakContinue*> continueList;
    VectorNative<AstBreakContinue*> fallThroughList;

    uint32_t breakableFlags           = BREAKABLE_CAN_HAVE_INDEX | BREAKABLE_CAN_HAVE_CONTINUE;
    uint32_t registerIndex            = 0;
    int      seekJumpBeforeContinue   = 0;
    int      seekJumpBeforeExpression = 0;
    int      seekJumpExpression       = 0;
    int      seekJumpAfterBlock       = 0;
};

struct AstLabelBreakable : public AstBreakable
{
    AstNode* clone(CloneContext& context) override;

    AstNode* block = nullptr;
};

struct AstWhile : public AstBreakable
{
    AstNode* clone(CloneContext& context) override;

    AstNode* boolExpression = nullptr;
    AstNode* block          = nullptr;
};

struct AstFor : public AstBreakable
{
    AstNode* clone(CloneContext& context) override;

    AstNode* preExpression  = nullptr;
    AstNode* boolExpression = nullptr;
    AstNode* postExpression = nullptr;
    AstNode* block          = nullptr;

    int seekJumpToBlock    = 0;
    int seekJumpBeforePost = 0;
};

struct AstLoop : public AstBreakable
{
    AstNode* clone(CloneContext& context) override;

    AstNode* specificName = nullptr;
    AstNode* expression   = nullptr;
    AstNode* block        = nullptr;
};

struct AstVisit : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    vector<Utf8> aliasNames;
    Token        extraNameToken;
    Token        wantPointerToken;

    AstNode* expression = nullptr;
    AstNode* block      = nullptr;

    bool wantPointer      = false;
    bool wantConstPointer = false;
};

struct AstSwitch : public AstBreakable
{
    AstSwitch()
    {
        breakableFlags &= ~BREAKABLE_CAN_HAVE_INDEX;
        breakableFlags &= ~BREAKABLE_CAN_HAVE_CONTINUE;
    }

    AstNode* clone(CloneContext& context) override;

    VectorNative<AstSwitchCase*> cases;

    AstNode* expression = nullptr;
    AstNode* block      = nullptr;
};

struct AstSwitchCase : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    VectorNative<AstNode*> expressions;

    AstNode*   block       = nullptr;
    AstSwitch* ownerSwitch = nullptr;

    int caseIndex = 0;

    bool isDefault = false;
};

struct AstSwitchCaseBlock : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstSwitchCase* ownerCase = nullptr;

    int seekStart        = 0;
    int seekJumpNextCase = 0;

    bool isDefault = false;
};

struct AstTypeExpression : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstNode* identifier = nullptr;

    int ptrCount = 0;
    int arrayDim = 0;

    bool isRef          = false;
    bool isSlice        = false;
    bool isConst        = false;
    bool isTypeOf       = false;
    bool isCode         = false;
    bool forceConstType = false;
    bool isSelf         = false;
};

struct AstTypeLambda : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstNode* parameters = nullptr;
    AstNode* returnType = nullptr;
};

struct AstArrayPointerIndex : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    VectorNative<AstNode*> structFlatParams;

    AstNode* array  = nullptr;
    AstNode* access = nullptr;
};

struct AstArrayPointerSlicing : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    VectorNative<AstNode*> structFlatParams;

    AstNode* array      = nullptr;
    AstNode* lowerBound = nullptr;
    AstNode* upperBound = nullptr;
};

struct AstProperty : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    bool typeOfAsType  = false;
    bool typeOfAsConst = false;
};

struct AstExpressionList : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    uint32_t storageOffset        = 0;
    uint32_t storageOffsetSegment = UINT32_MAX;
    bool     forTuple             = false;
};

struct AstStruct : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    DependentJobs dependentJobs;

    AstNode* genericParameters = nullptr;
    AstNode* content           = nullptr;
    Scope*   scope             = nullptr;

    uint32_t packing = sizeof(uint64_t);
};

struct AstImpl : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    Scope*   structScope   = nullptr;
    AstNode* identifier    = nullptr;
    AstNode* identifierFor = nullptr;
};

struct AstInit : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstNode* expression = nullptr;
    AstNode* count      = nullptr;
    AstNode* parameters = nullptr;
};

struct AstDrop : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstNode* expression = nullptr;
    AstNode* count      = nullptr;
};

struct AstReturn : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    SymbolOverload* forceNoDrop = nullptr;
    int             seekJump    = 0;
};

struct AstCompilerInline : public AstNode
{
    AstNode* clone(CloneContext& context) override;
};

struct AstCompilerMacro : public AstNode
{
    AstNode* clone(CloneContext& context) override;
};

struct AstCompilerMixin : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    map<TokenId, AstNode*> replaceTokens;
};

struct AstInline : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    VectorNative<AstReturn*> returnList;

    AstFuncDecl* func  = nullptr;
    Scope*       scope = nullptr;
};

struct AstCompilerIfBlock : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    void addSymbol(SymbolName* symbolName)
    {
        scoped_lock lk(mutex);
        symbols.push_back(symbolName);
    }

    VectorNative<AstCompilerIfBlock*>        blocks;
    VectorNative<SymbolName*>                symbols;
    VectorNative<TypeInfoStruct*>            interfacesCount;
    VectorNative<pair<TypeInfoStruct*, int>> methodsCount;
};

enum class CompilerAstKind
{
    EmbeddedInstruction,
    TopLevelInstruction,
    StructVarDecl,
    EnumValue,
};

struct AstCompilerAst : public AstNode
{
    AstNode*        clone(CloneContext& context) override;
    CompilerAstKind embeddedKind = CompilerAstKind::EmbeddedInstruction;
};
