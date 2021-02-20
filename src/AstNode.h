#pragma once
#include "DependentJobs.h"
#include "Pool.h"
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
    TypeSet,
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
    CompilerCode,
    CompilerImport,
    CompilerPlaceHolder,
    CompilerSpecialFunction,
    CompilerForeignLib,
    CompilerDefined,
    CompilerLoad,
    ConditionalExpression,
    NullConditionalExpression,
    Defer,
    Init,
    Drop,
    PostCopy,
    PostMove,
    Reloc,
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

    AstInline*          ownerInline            = nullptr;
    AstBreakable*       replaceTokensBreakable = nullptr;
    AstBreakable*       ownerBreakable         = nullptr;
    AstFuncDecl*        ownerFct               = nullptr;
    AstNode*            parent                 = nullptr;
    Scope*              parentScope            = nullptr;
    Scope*              ownerStructScope       = nullptr;
    AstNode*            ownerMainNode          = nullptr;
    AstCompilerIfBlock* ownerCompilerIfBlock   = nullptr;
    uint64_t            forceFlags             = 0;
    uint64_t            removeFlags            = 0;
    bool                rawClone               = false;
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

    void inheritTokenName(Token& tkn)
    {
        SWAG_ASSERT(!tkn.text.empty());
        token.text = move(tkn.text);
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
        ownerInline          = job->currentInline;
        flags |= job->currentFlags;
    }

    bool hasComputedValue()
    {
        return (flags & AST_VALUE_COMPUTED);
    }

    void setFlagsValueIsComputed()
    {
        flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED | AST_R_VALUE;
    }

    void inheritComputedValue(AstNode* from)
    {
        if (!from)
            return;
        inheritOrFlag(from, AST_VALUE_COMPUTED | AST_VALUE_IS_TYPEINFO);
        if (flags & AST_VALUE_COMPUTED)
        {
            flags |= AST_CONST_EXPR | AST_R_VALUE;
            computedValue = move(from->computedValue);
        }
    }

    bool isConstantTrue()
    {
        return (flags & AST_VALUE_COMPUTED) && computedValue.reg.b;
    }

    bool isConstantFalse()
    {
        return (flags & AST_VALUE_COMPUTED) && !computedValue.reg.b;
    }

    bool forceTakeAddress()
    {
        if ((flags & AST_TAKE_ADDRESS) && !(semFlags & AST_SEM_FORCE_NO_TAKE_ADDRESS))
            return true;
        if (semFlags & AST_SEM_FORCE_TAKE_ADDRESS)
            return true;
        return false;
    }

    bool isConstant0();
    bool isConstant1();

    bool             isParentOf(AstNode* child);
    bool             isSelectIfParam(SymbolOverload* overload);
    bool             isSameStackFrame(SymbolOverload* overload);
    void             setPassThrough();
    void             inheritLocationFromChilds();
    static Utf8      getArticleKindName(AstNode* node);
    static Utf8      getKindName(AstNode* node);
    AstNode*         findChildRef(AstNode* ref, AstNode* fromChild);
    AstNode*         findChildRefRec(AstNode* ref, AstNode* fromChild);
    virtual AstNode* clone(CloneContext& context);
    void             cloneChilds(CloneContext& context, AstNode* from);
    void             copyFrom(CloneContext& context, AstNode* from, bool cloneHie = true);
    Utf8             computeScopedName();
    Utf8             computeScopedNameForeign();
    bool             mustInline();
    void             setOwnerAttrUse(AstAttrUse* attrUse);
    void             allocateExtension();
    void             swap2Childs();
    bool             hasSpecialFuncCall();
    AstNode*         inSimpleReturn();
    bool             isSpecialFunctionName();

    struct Extension
    {
        VectorNative<Scope*>           alternativeScopes;
        VectorNative<AlternativeScope> alternativeScopesVars;
        SemanticFct                    semanticBeforeFct;
        SemanticFct                    semanticAfterFct;
        ByteCodeNotifyFct              byteCodeBeforeFct;
        ByteCodeNotifyFct              byteCodeAfterFct;
        ByteCodeGenJob*                byteCodeJob;
        ByteCode*                      bc;
        SymbolOverload*                resolvedUserOpSymbolOverload;
        TypeInfo*                      collectTypeInfo;
    };

    shared_mutex           mutex;
    Token                  token;
    VectorNative<AstNode*> childs;
    ComputedValue          computedValue;

    Scope*              ownerScope;
    AstBreakable*       ownerBreakable;
    AstAttrUse*         ownerAttrUse;
    AstInline*          ownerInline;
    AstFuncDecl*        ownerFct;
    Scope*              ownerStructScope;
    AstNode*            ownerMainNode;
    AstCompilerIfBlock* ownerCompilerIfBlock;
    TypeInfo*           typeInfo;
    TypeInfo*           castedTypeInfo;
    SymbolName*         resolvedSymbolName;
    SymbolOverload*     resolvedSymbolOverload;
    AstNode*            parent;
    SemanticFct         semanticFct;
    ByteCodeFct         byteCodeFct;
    SourceFile*         sourceFile;
    Extension*          extension;

    uint64_t flags;
    uint64_t attributeFlags;

    RegisterList resultRegisterRC;
    RegisterList additionalRegisterRC;

    uint32_t doneFlags;
    uint32_t semFlags;

    uint32_t castOffset;
    uint32_t concreteTypeInfoStorage;

    uint32_t            childParentIdx;
    AstNodeResolveState semanticState;
    AstNodeResolveState bytecodeState;
    AstNodeKind         kind;
};

struct AstVarDecl : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    SymbolAttributes attributes;
    Utf8             publicName;
    AstNode*         type;
    AstNode*         assignment;
    bool             constAssign;
};

struct AstIdentifierRef : public AstNode
{
    AstNode* clone(CloneContext& context) override;
    void     computeName();

    Scope*   startScope;
    AstNode* previousResolvedNode;
};

struct AstIdentifier : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    vector<Utf8> aliasNames;

    AstIdentifierRef* identifierRef;
    AstNode*          genericParameters;
    AstNode*          callParameters;
};

struct AstFuncDecl : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    DependentJobs          dependentJobs;
    Utf8                   fullnameForeign;
    VectorNative<AstNode*> subDecls;

    AstNode*       parameters;
    AstNode*       genericParameters;
    AstNode*       returnType;
    AstNode*       content;
    AstNode*       selectIf;
    Scope*         scope;
    TypeInfoParam* methodParam;
    Job*           pendingLambdaJob;

    uint32_t stackSize;
    int      exportForeignLine;
    int      numThrow;
    int      numTry;

    void computeFullNameForeign(bool forExport);
    Utf8 getNameForMessage();
    Utf8 getNameForUserCompiler();
};

struct AstAttrDecl : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstNode* parameters;
};

struct AstAttrUse : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstNode*         content;
    SymbolAttributes attributes;
    bool             isGlobal = false;
};

struct AstFuncCallParam : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    Utf8 namedParam;

    AstReturn*     autoTupleReturn;
    AstNode*       namedParamNode;
    TypeInfoParam* resolvedParameter;

    int  index;
    bool mustSortParameters;
};

struct AstBinaryOpNode : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    int seekJumpExpression;
};

struct AstConditionalOpNode : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    int seekJumpIfFalse;
    int seekJumpAfterIfFalse;
};

struct AstIf : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstNode* boolExpression;
    AstNode* ifBlock;
    AstNode* elseBlock;

    int seekJumpExpression;
    int seekJumpAfterIf;
};

struct AstBreakContinue : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    Utf8 label;

    AstSwitchCase* switchCase;

    int jumpInstruction;
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

    uint32_t breakableFlags = BREAKABLE_CAN_HAVE_INDEX | BREAKABLE_CAN_HAVE_CONTINUE;
    uint32_t registerIndex;
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

    AstNode* clone(CloneContext& context) override;

    AstNode* block;
};

struct AstWhile : public AstBreakable
{
    AstNode* clone(CloneContext& context) override;

    AstNode* boolExpression;
    AstNode* block;
};

struct AstFor : public AstBreakable
{
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
    AstNode* clone(CloneContext& context) override;

    AstNode* specificName;
    AstNode* expression;
    AstNode* block;
};

struct AstVisit : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    vector<Utf8> aliasNames;
    Token        extraNameToken;
    Token        wantPointerToken;

    AstNode* expression;
    AstNode* block;

    bool wantPointer;
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

    AstNode* expression;
    AstNode* block;
};

struct AstSwitchCase : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    VectorNative<AstNode*> expressions;

    AstNode*   block;
    AstSwitch* ownerSwitch;

    int caseIndex;

    bool isDefault;
};

struct AstSwitchCaseBlock : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstSwitchCase* ownerCase;

    int seekStart;
    int seekJumpNextCase;

    bool isDefault;
};

const uint16_t TYPEFLAG_ISREF   = 0x0001;
const uint16_t TYPEFLAG_ISSLICE = 0x0002;
const uint16_t TYPEFLAG_ISCONST = 0x0004;
//const uint16_t TYPEFLAG_ISPTRCONST  = 0x0008;
const uint16_t TYPEFLAG_ISTYPEOF    = 0x0010;
const uint16_t TYPEFLAG_ISCODE      = 0x0020;
const uint16_t TYPEFLAG_FORCECONST  = 0x0040;
const uint16_t TYPEFLAG_ISSELF      = 0x0080;
const uint16_t TYPEFLAG_RETVAL      = 0x0100;
const uint16_t TYPEFLAG_ISNAMEALIAS = 0x0200;

struct AstTypeExpression : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstNode*  identifier;
    TypeInfo* literalType;

    uint16_t             typeFlags;
    static const int     MAX_PTR_COUNT = 4;
    static const uint8_t PTR_CONST     = 0x01;
    uint8_t              ptrFlags[MAX_PTR_COUNT];
    uint8_t              ptrRel[MAX_PTR_COUNT];
    AstNode*             ptrRelIds[MAX_PTR_COUNT];
    uint8_t              ptrCount;
    uint8_t              arrayDim;
    uint8_t              relValue;
    AstNode*             relId;
};

struct AstTypeLambda : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstNode* parameters;
    AstNode* returnType;

    bool canThrow;
};

struct AstArrayPointerIndex : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    VectorNative<AstNode*> structFlatParams;

    AstNode* array;
    AstNode* access;
    bool     isDeref;
};

struct AstArrayPointerSlicing : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    VectorNative<AstNode*> structFlatParams;

    AstNode* array;
    AstNode* lowerBound;
    AstNode* upperBound;
};

struct AstIntrinsicProp : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    bool typeOfAsType;
    bool typeOfAsConst;
};

struct AstExpressionList : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    bool forTuple;
};

struct AstStruct : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    DependentJobs dependentJobs;

    AstNode* genericParameters;
    AstNode* content;
    Scope*   scope;
    AstNode* ownerGeneric;
    AstNode* originalParent;

    uint32_t packing = sizeof(uint64_t);
};

struct AstEnum : public AstNode
{
    AstNode* clone(CloneContext& context) override;
    Scope*   scope;
};

struct AstImpl : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    Scope*   structScope;
    Scope*   scope;
    AstNode* identifier;
    AstNode* identifierFor;
};

struct AstInit : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstNode* expression;
    AstNode* count;
    AstNode* parameters;
};

struct AstDropCopyMove : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstNode* expression;
    AstNode* count;
};

struct AstReloc : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    AstNode* expression1;
    AstNode* expression2;
    AstNode* count;
};

struct AstReturn : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    VectorNative<SymbolOverload*> forceNoDrop;

    int seekJump;
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

    AstFuncDecl* func;
    Scope*       scope;
    Scope*       parametersScope;
};

struct AstCompilerIfBlock : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    void addSymbol(AstNode* node, SymbolName* symbolName)
    {
        scoped_lock lk(mutex);
        symbols.push_back({node, symbolName});
    }

    VectorNative<AstCompilerIfBlock*>         blocks;
    VectorNative<pair<AstNode*, SymbolName*>> symbols;
    VectorNative<TypeInfoStruct*>             interfacesCount;
    VectorNative<pair<TypeInfoStruct*, int>>  methodsCount;
    int                                       numTestErrors;
    int                                       numThrow;
    int                                       numTry;
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
    AstNode* clone(CloneContext& context) override;

    CompilerAstKind embeddedKind;
};

struct AstNameSpace : public AstNode
{
    AstNode* clone(CloneContext& context) override;

    Utf8 importedModuleName;
};

struct AstTryCatch : public AstReturn
{
    AstNode* clone(CloneContext& context) override;

    RegisterList regInit;
    int          seekInsideJump;
};
