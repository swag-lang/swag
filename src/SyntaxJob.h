#pragma once
#include "Tokenizer.h"
#include "Job.h"
#include "AstFlags.h"
#include "Scope.h"
struct SourceFile;
struct AstVarDecl;
struct AstNode;
struct AstBreakable;
struct AstIdentifier;
struct AstIdentifierRef;
struct Scope;
struct Utf8;
struct AstFuncDecl;
struct AstStruct;
struct AstCompilerIfBlock;
struct AstTryCatchAssume;
struct SyntaxJob;
struct AstAttrUse;
struct AstInline;
struct AstFuncCallParams;
struct AstWith;
struct AstTypeLambda;

enum class AstNodeKind : uint8_t;

enum class InvalidTokenError
{
    EmbeddedInstruction,
    TopLevelInstruction,
    PrimaryExpression,
    LeftExpression,
    LeftExpressionVar,
};

enum class SyntaxStructType
{
    Struct,
    Interface,
    Tuple
};

static const uint32_t MODIFIER_SAFE          = 0x00000001;
static const uint32_t MODIFIER_SMALL         = 0x00000002;
static const uint32_t MODIFIER_NO_LEFT_DROP  = 0x00000004;
static const uint32_t MODIFIER_MOVE          = 0x00000008;
static const uint32_t MODIFIER_NO_RIGHT_DROP = 0x00000010;
static const uint32_t MODIFIER_BIT           = 0x00000020;

static const uint32_t IDENTIFIER_NO_FCT_PARAMS   = 0x00000001;
static const uint32_t IDENTIFIER_NO_GEN_PARAMS   = 0x00000002;
static const uint32_t IDENTIFIER_TYPE_DECL       = 0x00000004;
static const uint32_t IDENTIFIER_ACCEPT_QUESTION = 0x00000008;
static const uint32_t IDENTIFIER_NO_ARRAY        = 0x00000010;
static const uint32_t IDENTIFIER_NO_PARAMS       = IDENTIFIER_NO_FCT_PARAMS | IDENTIFIER_NO_GEN_PARAMS | IDENTIFIER_NO_ARRAY;

static const uint32_t EXPR_FLAG_NONE        = 0x00000000;
static const uint32_t EXPR_FLAG_SIMPLE      = 0x00000001;
static const uint32_t EXPR_FLAG_ALIAS       = 0x00000002;
static const uint32_t EXPR_FLAG_TYPEOF      = 0x00000004;
static const uint32_t EXPR_FLAG_STOP_AFFECT = 0x00000008;
static const uint32_t EXPR_FLAG_PARAMETER   = 0x00000010;

static const uint32_t CONTEXT_FLAG_EXPRESSION              = 0x00000001;
static const uint32_t CONTEXT_FLAG_VARDECL_TYPE_EXPRESSION = 0x00000002;

struct SyntaxContext : public JobContext
{
    SyntaxJob* job = nullptr;
};

struct SyntaxJob : public Job
{
    void release() override
    {
        g_Allocator.free<SyntaxJob>(this);
    }

    JobResult execute() override;
    bool      saveEmbedded(const Utf8& content, AstNode* parent, AstNode* fromNode, Utf8& tmpFileName, Utf8& tmpFilePath, uint32_t& previousLogLine);
    bool      constructEmbedded(const Utf8& content, AstNode* parent, AstNode* fromNode, enum class CompilerAstKind kind, bool logGenerated);

    bool error(const Token& tk, const Utf8& msg, const char* help = nullptr, const char* hint = nullptr);
    bool error(AstNode* node, const Utf8& msg, const char* help = nullptr, const char* hint = nullptr);
    bool error(const SourceLocation& startLocation, const SourceLocation& endLocation, const Utf8& msg, const char* help = nullptr);
    bool invalidTokenError(InvalidTokenError kind);

    bool eatToken();
    bool eatCloseToken(TokenId id, const SourceLocation& start, const char* msg);
    bool eatToken(TokenId id, const char* msg = nullptr);
    bool eatSemiCol(const char* msg = nullptr);

    bool        testIsSingleIdentifier(AstNode* node);
    bool        checkIsSingleIdentifier(AstNode* node, const char* msg);
    bool        testIsValidUserName(AstNode* node);
    bool        checkIsValidUserName(AstNode* node, Token* loc = nullptr);
    bool        checkIsValidVarName(AstNode* node);
    void        registerSubDecl(AstNode* subDecl);
    static void relaxIdentifier(Token& token);
    static void forceTakeAddress(AstNode* node);

    bool doTupleOrAnonymousType(AstNode* parent, AstNode** result, bool isConst, bool anonymousStruct, bool anonymousUnion);
    bool doCompilerScopeBreakable(AstNode* parent, AstNode** result = nullptr);
    bool doGenericFuncCallParameters(AstNode* parent, AstNode** result = nullptr);
    bool doFuncCallParameters(AstNode* parent, AstFuncCallParams** result, TokenId closeToken);
    bool doIntrinsicTag(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerIfFor(AstNode* parent, AstNode** result, AstNodeKind kind);
    bool doCompilerIf(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerInline(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerMacro(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerMixin(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerAssert(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerPrint(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerError(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerWarning(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerAst(AstNode* parent, AstNode** result);
    bool doCompilerValidIf(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerRunTopLevel(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerRunEmbedded(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerForeignLib(AstNode* parent, AstNode** result);
    bool doCompilerGlobal(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerLocation(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerSpecialFunction(AstNode* parent, AstNode** result = nullptr);
    bool doIntrinsicDefined(AstNode* parent, AstNode** result = nullptr);
    bool doIntrinsicLocation(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerDependencies(AstNode* parent);
    bool doCompilerInclude(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerLoad(AstNode* parent);
    bool doCompilerImport(AstNode* parent);
    bool doCompilerPlaceHolder(AstNode* parent);
    bool doTopLevelInstruction(AstNode* parent, AstNode** result = nullptr);
    bool doVarDecl(AstNode* parent, AstNode** result = nullptr);
    bool doVarDecl(AstNode* parent, AstNode** result, AstNodeKind kind);
    bool doAlias(AstNode* parent, AstNode** result = nullptr);
    bool doTypeExpression(AstNode* parent, AstNode** result = nullptr, bool inTypeVarDecl = false);
    bool doLambdaClosureType(AstNode* parent, AstNode** result = nullptr, bool inTypeVarDecl = false);
    bool doLambdaClosureTypePriv(AstTypeLambda* node, AstNode** result, bool inTypeVarDecl);
    bool doDefer(AstNode* parent, AstNode** result = nullptr);
    bool doVarDeclExpression(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, const Token& assignToken, AstNodeKind kind, AstNode** result);
    bool doAffectExpression(AstNode* parent, AstNode** result = nullptr, AstWith* withNode = nullptr);
    bool doIdentifier(AstNode* parent, uint32_t identifierFlags = 0);
    bool doIdentifierRef(AstNode* parent, AstNode** result = nullptr, uint32_t identifierFlags = 0);
    bool doDiscard(AstNode* parent, AstNode** result = nullptr);
    bool doTryCatchAssume(AstNode* parent, AstNode** result = nullptr, bool afterDiscard = false);
    bool doThrow(AstNode* parent, AstNode** result = nullptr);
    bool doGlobalAttributeExpose(AstNode* parent, AstNode** result, bool forGlobal);
    bool doNamespace(AstNode* parent, AstNode** result = nullptr);
    bool doNamespace(AstNode* parent, AstNode** result, bool forGlobal, bool forUsing);
    bool doNamespaceOnName(AstNode* parent, AstNode** result, bool forGlobal, bool forUsing, Token* privName = nullptr);
    bool doEnumContent(AstNode* parent, AstNode** result = nullptr);
    bool doEnumValue(AstNode* parent, AstNode** result = nullptr);
    bool doEnum(AstNode* parent, AstNode** result = nullptr);
    bool doTupleBody(AstNode* parent, bool acceptEmpty);
    bool doStructBody(AstNode* parent, SyntaxStructType structType, AstNode** result = nullptr);
    bool doStruct(AstNode* parent, AstNode** result = nullptr);
    bool doStructContent(AstStruct* structNode, SyntaxStructType structType);
    bool doImpl(AstNode* parent, AstNode** result = nullptr);
    bool doAssignmentExpression(AstNode* parent, AstNode** result = nullptr);
    bool doExpressionListTuple(AstNode* parent, AstNode** result = nullptr);
    bool doExpressionListArray(AstNode* parent, AstNode** result = nullptr);
    bool doInitializationExpression(Token& forToken, AstNode* parent, AstNode** result = nullptr);
    bool doLiteral(AstNode* parent, AstNode** result = nullptr);
    bool doIntrinsicProp(AstNode* parent, AstNode** result = nullptr);
    bool doGetErr(AstNode* parent, AstNode** result = nullptr);
    bool doIndex(AstNode* parent, AstNode** result = nullptr);
    bool doKeepRef(AstNode* parent, AstNode** result = nullptr);
    bool doDeRef(AstNode* parent, AstNode** result = nullptr);
    bool doOperatorPrecedence(AstNode** result);
    bool doModifiers(Token& forNode, uint32_t& mdfFlags);
    bool doSinglePrimaryExpression(AstNode* parent, uint32_t exprFlags, AstNode** result = nullptr);
    bool doPrimaryExpression(AstNode* parent, uint32_t exprFlags, AstNode** result = nullptr);
    bool doUnaryExpression(AstNode* parent, uint32_t exprFlags, AstNode** result = nullptr);
    bool doFactorExpression(AstNode** parent, uint32_t exprFlags, AstNode** result = nullptr);
    bool doCompareExpression(AstNode* parent, uint32_t exprFlags, AstNode** result = nullptr);
    bool doBoolExpression(AstNode* parent, uint32_t exprFlags, AstNode** result = nullptr);
    bool doExpression(AstNode* parent, uint32_t exprFlags, AstNode** result = nullptr);
    bool doMoveExpression(Token& forToken, AstNode* parent, AstNode** result = nullptr);
    bool doGenericDeclParameters(AstNode* parent, AstNode** result = nullptr);
    bool doLambdaFuncDecl(AstNode* parent, AstNode** result = nullptr, bool acceptMissingType = false, bool* hasMissingType = nullptr);
    bool doFuncDecl(AstNode* parent, AstNode** result = nullptr, TokenId typeFuncId = TokenId::Invalid);
    bool doFuncDeclParameter(AstNode* parent, bool acceptMissingType = false, bool* hasMissingType = nullptr);
    bool doFuncDeclParameters(AstNode* parent, AstNode** result = nullptr, bool acceptMissingType = false, bool* hasMissingType = nullptr, bool isMethod = false, bool isConstMethod = false, bool isItfMethod = false);
    bool doAttrDecl(AstNode* parent, AstNode** result = nullptr);
    bool doAttrUse(AstNode* parent, AstNode** result = nullptr, bool single = false);
    bool doEmbeddedInstruction(AstNode* parent, AstNode** result = nullptr);
    bool doEmbeddedStatement(AstNode* parent, AstNode** result = nullptr);
    bool doStatementFor(AstNode* parent, AstNode** result, AstNodeKind kind);
    bool doStatement(AstNode* parent, AstNode** result = nullptr);
    bool doGlobalCurlyStatement(AstNode* parent, AstNode** result = nullptr);
    bool doCurlyStatement(AstNode* parent, AstNode** result = nullptr);
    bool doScopedCurlyStatement(AstNode* parent, AstNode** result = nullptr, ScopeKind scopeKind = ScopeKind::Statement);
    bool doReturn(AstNode* parent, AstNode** result = nullptr);
    bool doLambdaExpression(AstNode* parent, AstNode** result = nullptr);
    bool doWith(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerScopeFile(AstNode* parent, AstNode** result = nullptr);
    bool doUsing(AstNode* parent, AstNode** result = nullptr);
    bool doCast(AstNode* parent, AstNode** result = nullptr);
    bool doAutoCast(AstNode* parent, AstNode** result = nullptr);
    bool doIf(AstNode* parent, AstNode** result = nullptr);
    bool doWhile(AstNode* parent, AstNode** result = nullptr);
    bool doFor(AstNode* parent, AstNode** result = nullptr);
    bool doLoop(AstNode* parent, AstNode** result = nullptr);
    bool doVisit(AstNode* parent, AstNode** result = nullptr);
    bool doSwitch(AstNode* parent, AstNode** result = nullptr);
    bool doBreak(AstNode* parent, AstNode** result = nullptr);
    bool doFallThrough(AstNode* parent, AstNode** result = nullptr);
    bool doContinue(AstNode* parent, AstNode** result = nullptr);
    bool doArrayPointerIndex(AstNode** exprNode);
    bool doLeftInstruction(AstNode* parent, AstNode** result = nullptr, AstWith* withNode = nullptr);
    bool doLeftExpressionVar(AstNode* parent, AstNode** result, uint32_t identifierFlags = 0, AstWith* withNode = nullptr);
    bool doLeftExpressionAffect(AstNode* parent, AstNode** result = nullptr, AstWith* withNode = nullptr);
    bool doInit(AstNode* parent, AstNode** result = nullptr);
    bool doDropCopyMove(AstNode* parent, AstNode** result = nullptr);
    bool doRange(AstNode* parent, AstNode* expression, AstNode** result = nullptr);

    SyntaxContext context;
    Tokenizer     tokenizer;
    Token         token;

    Scope*              currentScope           = nullptr;
    AstFuncDecl*        currentFct             = nullptr;
    AstBreakable*       currentBreakable       = nullptr;
    Scope*              currentStructScope     = nullptr;
    Scope*              currentSelfStructScope = nullptr;
    AstCompilerIfBlock* currentCompilerIfBlock = nullptr;
    AstTryCatchAssume*  currentTryCatchAssume  = nullptr;
    Token*              currentTokenLocation   = nullptr;
    AstInline*          currentInline          = nullptr;

    uint64_t currentFlags = 0;
    uint64_t contextFlags = 0;

    uint32_t contextualNoInline = 0;

    bool moduleSpecified = false;
    bool inFunCall       = false;
    bool afterGlobal     = false;
    bool setupDone       = false;
};

struct PushSyntaxContextFlags
{
    PushSyntaxContextFlags(SyntaxJob* job, uint64_t flags)
        : job{job}
    {
        oldFlags = job->contextFlags;
        job->contextFlags |= flags;
    }

    ~PushSyntaxContextFlags()
    {
        job->contextFlags = oldFlags;
    }

    SyntaxJob* job;
    uint64_t   oldFlags;
};