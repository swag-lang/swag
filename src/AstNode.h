#pragma once
#include "DependentJobs.h"
#include "Utf8.h"
#include "Tokenizer.h"
#include "SyntaxJob.h"
#include "Register.h"
#include "AstFlags.h"
#include "RegisterList.h"
#include "SymTable.h"
#include "Attribute.h"
#include "Scope.h"
#include "VectorNative.h"
#include "Mutex.h"

struct AstTypeExpression;
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
struct AstReturn;
struct AstCompilerIfBlock;
struct AstFuncCallParams;
struct AstMakePointer;
struct TypeInfoFuncAttr;
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
    IdentifierRef,
    Identifier,
    TypeExpression,
    TypeLambda,
    TypeClosure,
    Namespace,
    Using,
    With,
    Alias,
    AliasImport,
    If,
    While,
    For,
    Loop,
    Range,
    Visit,
    Switch,
    SwitchCase,
    SwitchCaseBlock,
    SubstBreakContinue,
    Break,
    FallThrough,
    Continue,
    Statement,
    StatementNoScope,
    ScopeBreakable,
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
    FuncCallParams,
    FuncCallParam,
    FuncCall,
    FuncContent,
    Return,
    RetVal,
    EnumType,
    EnumValue,
    Literal,
    SizeOf,
    IntrinsicProp,
    Index,
    GetErr,
    AutoCast,
    Cast,
    BitCast,
    TypeList,
    SingleOp,
    MakePointer,
    MakePointerLambda,
    BinaryOp,
    FactorOp,
    ExpressionList,
    ExplicitNoInit,
    MultiIdentifier,
    MultiIdentifierTuple,
    AffectOp,
    ArrayPointerIndex,
    ArrayPointerSlicing,
    NoDrop,
    Move,
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
    CompilerSelectIf,
    CompilerCheckIf,
    CompilerCode,
    CompilerDependencies,
    CompilerImport,
    CompilerInclude,
    CompilerPlaceHolder,
    CompilerSpecialFunction,
    CompilerForeignLib,
    IntrinsicDefined,
    CompilerLoad,
    ConditionalExpression,
    NullConditionalExpression,
    Defer,
    Init,
    Drop,
    PostCopy,
    PostMove,
    Try,
    Catch,
    TryCatch,
    Assume,
    Throw,
    KeepRef,
};

static const uint32_t CLONE_RAW             = 0x00000001;
static const uint32_t CLONE_FORCE_OWNER_FCT = 0x00000002;

struct CloneUpdateRef
{
    AstNode*  node;
    AstNode** ref;
};

struct CloneContext
{
    map<Utf8, TypeInfo*>         replaceTypes;
    map<TokenId, AstNode*>       replaceTokens;
    map<Utf8, Utf8>              replaceNames;
    set<Utf8>                    usedReplaceNames;
    VectorNative<CloneUpdateRef> nodeRefsToUpdate;

    AstInline*          ownerInline            = nullptr;
    AstBreakable*       replaceTokensBreakable = nullptr;
    AstBreakable*       ownerBreakable         = nullptr;
    AstFuncDecl*        ownerFct               = nullptr;
    AstTryCatchAssume*  ownerTryCatchAssume    = nullptr;
    AstNode*            parent                 = nullptr;
    Scope*              parentScope            = nullptr;
    Scope*              ownerStructScope       = nullptr;
    Scope*              alternativeScope       = nullptr;
    Scope*              ownerDeferScope        = nullptr;
    AstCompilerIfBlock* ownerCompilerIfBlock   = nullptr;
    Token*              forceLocation          = nullptr;
    uint64_t            forceFlags             = 0;
    uint64_t            removeFlags            = 0;
    uint64_t            forceSemFlags          = 0;
    uint32_t            cloneFlags             = 0;

    void propageResult(CloneContext& context)
    {
        usedReplaceNames.insert(context.usedReplaceNames.begin(), context.usedReplaceNames.end());
    }
};

static const uint32_t ALTSCOPE_USING     = 0x00000001;
static const uint32_t ALTSCOPE_SCOPEFILE = 0x00000002;

struct AlternativeScope
{
    Scope*   scope;
    uint32_t flags;
};

struct AlternativeScopeVar
{
    AstNode* node;
    AstNode* leafNode;
    Scope*   scope;
    uint32_t flags;
};

enum class ExtensionKind
{
    ByteCode,
    Semantic,
    Owner,
    AltScopes,
    AdditionalRegs,
    Resolve,
    StackSize,
    ExportNode,
    Collect,
    Any,
    Misc,
};

struct AstNode
{
    AstNode* clone(CloneContext& context);
    void     cloneChilds(CloneContext& context, AstNode* from);
    void     copyFrom(CloneContext& context, AstNode* from, bool cloneHie = true);

    void inheritOrFlag(uint64_t flag);
    void inheritOrFlag(AstNode* op, uint64_t flag);
    void inheritAndFlag1(uint64_t flag);
    void inheritAndFlag2(uint64_t flag1, uint64_t flag2);
    void inheritAndFlag3(uint64_t flag1, uint64_t flag2, uint64_t flag3);
    void inheritAndFlag1(AstNode* who, uint64_t flag);
    void inheritAndFlag2(AstNode* who, uint64_t flag1, uint64_t flag2);
    void inheritAndFlag3(AstNode* who, uint64_t flag1, uint64_t flag2, uint64_t flag3);
    void inheritTokenName(Token& tkn);
    void inheritTokenLocation(Token& tkn);
    void inheritTokenLocation(AstNode* node);
    void inheritOwners(AstNode* op);
    void inheritOwnersAndFlags(SyntaxJob* job);

    void allocateComputedValue();
    void setFlagsValueIsComputed();
    void inheritComputedValue(AstNode* from);

    bool isConstantTrue();
    bool isConstantFalse();
    bool isConstant0();
    bool isConstant1();
    bool isParentOf(AstNode* child);
    bool isSelectIfParam(SymbolOverload* overload);
    bool isSameStackFrame(SymbolOverload* overload);
    bool isSpecialFunctionName();
    bool isSpecialFunctionGenerated();
    bool isPublic();

    void        setPassThrough();
    static Utf8 getArticleKindName(AstNode* node);
    static Utf8 getKindName(AstNode* node);
    AstNode*    findChildRef(AstNode* ref, AstNode* fromChild);
    AstNode*    findChildRefRec(AstNode* ref, AstNode* fromChild);
    AstNode*    findParent(AstNodeKind parentKind);
    AstNode*    findParent(AstNodeKind parentKind1, AstNodeKind parentKind2);
    Utf8        getScopedName();
    void        setOwnerAttrUse(AstAttrUse* attrUse);
    void        allocateExtension(ExtensionKind extensionKind);
    void        swap2Childs();
    bool        hasSpecialFuncCall();
    bool        hasSpecialFuncCall(const Utf8& name);
    bool        mustInline();
    AstNode*    inSimpleReturn();
    bool        forceTakeAddress();
    void        computeEndLocation();
    void        addAlternativeScope(Scope* scope, uint32_t altFlags = 0);
    void        addAlternativeScopeVar(Scope* scope, AstNode* varNode, uint32_t altFlags = 0);
    void        addAlternativeScopes(const VectorNative<AlternativeScope>& scopes);

    struct ExtensionByteCode
    {
        ByteCodeNotifyFct      byteCodeBeforeFct = nullptr;
        ByteCodeNotifyFct      byteCodeAfterFct  = nullptr;
        ByteCodeGenJob*        byteCodeJob       = nullptr;
        ByteCode*              bc                = nullptr;
        VectorNative<AstNode*> dependentNodes;
    };

    struct ExtensionSemantic
    {
        SemanticFct semanticBeforeFct = nullptr;
        SemanticFct semanticAfterFct  = nullptr;
    };

    struct ExtensionOwner
    {
        AstAttrUse*        ownerAttrUse        = nullptr;
        AstTryCatchAssume* ownerTryCatchAssume = nullptr;
    };

    struct ExtensionMisc
    {
        SharedMutex                       mutexAltScopes;
        VectorNative<AlternativeScope>    alternativeScopes;
        VectorNative<AlternativeScopeVar> alternativeScopesVars;
        VectorNative<uint32_t>            registersToRelease;
        RegisterList                      additionalRegisterRC;

        SymbolOverload* resolvedUserOpSymbolOverload = nullptr;
        TypeInfo*       collectTypeInfo              = nullptr;
        AstNode*        alternativeNode              = nullptr;
        AstNode*        exportNode                   = nullptr;
        DataSegment*    anyTypeSegment               = nullptr;
        TypeInfoParam*  castItf                      = nullptr;

        uint32_t castOffset    = 0;
        uint32_t stackOffset   = 0;
        uint32_t anyTypeOffset = 0;
        uint32_t stackSize     = 0;
    };

    struct Extension
    {
        ExtensionByteCode* bytecode = nullptr;
        ExtensionSemantic* semantic = nullptr;
        ExtensionOwner*    owner    = nullptr;
        ExtensionMisc*     misc     = nullptr;
    };

    AstNodeKind         kind           = (AstNodeKind) 0;
    AstNodeResolveState semanticState  = (AstNodeResolveState) 0;
    AstNodeResolveState bytecodeState  = (AstNodeResolveState) 0;
    uint8_t             specFlags      = 0;
    uint32_t            childParentIdx = 0;

    SharedMutex            mutex;
    Token                  token;
    VectorNative<AstNode*> childs;
    ComputedValue*         computedValue = nullptr;

    Scope*              ownerScope           = nullptr;
    Scope*              ownerStructScope     = nullptr;
    AstBreakable*       ownerBreakable       = nullptr;
    AstInline*          ownerInline          = nullptr;
    AstFuncDecl*        ownerFct             = nullptr;
    AstCompilerIfBlock* ownerCompilerIfBlock = nullptr;

    TypeInfo* typeInfo       = nullptr;
    TypeInfo* castedTypeInfo = nullptr;

    SymbolName*     resolvedSymbolName     = nullptr;
    SymbolOverload* resolvedSymbolOverload = nullptr;

    AstNode*    parent     = nullptr;
    SourceFile* sourceFile = nullptr;
    Extension*  extension  = nullptr;

    SemanticFct semanticFct = nullptr;
    ByteCodeFct byteCodeFct = nullptr;

    uint64_t flags          = 0;
    uint64_t attributeFlags = 0;

    RegisterList resultRegisterRC;

    uint32_t doneFlags = 0;
    uint32_t semFlags  = 0;
};

struct AstVarDecl : public AstNode
{
    AstNode* clone(CloneContext& context);

    AttributeList attributes;
    Utf8          publicName;
    Token         assignToken;

    AstNode* type       = nullptr;
    AstNode* assignment = nullptr;
};

struct AstIdentifierRef : public AstNode
{
    AstNode* clone(CloneContext& context);
    void     computeName();

    Scope*   startScope           = nullptr;
    AstNode* previousResolvedNode = nullptr;
};

struct AstIdentifier : public AstNode
{
    AstNode* clone(CloneContext& context);

    vector<Token> aliasNames;

    AstIdentifierRef*  identifierRef     = nullptr;
    AstNode*           genericParameters = nullptr;
    AstFuncCallParams* callParameters    = nullptr;
    TypeInfo*          alternateEnum     = nullptr;
};

static const uint32_t FUNC_FLAG_FULL_RESOLVE    = 0x00000001;
static const uint32_t FUNC_FLAG_PARTIAL_RESOLVE = 0x00000002;

struct AstFuncDecl : public AstNode
{
    AstNode*    clone(CloneContext& context);
    bool        cloneSubDecls(JobContext* context, CloneContext& cloneContext, AstNode* oldOwnerNode, AstFuncDecl* newFctNode, AstNode* refNode);
    void        computeFullNameForeign(bool forExport);
    Utf8        getDisplayName();
    const char* getDisplayNameC();
    Utf8        getNameForUserCompiler();
    Utf8        getCallName();

    bool isForeign()
    {
        return attributeFlags & ATTRIBUTE_FOREIGN;
    }

    DependentJobs          dependentJobs;
    Utf8                   fullnameForeign;
    VectorNative<AstNode*> subDecls;
    VectorNative<AstNode*> localGlobalVars;
    VectorNative<AstNode*> localConstants;
    Mutex                  funcMutex;
    Token                  tokenName;

    AstNode*        captureParameters     = nullptr;
    AstNode*        parameters            = nullptr;
    AstNode*        genericParameters     = nullptr;
    AstNode*        returnType            = nullptr;
    AstNode*        content               = nullptr;
    AstNode*        selectIf              = nullptr;
    AstNode*        returnTypeDeducedNode = nullptr;
    AstNode*        originalGeneric       = nullptr;
    Scope*          scope                 = nullptr;
    TypeInfoParam*  methodParam           = nullptr;
    Job*            pendingLambdaJob      = nullptr;
    AstMakePointer* makePointerLambda     = nullptr;

    uint32_t aliasMask              = 0;
    uint32_t stackSize              = 0;
    uint32_t nodeCounts             = 0;
    uint32_t funcFlags              = 0;
    uint32_t registerGetContext     = UINT32_MAX;
    bool     needRegisterGetContext = false;
    bool     hasSpecMixin           = false;
};

struct AstAttrDecl : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode* parameters = nullptr;
};

struct AstAttrUse : public AstNode
{
    AstNode* clone(CloneContext& context);

    AttributeList attributes;

    AstNode* content = nullptr;
};

struct AstFuncCallParams : public AstNode
{
    AstNode* clone(CloneContext& context);

    vector<Token> aliasNames;
};

struct AstFuncCallParam : public AstNode
{
    AstNode* clone(CloneContext& context);

    Utf8 namedParam;

    AstReturn*     autoTupleReturn   = nullptr;
    AstNode*       namedParamNode    = nullptr;
    TypeInfoParam* resolvedParameter = nullptr;

    int indexParam = 0;
};

struct AstBinaryOpNode : public AstNode
{
    AstNode* clone(CloneContext& context);

    int seekJumpExpression = 0;
};

struct AstConditionalOpNode : public AstNode
{
    AstNode* clone(CloneContext& context);

    int seekJumpIfFalse      = 0;
    int seekJumpAfterIfFalse = 0;
};

struct AstIf : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode*            boolExpression = nullptr;
    AstCompilerIfBlock* ifBlock        = nullptr;
    AstCompilerIfBlock* elseBlock      = nullptr;

    int seekJumpExpression = 0;
    int seekJumpAfterIf    = 0;
};

struct AstBreakContinue : public AstNode
{
    AstNode* clone(CloneContext& context);

    Token label;

    AstSwitchCase* switchCase = nullptr;

    int jumpInstruction = 0;
};

const uint32_t BREAKABLE_CAN_HAVE_INDEX         = 0x00000001;
const uint32_t BREAKABLE_CAN_HAVE_CONTINUE      = 0x00000002;
const uint32_t BREAKABLE_NEED_INDEX             = 0x00000004;
const uint32_t BREAKABLE_NEED_INDEX1            = 0x00000008;
const uint32_t BREAKABLE_RETURN_IN_INFINIT_LOOP = 0x00000010;

struct AstBreakable : public AstNode
{
    bool needIndex()
    {
        return breakableFlags & BREAKABLE_NEED_INDEX;
    }

    bool needIndex1()
    {
        return breakableFlags & BREAKABLE_NEED_INDEX1;
    }

    void copyFrom(CloneContext& context, AstBreakable* from);

    VectorNative<AstBreakContinue*> breakList;
    VectorNative<AstBreakContinue*> continueList;
    VectorNative<AstBreakContinue*> fallThroughList;

    uint32_t breakableFlags           = BREAKABLE_CAN_HAVE_INDEX | BREAKABLE_CAN_HAVE_CONTINUE;
    uint32_t registerIndex            = 0;
    uint32_t registerIndex1           = 0;
    int      seekJumpBeforeContinue   = 0;
    int      seekJumpBeforeExpression = 0;
    int      seekJumpExpression       = 0;
    int      seekJumpAfterBlock       = 0;
};

struct AstScopeBreakable : public AstBreakable
{
    AstScopeBreakable()
    {
        breakableFlags &= ~BREAKABLE_CAN_HAVE_INDEX;
    }

    AstNode* clone(CloneContext& context);

    AstNode* block = nullptr;
};

struct AstWhile : public AstBreakable
{
    AstNode* clone(CloneContext& context);

    AstNode* boolExpression = nullptr;
    AstNode* block          = nullptr;
};

struct AstFor : public AstBreakable
{
    AstNode* clone(CloneContext& context);

    AstNode* preExpression  = nullptr;
    AstNode* boolExpression = nullptr;
    AstNode* postExpression = nullptr;
    AstNode* block          = nullptr;

    int seekJumpToExpression = 0;
};

struct AstLoop : public AstBreakable
{
    AstNode* clone(CloneContext& context);

    AstNode* specificName = nullptr;
    AstNode* expression   = nullptr;
    AstNode* block        = nullptr;
};

struct AstVisit : public AstNode
{
    AstNode* clone(CloneContext& context);

    vector<Token> aliasNames;
    Token         extraNameToken;
    Token         wantPointerToken;

    AstNode* expression = nullptr;
    AstNode* block      = nullptr;
};

struct AstSwitch : public AstBreakable
{
    AstSwitch()
    {
        breakableFlags &= ~BREAKABLE_CAN_HAVE_INDEX;
        breakableFlags &= ~BREAKABLE_CAN_HAVE_CONTINUE;
    }

    AstNode* clone(CloneContext& context);

    VectorNative<AstSwitchCase*> cases;

    AstNode*  expression         = nullptr;
    TypeInfo* beforeAutoCastType = nullptr;
};

struct AstSwitchCase : public AstNode
{
    AstNode* clone(CloneContext& context);

    VectorNative<AstNode*> expressions;

    AstNode*   block       = nullptr;
    AstSwitch* ownerSwitch = nullptr;

    int caseIndex = 0;
};

struct AstSwitchCaseBlock : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstSwitchCase* ownerCase = nullptr;

    int seekStart        = 0;
    int seekJumpNextCase = 0;
};

const uint16_t TYPEFLAG_ISREF          = 0x0001;
const uint16_t TYPEFLAG_IS_SLICE       = 0x0002;
const uint16_t TYPEFLAG_IS_CONST       = 0x0004;
const uint16_t TYPEFLAG_IS_CODE        = 0x0008;
const uint16_t TYPEFLAG_FORCE_CONST    = 0x0010;
const uint16_t TYPEFLAG_IS_SELF        = 0x0020;
const uint16_t TYPEFLAG_RETVAL         = 0x0040;
const uint16_t TYPEFLAG_USING          = 0x0080;
const uint16_t TYPEFLAG_IS_CONST_SLICE = 0x0100;
const uint16_t TYPEFLAG_IS_REF         = 0x0200;

struct AstTypeExpression : public AstNode
{
    static const int     MAX_PTR_COUNT = 4;
    static const uint8_t PTR_CONST     = 0x01;
    static const uint8_t PTR_ARITMETIC = 0x02;

    AstNode* clone(CloneContext& context);

    AstNode*  identifier      = nullptr;
    TypeInfo* typeFromLiteral = nullptr;

    uint16_t typeFlags               = 0;
    uint8_t  ptrFlags[MAX_PTR_COUNT] = {0};
    uint8_t  ptrCount                = 0;
    uint8_t  arrayDim                = 0;
};

struct AstTypeLambda : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode* parameters = nullptr;
    AstNode* returnType = nullptr;
};

struct AstArrayPointerIndex : public AstNode
{
    AstNode* clone(CloneContext& context);

    VectorNative<AstNode*> structFlatParams;

    AstNode* array  = nullptr;
    AstNode* access = nullptr;
};

struct AstArrayPointerSlicing : public AstNode
{
    AstNode* clone(CloneContext& context);

    VectorNative<AstNode*> structFlatParams;

    AstNode* array      = nullptr;
    AstNode* lowerBound = nullptr;
    AstNode* upperBound = nullptr;
};

struct AstIntrinsicProp : public AstNode
{
};

struct AstExpressionList : public AstNode
{
};

const uint32_t STRUCTFLAG_UNION     = 0x00000001;
const uint32_t STRUCTFLAG_ANONYMOUS = 0x00000002;

struct AstStruct : public AstNode
{
    AstNode* clone(CloneContext& context);

    DependentJobs dependentJobs;

    AstNode* genericParameters = nullptr;
    AstNode* content           = nullptr;
    Scope*   scope             = nullptr;
    AstNode* ownerGeneric      = nullptr;
    AstNode* originalParent    = nullptr;

    uint32_t packing     = sizeof(uint64_t);
    uint32_t structFlags = 0;
};

struct AstEnum : public AstNode
{
    AstNode* clone(CloneContext& context);

    Scope* scope = nullptr;
};

struct AstEnumValue : public AstNode
{
    AstNode* clone(CloneContext& context);

    AttributeList attributes;
};

struct AstImpl : public AstNode
{
    AstNode* clone(CloneContext& context);

    Scope*   structScope   = nullptr;
    Scope*   scope         = nullptr;
    AstNode* identifier    = nullptr;
    AstNode* identifierFor = nullptr;
};

struct AstInit : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode*           expression = nullptr;
    AstNode*           count      = nullptr;
    AstFuncCallParams* parameters = nullptr;
};

struct AstDropCopyMove : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode* expression = nullptr;
    AstNode* count      = nullptr;
};

struct AstReturn : public AstNode
{
    AstNode* clone(CloneContext& context);

    VectorNative<SymbolOverload*> forceNoDrop;

    AstFuncDecl* resolvedFuncDecl = nullptr;
    int          seekJump         = 0;
};

struct AstCompilerInline : public AstNode
{
    AstNode* clone(CloneContext& context);

    Scope* scope = nullptr;
};

struct AstCompilerMacro : public AstNode
{
    AstNode* clone(CloneContext& context);

    Scope* scope = nullptr;
};

struct AstCompilerMixin : public AstNode
{
    AstNode* clone(CloneContext& context);

    map<TokenId, AstNode*> replaceTokens;
};

struct AstInline : public AstNode
{
    AstNode* clone(CloneContext& context);

    VectorNative<AstReturn*> returnList;

    AstFuncDecl* func            = nullptr;
    Scope*       scope           = nullptr;
    Scope*       parametersScope = nullptr;
};

struct AstCompilerIfBlock : public AstNode
{
    AstNode* clone(CloneContext& context);

    void addSymbol(AstNode* node, SymbolName* symbolName)
    {
        ScopedLock lk(mutex);
        symbols.push_back({node, symbolName});
    }

    VectorNative<AstCompilerIfBlock*>         blocks;
    VectorNative<pair<AstNode*, SymbolName*>> symbols;
    VectorNative<TypeInfoStruct*>             interfacesCount;

    struct MethodCount
    {
        AstFuncDecl*    funcNode;
        TypeInfoStruct* typeInfo;
        int             methodIdx;
    };
    VectorNative<MethodCount> methodsCount;
    VectorNative<AstNode*>    subDecls;
    VectorNative<AstNode*>    imports;
    VectorNative<AstNode*>    includes;

    int numTestErrors   = 0;
    int numTestWarnings = 0;
};

enum class CompilerAstKind
{
    EmbeddedInstruction,
    TopLevelInstruction,
    StructVarDecl,
    EnumValue,
    MissingInterfaceMtd,
    Expression,
};

struct AstCompilerSpecFunc : public AstNode
{
    AstNode* clone(CloneContext& context);
};

struct AstNameSpace : public AstNode
{
    AstNode* clone(CloneContext& context);
};

struct AstTryCatchAssume : public AstReturn
{
    AstNode* clone(CloneContext& context);

    RegisterList regInit;
    int          seekInsideJump = 0;
};

struct AstAlias : public AstNode
{
    AstNode* clone(CloneContext& context);
};

struct AstCast : public AstNode
{
    AstNode* clone(CloneContext& context);

    TypeInfo* toCastTypeInfo = nullptr;
};

struct AstOp : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode*          dependentLambda   = nullptr;
    TypeInfoFuncAttr* deducedLambdaType = nullptr;
    TypeInfoFuncAttr* tryLambdaType     = nullptr;
};

struct AstRange : public AstNode
{
    AstNode* clone(CloneContext& context);
    AstNode* expressionLow = nullptr;
    AstNode* expressionUp  = nullptr;
};

struct AstMakePointer : public AstNode
{
    AstNode*     clone(CloneContext& context);
    AstFuncDecl* lambda = nullptr;
};

struct AstSubstBreakContinue : public AstNode
{
    AstNode*      clone(CloneContext& context);
    AstNode*      defaultSubst      = nullptr;
    AstNode*      altSubst          = nullptr;
    AstBreakable* altSubstBreakable = nullptr;
};

enum class DeferKind
{
    Normal,
    Error,
    NoError,
};

struct AstDefer : public AstNode
{
    AstNode*  clone(CloneContext& context);
    DeferKind deferKind = DeferKind::Normal;
};

struct AstWith : public AstNode
{
    AstNode*     clone(CloneContext& context);
    vector<Utf8> id;
};