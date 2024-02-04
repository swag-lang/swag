#pragma once
#include "Attribute.h"
#include "DependentJobs.h"
#include "Mutex.h"
#include "Register.h"
#include "Tokenizer.h"

struct AstAttrUse;
struct AstBreakable;
struct AstCompilerIfBlock;
struct AstFuncCallParams;
struct AstFuncDecl;
struct AstInline;
struct AstMakePointer;
struct AstReturn;
struct AstStruct;
struct AstSwitch;
struct AstSwitchCase;
struct AstTryCatchAssume;
struct AstTypeExpression;
struct ByteCode;
struct ByteCodeGenContext;
struct ByteCodeGenJob;
struct DocContent;
struct ExportedTypeInfo;
struct GenericReplaceType;
struct Job;
struct Parser;
struct Scope;
struct SemanticContext;
struct SymbolName;
struct SymbolOverload;
struct TypeInfo;
struct TypeInfoFuncAttr;
struct TypeInfoParam;
struct TypeInfoStruct;

typedef bool (*SemanticFct)(SemanticContext* context);
typedef bool (*ByteCodeFct)(ByteCodeGenContext* context);
typedef bool (*ByteCodeNotifyFct)(ByteCodeGenContext* context);

constexpr uint32_t CLONE_RAW             = 0x00000001;
constexpr uint32_t CLONE_FORCE_OWNER_FCT = 0x00000002;

constexpr uint32_t ALTSCOPE_STRUCT_USING = 0x00000001;
constexpr uint32_t ALTSCOPE_FILE_PRIV    = 0x00000002;
constexpr uint32_t ALTSCOPE_UFCS         = 0x00000004;
constexpr uint32_t ALTSCOPE_USING        = 0x00000008;

struct CloneUpdateRef
{
    AstNode*  node;
    AstNode** ref;
};

struct CloneContext
{
    VectorMap<Utf8, GenericReplaceType> replaceTypes;
    VectorMap<Utf8, ComputedValue*>     replaceValues;
    VectorMap<Utf8, Utf8>               replaceNames;
    Map<TokenId, AstNode*>              replaceTokens;
    SetUtf8                             usedReplaceNames;
    VectorNative<CloneUpdateRef>        nodeRefsToUpdate;

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
    uint32_t            cloneFlags             = 0;

    void propagateResult(CloneContext& context)
    {
        usedReplaceNames.insert(context.usedReplaceNames.begin(), context.usedReplaceNames.end());
        nodeRefsToUpdate = std::move(context.nodeRefsToUpdate);
    }
};

struct AlternativeScope
{
    Scope*   scope = nullptr;
    uint32_t flags = 0;
};

struct AlternativeScopeVar
{
    AstNode* node     = nullptr;
    AstNode* leafNode = nullptr;
    Scope*   scope    = nullptr;
    uint32_t flags    = 0;
};

enum class IdentifierScopeUpMode : uint8_t
{
    None,
    Count,
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
    TypeAlias,
    NameAlias,
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
    Unreachable,
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
    TypeList,
    SingleOp,
    MakePointer,
    MakePointerLambda,
    RefSubDecl,
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
    CompilerPrint,
    CompilerError,
    CompilerWarning,
    CompilerRun,
    CompilerRunExpression,
    CompilerAst,
    CompilerValidIf,
    CompilerValidIfx,
    CompilerCode,
    CompilerDependencies,
    CompilerImport,
    CompilerLoad,
    CompilerPlaceHolder,
    CompilerSpecialValue,
    CompilerForeignLib,
    IntrinsicDefined,
    CompilerInclude,
    IntrinsicLocation,
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
    MoveRef,
    AutoSlicingUp,
};

enum class AstNodeResolveState : uint8_t
{
    Enter,
    ProcessingChilds,
    PostChilds,
    Done,
};

enum class ExtensionKind
{
    ByteCode,
    Semantic,
    Owner,
    Misc,
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

enum class DeferKind
{
    Normal,
    Error,
    NoError,
};

struct AstNode
{
    template<typename T>
    static AstNode* clone(AstNode* node, CloneContext& context)
    {
        return ((T*) node)->clone(context);
    }

    AstNode* clone(CloneContext& context);
    void     releaseChilds();
    void     release();
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
    void inheritTokenLocation(const Token& tkn);
    void inheritTokenLocation(const AstNode* node);
    void inheritOwners(AstNode* op);
    void inheritOwnersAndFlags(const Parser* parser);

    void              allocateComputedValue();
    void              setFlagsValueIsComputed();
    void              inheritComputedValue(AstNode* from);
    bool              hasComputedValue() const;
    bool              isConstantGenTypeInfo() const;
    ExportedTypeInfo* getConstantGenTypeInfo() const;
    bool              isConstantTrue() const;
    bool              isConstantFalse() const;
    bool              isConstant0() const;
    bool              isConstant1() const;

    bool isParentOf(const AstNode* child) const;
    bool isValidIfParam(const SymbolOverload* overload) const;
    bool isSameStackFrame(const SymbolOverload* overload) const;
    bool isSpecialFunctionName() const;
    bool isSpecialFunctionGenerated();
    bool isFunctionCall();
    bool isGeneratedSelf() const;
    bool isEmptyFct();
    bool isForeign() const;
    bool isSilentCall() const;
    bool isPublic() const;

    AstNode* findChildRef(const AstNode* ref, AstNode* fromChild);
    AstNode* findChildRefRec(AstNode* ref, AstNode* fromChild);
    AstNode* findParent(AstNodeKind parentKind) const;
    AstNode* findParent(AstNodeKind parentKind1, AstNodeKind parentKind2) const;
    AstNode* findChild(AstNodeKind childKind);
    AstNode* findParentAttrUse(const Utf8& name) const;
    AstNode* findParent(TokenId tkn) const;

    const Token& getTokenName();
    Utf8         getScopedName();
    void         setPassThrough();
    void         setOwnerAttrUse(AstAttrUse* attrUse);
    void         swap2Childs();
    bool         hasSpecialFuncCall();
    bool         hasSpecialFuncCall(const Utf8& name);
    AstNode*     inSimpleReturn() const;
    bool         isForceTakeAddress() const;
    void         computeLocation(SourceLocation& start, SourceLocation& end);
    void         addAlternativeScope(Scope* scope, uint32_t altFlags = 0);
    void         addAlternativeScopeVar(Scope* scope, AstNode* varNode, uint32_t altFlags = 0);
    uint32_t     childParentIdx() const;
    void         printLoc() const;

    struct NodeExtensionByteCode
    {
        ByteCodeNotifyFct      byteCodeBeforeFct = nullptr;
        ByteCodeNotifyFct      byteCodeAfterFct  = nullptr;
        ByteCodeGenJob*        byteCodeJob       = nullptr;
        ByteCode*              bc                = nullptr;
        VectorNative<AstNode*> dependentNodes;
    };

    struct NodeExtensionSemantic
    {
        SemanticFct semanticBeforeFct = nullptr;
        SemanticFct semanticAfterFct  = nullptr;
    };

    struct NodeExtensionOwner
    {
        AstAttrUse*            ownerAttrUse         = nullptr;
        AstTryCatchAssume*     ownerTryCatchAssume  = nullptr;
        AstCompilerIfBlock*    ownerCompilerIfBlock = nullptr;
        VectorNative<AstNode*> nodesToFree;
    };

    struct NodeExtensionMisc
    {
        SharedMutex                       mutexAltScopes;
        VectorNative<AlternativeScope>    alternativeScopes;
        VectorNative<AlternativeScopeVar> alternativeScopesVars;
        VectorNative<uint32_t>            registersToRelease;
        RegisterList                      additionalRegisterRC;
        Utf8                              docComment;

        SymbolOverload* resolvedUserOpSymbolOverload = nullptr;
        TypeInfo*       collectTypeInfo              = nullptr;
        AstNode*        alternativeNode              = nullptr;
        AstNode*        exportNode                   = nullptr;
        DataSegment*    anyTypeSegment               = nullptr;
        TypeInfoParam*  castItf                      = nullptr;
        AstNode*        isNamed                      = nullptr;

        uint32_t castOffset    = 0;
        uint32_t stackOffset   = 0;
        uint32_t anyTypeOffset = 0;
        uint32_t stackSize     = 0;
    };

    struct NodeExtension
    {
        NodeExtensionByteCode* bytecode = nullptr;
        NodeExtensionSemantic* semantic = nullptr;
        NodeExtensionOwner*    owner    = nullptr;
        NodeExtensionMisc*     misc     = nullptr;
    };

    void allocateExtension(ExtensionKind extensionKind);
    void allocateExtensionNoLock(ExtensionKind extensionKind);
    void setBcNotifyBefore(ByteCodeNotifyFct fct, ByteCodeNotifyFct checkIf = nullptr);
    void setBcNotifyAfter(ByteCodeNotifyFct fct, ByteCodeNotifyFct checkIf = nullptr);
    void addSpecFlags(uint16_t fl);
    void removeSpecFlags(uint16_t fl);
    void addAlternativeScopes(NodeExtensionMisc* ext);

    // clang-format off
    bool                   hasExtByteCode() const { return extension && extension->bytecode; }
    bool                   hasExtSemantic() const { return extension && extension->semantic; }
    bool                   hasExtOwner() const    { return extension && extension->owner; }
    bool                   hasExtMisc() const     { return extension && extension->misc; }
    NodeExtensionByteCode* extByteCode() const    { return extension->bytecode; }
    NodeExtensionSemantic* extSemantic() const    { return extension->semantic; }
    NodeExtensionOwner*    extOwner() const       { return extension->owner; }
    NodeExtensionMisc*     extMisc() const        { return extension->misc; }
    // clang-format on

    AstNodeKind         kind;
    AstNodeResolveState semanticState;
    AstNodeResolveState bytecodeState;
    uint8_t             padding0;
    TokenId             tokenId;
    atomic<uint16_t>    specFlags;

    SharedMutex            mutex;
    Token                  token;
    VectorNative<AstNode*> childs;
    ComputedValue*         computedValue;

    Scope*        ownerScope;
    Scope*        ownerStructScope;
    AstBreakable* ownerBreakable;
    AstInline*    ownerInline;
    AstFuncDecl*  ownerFct;

    TypeInfo* typeInfo;
    TypeInfo* castedTypeInfo;

    SymbolName*     resolvedSymbolName;
    SymbolOverload* resolvedSymbolOverload;

    AstNode*       parent;
    SourceFile*    sourceFile;
    NodeExtension* extension;

    SemanticFct semanticFct;
    ByteCodeFct byteCodeFct;

    uint64_t flags;
    uint64_t semFlags;
    uint64_t attributeFlags;

    RegisterList resultRegisterRC;
    uint16_t     safetyOn;
    uint16_t     safetyOff;

#ifdef SWAG_DEV_MODE
    uint32_t rankId;
#endif

    SWAG_RACE_CONDITION_INSTANCE(raceC);
};

struct AstVarDecl : public AstNode
{
    static constexpr uint16_t SPECFLAG_CONST_ASSIGN     = 0x0001;
    static constexpr uint16_t SPECFLAG_IS_LET_TO_CONST  = 0x0002;
    static constexpr uint16_t SPECFLAG_INLINE_STORAGE   = 0x0004;
    static constexpr uint16_t SPECFLAG_UNNAMED          = 0x0008;
    static constexpr uint16_t SPECFLAG_GENERATED_SELF   = 0x0010;
    static constexpr uint16_t SPECFLAG_GENERIC_TYPE     = 0x0020;
    static constexpr uint16_t SPECFLAG_GENERIC_CONSTANT = 0x0040;
    static constexpr uint16_t SPECFLAG_AUTO_NAME        = 0x0080;
    static constexpr uint16_t SPECFLAG_IS_LET           = 0x0100;
    static constexpr uint16_t SPECFLAG_TUPLE_AFFECT     = 0x0200;

    AstNode* clone(CloneContext& context);
    bool     isConstDecl();

    AttributeList attributes;
    Utf8          publicName;
    TokenParse    assignToken;

    AstNode*    type;
    AstNode*    assignment;
    AstNode*    typeConstraint;
    AstAttrUse* attrUse;
    AstNode*    genTypeComesFrom;
};

struct AstIdentifierRef : public AstNode
{
    static constexpr uint16_t SPECFLAG_AUTO_SCOPE = 0x0001;
    static constexpr uint16_t SPECFLAG_WITH_SCOPE = 0x0002;

    AstNode* clone(CloneContext& context);
    void     computeName();

    Scope*   startScope;
    AstNode* previousResolvedNode;
};

struct AstIdentifier : public AstNode
{
    static constexpr uint16_t SPECFLAG_NO_INLINE           = 0x0001;
    static constexpr uint16_t SPECFLAG_FROM_WITH           = 0x0002;
    static constexpr uint16_t SPECFLAG_FROM_USING          = 0x0004;
    static constexpr uint16_t SPECFLAG_CLOSURE_FIRST_PARAM = 0x0008;
    static constexpr uint16_t SPECFLAG_SILENT_CALL         = 0x0010;
    static constexpr uint16_t SPECFLAG_NAME_ALIAS          = 0x0020;

    struct IdentifierExtension
    {
        Vector<Token>         aliasNames;
        TokenParse            scopeUpValue;
        TypeInfo*             alternateEnum    = nullptr;
        AstNode*              fromAlternateVar = nullptr;
        IdentifierScopeUpMode scopeUpMode      = IdentifierScopeUpMode::None;
    };

    ~AstIdentifier();
    AstNode*          clone(CloneContext& context);
    void              allocateIdentifierExtension();
    AstIdentifierRef* identifierRef();

    AstFuncCallParams*   genericParameters;
    AstFuncCallParams*   callParameters;
    IdentifierExtension* identifierExtension;
};

struct AstFuncDecl : public AstNode
{
    static constexpr uint16_t SPECFLAG_THROW                = 0x0001;
    static constexpr uint16_t SPECFLAG_PATCH                = 0x0002;
    static constexpr uint16_t SPECFLAG_FORCE_LATE_REGISTER  = 0x0004;
    static constexpr uint16_t SPECFLAG_LATE_REGISTER_DONE   = 0x0008;
    static constexpr uint16_t SPECFLAG_FULL_RESOLVE         = 0x0010;
    static constexpr uint16_t SPECFLAG_PARTIAL_RESOLVE      = 0x0020;
    static constexpr uint16_t SPECFLAG_REG_GET_CONTEXT      = 0x0040;
    static constexpr uint16_t SPECFLAG_SPEC_MIXIN           = 0x0080;
    static constexpr uint16_t SPECFLAG_SHORT_FORM           = 0x0100;
    static constexpr uint16_t SPECFLAG_SHORT_LAMBDA         = 0x0200;
    static constexpr uint16_t SPECFLAG_RETURN_DEFINED       = 0x0400;
    static constexpr uint16_t SPECFLAG_CHECK_ATTR           = 0x0800;
    static constexpr uint16_t SPECFLAG_IS_LAMBDA_EXPRESSION = 0x1000;
    static constexpr uint16_t SPECFLAG_ASSUME               = 0x2000;
    static constexpr uint16_t SPECFLAG_IMPL                 = 0x4000;

    ~AstFuncDecl();
    AstNode*    clone(CloneContext& context);
    bool        cloneSubDecls(ErrorContext* context, CloneContext& cloneContext, AstNode* oldOwnerNode, AstFuncDecl* newFctNode, AstNode* refNode);
    void        computeFullNameForeign(bool forExport);
    Utf8        getDisplayName();
    const char* getDisplayNameC();
    Utf8        getNameForUserCompiler();
    bool        mustAutoInline();
    bool        mustUserInline(bool forExport = false);
    bool        mustInline();
    Utf8        getCallName();

    DependentJobs          dependentJobs;
    Utf8                   fullnameForeign;
    VectorNative<AstNode*> subDecls;
    VectorNative<AstNode*> localGlobalVars;
    VectorNative<AstNode*> localConstants;
    Mutex                  funcMutex;
    Token                  tokenName;
    SourceLocation         implLoc;

    AstNode*        captureParameters;
    AstNode*        parameters;
    AstNode*        genericParameters;
    AstNode*        returnType;
    AstNode*        content;
    AstNode*        validif;
    AstNode*        returnTypeDeducedNode;
    AstNode*        originalGeneric;
    AstNode*        requestedGeneric;
    Scope*          scope;
    TypeInfoParam*  methodParam;
    Job*            pendingLambdaJob;
    AstMakePointer* makePointerLambda;
    TypeInfoParam*  fromItfSymbol;

    uint32_t aliasMask;
    uint32_t stackSize;
    uint32_t nodeCounts;
    uint32_t registerGetContext = UINT32_MAX;
    uint32_t registerStoreRR    = UINT32_MAX;
};

struct AstAttrDecl : public AstNode
{
    AstNode* clone(CloneContext& context);

    Token    tokenName;
    AstNode* parameters;
};

struct AstAttrUse : public AstNode
{
    static constexpr uint16_t SPECFLAG_GLOBAL = 0x0001;

    AstNode* clone(CloneContext& context);

    AttributeList attributes;

    AstNode* content;
};

struct AstFuncCallParams : public AstNode
{
    static constexpr uint16_t SPECFLAG_CALL_FOR_STRUCT = 0x0001;

    AstNode* clone(CloneContext& context);

    Vector<Token> aliasNames;
};

struct AstFuncCallParam : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstReturn*     autoTupleReturn;
    TypeInfoParam* resolvedParameter;
    AstNode*       specUfcsNode;

    int indexParam = 0;
};

struct AstBinaryOpNode : public AstNode
{
    static constexpr uint16_t SPECFLAG_IMPLICIT_KINDOF = 0x0001;

    AstNode* clone(CloneContext& context);

    int seekJumpExpression;
};

struct AstConditionalOpNode : public AstNode
{
    AstNode* clone(CloneContext& context);

    int seekJumpIfFalse;
    int seekJumpAfterIfFalse;
};

struct AstIf : public AstNode
{
    static constexpr uint16_t SPECFLAG_ASSIGN = 0x0001;

    AstNode* clone(CloneContext& context);

    AstNode*            boolExpression;
    AstCompilerIfBlock* ifBlock;
    AstCompilerIfBlock* elseBlock;

    int seekJumpExpression;
    int seekJumpAfterIf;
};

struct AstBreakContinue : public AstNode
{
    AstNode* clone(CloneContext& context);

    Token label;

    AstSwitchCase* switchCase;

    int jumpInstruction;
};

constexpr uint32_t BREAKABLE_CAN_HAVE_INDEX         = 0x00000001;
constexpr uint32_t BREAKABLE_CAN_HAVE_CONTINUE      = 0x00000002;
constexpr uint32_t BREAKABLE_NEED_INDEX             = 0x00000004;
constexpr uint32_t BREAKABLE_RETURN_IN_INFINIT_LOOP = 0x00000008;

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

    uint32_t registerIndex;
    uint32_t breakableFlags = BREAKABLE_CAN_HAVE_INDEX | BREAKABLE_CAN_HAVE_CONTINUE;

    int seekJumpBeforeContinue;
    int seekJumpBeforeExpression;
    int seekJumpExpression;
    int seekJumpAfterBlock;
};

struct AstScopeBreakable : public AstBreakable
{
    AstScopeBreakable()
    {
        breakableFlags &= ~BREAKABLE_CAN_HAVE_INDEX;
    }

    AstNode* clone(CloneContext& context);

    AstNode* block;
};

struct AstWhile : public AstBreakable
{
    AstNode* clone(CloneContext& context);

    AstNode* boolExpression;
    AstNode* block;
};

struct AstFor : public AstBreakable
{
    ~AstFor();
    AstNode* clone(CloneContext& context);

    AstNode* preExpression;
    AstNode* boolExpression;
    AstNode* postExpression;
    AstNode* block;

    int seekJumpToExpression = 0;
};

struct AstLoop : public AstBreakable
{
    static constexpr uint16_t SPECFLAG_BACK = 0x0001;

    ~AstLoop();
    AstNode* clone(CloneContext& context);

    AstNode* specificName;
    AstNode* expression;
    AstNode* block;
};

struct AstVisit : public AstNode
{
    static constexpr uint16_t SPECFLAG_WANT_POINTER = 0x0001;
    static constexpr uint16_t SPECFLAG_BACK         = 0x0002;

    AstNode* clone(CloneContext& context);

    Vector<Token> aliasNames;
    Token         extraNameToken;
    Token         wantPointerToken;

    AstNode* expression;
    AstNode* block;
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

    AstNode* expression;
};

struct AstSwitchCase : public AstNode
{
    static constexpr uint16_t SPECFLAG_IS_DEFAULT = 0x0001;
    static constexpr uint16_t SPECFLAG_IS_FALSE   = 0x0002;
    static constexpr uint16_t SPECFLAG_IS_TRUE    = 0x0004;

    AstNode* clone(CloneContext& context);

    VectorNative<AstNode*> expressions;

    AstNode*   block;
    AstSwitch* ownerSwitch;

    int caseIndex = 0;
};

struct AstSwitchCaseBlock : public AstNode
{
    ~AstSwitchCaseBlock();
    AstNode* clone(CloneContext& context);

    AstSwitchCase* ownerCase;

    int seekStart;
    int seekJumpNextCase;
};

struct AstType : public AstNode
{
    static constexpr uint16_t SPECFLAG_FORCE_TYPE                = 0x1000;
    static constexpr uint16_t SPECFLAG_HAS_STRUCT_PARAMETERS     = 0x2000;
    static constexpr uint16_t SPECFLAG_CREATED_STRUCT_PARAMETERS = 0x4000;
};

constexpr uint16_t TYPEFLAG_IS_ARRAY          = 0x0001;
constexpr uint16_t TYPEFLAG_IS_SLICE          = 0x0002;
constexpr uint16_t TYPEFLAG_IS_CONST          = 0x0004;
constexpr uint16_t TYPEFLAG_IS_CODE           = 0x0008;
constexpr uint16_t TYPEFLAG_FORCE_CONST       = 0x0010;
constexpr uint16_t TYPEFLAG_IS_SELF           = 0x0020;
constexpr uint16_t TYPEFLAG_IS_RETVAL         = 0x0040;
constexpr uint16_t TYPEFLAG_HAS_USING         = 0x0080;
constexpr uint16_t TYPEFLAG_IS_REF            = 0x0100;
constexpr uint16_t TYPEFLAG_IS_MOVE_REF       = 0x0200;
constexpr uint16_t TYPEFLAG_IS_PTR            = 0x0400;
constexpr uint16_t TYPEFLAG_IS_PTR_ARITHMETIC = 0x0800;
constexpr uint16_t TYPEFLAG_IS_SUB_TYPE       = 0x1000;
constexpr uint16_t TYPEFLAG_HAS_LOC_CONST     = 0x2000;

struct AstTypeExpression : public AstType
{
    static constexpr uint16_t SPECFLAG_DONE_GEN = 0x0001;

    AstNode* clone(CloneContext& context);

    SourceLocation locConst;

    AstNode*  identifier;
    TypeInfo* typeFromLiteral;

    LiteralType literalType;
    uint16_t    typeFlags;
    uint8_t     arrayDim;
};

struct AstTypeLambda : public AstType
{
    static constexpr uint16_t SPECFLAG_CAN_THROW = 0x0001;

    AstNode* clone(CloneContext& context);

    AstNode* parameters;
    AstNode* returnType;
};

struct AstArrayPointerIndex : public AstNode
{
    static constexpr uint16_t SPECFLAG_SERIAL  = 0x0001;
    static constexpr uint16_t SPECFLAG_IS_DREF = 0x0002;

    AstNode* clone(CloneContext& context);

    VectorNative<AstNode*> structFlatParams;

    AstNode* array;
    AstNode* access;
};

struct AstArrayPointerSlicing : public AstNode
{
    static constexpr uint16_t SPECFLAG_EXCLUDE_UP = 0x0001;

    AstNode* clone(CloneContext& context);

    VectorNative<AstNode*> structFlatParams;

    AstNode* array;
    AstNode* lowerBound;
    AstNode* upperBound;
};

struct AstIntrinsicProp : public AstNode
{
};

struct AstExpressionList : public AstNode
{
    static constexpr uint16_t SPECFLAG_FOR_TUPLE   = 0x0001;
    static constexpr uint16_t SPECFLAG_FOR_CAPTURE = 0x0002;

    AstNode* clone(CloneContext& context);

    TypeInfo* castToStruct;
};

struct AstStruct : public AstNode
{
    static constexpr uint16_t SPECFLAG_HAS_USING = 0x0001;
    static constexpr uint16_t SPECFLAG_UNION     = 0x0002;
    static constexpr uint16_t SPECFLAG_ANONYMOUS = 0x0004;

    ~AstStruct();
    AstNode* clone(CloneContext& context);

    DependentJobs dependentJobs;
    Token         tokenName;

    AstNode* genericParameters;
    AstNode* content;
    Scope*   scope;
    AstNode* originalGeneric;
    AstNode* originalParent;
    AstNode* validif;

    uint32_t packing = sizeof(uint64_t);
};

struct AstEnum : public AstNode
{
    ~AstEnum();
    AstNode* clone(CloneContext& context);

    Token    tokenName;
    AstNode* type;
    Scope*   scope;
};

struct AstEnumValue : public AstNode
{
    static constexpr uint16_t SPECFLAG_HAS_USING = 0x0001;

    AstNode* clone(CloneContext& context);

    AttributeList attributes;
};

struct AstImpl : public AstNode
{
    ~AstImpl();
    AstNode* clone(CloneContext& context);

    Scope*   structScope;
    Scope*   scope;
    AstNode* identifier;
    AstNode* identifierFor;
};

struct AstInit : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode*           expression;
    AstNode*           count;
    AstFuncCallParams* parameters;
};

struct AstDropCopyMove : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode* expression;
    AstNode* count;
};

struct AstReturn : public AstNode
{
    AstNode* clone(CloneContext& context);

    VectorNative<SymbolOverload*> forceNoDrop;

    AstFuncDecl* resolvedFuncDecl;

    int seekJump;
};

struct AstCompilerMacro : public AstNode
{
    ~AstCompilerMacro();
    AstNode* clone(CloneContext& context);

    Scope* scope;
};

struct AstCompilerMixin : public AstNode
{
    AstNode* clone(CloneContext& context);

    Map<TokenId, AstNode*> replaceTokens;
};

struct AstInline : public AstNode
{
    ~AstInline();
    AstNode* clone(CloneContext& context);

    VectorNative<AstReturn*> returnList;

    AstFuncDecl* func;
    Scope*       scope;
    Scope*       parametersScope;
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
};

struct AstCompilerSpecFunc : public AstNode
{
    static constexpr uint16_t SPECFLAG_AST_BLOCK = 0x0001;

    AstNode* clone(CloneContext& context);
};

struct AstNameSpace : public AstNode
{
    AstNode* clone(CloneContext& context);
};

struct AstTryCatchAssume : public AstReturn
{
    static constexpr uint16_t SPECFLAG_BLOCK        = 0x0001;
    static constexpr uint16_t SPECFLAG_GENERATED    = 0x0002;
    static constexpr uint16_t SPECFLAG_THROW_GETERR = 0x0004;

    AstNode* clone(CloneContext& context);

    RegisterList regInit;

    int seekInsideJump;
};

struct AstAlias : public AstNode
{
    AstNode* clone(CloneContext& context);

    Token kwdLoc;
};

struct AstCast : public AstNode
{
    static constexpr uint16_t SPECFLAG_OVERFLOW = 0x0001;
    static constexpr uint16_t SPECFLAG_BIT      = 0x0002;
    static constexpr uint16_t SPECFLAG_UNCONST  = 0x0004;

    AstNode* clone(CloneContext& context);

    TypeInfo* toCastTypeInfo;
};

struct AstOp : public AstNode
{
    static constexpr uint16_t SPECFLAG_OVERFLOW = 0x0001;
    static constexpr uint16_t SPECFLAG_UP       = 0x0002;
    static constexpr uint16_t SPECFLAG_FMA      = 0x0004;

    AstNode* clone(CloneContext& context);
};

struct AstRange : public AstNode
{
    static constexpr uint16_t SPECFLAG_EXCLUDE_UP = 0x0001;

    AstNode* clone(CloneContext& context);

    AstNode* expressionLow;
    AstNode* expressionUp;
};

struct AstMakePointer : public AstNode
{
    static constexpr uint16_t SPECFLAG_TOREF    = 0x0001;
    static constexpr uint16_t SPECFLAG_DEP_TYPE = 0x0002;

    AstNode* clone(CloneContext& context);

    AstFuncDecl*      lambda;
    TypeInfoFuncAttr* deducedLambdaType = nullptr;
    TypeInfoFuncAttr* tryLambdaType     = nullptr;
};

struct AstRefSubDecl : public AstNode
{
    AstNode* clone(CloneContext& context);
    AstNode* refSubDecl;
};

struct AstSubstBreakContinue : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode*      defaultSubst;
    AstNode*      altSubst;
    AstBreakable* altSubstBreakable;
};

struct AstDefer : public AstNode
{
    AstNode* clone(CloneContext& context);

    DeferKind deferKind;
};

struct AstWith : public AstNode
{
    AstNode* clone(CloneContext& context);

    Vector<Utf8> id;
};

struct AstLiteral : public AstNode
{
    AstNode* clone(CloneContext& context);

    Register literalValue;

    LiteralType literalType;
};
