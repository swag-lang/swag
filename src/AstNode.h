#pragma once
#include "AstFlags.h"
#include "Attribute.h"
#include "DependentJobs.h"
#include "Flags.h"
#include "Mutex.h"
#include "Register.h"
#include "Symbol.h"
#include "Token.h"
#include "TokenParse.h"

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

using SemanticFct       = bool       (*)(SemanticContext* context);
using ByteCodeFct       = bool       (*)(ByteCodeGenContext* context);
using ByteCodeNotifyFct = bool (*)(ByteCodeGenContext* context);

using AstNodeFlags   = Flags<uint64_t>;
using AstSemFlags    = Flags<uint64_t>;
using AltScopeFlags  = Flags<uint32_t>;
using BreakableFlags = Flags<uint32_t>;
using CloneFlags     = Flags<uint32_t>;
using SpecFlags      = Flags<uint16_t>;
using SafetyFlags    = Flags<uint16_t>;
using TypeFlags      = Flags<uint16_t>;

constexpr CloneFlags CLONE_RAW             = 0x00000001;
constexpr CloneFlags CLONE_FORCE_OWNER_FCT = 0x00000002;

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
    AstNodeFlags        forceFlags             = 0;
    AstNodeFlags        removeFlags            = 0;
    CloneFlags          cloneFlags             = 0;

    void propagateResult(CloneContext& context)
    {
        usedReplaceNames.insert(context.usedReplaceNames.begin(), context.usedReplaceNames.end());
        nodeRefsToUpdate = std::move(context.nodeRefsToUpdate);
    }
};

constexpr AltScopeFlags ALT_SCOPE_STRUCT_USING = 0x00000001;
constexpr AltScopeFlags ALT_SCOPE_FILE_PRIVATE = 0x00000002;
constexpr AltScopeFlags ALT_SCOPE_UFCS         = 0x00000004;
constexpr AltScopeFlags ALT_SCOPE_USING        = 0x00000008;

struct AlternativeScope
{
    Scope*        scope = nullptr;
    AltScopeFlags flags = 0;
};

struct AlternativeScopeVar
{
    AstNode*      node     = nullptr;
    AstNode*      leafNode = nullptr;
    Scope*        scope    = nullptr;
    AltScopeFlags flags    = 0;
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
    ProcessingChildren,
    PostChildren,
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

enum class ExtraPointerKind
{
    CastItf,
    AnyTypeSegment,
    ExportNode,
    CollectTypeInfo,
    AlternativeNode,
    IsNamed,
    UserOp
};

struct AstNode
{
    template<typename T>
    static AstNode* clone(AstNode* node, CloneContext& context)
    {
        return static_cast<T*>(node)->clone(context);
    }

    AstNode* clone(CloneContext& context);
    void     releaseChildren();
    void     release();
    void     cloneChildren(CloneContext& context, AstNode* from);
    void     copyFrom(CloneContext& context, AstNode* from, bool cloneHie = true);

    void inheritAstFlagsOr(AstNodeFlags flag);
    void inheritAstFlagsOr(const AstNode* op, AstNodeFlags flag);
    void inheritAstFlagsAnd(AstNodeFlags flag);
    void inheritAstFlagsAnd(AstNodeFlags flag1, AstNodeFlags flag2);
    void inheritAstFlagsAnd(AstNodeFlags flag1, AstNodeFlags flag2, AstNodeFlags flag3);
    void inheritAstFlagsAnd(AstNode* who, AstNodeFlags flag);
    void inheritAstFlagsAnd(AstNode* who, AstNodeFlags flag1, AstNodeFlags flag2);
    void inheritAstFlagsAnd(AstNode* who, AstNodeFlags flag1, AstNodeFlags flag2, AstNodeFlags flag3);
    void inheritTokenName(Token& tkn);
    void inheritTokenLocation(const Token& tkn);
    void inheritOwners(const AstNode* from);
    void inheritFormatFromBefore(TokenParse& tokenParse);
    void inheritFormatFromAfter(TokenParse& tokenParse);
    void inheritOwnersAndFlags(const Parser* parser);

    void allocateComputedValue();
    void releaseComputedValue();
    void setFlagsValueIsComputed();
    void inheritComputedValue(const AstNode* from);
    bool hasFlagComputedValue() const;

    ComputedValue* safeComputedValue() const { return hasExtSemantic() ? extSemantic()->computedValue : nullptr; }
    ComputedValue* computedValue() const { return extSemantic()->computedValue; }
    bool           hasComputedValue() const { return safeComputedValue() != nullptr; }

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
    bool isSpecialFunctionGenerated() const;
    bool isGeneratedSelf() const;
    bool isEmptyFct();
    bool isForeign() const;
    bool isSilentCall() const;
    bool isPublic() const;
    bool isForceTakeAddress() const;

    const Token& getTokenName() const;
    Utf8         getScopedName() const;
    void         setPassThrough();
    bool         hasSpecialFuncCall() const;
    bool         hasSpecialFuncCall(const Utf8& name) const;
    AstNode*     inSimpleReturn() const;
    bool         hasIntrinsicName() const;
    void         computeLocation(SourceLocation& start, SourceLocation& end);
    void         addAlternativeScope(Scope* scope, AltScopeFlags altFlags = 0);
    void         addAlternativeScopeVar(Scope* scope, AstNode* varNode, AltScopeFlags altFlags = 0);
    void         printLoc() const;

    bool is(AstNodeKind what) const { return kind == what; }
    bool isNot(AstNodeKind what) const { return kind != what; }

    AstNode* findChildRef(const AstNode* ref, AstNode* fromChild) const;
    AstNode* findChildRefRec(AstNode* ref, AstNode* fromChild) const;
    AstNode* findParent(AstNodeKind parentKind) const;
    AstNode* findParent(AstNodeKind parentKind1, AstNodeKind parentKind2) const;
    AstNode* findParentOrMe(AstNodeKind parentKind);
    AstNode* findChild(AstNodeKind childKind) const;
    AstNode* findParentAttrUse(const Utf8& name) const;
    AstNode* findParent(TokenId tkn) const;
    uint32_t childParentIdx() const;
    void     swap2Children();

    AstNode* firstChild() const { return children.empty() ? nullptr : children.front(); }
    AstNode* secondChild() const { return children.size() < 2 ? nullptr : children[1]; }
    AstNode* lastChild() const { return children.empty() ? nullptr : children.back(); }
    uint32_t childCount() const { return children.size(); }

    bool hasAstFlag(AstNodeFlags fl) const { return flags.has(fl); }
    void addAstFlag(AstNodeFlags fl) { flags.add(fl); }
    void removeAstFlag(AstNodeFlags fl) { flags.remove(fl); }

    bool hasSemFlag(AstSemFlags fl) const { return semFlags.has(fl); }
    void addSemFlag(AstSemFlags fl) { semFlags.add(fl); }
    void removeSemFlag(AstSemFlags fl) { semFlags.remove(fl); }

    bool hasAttribute(AttributeFlags attr) const { return attributeFlags.has(attr); }
    void addAttribute(AttributeFlags attr) { attributeFlags.add(attr); }
    void removeAttribute(AttributeFlags attr) { attributeFlags.remove(attr); }
    void inheritAttribute(const AstNode* from, AttributeFlags w) { attributeFlags.add(from->attributeFlags.mask(w)); }

    bool hasSpecFlag(SpecFlags fl) const { return specFlags.has(fl); }
    void addSpecFlag(SpecFlags fl) { specFlags.add(fl); }
    void removeSpecFlag(SpecFlags fl) { specFlags.remove(fl); }

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
        SemanticFct    semanticBeforeFct = nullptr;
        SemanticFct    semanticAfterFct  = nullptr;
        ComputedValue* computedValue     = nullptr;
    };

    struct NodeExtensionOwner
    {
        AstAttrUse*            ownerAttrUse         = nullptr;
        AstTryCatchAssume*     ownerTryCatchAssume  = nullptr;
        AstCompilerIfBlock*    ownerCompilerIfBlock = nullptr;
        AstInline*             ownerInline          = nullptr;
        AstBreakable*          ownerBreakable       = nullptr;
        VectorNative<AstNode*> nodesToFree;
    };

    struct NodeExtensionMisc
    {
        SharedMutex                        mutexAltScopes;
        VectorNative<AlternativeScope>     alternativeScopes;
        VectorNative<AlternativeScopeVar>  alternativeScopesVars;
        VectorNative<uint32_t>             registersToRelease;
        TokenComments                      comments;
        VectorMap<ExtraPointerKind, void*> extraPointers;

        RegisterList additionalRegisterRC;
        uint32_t     castOffset    = 0;
        uint32_t     stackOffset   = 0;
        uint32_t     anyTypeOffset = 0;
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
    void addAlternativeScopes(NodeExtensionMisc* ext);

    bool                   hasExtByteCode() const { return extension && extension->bytecode; }
    bool                   hasExtSemantic() const { return extension && extension->semantic; }
    bool                   hasExtOwner() const { return extension && extension->owner; }
    bool                   hasExtMisc() const { return extension && extension->misc; }
    NodeExtensionByteCode* extByteCode() const { return extension->bytecode; }
    NodeExtensionSemantic* extSemantic() const { return extension->semantic; }
    NodeExtensionOwner*    extOwner() const { return extension->owner; }
    NodeExtensionMisc*     extMisc() const { return extension->misc; }

    AstTryCatchAssume*  ownerTryCatchAssume() const { return extOwner()->ownerTryCatchAssume; }
    bool                hasOwnerTryCatchAssume() const { return hasExtOwner() && extOwner()->ownerTryCatchAssume; }
    AstCompilerIfBlock* ownerCompilerIfBlock() const { return extOwner()->ownerCompilerIfBlock; }
    bool                hasOwnerCompilerIfBlock() const { return hasExtOwner() && extOwner()->ownerCompilerIfBlock; }
    AstInline*          ownerInline() const { return extOwner()->ownerInline; }
    AstInline*          safeOwnerInline() const { return hasExtOwner() ? extOwner()->ownerInline : nullptr; }
    bool                hasOwnerInline() const { return safeOwnerInline() != nullptr; }
    AstBreakable*       ownerBreakable() const { return extOwner()->ownerBreakable; }
    AstBreakable*       safeOwnerBreakable() const { return hasExtOwner() ? extOwner()->ownerBreakable : nullptr; }
    bool                hasOwnerBreakable() const { return safeOwnerBreakable() != nullptr; }

    template<typename T>
    T* extraPointer(ExtraPointerKind extraPtrKind)
    {
        if (!hasExtMisc())
            return nullptr;
        const auto it = extMisc()->extraPointers.find(extraPtrKind);
        if (it != extMisc()->extraPointers.end())
            return static_cast<T*>(it->second);
        return nullptr;
    }

    template<typename T>
    const T* extraPointer(ExtraPointerKind extraPtrKind) const
    {
        if (!hasExtMisc())
            return nullptr;
        const auto it = extMisc()->extraPointers.find(extraPtrKind);
        if (it != extMisc()->extraPointers.end())
            return static_cast<const T*>(it->second);
        return nullptr;
    }

    void addExtraPointer(ExtraPointerKind extraPtrKind, void* value)
    {
        allocateExtension(ExtensionKind::Misc);
        extMisc()->extraPointers[extraPtrKind] = value;
    }

    bool hasExtraPointer(ExtraPointerKind extraPtrKind) const
    {
        if (!hasExtMisc())
            return false;
        const auto it = extMisc()->extraPointers.find(extraPtrKind);
        if (it == extMisc()->extraPointers.end())
            return false;
        return it->second != nullptr;
    }

    void setOwnerAttrUse(AstAttrUse* attrUse);
    void setOwnerBreakable(AstBreakable* bkp);

    Token               token;
    SpecFlags           specFlags;
    AstNodeKind         kind;
    AstNodeResolveState semanticState;
    AstNodeResolveState bytecodeState;
    AstFormatFlags      formatFlags;

    mutable SharedMutex    mutex;
    VectorNative<AstNode*> children;

    Scope*       ownerScope;
    Scope*       ownerStructScope;
    AstFuncDecl* ownerFct;

    TypeInfo* typeInfo;
    TypeInfo* castedTypeInfo;

    union
    {
        SymbolName*     symbolName;
        SymbolOverload* symbolOverload;
    };

    SymbolName*     resolvedSymbolName() const { return hasSemFlag(SEMFLAG_HAS_SYMBOL_NAME) ? symbolName : (symbolOverload ? symbolOverload->symbol : nullptr); }
    SymbolOverload* resolvedSymbolOverload() const { return hasSemFlag(SEMFLAG_HAS_SYMBOL_NAME) ? nullptr : symbolOverload; }
    void            setResolvedSymbolName(SymbolName* sym);
    void            setResolvedSymbolOverload(SymbolOverload* over);
    void            setResolvedSymbol(SymbolName* sym, SymbolOverload* over);

    AstNode*       parent;
    NodeExtension* extension;

    SemanticFct semanticFct;
    ByteCodeFct byteCodeFct;

    AstNodeFlags   flags;
    AstSemFlags    semFlags;
    AttributeFlags attributeFlags;

    RegisterList resultRegisterRc;
    SafetyFlags  safetyOn;
    SafetyFlags  safetyOff;

#ifdef SWAG_DEV_MODE
    uint32_t rankId;
#endif

    SWAG_RACE_CONDITION_INSTANCE(raceC);
};

struct AstVarDecl : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_CONST_ASSIGN     = 0x0001;
    static constexpr SpecFlags SPEC_FLAG_IS_LET_TO_CONST  = 0x0002;
    static constexpr SpecFlags SPEC_FLAG_INLINE_STORAGE   = 0x0004;
    static constexpr SpecFlags SPEC_FLAG_UNNAMED          = 0x0008;
    static constexpr SpecFlags SPEC_FLAG_GENERATED_SELF   = 0x0010;
    static constexpr SpecFlags SPEC_FLAG_GENERIC_TYPE     = 0x0020;
    static constexpr SpecFlags SPEC_FLAG_GENERIC_CONSTANT = 0x0040;
    static constexpr SpecFlags SPEC_FLAG_AUTO_NAME        = 0x0080;
    static constexpr SpecFlags SPEC_FLAG_IS_LET           = 0x0100;
    static constexpr SpecFlags SPEC_FLAG_TUPLE_AFFECT     = 0x0200;

    AstNode* clone(CloneContext& context);
    bool     isConstDecl() const;

    AttributeList attributes;
    Utf8          publicName;
    Token         assignToken;
    AstNode*      type;
    AstNode*      assignment;
    AstNode*      typeConstraint;
    AstAttrUse*   attrUse;
    AstNode*      genTypeComesFrom;
};

struct AstIdentifierRef : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_AUTO_SCOPE = 0x0001;
    static constexpr SpecFlags SPEC_FLAG_WITH_SCOPE = 0x0002;

    AstNode* clone(CloneContext& context);
    void     computeName();

    Scope*   startScope;
    AstNode* previousResolvedNode;
};

struct AstIdentifier : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_NO_INLINE           = 0x0001;
    static constexpr SpecFlags SPEC_FLAG_FROM_WITH           = 0x0002;
    static constexpr SpecFlags SPEC_FLAG_FROM_USING          = 0x0004;
    static constexpr SpecFlags SPEC_FLAG_CLOSURE_FIRST_PARAM = 0x0008;
    static constexpr SpecFlags SPEC_FLAG_SILENT_CALL         = 0x0010;
    static constexpr SpecFlags SPEC_FLAG_NAME_ALIAS          = 0x0020;

    struct IdentifierExtension
    {
        Vector<Token>         aliasNames;
        TokenParse            scopeUpValue;
        TypeInfo*             alternateEnum    = nullptr;
        AstNode*              fromAlternateVar = nullptr;
        IdentifierScopeUpMode scopeUpMode      = IdentifierScopeUpMode::None;
    };

    ~                 AstIdentifier();
    AstNode*          clone(CloneContext& context);
    void              allocateIdentifierExtension();
    bool              isForcedUFCS() const;
    AstIdentifierRef* identifierRef() const;

    AstFuncCallParams*   genericParameters;
    AstFuncCallParams*   callParameters;
    IdentifierExtension* identifierExtension;
};

struct AstFuncDecl : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_THROW                = 0x0001;
    static constexpr SpecFlags SPEC_FLAG_PATCH                = 0x0002;
    static constexpr SpecFlags SPEC_FLAG_FORCE_LATE_REGISTER  = 0x0004;
    static constexpr SpecFlags SPEC_FLAG_LATE_REGISTER_DONE   = 0x0008;
    static constexpr SpecFlags SPEC_FLAG_FULL_RESOLVE         = 0x0010;
    static constexpr SpecFlags SPEC_FLAG_PARTIAL_RESOLVE      = 0x0020;
    static constexpr SpecFlags SPEC_FLAG_REG_GET_CONTEXT      = 0x0040;
    static constexpr SpecFlags SPEC_FLAG_SPEC_MIXIN           = 0x0080;
    static constexpr SpecFlags SPEC_FLAG_SHORT_FORM           = 0x0100;
    static constexpr SpecFlags SPEC_FLAG_SHORT_LAMBDA         = 0x0200;
    static constexpr SpecFlags SPEC_FLAG_RETURN_DEFINED       = 0x0400;
    static constexpr SpecFlags SPEC_FLAG_CHECK_ATTR           = 0x0800;
    static constexpr SpecFlags SPEC_FLAG_IS_LAMBDA_EXPRESSION = 0x1000;
    static constexpr SpecFlags SPEC_FLAG_ASSUME               = 0x2000;
    static constexpr SpecFlags SPEC_FLAG_IMPL                 = 0x4000;
    static constexpr SpecFlags SPEC_FLAG_METHOD               = 0x8000;

    ~           AstFuncDecl();
    AstNode*    clone(CloneContext& context);
    bool        cloneSubDecl(ErrorContext* context, CloneContext& cloneContext, const AstNode* oldOwnerNode, AstFuncDecl* newFctNode, AstNode* refNode);
    void        computeFullNameForeignExport();
    const Utf8& getFullNameForeignImport() const;
    Utf8        getDisplayName() const;
    const char* getDisplayNameC() const;
    Utf8        getNameForUserCompiler() const;
    bool        mustAutoInline() const;
    bool        mustUserInline(bool forExport = false) const;
    bool        mustInline() const;
    Utf8        getCallName();

    DependentJobs          dependentJobs;
    Utf8                   fullnameForeignExport;
    VectorNative<AstNode*> subDecl;
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
    AstNode*        validIf;
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

struct AstAttrDecl : AstNode
{
    AstNode* clone(CloneContext& context);

    Token    tokenName;
    AstNode* parameters;
};

struct AstAttrUse : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_GLOBAL = 0x0001;

    AstNode* clone(CloneContext& context);

    AttributeList attributes;
    AstNode*      content;
};

struct AstFuncCallParams : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_CALL_FOR_STRUCT = 0x0001;

    AstNode* clone(CloneContext& context);

    Vector<Token> aliasNames;
};

struct AstFuncCallParam : AstNode
{
    AstNode* clone(CloneContext& context);

    AstReturn*     autoTupleReturn;
    TypeInfoParam* resolvedParameter;
    AstNode*       specUFCSNode;
    uint32_t       indexParam;
};

struct AstBinaryOpNode : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_IMPLICIT_KINDOF = 0x0001;

    AstNode* clone(CloneContext& context);

    uint32_t seekJumpExpression;
};

struct AstConditionalOpNode : AstNode
{
    AstNode* clone(CloneContext& context);

    uint32_t seekJumpIfFalse;
    uint32_t seekJumpAfterIfFalse;
};

struct AstIf : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_ASSIGN = 0x0001;

    AstNode* clone(CloneContext& context);

    AstNode*            boolExpression;
    AstCompilerIfBlock* ifBlock;
    AstCompilerIfBlock* elseBlock;
    uint32_t            seekJumpExpression;
    uint32_t            seekJumpAfterIf;
};

struct AstBreakContinue : AstNode
{
    AstNode* clone(CloneContext& context);

    Token          label;
    AstSwitchCase* switchCase;
    uint32_t       jumpInstruction;
};

constexpr BreakableFlags BREAKABLE_CAN_HAVE_INDEX          = 0x00000001;
constexpr BreakableFlags BREAKABLE_CAN_HAVE_CONTINUE       = 0x00000002;
constexpr BreakableFlags BREAKABLE_NEED_INDEX              = 0x00000004;
constexpr BreakableFlags BREAKABLE_RETURN_IN_INFINITE_LOOP = 0x00000008;

struct AstBreakable : AstNode
{
    void copyFrom(CloneContext& context, AstBreakable* from);

    bool needIndex() const { return breakableFlags.has(BREAKABLE_NEED_INDEX); }

    VectorNative<AstBreakContinue*> breakList;
    VectorNative<AstBreakContinue*> continueList;
    VectorNative<AstBreakContinue*> fallThroughList;
    BreakableFlags                  breakableFlags = BREAKABLE_CAN_HAVE_INDEX | BREAKABLE_CAN_HAVE_CONTINUE;
    uint32_t                        registerIndex;
    uint32_t                        seekJumpBeforeContinue;
    uint32_t                        seekJumpBeforeExpression;
    uint32_t                        seekJumpExpression;
    uint32_t                        seekJumpAfterBlock;
};

struct AstScopeBreakable : AstBreakable
{
             AstScopeBreakable();
    AstNode* clone(CloneContext& context);

    AstNode* block;
};

struct AstWhile : AstBreakable
{
    AstNode* clone(CloneContext& context);

    AstNode* boolExpression;
    AstNode* block;
};

struct AstFor : AstBreakable
{
    ~        AstFor();
    AstNode* clone(CloneContext& context);

    AstNode* preExpression;
    AstNode* boolExpression;
    AstNode* postExpression;
    AstNode* block;
    uint32_t seekJumpToExpression = 0;
};

struct AstLoop : AstBreakable
{
    static constexpr SpecFlags SPEC_FLAG_BACK = 0x0001;

    ~        AstLoop();
    AstNode* clone(CloneContext& context);

    AstNode* specificName;
    AstNode* expression;
    AstNode* block;
};

struct AstVisit : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_WANT_POINTER = 0x0001;
    static constexpr SpecFlags SPEC_FLAG_BACK         = 0x0002;

    AstNode* clone(CloneContext& context);

    Vector<Token> aliasNames;
    Token         extraNameToken;
    Token         wantPointerToken;
    AstNode*      expression;
    AstNode*      block;
};

struct AstSwitch : AstBreakable
{
             AstSwitch();
    AstNode* clone(CloneContext& context);

    VectorNative<AstSwitchCase*> cases;
    AstNode*                     expression;
};

struct AstSwitchCase : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_IS_DEFAULT = 0x0001;
    static constexpr SpecFlags SPEC_FLAG_IS_FALSE   = 0x0002;
    static constexpr SpecFlags SPEC_FLAG_IS_TRUE    = 0x0004;

    AstNode* clone(CloneContext& context);

    VectorNative<AstNode*> expressions;
    AstNode*               block;
    AstSwitch*             ownerSwitch;
    uint32_t               caseIndex;
};

struct AstSwitchCaseBlock : AstNode
{
    ~        AstSwitchCaseBlock();
    AstNode* clone(CloneContext& context);

    AstSwitchCase* ownerCase;
    uint32_t       seekStart;
    uint32_t       seekJumpNextCase;
};

struct AstType : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_FORCE_TYPE                = 0x1000;
    static constexpr SpecFlags SPEC_FLAG_HAS_STRUCT_PARAMETERS     = 0x2000;
    static constexpr SpecFlags SPEC_FLAG_CREATED_STRUCT_PARAMETERS = 0x4000;
};

constexpr TypeFlags TYPEFLAG_IS_ARRAY          = 0x0001;
constexpr TypeFlags TYPEFLAG_IS_SLICE          = 0x0002;
constexpr TypeFlags TYPEFLAG_IS_CONST          = 0x0004;
constexpr TypeFlags TYPEFLAG_IS_CODE           = 0x0008;
constexpr TypeFlags TYPEFLAG_FORCE_CONST       = 0x0010;
constexpr TypeFlags TYPEFLAG_IS_SELF           = 0x0020;
constexpr TypeFlags TYPEFLAG_IS_RETVAL         = 0x0040;
constexpr TypeFlags TYPEFLAG_HAS_USING         = 0x0080;
constexpr TypeFlags TYPEFLAG_IS_REF            = 0x0100;
constexpr TypeFlags TYPEFLAG_IS_MOVE_REF       = 0x0200;
constexpr TypeFlags TYPEFLAG_IS_PTR            = 0x0400;
constexpr TypeFlags TYPEFLAG_IS_PTR_ARITHMETIC = 0x0800;
constexpr TypeFlags TYPEFLAG_IS_SUB_TYPE       = 0x1000;
constexpr TypeFlags TYPEFLAG_HAS_LOC_CONST     = 0x2000;

struct AstTypeExpression : AstType
{
    static constexpr SpecFlags SPEC_FLAG_DONE_GEN = 0x0001;

    AstNode* clone(CloneContext& context);

    SourceLocation locConst;
    AstNode*       identifier;
    TypeInfo*      typeFromLiteral;

    TypeFlags   typeFlags;
    LiteralType literalType;
    uint8_t     arrayDim;
};

struct AstTypeLambda : AstType
{
    static constexpr SpecFlags SPEC_FLAG_CAN_THROW = 0x0001;

    AstNode* clone(CloneContext& context);

    AstNode* parameters;
    AstNode* returnType;
};

struct AstArrayPointerIndex : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_SERIAL   = 0x0001;
    static constexpr SpecFlags SPEC_FLAG_IS_DEREF = 0x0002;

    AstNode* clone(CloneContext& context);

    VectorNative<AstNode*> structFlatParams;
    AstNode*               array;
    AstNode*               access;
};

struct AstArrayPointerSlicing : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_EXCLUDE_UP = 0x0001;

    AstNode* clone(CloneContext& context);

    VectorNative<AstNode*> structFlatParams;

    AstNode* array;
    AstNode* lowerBound;
    AstNode* upperBound;
};

struct AstIntrinsicProp : AstNode
{
};

struct AstExpressionList : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_FOR_TUPLE   = 0x0001;
    static constexpr SpecFlags SPEC_FLAG_FOR_CAPTURE = 0x0002;

    AstNode* clone(CloneContext& context);

    TypeInfo* castToStruct;
};

struct AstStruct : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_HAS_USING = 0x0001;
    static constexpr SpecFlags SPEC_FLAG_UNION     = 0x0002;
    static constexpr SpecFlags SPEC_FLAG_ANONYMOUS = 0x0004;

    ~        AstStruct();
    AstNode* clone(CloneContext& context);

    DependentJobs dependentJobs;
    Token         tokenName;
    AstNode*      genericParameters;
    AstNode*      content;
    Scope*        scope;
    AstNode*      originalGeneric;
    AstNode*      originalParent;
    AstNode*      validif;
    uint32_t      packing = sizeof(uint64_t);
};

struct AstEnum : AstNode
{
    ~        AstEnum();
    AstNode* clone(CloneContext& context);

    Token    tokenName;
    AstNode* type;
    Scope*   scope;
};

struct AstEnumValue : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_HAS_USING = 0x0001;

    AstNode* clone(CloneContext& context);

    AttributeList attributes;
};

struct AstImpl : AstNode
{
    ~        AstImpl();
    AstNode* clone(CloneContext& context);

    Scope*   structScope;
    Scope*   scope;
    AstNode* identifier;
    AstNode* identifierFor;
};

struct AstInit : AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode*           expression;
    AstNode*           count;
    AstFuncCallParams* parameters;
};

struct AstDropCopyMove : AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode* expression;
    AstNode* count;
};

struct AstReturn : AstNode
{
    AstNode* clone(CloneContext& context);

    VectorNative<SymbolOverload*> forceNoDrop;
    AstFuncDecl*                  resolvedFuncDecl;
    uint32_t                      seekJump;
};

struct AstCompilerMacro : AstNode
{
    ~        AstCompilerMacro();
    AstNode* clone(CloneContext& context);

    Scope* scope;
};

struct AstCompilerMixin : AstNode
{
    AstNode* clone(CloneContext& context);

    Map<TokenId, AstNode*> replaceTokens;
};

struct AstInline : AstNode
{
    ~        AstInline();
    AstNode* clone(CloneContext& context);

    VectorNative<AstReturn*> returnList;
    AstFuncDecl*             func;
    Scope*                   scope;
    Scope*                   parametersScope;
};

struct AstCompilerIfBlock : AstNode
{
    AstNode* clone(CloneContext& context);
    void     addSymbol(AstNode* node, SymbolName* symName);

    struct MethodCount
    {
        AstFuncDecl*    funcNode;
        TypeInfoStruct* typeInfo;
        uint32_t        methodIdx;
    };

    VectorNative<AstCompilerIfBlock*>              blocks;
    VectorNative<std::pair<AstNode*, SymbolName*>> symbols;
    VectorNative<TypeInfoStruct*>                  interfacesCount;
    VectorNative<MethodCount>                      methodsCount;
    VectorNative<AstNode*>                         subDecl;
    VectorNative<AstNode*>                         imports;
    VectorNative<AstNode*>                         includes;
};

struct AstCompilerSpecFunc : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_AST_BLOCK = 0x0001;

    AstNode* clone(CloneContext& context);
};

struct AstNameSpace : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_GENERATED_TOP_LEVEL = 0x0001;

    AstNode* clone(CloneContext& context);
};

struct AstTryCatchAssume : AstReturn
{
    static constexpr SpecFlags SPEC_FLAG_BLOCK         = 0x0001;
    static constexpr SpecFlags SPEC_FLAG_GENERATED     = 0x0002;
    static constexpr SpecFlags SPEC_FLAG_THROW_GET_ERR = 0x0004;

    AstNode* clone(CloneContext& context);

    RegisterList regInit;
    uint32_t     seekInsideJump;
};

struct AstAlias : AstNode
{
    AstNode* clone(CloneContext& context);

    Token kwdLoc;
};

struct AstCast : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_OVERFLOW = 0x0001;
    static constexpr SpecFlags SPEC_FLAG_BIT      = 0x0002;
    static constexpr SpecFlags SPEC_FLAG_UN_CONST = 0x0004;

    AstNode* clone(CloneContext& context);

    TypeInfo* toCastTypeInfo;
};

struct AstOp : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_OVERFLOW = 0x0001;
    static constexpr SpecFlags SPEC_FLAG_UP       = 0x0002;
    static constexpr SpecFlags SPEC_FLAG_FMA      = 0x0004;

    AstNode* clone(CloneContext& context);
};

struct AstRange : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_EXCLUDE_UP = 0x0001;

    AstNode* clone(CloneContext& context);

    AstNode* expressionLow;
    AstNode* expressionUp;
};

struct AstMakePointer : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_TO_REF   = 0x0001;
    static constexpr SpecFlags SPEC_FLAG_DEP_TYPE = 0x0002;

    AstNode* clone(CloneContext& context);

    AstFuncDecl*      lambda;
    TypeInfoFuncAttr* deducedLambdaType;
    TypeInfoFuncAttr* tryLambdaType;
};

struct AstRefSubDecl : AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode* refSubDecl;
};

struct AstSubstBreakContinue : AstNode
{
    AstNode* clone(CloneContext& context) const;

    AstNode*      defaultSubst;
    AstNode*      altSubst;
    AstBreakable* altSubstBreakable;
};

struct AstDefer : AstNode
{
    AstNode* clone(CloneContext& context);

    DeferKind deferKind;
};

struct AstWith : AstNode
{
    AstNode* clone(CloneContext& context);

    Vector<Utf8> id;
};

struct AstLiteral : AstNode
{
    AstNode* clone(CloneContext& context);

    Register    literalValue;
    LiteralType literalType;
};

struct AstStatement : AstNode
{
    static constexpr SpecFlags SPEC_FLAG_NEED_SCOPE = 0x0001;
    static constexpr SpecFlags SPEC_FLAG_CURLY      = 0x0002;

    ~        AstStatement();
    AstNode* clone(CloneContext& context);
};

struct AstFile : AstNode
{
    AstNode* clone(CloneContext& context);

    uint32_t stackSize;
};
