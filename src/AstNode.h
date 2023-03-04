#pragma once
#include "Utf8.h"
#include "Tokenizer.h"
#include "AstFlags.h"
#include "Register.h"
#include "SymTable.h"
#include "Attribute.h"
#include "Scope.h"
#include "Map.h"
#include "VectorNative.h"
#include "Mutex.h"

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
struct Job;
struct Parser;
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

const uint32_t CLONE_RAW             = 0x00000001;
const uint32_t CLONE_FORCE_OWNER_FCT = 0x00000002;

const uint32_t ALTSCOPE_USING     = 0x00000001;
const uint32_t ALTSCOPE_SCOPEFILE = 0x00000002;
const uint32_t ALTSCOPE_UFCS      = 0x00000004;

const uint32_t BREAKABLE_CAN_HAVE_INDEX         = 0x00000001;
const uint32_t BREAKABLE_CAN_HAVE_CONTINUE      = 0x00000002;
const uint32_t BREAKABLE_NEED_INDEX             = 0x00000004;
const uint32_t BREAKABLE_NEED_INDEX1            = 0x00000008;
const uint32_t BREAKABLE_RETURN_IN_INFINIT_LOOP = 0x00000010;

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
const uint16_t TYPEFLAG_IS_MOVE_REF    = 0x0400;

struct CloneUpdateRef
{
    AstNode*  node;
    AstNode** ref;
};

struct CloneContext
{
    VectorMap<Utf8, TypeInfo*>   replaceTypes;
    VectorMap<Utf8, AstNode*>    replaceTypesFrom;
    VectorMap<Utf8, Utf8>        replaceNames;
    Map<TokenId, AstNode*>       replaceTokens;
    SetUtf8                      usedReplaceNames;
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
    uint32_t            cloneFlags             = 0;

    void propageResult(CloneContext& context)
    {
        usedReplaceNames.insert(context.usedReplaceNames.begin(), context.usedReplaceNames.end());
    }
};

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

enum class IdentifierScopeUpMode : uint8_t
{
    None,
    Count,
};

struct AstIdentifierExtension
{
    Vector<Token>         aliasNames;
    TokenParse            scopeUpValue;
    TypeInfo*             alternateEnum    = nullptr;
    AstNode*              fromAlternateVar = nullptr;
    IdentifierScopeUpMode scopeUpMode      = IdentifierScopeUpMode::None;
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
    CompilerInclude,
    CompilerPlaceHolder,
    CompilerSpecialValue,
    CompilerForeignLib,
    CompilerLoad,
    IntrinsicDefined,
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
    AstNode* clone(AstNode* node, CloneContext& context)
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
    void inheritTokenLocation(Token& tkn);
    void inheritTokenLocation(AstNode* node);
    void inheritOwners(AstNode* op);
    void inheritOwnersAndFlags(Parser* parser);

    void allocateComputedValue();
    void setFlagsValueIsComputed();
    void inheritComputedValue(AstNode* from);

    bool isConstantTrue();
    bool isConstantFalse();
    bool isConstant0();
    bool isConstant1();
    bool isParentOf(AstNode* child);
    bool isValidIfParam(SymbolOverload* overload);
    bool isSameStackFrame(SymbolOverload* overload);
    bool isSpecialFunctionName();
    bool isSpecialFunctionGenerated();
    bool isPublic();
    bool isFunctionCall();
    bool isGeneratedSelf();

    // clang-format off
    bool isForeign() { return attributeFlags & ATTRIBUTE_FOREIGN; }
    // clang-format on

    void     setPassThrough();
    AstNode* findChildRef(AstNode* ref, AstNode* fromChild);
    AstNode* findChildRefRec(AstNode* ref, AstNode* fromChild);
    AstNode* findParent(AstNodeKind parentKind);
    AstNode* findParent(AstNodeKind parentKind1, AstNodeKind parentKind2);
    Utf8     getScopedName();
    void     setOwnerAttrUse(AstAttrUse* attrUse);
    void     swap2Childs();
    bool     hasSpecialFuncCall();
    bool     hasSpecialFuncCall(const Utf8& name);
    AstNode* inSimpleReturn();
    bool     forceTakeAddress();
    void     computeLocation(SourceLocation& start, SourceLocation& end);
    void     addAlternativeScope(Scope* scope, uint32_t altFlags = 0);
    void     addAlternativeScopeVar(Scope* scope, AstNode* varNode, uint32_t altFlags = 0);
    void     addAlternativeScopes(const VectorNative<AlternativeScope>& scopes);
    uint32_t childParentIdx();

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
        AstAttrUse*            ownerAttrUse        = nullptr;
        AstTryCatchAssume*     ownerTryCatchAssume = nullptr;
        VectorNative<AstNode*> nodesToFree;
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
        AstNode*        isNamed                      = nullptr;

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

    void allocateExtension(ExtensionKind extensionKind);
    void allocateExtensionNoLock(ExtensionKind extensionKind);

    // clang-format off
    bool               hasExtByteCode() { return extension && extension->bytecode; }
    bool               hasExtSemantic() { return extension && extension->semantic; }
    bool               hasExtOwner()    { return extension && extension->owner; }
    bool               hasExtMisc()     { return extension && extension->misc; }
    ExtensionByteCode* extByteCode()    { return extension->bytecode; }
    ExtensionSemantic* extSemantic()    { return extension->semantic; }
    ExtensionOwner*    extOwner()       { return extension->owner; }
    ExtensionMisc*     extMisc()        { return extension->misc; }
    // clang-format on

    AstNodeKind         kind          = (AstNodeKind) 0;
    AstNodeResolveState semanticState = (AstNodeResolveState) 0;
    AstNodeResolveState bytecodeState = (AstNodeResolveState) 0;
    uint8_t             padding0      = 0;
    TokenId             tokenId       = TokenId::Invalid;
    uint16_t            specFlags     = 0;

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
    uint64_t semFlags       = 0;
    uint64_t attributeFlags = 0;

    RegisterList resultRegisterRC;
    uint16_t     safetyOn  = 0;
    uint16_t     safetyOff = 0;

#ifdef SWAG_DEV_MODE
    uint32_t rankId;
#endif
};

struct AstVarDecl : public AstNode
{
    static const uint16_t SPECFLAG_CONST_ASSIGN     = 0x0001;
    static const uint16_t SPECFLAG_GEN_ITF          = 0x0002;
    static const uint16_t SPECFLAG_INLINE_STORAGE   = 0x0004;
    static const uint16_t SPECFLAG_UNNAMED          = 0x0008;
    static const uint16_t SPECFLAG_GENERATED_SELF   = 0x0010;
    static const uint16_t SPECFLAG_GENERIC_TYPE     = 0x0020;
    static const uint16_t SPECFLAG_GENERIC_CONSTANT = 0x0040;

    AstNode* clone(CloneContext& context);

    AttributeList attributes;
    Utf8          publicName;
    TokenParse    assignToken;

    AstNode*    type             = nullptr;
    AstNode*    assignment       = nullptr;
    AstNode*    typeConstraint   = nullptr;
    AstAttrUse* attrUse          = nullptr;
    AstNode*    genTypeComesFrom = nullptr;
};

struct AstIdentifierRef : public AstNode
{
    static const uint16_t SPECFLAG_AUTO_SCOPE = 0x0001;
    static const uint16_t SPECFLAG_WITH_SCOPE = 0x0002;
    static const uint16_t SPECFLAG_GLOBAL     = 0x0004;

    AstNode* clone(CloneContext& context);
    void     computeName();

    Scope*   startScope           = nullptr;
    AstNode* previousResolvedNode = nullptr;
};

struct AstIdentifier : public AstNode
{
    static const uint16_t SPECFLAG_NO_INLINE           = 0x0001;
    static const uint16_t SPECFLAG_FROM_WITH           = 0x0002;
    static const uint16_t SPECFLAG_FROM_USING          = 0x0004;
    static const uint16_t SPECFLAG_CLOSURE_FIRST_PARAM = 0x0008;

    ~AstIdentifier();
    AstNode*          clone(CloneContext& context);
    void              allocateIdentifierExtension();
    AstIdentifierRef* identifierRef();

    AstNode*                genericParameters   = nullptr;
    AstFuncCallParams*      callParameters      = nullptr;
    AstIdentifierExtension* identifierExtension = nullptr;
};

struct AstFuncDecl : public AstNode
{
    static const uint16_t SPECFLAG_THROW               = 0x0001;
    static const uint16_t SPECFLAG_PATCH               = 0x0002;
    static const uint16_t SPECFLAG_FORCE_LATE_REGISTER = 0x0004;
    static const uint16_t SPECFLAG_LATE_REGISTER_DONE  = 0x0008;
    static const uint16_t SPECFLAG_FULL_RESOLVE        = 0x0010;
    static const uint16_t SPECFLAG_PARTIAL_RESOLVE     = 0x0020;
    static const uint16_t SPECFLAG_REG_GET_CONTEXT     = 0x0040;
    static const uint16_t SPECFLAG_SPEC_MIXIN          = 0x0080;
    static const uint16_t SPECFLAG_SHORT_FORM          = 0x0100;
    static const uint16_t SPECFLAG_SHORT_LAMBDA        = 0x0200;
    static const uint16_t SPECFLAG_RETURN_DEFINED      = 0x0400;
    static const uint16_t SPECFLAG_CHECK_ATTR          = 0x0800;

    ~AstFuncDecl();
    AstNode*    clone(CloneContext& context);
    bool        cloneSubDecls(ErrorContext* context, CloneContext& cloneContext, AstNode* oldOwnerNode, AstFuncDecl* newFctNode, AstNode* refNode);
    void        computeFullNameForeign(bool forExport);
    Utf8        getDisplayName();
    const char* getDisplayNameC();
    Utf8        getNameForUserCompiler();
    bool        mustInline();
    Utf8        getCallName();

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
    AstNode*        validif               = nullptr;
    AstNode*        returnTypeDeducedNode = nullptr;
    AstNode*        originalGeneric       = nullptr;
    Scope*          scope                 = nullptr;
    TypeInfoParam*  methodParam           = nullptr;
    Job*            pendingLambdaJob      = nullptr;
    AstMakePointer* makePointerLambda     = nullptr;
    TypeInfoParam*  fromItfSymbol         = nullptr;

    uint32_t aliasMask          = 0;
    uint32_t stackSize          = 0;
    uint32_t nodeCounts         = 0;
    uint32_t registerGetContext = UINT32_MAX;
};

struct AstAttrDecl : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode* parameters = nullptr;
};

struct AstAttrUse : public AstNode
{
    static const uint16_t SPECFLAG_GLOBAL = 0x0001;

    AstNode* clone(CloneContext& context);

    AttributeList attributes;

    AstNode* content = nullptr;
};

struct AstFuncCallParams : public AstNode
{
    AstNode* clone(CloneContext& context);

    Vector<Token> aliasNames;
};

struct AstFuncCallParam : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstReturn*     autoTupleReturn   = nullptr;
    TypeInfoParam* resolvedParameter = nullptr;
    AstNode*       specUfcsNode      = nullptr;

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
    ~AstFor();
    AstNode* clone(CloneContext& context);

    AstNode* preExpression  = nullptr;
    AstNode* boolExpression = nullptr;
    AstNode* postExpression = nullptr;
    AstNode* block          = nullptr;

    int seekJumpToExpression = 0;
};

struct AstLoop : public AstBreakable
{
    ~AstLoop();
    AstNode* clone(CloneContext& context);

    AstNode* specificName = nullptr;
    AstNode* expression   = nullptr;
    AstNode* block        = nullptr;
};

struct AstVisit : public AstNode
{
    static const uint16_t SPECFLAG_WANT_POINTER = 0x0001;

    AstNode* clone(CloneContext& context);

    Vector<Token> aliasNames;
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
    static const uint16_t SPECFLAG_IS_DEFAULT = 0x0001;

    AstNode* clone(CloneContext& context);

    VectorNative<AstNode*> expressions;

    AstNode*   block       = nullptr;
    AstSwitch* ownerSwitch = nullptr;

    int caseIndex = 0;
};

struct AstSwitchCaseBlock : public AstNode
{
    ~AstSwitchCaseBlock();
    AstNode* clone(CloneContext& context);

    AstSwitchCase* ownerCase = nullptr;

    int seekStart        = 0;
    int seekJumpNextCase = 0;
};

struct AstTypeExpression : public AstNode
{
    static const uint16_t SPECFLAG_DONE_GEN = 0x0001;

    static const int     MAX_PTR_COUNT = 4;
    static const uint8_t PTR_CONST     = 0x01;
    static const uint8_t PTR_ARITMETIC = 0x02;

    AstNode* clone(CloneContext& context);

    AstNode*  identifier      = nullptr;
    TypeInfo* typeFromLiteral = nullptr;

    uint8_t  ptrFlags[MAX_PTR_COUNT] = {0};
    uint16_t typeFlags               = 0;
    uint8_t  ptrCount                = 0;
    uint8_t  arrayDim                = 0;

    LiteralType literalType = (LiteralType) 0;
};

struct AstTypeLambda : public AstNode
{
    static const uint16_t SPECFLAG_CAN_THROW = 0x0001;

    AstNode* clone(CloneContext& context);

    AstNode* parameters = nullptr;
    AstNode* returnType = nullptr;
};

struct AstArrayPointerIndex : public AstNode
{
    static const uint16_t SPECFLAG_ISDEREF = 0x0001;

    AstNode* clone(CloneContext& context);

    VectorNative<AstNode*> structFlatParams;

    AstNode* array  = nullptr;
    AstNode* access = nullptr;
};

struct AstArrayPointerSlicing : public AstNode
{
    static const uint16_t SPECFLAG_EXCLUDE_UP = 0x0001;

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
    static const uint16_t SPECFLAG_FOR_TUPLE   = 0x0001;
    static const uint16_t SPECFLAG_FOR_CAPTURE = 0x0002;

    AstNode* clone(CloneContext& context);

    TypeInfo* castToStruct = nullptr;
};

struct AstStruct : public AstNode
{
    static const uint16_t SPECFLAG_HAS_USING = 0x0001;
    static const uint16_t SPECFLAG_UNION     = 0x0002;
    static const uint16_t SPECFLAG_ANONYMOUS = 0x0004;

    ~AstStruct();
    AstNode* clone(CloneContext& context);

    DependentJobs dependentJobs;

    AstNode* genericParameters = nullptr;
    AstNode* content           = nullptr;
    Scope*   scope             = nullptr;
    AstNode* ownerGeneric      = nullptr;
    AstNode* originalParent    = nullptr;
    AstNode* validif           = nullptr;

    uint32_t packing = sizeof(uint64_t);
};

struct AstEnum : public AstNode
{
    ~AstEnum();
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
    ~AstImpl();
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

    int seekJump = 0;
};

struct AstCompilerInline : public AstNode
{
    ~AstCompilerInline();
    AstNode* clone(CloneContext& context);

    Scope* scope = nullptr;
};

struct AstCompilerMacro : public AstNode
{
    ~AstCompilerMacro();
    AstNode* clone(CloneContext& context);

    Scope* scope = nullptr;
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
};

struct AstCompilerSpecFunc : public AstNode
{
    static const uint16_t SPECFLAG_AST_BLOCK = 0x0001;

    AstNode* clone(CloneContext& context);
};

struct AstNameSpace : public AstNode
{
    AstNode* clone(CloneContext& context);
};

struct AstTryCatchAssume : public AstReturn
{
    static const uint16_t SPECFLAG_BLOCK     = 0x0001;
    static const uint16_t SPECFLAG_GENERATED = 0x0002;

    AstNode* clone(CloneContext& context);

    RegisterList regInit;

    int seekInsideJump = 0;
};

struct AstAlias : public AstNode
{
    AstNode* clone(CloneContext& context);
};

struct AstCast : public AstNode
{
    static const uint16_t SPECFLAG_SAFE = 0x0001;
    static const uint16_t SPECFLAG_BIT  = 0x0002;

    AstNode* clone(CloneContext& context);

    TypeInfo* toCastTypeInfo = nullptr;
};

struct AstOp : public AstNode
{
    static const uint16_t SPECFLAG_SAFE  = 0x0001;
    static const uint16_t SPECFLAG_SMALL = 0x0002;

    AstNode* clone(CloneContext& context);

    AstNode*          dependentLambda   = nullptr;
    TypeInfoFuncAttr* deducedLambdaType = nullptr;
    TypeInfoFuncAttr* tryLambdaType     = nullptr;
};

struct AstRange : public AstNode
{
    static const uint16_t SPECFLAG_EXCLUDE_UP = 0x0001;

    AstNode* clone(CloneContext& context);

    AstNode* expressionLow = nullptr;
    AstNode* expressionUp  = nullptr;
};

struct AstMakePointer : public AstNode
{
    static const uint16_t SPECFLAG_TOREF = 0x0001;

    AstNode* clone(CloneContext& context);

    AstFuncDecl* lambda = nullptr;
};

struct AstSubstBreakContinue : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode*      defaultSubst      = nullptr;
    AstNode*      altSubst          = nullptr;
    AstBreakable* altSubstBreakable = nullptr;
};

struct AstDefer : public AstNode
{
    AstNode* clone(CloneContext& context);

    DeferKind deferKind = DeferKind::Normal;
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

    LiteralType literalType = (LiteralType) 0;
};