#pragma once
#include "Tokenizer.h"
#include "Job.h"
#include "AstFlags.h"
struct SourceFile;
struct AstVarDecl;
struct AstNode;
struct AstBreakable;
struct AstIdentifier;
struct AstIdentifierRef;
struct Scope;
struct Utf8;
struct AstFuncDecl;

struct SyntaxJob : public Job
{
    JobResult execute() override;

    bool isContextDisabled() const
    {
        return currentFlags & AST_DISABLED;
    }

    bool error(const Token& tk, const Utf8& msg);
    bool error(const SourceLocation& startLocation, const SourceLocation& endLocation, const Utf8& msg);
    bool syntaxError(const Token& tk, const Utf8& msg);
    bool notSupportedError(const Token& tk);
    bool eatToken();
    bool eatToken(TokenId id, const char* msg = nullptr);
    bool eatSemiCol(const char* msg = nullptr);
    bool recoverError();

	void generateOpInit(AstNode* node);

    bool doCompilerAssert(AstNode* parent);
    bool doCompilerPrint(AstNode* parent);
    bool doCompilerVersion(AstNode* parent);
    bool doCompilerRunDecl(AstNode* parent);
    bool doCompilerUnitTest();
    bool doCompilerModule();
    bool doCompilerImport(AstNode* parent);
    bool doTopLevelInstruction(AstNode* parent);
    bool doVarDecl(AstNode* parent, AstNode** result = nullptr);
    bool doTypeDecl(AstNode* parent, AstNode** result = nullptr);
	bool doTypeExpressionTuple(AstNode* parent, AstNode** result = nullptr);
    bool doTypeExpression(AstNode* parent, AstNode** result = nullptr);
    bool doTypeExpressionLambda(AstNode* parent, AstNode** result = nullptr);
	void forceTakeAddress(AstNode* node);
    bool doAffectExpression(AstNode* parent, AstNode** result = nullptr);
    bool doIdentifier(AstNode* parent, bool acceptInteger = false);
    bool doIdentifierRef(AstNode* parent, AstNode** result = nullptr);
    bool doNamespace(AstNode* parent);
    bool doEnum(AstNode* parent, AstNode** result = nullptr);
	bool doStruct(AstNode* parent, AstNode** result = nullptr);
	bool doImpl(AstNode* parent, AstNode** result = nullptr);
    bool doAssignmentExpression(AstNode* parent, AstNode** result = nullptr);
    bool doExpressionList(AstNode* parent, AstNode** result = nullptr);
    bool doInitializationExpression(AstNode* parent, AstNode** result = nullptr);
    bool doLiteral(AstNode* parent, AstNode** result = nullptr);
    bool doIntrinsicProp(AstNode* parent, AstNode** result = nullptr);
    bool doIndex(AstNode* parent, AstNode** result = nullptr);
    bool doSinglePrimaryExpression(AstNode* parent, AstNode** result = nullptr);
    bool doPrimaryExpression(AstNode* parent, AstNode** result = nullptr);
    bool doUnaryExpression(AstNode* parent, AstNode** result = nullptr);
    bool doFactorExpression(AstNode* parent, AstNode** result = nullptr);
    bool doCompareExpression(AstNode* parent, AstNode** result = nullptr);
    bool doBoolExpression(AstNode* parent, AstNode** result = nullptr);
    bool doExpression(AstNode* parent, AstNode** result = nullptr);
	bool doGenericArguments(AstNode* parent, AstNode** result = nullptr);
    bool doFuncDecl(AstNode* parent, AstNode** result = nullptr);
    bool doFuncDeclParameter(AstNode* parent);
    bool doFuncDeclParameters(AstNode* parent, AstNode** result = nullptr);
    bool doAttrDecl(AstNode* parent, AstNode** result = nullptr);
    bool doAttrUse(AstNode* parent, AstNode** result = nullptr);
    bool doEmbeddedInstruction(AstNode* parent, AstNode** result = nullptr);
    bool doEmbeddedStatement(AstNode* parent, AstNode** result = nullptr);
    bool doStatement(AstNode* parent, AstNode** result = nullptr);
    bool doCurlyStatement(AstNode* parent, AstNode** result = nullptr);
    bool doScopedCurlyStatement(AstNode* parent, AstNode** result = nullptr);
    bool doReturn(AstNode* parent, AstNode** result = nullptr);
    bool doUsing(AstNode* parent, AstNode** result = nullptr);
    bool doCast(AstNode* parent, AstNode** result = nullptr);
    bool doIf(AstNode* parent, AstNode** result = nullptr);
    bool doWhile(AstNode* parent, AstNode** result = nullptr);
    bool doFor(AstNode* parent, AstNode** result = nullptr);
    bool doLoop(AstNode* parent, AstNode** result = nullptr);
    bool doSwitch(AstNode* parent, AstNode** result = nullptr);
    bool doBreak(AstNode* parent, AstNode** result = nullptr);
    bool doContinue(AstNode* parent, AstNode** result = nullptr);
    bool doArrayPointerIndex(AstNode** exprNode);
    bool doLeftExpression(AstNode* parent, AstNode** result = nullptr);

    void reset() override
    {
        canChangeModule  = true;
        moduleSpecified  = false;
        currentFlags     = 0;
        currentScope     = nullptr;
        currentFct       = nullptr;
        currentBreakable = nullptr;
    }

    Tokenizer     tokenizer;
    Token         token;
    SourceFile*   sourceFile = nullptr;
    bool          canChangeModule;
    bool          moduleSpecified;
    uint64_t      currentFlags;
    Scope*        currentScope;
    AstFuncDecl*  currentFct;
    AstBreakable* currentBreakable;
};

extern Pool<SyntaxJob> g_Pool_syntaxJob;
