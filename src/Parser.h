#pragma once
#include "Tokenizer.h"
#include "AstFlags.h"
#include "Scope.h"
#include "ErrorContext.h"
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
struct Parser;
struct AstAttrUse;
struct AstInline;
struct AstFuncCallParams;
struct AstWith;
struct AstTypeLambda;
struct Module;
struct JobContext;

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

const uint32_t MODIFIER_SAFE          = 0x00000001;
const uint32_t MODIFIER_SMALL         = 0x00000002;
const uint32_t MODIFIER_NO_LEFT_DROP  = 0x00000004;
const uint32_t MODIFIER_MOVE          = 0x00000008;
const uint32_t MODIFIER_NO_RIGHT_DROP = 0x00000010;
const uint32_t MODIFIER_BIT           = 0x00000020;

const uint32_t IDENTIFIER_NO_FCT_PARAMS   = 0x00000001;
const uint32_t IDENTIFIER_NO_GEN_PARAMS   = 0x00000002;
const uint32_t IDENTIFIER_TYPE_DECL       = 0x00000004;
const uint32_t IDENTIFIER_ACCEPT_QUESTION = 0x00000008;
const uint32_t IDENTIFIER_NO_ARRAY        = 0x00000010;
const uint32_t IDENTIFIER_NO_PARAMS       = IDENTIFIER_NO_FCT_PARAMS | IDENTIFIER_NO_GEN_PARAMS | IDENTIFIER_NO_ARRAY;
const uint32_t IDENTIFIER_GLOBAL          = 0x00000020;

const uint32_t EXPR_FLAG_NONE        = 0x00000000;
const uint32_t EXPR_FLAG_SIMPLE      = 0x00000001;
const uint32_t EXPR_FLAG_ALIAS       = 0x00000002;
const uint32_t EXPR_FLAG_TYPEOF      = 0x00000004;
const uint32_t EXPR_FLAG_STOP_AFFECT = 0x00000008;
const uint32_t EXPR_FLAG_PARAMETER   = 0x00000010;
const uint32_t EXPR_FLAG_IN_CALL     = 0x00000020;

const uint32_t CONTEXT_FLAG_EXPRESSION = 0x00000001;

struct Parser
{
    void setup(ErrorContext* errorCxt, Module* mdl, SourceFile* file);
    bool generateAst();

    bool saveEmbeddedAst(const Utf8& content, AstNode* fromNode, Path& tmpFilePath, Utf8& tmpFileName, uint32_t& previousLogLine);
    bool constructEmbeddedAst(const Utf8& content, AstNode* parent, AstNode* fromNode, enum class CompilerAstKind kind, bool logGenerated);

    bool error(const Token& tk, const Utf8& msg, const char* help = nullptr, const char* hint = nullptr);
    bool error(AstNode* node, const Utf8& msg, const char* help = nullptr, const char* hint = nullptr);
    bool error(const SourceLocation& startLocation, const SourceLocation& endLocation, const Utf8& msg, const char* help = nullptr);
    bool invalidTokenError(InvalidTokenError kind);

    bool eatToken();
    bool eatCloseToken(TokenId id, const SourceLocation& start, const char* msg = "");
    bool eatToken(TokenId id, const char* msg = "");
    bool eatSemiCol(const char* msg = nullptr);

    bool        testIsSingleIdentifier(AstNode* node);
    bool        checkIsSingleIdentifier(AstNode* node, const char* msg);
    bool        testIsValidUserName(AstNode* node);
    bool        checkIsValidUserName(AstNode* node, Token* loc = nullptr);
    bool        checkIsValidVarName(AstNode* node);
    void        registerSubDecl(AstNode* subDecl);
    static void relaxIdentifier(TokenParse& token);
    static void forceTakeAddress(AstNode* node);

    bool doTupleOrAnonymousType(AstNode* parent, AstNode** result, bool isConst, bool anonymousStruct, bool anonymousUnion);
    bool doCompilerScopeBreakable(AstNode* parent, AstNode** result);
    bool doGenericFuncCallParameters(AstNode* parent, AstFuncCallParams** result);
    bool doFuncCallParameters(AstNode* parent, AstFuncCallParams** result, TokenId closeToken);
    bool doIntrinsicTag(AstNode* parent, AstNode** result);
    bool doCompilerIfFor(AstNode* parent, AstNode** result, AstNodeKind kind);
    bool doCompilerIf(AstNode* parent, AstNode** result);
    bool doCompilerMacro(AstNode* parent, AstNode** result);
    bool doCompilerMixin(AstNode* parent, AstNode** result);
    bool doCompilerAssert(AstNode* parent, AstNode** result);
    bool doCompilerPrint(AstNode* parent, AstNode** result);
    bool doCompilerError(AstNode* parent, AstNode** result);
    bool doCompilerWarning(AstNode* parent, AstNode** result);
    bool doCompilerAst(AstNode* parent, AstNode** result);
    bool doCompilerValidIf(AstNode* parent, AstNode** result);
    bool doCompilerRunTopLevel(AstNode* parent, AstNode** result);
    bool doCompilerRunEmbedded(AstNode* parent, AstNode** result);
    bool doCompilerForeignLib(AstNode* parent, AstNode** result);
    bool doCompilerGlobal(AstNode* parent, AstNode** result);
    bool doCompilerSpecialValue(AstNode* parent, AstNode** result);
    bool doIntrinsicDefined(AstNode* parent, AstNode** result);
    bool doIntrinsicLocation(AstNode* parent, AstNode** result);
    bool doCompilerDependencies(AstNode* parent);
    bool doCompilerInclude(AstNode* parent, AstNode** result);
    bool doCompilerLoad(AstNode* parent);
    bool doCompilerImport(AstNode* parent);
    bool doCompilerPlaceHolder(AstNode* parent);
    bool doTopLevelInstruction(AstNode* parent, AstNode** result);
    bool doVarDecl(AstNode* parent, AstNode** result);
    bool doVarDecl(AstNode* parent, AstNode** result, AstNodeKind kind);
    bool doAlias(AstNode* parent, AstNode** result);
    bool doTypeExpression(AstNode* parent, AstNode** result, bool inTypeVarDecl = false);
    bool doLambdaClosureType(AstNode* parent, AstNode** result, bool inTypeVarDecl = false);
    bool doLambdaClosureTypePriv(AstTypeLambda* node, AstNode** result, bool inTypeVarDecl);
    bool doDefer(AstNode* parent, AstNode** result);
    bool doVarDeclExpression(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, const TokenParse& assignToken, AstNodeKind kind, AstNode** result);
    bool doAffectExpression(AstNode* parent, AstNode** result, AstWith* withNode = nullptr);
    bool doIdentifier(AstNode* parent, uint32_t identifierFlags = 0);
    bool doIdentifierRef(AstNode* parent, AstNode** result, uint32_t identifierFlags = 0);
    bool doDiscard(AstNode* parent, AstNode** result);
    bool doTryCatchAssume(AstNode* parent, AstNode** result, bool afterDiscard = false);
    bool doThrow(AstNode* parent, AstNode** result);
    bool doGlobalAttributeExpose(AstNode* parent, AstNode** result, bool forGlobal);
    bool doNamespace(AstNode* parent, AstNode** result);
    bool doNamespace(AstNode* parent, AstNode** result, bool forGlobal, bool forUsing);
    bool doNamespaceOnName(AstNode* parent, AstNode** result, bool forGlobal, bool forUsing, Token* privName = nullptr);
    bool doEnumContent(AstNode* parent, AstNode** result);
    bool doEnumValue(AstNode* parent, AstNode** result);
    bool doEnum(AstNode* parent, AstNode** result);
    bool doTupleBody(AstNode* parent, bool acceptEmpty);
    bool doStructBody(AstNode* parent, SyntaxStructType structType, AstNode** result);
    bool doStruct(AstNode* parent, AstNode** result);
    bool doStructContent(AstStruct* structNode, SyntaxStructType structType);
    bool doImpl(AstNode* parent, AstNode** result);
    bool doAssignmentExpression(AstNode* parent, AstNode** result);
    bool doExpressionListTuple(AstNode* parent, AstNode** result);
    bool doExpressionListArray(AstNode* parent, AstNode** result);
    bool doInitializationExpression(TokenParse& forToken, AstNode* parent, AstNode** result);
    bool doLiteral(AstNode* parent, AstNode** result);
    bool doIntrinsicProp(AstNode* parent, AstNode** result);
    bool doGetErr(AstNode* parent, AstNode** result);
    bool doIndex(AstNode* parent, AstNode** result);
    bool doKeepRef(AstNode* parent, AstNode** result);
    bool doMoveRef(AstNode* parent, AstNode** result);
    bool doDeRef(AstNode* parent, AstNode** result);
    bool doOperatorPrecedence(AstNode** result);
    bool doModifiers(Token& forNode, TokenId tokenId, uint32_t& mdfFlags);
    bool doLambdaExpression(AstNode* parent, uint32_t exprFlags, AstNode** result);
    bool doSinglePrimaryExpression(AstNode* parent, uint32_t exprFlags, AstNode** result);
    bool doPrimaryExpression(AstNode* parent, uint32_t exprFlags, AstNode** result);
    bool doUnaryExpression(AstNode* parent, uint32_t exprFlags, AstNode** result);
    bool doFactorExpression(AstNode** parent, uint32_t exprFlags, AstNode** result);
    bool doCompareExpression(AstNode* parent, uint32_t exprFlags, AstNode** result);
    bool doBoolExpression(AstNode* parent, uint32_t exprFlags, AstNode** result);
    bool doExpression(AstNode* parent, uint32_t exprFlags, AstNode** result);
    bool doMoveExpression(Token& forToken, TokenId tokenId, AstNode* parent, AstNode** result);
    bool doGenericDeclParameters(AstNode* parent, AstNode** result);
    bool doLambdaFuncDecl(AstNode* parent, AstNode** result, bool acceptMissingType = false, bool* hasMissingType = nullptr);
    bool doFuncDecl(AstNode* parent, AstNode** result, TokenId typeFuncId = TokenId::Invalid);
    bool doFuncDeclParameter(AstNode* parent, bool acceptMissingType = false, bool* hasMissingType = nullptr);
    bool doFuncDeclParameters(AstNode* parent, AstNode** result, bool acceptMissingType = false, bool* hasMissingType = nullptr, bool isMethod = false, bool isConstMethod = false, bool isItfMethod = false);
    bool doAttrDecl(AstNode* parent, AstNode** result);
    bool doAttrUse(AstNode* parent, AstNode** result, bool single = false);
    bool doEmbeddedInstruction(AstNode* parent, AstNode** result);
    bool doEmbeddedStatement(AstNode* parent, AstNode** result);
    bool doStatementFor(AstNode* parent, AstNode** result, AstNodeKind kind);
    bool doStatement(AstNode* parent, AstNode** result);
    bool doGlobalCurlyStatement(AstNode* parent, AstNode** result);
    bool doCurlyStatement(AstNode* parent, AstNode** result);
    bool doScopedCurlyStatement(AstNode* parent, AstNode** result, ScopeKind scopeKind = ScopeKind::Statement);
    bool doReturn(AstNode* parent, AstNode** result);
    bool doWith(AstNode* parent, AstNode** result);
    bool doCompilerScopeFile(AstNode* parent, AstNode** result);
    bool doUsing(AstNode* parent, AstNode** result);
    bool doCast(AstNode* parent, AstNode** result);
    bool doAutoCast(AstNode* parent, AstNode** result);
    bool doIf(AstNode* parent, AstNode** result);
    bool doWhile(AstNode* parent, AstNode** result);
    bool doFor(AstNode* parent, AstNode** result);
    bool doLoop(AstNode* parent, AstNode** result);
    bool doVisit(AstNode* parent, AstNode** result);
    bool doSwitch(AstNode* parent, AstNode** result);
    bool doBreak(AstNode* parent, AstNode** result);
    bool doFallThrough(AstNode* parent, AstNode** result);
    bool doContinue(AstNode* parent, AstNode** result);
    bool doArrayPointerIndex(AstNode** exprNode);
    bool doLeftInstruction(AstNode* parent, AstNode** result, AstWith* withNode = nullptr);
    bool doLeftExpressionVar(AstNode* parent, AstNode** result, uint32_t identifierFlags = 0, AstWith* withNode = nullptr);
    bool doLeftExpressionAffect(AstNode* parent, AstNode** result, AstWith* withNode = nullptr);
    bool doInit(AstNode* parent, AstNode** result);
    bool doDropCopyMove(AstNode* parent, AstNode** result);
    bool doRange(AstNode* parent, AstNode* expression, AstNode** result);

    ErrorContext* context    = nullptr;
    SourceFile*   sourceFile = nullptr;
    Module*       module     = nullptr;

    Tokenizer  tokenizer;
    TokenParse token;

    AstNode*            dummyResult            = nullptr;
    Scope*              currentScope           = nullptr;
    AstFuncDecl*        currentFct             = nullptr;
    AstBreakable*       currentBreakable       = nullptr;
    Scope*              currentStructScope     = nullptr;
    Scope*              currentSelfStructScope = nullptr;
    AstCompilerIfBlock* currentCompilerIfBlock = nullptr;
    AstTryCatchAssume*  currentTryCatchAssume  = nullptr;
    AstInline*          currentInline          = nullptr;

    uint64_t currentFlags = 0;
    uint64_t contextFlags = 0;

    uint32_t contextualNoInline = 0;

    bool afterGlobal = false;
};

struct PushSyntaxContextFlags
{
    PushSyntaxContextFlags(Parser* job, uint64_t flags)
        : job{job}
    {
        oldFlags = job->contextFlags;
        job->contextFlags |= flags;
    }

    ~PushSyntaxContextFlags()
    {
        job->contextFlags = oldFlags;
    }

    Parser*  job;
    uint64_t oldFlags;
};