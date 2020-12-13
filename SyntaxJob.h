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
struct SyntaxJob;
struct AstAttrUse;
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
    TypeSet,
    Tuple
};

struct SyntaxContext : public JobContext
{
    SyntaxJob* job = nullptr;
};

struct SyntaxJob : public Job
{
    JobResult execute() override;
    bool      constructEmbedded(const Utf8& content, AstNode* parent, AstNode* fromNode, enum class CompilerAstKind kind, bool parentLocation = false);

    bool error(const Token& tk, const Utf8& msg);
    bool error(AstNode* node, const Utf8& msg);
    bool error(const SourceLocation& startLocation, const SourceLocation& endLocation, const Utf8& msg);
    bool syntaxError(const Token& tk, const Utf8& msg);
    bool syntaxError(AstNode* node, const Utf8& msg);
    bool invalidTokenError(InvalidTokenError kind);
    bool verifyError(const Token& tk, bool expr, const Utf8& msg);

    bool eatToken();
    bool eatToken(TokenId id, const char* msg = nullptr);
    bool eatSemiCol(const char* msg = nullptr);
    bool checkIsSingleIdentifier(AstNode* node, const char* msg);

    bool doLabel(AstNode* parent, AstNode** result = nullptr);
    bool doGenericFuncCallParameters(AstNode* parent, AstNode** result = nullptr);
    bool doFuncCallParameters(AstNode* parent, AstNode** result, TokenId closeToken);
    bool doCompilerIfFor(AstNode* parent, AstNode** result, AstNodeKind kind);
    bool doCompilerIf(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerForeignLib(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerTag(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerInline(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerMacro(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerMixin(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerAssert(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerPrint(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerAst(AstNode* parent, AstNode** result, CompilerAstKind kind);
    bool isValidScopeForCompilerRun(AstNode* node);
    bool doCompilerRunTopLevel(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerRunEmbedded(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerUnitTest();
    bool doCompilerPublic();
    bool doCompilerGenerated();
    bool doCompilerPass();
    bool doCompilerSpecialFunction(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerDefined(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerLoad(AstNode* parent, AstNode** result = nullptr);
    bool doCompilerImport(AstNode* parent);
    bool doCompilerPlaceHolder(AstNode* parent);
    bool doTopLevelInstruction(AstNode* parent, AstNode** result = nullptr);
    bool doVarDecl(AstNode* parent, AstNode** result = nullptr);
    bool doVarDecl(AstNode* parent, AstNode** result, AstNodeKind kind);
    bool doAlias(AstNode* parent, AstNode** result = nullptr);
    bool convertExpressionListToTuple(AstNode* parent, AstNode** result, bool isConst, bool anonymousStruct, bool anonymousUnion);
    bool doTypeExpression(AstNode* parent, AstNode** result = nullptr, bool inTypeVarDecl = false);
    bool doTypeExpressionLambda(AstNode* parent, AstNode** result = nullptr);
    void forceTakeAddress(AstNode* node);
    bool doDefer(AstNode* parent, AstNode** result = nullptr);
    bool checkIsValidUserName(AstNode* node);
    bool checkIsValidVarName(AstNode* node);
    bool doVarDeclExpression(AstNode* parent, AstNode* leftNode, AstNode* type, AstNode* assign, AstNodeKind kind, AstNode** result);
    bool doAffectExpression(AstNode* parent, AstNode** result = nullptr);
    bool doIdentifier(AstNode* parent, bool acceptParameters = true);
    bool doIdentifierRef(AstNode* parent, AstNode** result = nullptr, bool acceptParameters = true);
    bool doGlobalAttributeExpose(AstNode* parent, AstNode** result = nullptr);
    bool doNamespace(AstNode* parent, AstNode** result = nullptr);
    bool doEnumContent(AstNode* parent, AstNode** result = nullptr);
    bool doEnumValue(AstNode* parent, AstNode** result = nullptr);
    bool doEnum(AstNode* parent, AstNode** result = nullptr);
    bool doStructBodyTuple(AstNode* parent, bool acceptEmpty);
    bool doStructBody(AstNode* parent, SyntaxStructType structType, AstNode** result = nullptr);
    bool doStruct(AstNode* parent, AstNode** result = nullptr);
    bool doStructContent(AstStruct* structNode, SyntaxStructType structType);
    bool doImpl(AstNode* parent, AstNode** result = nullptr);
    bool doAssignmentExpression(AstNode* parent, AstNode** result = nullptr);
    bool doExpressionListTuple(AstNode* parent, AstNode** result = nullptr);
    bool doExpressionListArray(AstNode* parent, AstNode** result = nullptr);
    bool doInitializationExpression(AstNode* parent, AstNode** result = nullptr);
    bool doLiteral(AstNode* parent, AstNode** result = nullptr);
    bool doIntrinsicProp(AstNode* parent, AstNode** result = nullptr);
    bool doIndex(AstNode* parent, AstNode** result = nullptr);
    bool doSinglePrimaryExpression(AstNode* parent, AstNode** result = nullptr);
    bool doPrimaryExpression(AstNode* parent, AstNode** result = nullptr);
    bool doDeRef(AstNode* parent, AstNode** result = nullptr);
    bool doUnaryExpression(AstNode* parent, AstNode** result = nullptr);
    bool doOperatorPrecedence(AstNode** result);
    bool doFactorExpression(AstNode* parent, AstNode** result = nullptr);
    bool doCompareExpression(AstNode* parent, AstNode** result = nullptr);
    bool doBoolExpression(AstNode* parent, AstNode** result = nullptr);
    bool doMoveExpression(AstNode* parent, AstNode** result = nullptr);
    bool doExpression(AstNode* parent, AstNode** result = nullptr);
    bool doGenericDeclParameters(AstNode* parent, AstNode** result = nullptr);
    bool doLambdaFuncDecl(AstNode* parent, AstNode** result = nullptr, bool acceptMissingType = false);
    bool doFuncDecl(AstNode* parent, AstNode** result = nullptr, TokenId typeFuncId = TokenId::Invalid);
    bool doFuncDeclParameter(AstNode* parent, bool acceptMissingType = false);
    bool doFuncDeclParameters(AstNode* parent, AstNode** result = nullptr, bool acceptMissingType = false);
    bool doAttrDecl(AstNode* parent, AstNode** result = nullptr);
    void setOwnerAttrUse(AstAttrUse* attrUse, AstNode* who);
    bool doAttrUse(AstNode* parent, AstNode** result = nullptr);
    bool doEmbeddedInstruction(AstNode* parent, AstNode** result = nullptr);
    bool doEmbeddedStatement(AstNode* parent, AstNode** result = nullptr);
    bool doStatementFor(AstNode* parent, AstNode** result, AstNodeKind kind);
    bool doStatement(AstNode* parent, AstNode** result = nullptr);
    bool doGlobalCurlyStatement(AstNode* parent, AstNode** result = nullptr);
    bool doCurlyStatement(AstNode* parent, AstNode** result = nullptr);
    bool doScopedCurlyStatement(AstNode* parent, AstNode** result = nullptr, ScopeKind scopeKind = ScopeKind::Statement);
    bool doReturn(AstNode* parent, AstNode** result = nullptr);
    bool doLambdaExpression(AstNode* parent, AstNode** result = nullptr);
    bool doUsing(AstNode* parent, AstNode** result = nullptr);
    bool doCast(AstNode* parent, AstNode** result = nullptr);
    bool doBitCast(AstNode* parent, AstNode** result = nullptr);
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
    bool doLeftExpressionVar(AstNode** result, bool acceptParameters = true);
    bool doLeftExpression(AstNode** result);
    bool doInit(AstNode* parent, AstNode** result = nullptr);
    bool doDropCopyMove(AstNode* parent, AstNode** result = nullptr);

    SyntaxContext       context;
    Tokenizer           tokenizer;
    Token               token;
    Scope*              currentScope           = nullptr;
    AstFuncDecl*        currentFct             = nullptr;
    AstBreakable*       currentBreakable       = nullptr;
    Scope*              currentStructScope     = nullptr;
    AstCompilerIfBlock* currentCompilerIfBlock = nullptr;
    Token*              currentTokenLocation   = nullptr;
    AstNode*            currentMainNode        = nullptr;
    uint64_t            currentFlags           = 0;
    bool                canChangeModule        = true;
    bool                moduleSpecified        = false;
    bool                inFunCall              = false;

    void reset() override
    {
        Job::reset();
        context.reset();
        sourceFile             = nullptr;
        currentScope           = nullptr;
        currentFct             = nullptr;
        currentBreakable       = nullptr;
        currentStructScope     = nullptr;
        currentCompilerIfBlock = nullptr;
        currentMainNode        = nullptr;
        currentTokenLocation   = nullptr;
        currentFlags           = 0;
        canChangeModule        = true;
        moduleSpecified        = false;
        inFunCall              = false;
    }

    void release() override
    {
        extern thread_local Pool<SyntaxJob> g_Pool_syntaxJob;
        g_Pool_syntaxJob.release(this);
    }
};

extern thread_local Pool<SyntaxJob> g_Pool_syntaxJob;
