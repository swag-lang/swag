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
#include "ScopedLock.h"

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
    Namespace,
    Using,
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
    FuncDeclParams,
    FuncDeclParam,
    FuncDeclType,
    FuncDeclGenericParams,
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
    CompilerSemError,
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
    CompilerPlaceHolder,
    CompilerSpecialFunction,
    CompilerForeignLib,
    CompilerDefined,
    CompilerLoad,
    ConditionalExpression,
    NullConditionalExpression,
    Defer,
    ErrDefer,
    Init,
    Drop,
    PostCopy,
    PostMove,
    Try,
    Catch,
    Assume,
    Throw,
};

struct CloneContext
{
    map<Utf8, TypeInfo*>   replaceTypes;
    map<TokenId, AstNode*> replaceTokens;
    map<Utf8, Utf8>        replaceNames;
    set<Utf8>              usedReplaceNames;

    AstInline*          ownerInline            = nullptr;
    AstBreakable*       replaceTokensBreakable = nullptr;
    AstBreakable*       ownerBreakable         = nullptr;
    AstFuncDecl*        ownerFct               = nullptr;
    AstTryCatchAssume*  ownerTryCatchAssume    = nullptr;
    AstNode*            parent                 = nullptr;
    Scope*              parentScope            = nullptr;
    Scope*              ownerStructScope       = nullptr;
    Scope*              alternativeScope       = nullptr;
    AstCompilerIfBlock* ownerCompilerIfBlock   = nullptr;
    Token*              forceLocation          = nullptr;
    uint64_t            forceFlags             = 0;
    uint64_t            removeFlags            = 0;
    bool                rawClone               = false;

    void propageResult(CloneContext& context)
    {
        usedReplaceNames.insert(context.usedReplaceNames.begin(), context.usedReplaceNames.end());
    }
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
    void inheritOwners(AstNode* op);
    void inheritOwnersAndFlags(SyntaxJob* job);
    void inheritLocationFromChilds();

    bool hasComputedValue();
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

    void        setPassThrough();
    static Utf8 getArticleKindName(AstNode* node);
    static Utf8 getKindName(AstNode* node);
    AstNode*    findChildRef(AstNode* ref, AstNode* fromChild);
    AstNode*    findChildRefRec(AstNode* ref, AstNode* fromChild);
    AstNode*    findParent(AstNodeKind parentKind);
    AstNode*    findParent(AstNodeKind parentKind1, AstNodeKind parentKind2);
    Utf8        getScopedName();
    void        setOwnerAttrUse(AstAttrUse* attrUse);
    void        allocateExtension();
    void        swap2Childs();
    bool        hasSpecialFuncCall();
    bool        mustInline();
    AstNode*    inSimpleReturn();
    bool        forceTakeAddress();

    struct Extension
    {
        VectorNative<Scope*>           alternativeScopes;
        VectorNative<AlternativeScope> alternativeScopesVars;
        VectorNative<uint32_t>         registersToRelease;
        SemanticFct                    semanticBeforeFct;
        SemanticFct                    semanticAfterFct;
        ByteCodeNotifyFct              byteCodeBeforeFct;
        ByteCodeNotifyFct              byteCodeAfterFct;
        ByteCodeGenJob*                byteCodeJob;
        ByteCode*                      bc;
        SymbolOverload*                resolvedUserOpSymbolOverload;
        TypeInfo*                      collectTypeInfo;
        AstNode*                       alternativeNode;
        AstNode*                       exportNode;
        DataSegment*                   anyTypeSegment;
        AstAttrUse*                    ownerAttrUse;
        AstTryCatchAssume*             ownerTryCatchAssume;

        uint32_t castOffset;
        uint32_t stackOffset;
        uint32_t anyTypeOffset;
    };

    AstNodeKind         kind;
    AstNodeResolveState semanticState;
    AstNodeResolveState bytecodeState;
    uint8_t             specFlags;
    uint32_t            childParentIdx;

    shared_mutex           mutex;
    Token                  token;
    VectorNative<AstNode*> childs;
    ComputedValue*         computedValue;

    Scope*              ownerScope;
    Scope*              ownerStructScope;
    AstBreakable*       ownerBreakable;
    AstInline*          ownerInline;
    AstFuncDecl*        ownerFct;
    AstCompilerIfBlock* ownerCompilerIfBlock;

    TypeInfo* typeInfo;
    TypeInfo* castedTypeInfo;

    SymbolName*     resolvedSymbolName;
    SymbolOverload* resolvedSymbolOverload;

    AstNode*    parent;
    SourceFile* sourceFile;
    Extension*  extension;

    SemanticFct semanticFct;
    ByteCodeFct byteCodeFct;

    uint64_t flags;
    uint64_t attributeFlags;

    RegisterList resultRegisterRC;
    RegisterList additionalRegisterRC;

    uint32_t doneFlags;
    uint32_t semFlags;
};

struct AstVarDecl : public AstNode
{
    AstNode* clone(CloneContext& context);

    AttributeList attributes;
    Utf8          publicName;

    AstNode* type;
    AstNode* assignment;
};

struct AstIdentifierRef : public AstNode
{
    AstNode* clone(CloneContext& context);
    void     computeName();

    Scope*   startScope;
    AstNode* previousResolvedNode;
};

struct AstIdentifier : public AstNode
{
    AstNode* clone(CloneContext& context);

    vector<Token> aliasNames;

    AstIdentifierRef*  identifierRef;
    AstNode*           genericParameters;
    AstFuncCallParams* callParameters;
};

struct AstFuncDecl : public AstNode
{
    AstNode* clone(CloneContext& context);
    bool     cloneSubDecls(JobContext* context, CloneContext& cloneContext, AstNode* oldOwnerNode, AstFuncDecl* newFctNode, AstNode* refNode);
    void     computeFullNameForeign(bool forExport);
    Utf8     getDisplayName();
    Utf8     getNameForUserCompiler();

    bool isForeign()
    {
        return attributeFlags & ATTRIBUTE_FOREIGN;
    }

    DependentJobs          dependentJobs;
    Utf8                   fullnameForeign;
    VectorNative<AstNode*> subDecls;
    VectorNative<AstNode*> localGlobalVars;

    AstNode*       parameters;
    AstNode*       genericParameters;
    AstNode*       returnType;
    AstNode*       content;
    AstNode*       selectIf;
    Scope*         scope;
    TypeInfoParam* methodParam;
    Job*           pendingLambdaJob;

    uint32_t aliasMask;
    uint32_t stackSize;
    int      exportForeignLine;
};

struct AstAttrDecl : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode* parameters;
};

struct AstAttrUse : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode*      content;
    AttributeList attributes;
};

struct AstFuncCallParams : public AstNode
{
    AstNode*      clone(CloneContext& context);
    vector<Token> aliasNames;
};

struct AstFuncCallParam : public AstNode
{
    AstNode* clone(CloneContext& context);

    Utf8 namedParam;

    AstReturn*     autoTupleReturn;
    AstNode*       namedParamNode;
    TypeInfoParam* resolvedParameter;

    int indexParam;
};

struct AstBinaryOpNode : public AstNode
{
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

    Utf8 label;

    AstSwitchCase* switchCase;

    int jumpInstruction;
};

const uint32_t BREAKABLE_CAN_HAVE_INDEX    = 0x00000001;
const uint32_t BREAKABLE_CAN_HAVE_CONTINUE = 0x00000002;
const uint32_t BREAKABLE_NEED_INDEX        = 0x00000004;
const uint32_t BREAKABLE_NEED_INDEX1       = 0x00000008;

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

    uint32_t breakableFlags = BREAKABLE_CAN_HAVE_INDEX | BREAKABLE_CAN_HAVE_CONTINUE;
    uint32_t registerIndex;
    uint32_t registerIndex1;
    int      seekJumpBeforeContinue;
    int      seekJumpBeforeExpression;
    int      seekJumpExpression;
    int      seekJumpAfterBlock;
};

struct AstLabelBreakable : public AstBreakable
{
    AstLabelBreakable()
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
    AstNode* clone(CloneContext& context);

    AstNode* preExpression;
    AstNode* boolExpression;
    AstNode* postExpression;
    AstNode* block;

    int seekJumpToExpression;
};

struct AstLoop : public AstBreakable
{
    AstNode* clone(CloneContext& context);

    AstNode* specificName;
    AstNode* expression;
    AstNode* block;
};

struct AstVisit : public AstNode
{
    AstNode* clone(CloneContext& context);

    vector<Token> aliasNames;
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

    AstNode*  expression;
    TypeInfo* beforeAutoCastType;
};

struct AstSwitchCase : public AstNode
{
    AstNode* clone(CloneContext& context);

    VectorNative<AstNode*> expressions;

    AstNode*   block;
    AstSwitch* ownerSwitch;

    int caseIndex;
};

struct AstSwitchCaseBlock : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstSwitchCase* ownerCase;

    int seekStart;
    int seekJumpNextCase;
};

const uint16_t TYPEFLAG_ISREF       = 0x0001;
const uint16_t TYPEFLAG_ISSLICE     = 0x0002;
const uint16_t TYPEFLAG_ISCONST     = 0x0004;
const uint16_t TYPEFLAG_ISCODE      = 0x0008;
const uint16_t TYPEFLAG_FORCECONST  = 0x0010;
const uint16_t TYPEFLAG_ISSELF      = 0x0020;
const uint16_t TYPEFLAG_RETVAL      = 0x0040;
const uint16_t TYPEFLAG_ISNAMEALIAS = 0x0080;
const uint16_t TYPEFLAG_USING       = 0x0100;

struct AstTypeExpression : public AstNode
{
    static const int     MAX_PTR_COUNT = 4;
    static const uint8_t PTR_CONST     = 0x01;
    static const uint8_t PTR_REF       = 0x02;

    AstNode* clone(CloneContext& context);

    AstNode*  identifier;
    TypeInfo* literalType;

    uint16_t typeFlags;
    uint8_t  ptrFlags[MAX_PTR_COUNT];
    uint8_t  ptrCount;
    uint8_t  arrayDim;
};

struct AstTypeLambda : public AstNode
{
    AstNode* clone(CloneContext& context);

    AstNode* parameters;
    AstNode* returnType;
};

struct AstArrayPointerIndex : public AstNode
{
    AstNode* clone(CloneContext& context);

    VectorNative<AstNode*> structFlatParams;

    AstNode* array;
    AstNode* access;
};

struct AstArrayPointerSlicing : public AstNode
{
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
};

const uint32_t STRUCTFLAG_UNION     = 0x00000001;
const uint32_t STRUCTFLAG_ANONYMOUS = 0x00000002;

struct AstStruct : public AstNode
{
    AstNode* clone(CloneContext& context);

    DependentJobs dependentJobs;

    AstNode* genericParameters;
    AstNode* content;
    Scope*   scope;
    AstNode* ownerGeneric;
    AstNode* originalParent;

    uint32_t packing = sizeof(uint64_t);
    uint32_t structFlags;
};

struct AstEnum : public AstNode
{
    AstNode* clone(CloneContext& context);
    Scope*   scope;
};

struct AstEnumValue : public AstNode
{
    AstNode*      clone(CloneContext& context);
    AttributeList attributes;
};

struct AstImpl : public AstNode
{
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
    int          seekJump;
};

struct AstCompilerInline : public AstNode
{
    AstNode* clone(CloneContext& context);

    Scope* scope;
};

struct AstCompilerMacro : public AstNode
{
    AstNode* clone(CloneContext& context);

    Scope* scope;
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
    VectorNative<pair<TypeInfoStruct*, int>>  methodsCount;
    VectorNative<AstNode*>                    subDecls;
    VectorNative<AstNode*>                    imports;
    int                                       numTestErrors;
    int                                       numTestWarnings;
};

enum class CompilerAstKind
{
    EmbeddedInstruction,
    TopLevelInstruction,
    StructVarDecl,
    EnumValue,
};

struct AstCompilerSpecFunc : public AstNode
{
    AstNode* clone(CloneContext& context);

    CompilerAstKind embeddedKind;
};

struct AstNameSpace : public AstNode
{
    AstNode* clone(CloneContext& context);
};

struct AstTryCatchAssume : public AstReturn
{
    AstNode* clone(CloneContext& context);

    RegisterList regInit;
    int          seekInsideJump;
};

struct AstAlias : public AstNode
{
    AstNode* clone(CloneContext& context);
};

struct AstCast : public AstNode
{
    AstNode* clone(CloneContext& context);

    TypeInfo* toCastTypeInfo;
};

struct AstOp : public AstNode
{
};

struct AstRange : public AstNode
{
    AstNode* clone(CloneContext& context);
    AstNode* expressionLow;
    AstNode* expressionUp;
};

struct AstMakePointerLambda : public AstNode
{
    AstNode* clone(CloneContext& context);
    AstNode* lambda;
};