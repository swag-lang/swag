#pragma once
#include "Tokenizer.h"
#include "Job.h"
struct SourceFile;
struct AstVarDecl;
struct AstNode;
struct AstIdentifier;
struct AstIdentifierRef;
struct Scope;
struct Utf8;
struct AstFuncDecl;

struct SyntaxJob : public Job
{
    JobResult execute() override;

    void reset() override
    {
        currentScope    = nullptr;
        currentFct      = nullptr;
        canChangeModule = true;
        moduleSpecified = false;
    }

    bool error(const Token& tk, const Utf8& msg);
    bool error(const SourceLocation& startLocation, const SourceLocation& endLocation, const Utf8& msg);
    bool syntaxError(const Token& tk, const Utf8& msg);
    bool notSupportedError(const Token& tk);
    bool eatToken(TokenId id);
    bool recoverError();

    bool doCompilerAssert(AstNode* parent);
    bool doCompilerPrint(AstNode* parent);
    bool doCompilerRun(AstNode* parent);
    bool doCompilerUnitTest();
    bool doTopLevelInstruction(AstNode* parent);
    bool doVarDecl(AstNode* parent);
    bool doTypeDecl(AstNode* parent, AstNode** result = nullptr);
    bool doTypeExpression(AstNode* parent, AstNode** result = nullptr);
    bool doAffectExpression(AstNode* parent);
    bool doIdentifier(AstNode* parent);
    bool doIdentifierRef(AstNode* parent, AstNode** result = nullptr);
    bool doNamespace(AstNode* parent);
    bool doEnum(AstNode* parent, AstNode** result = nullptr);
    bool doAssignmentExpression(AstNode* parent, AstNode** result = nullptr);
    bool doLiteral(AstNode* parent, AstNode** result = nullptr);
    bool doSinglePrimaryExpression(AstNode* parent, AstNode** result = nullptr);
    bool doUnaryExpression(AstNode* parent, AstNode** result = nullptr);
    bool doFactorExpression(AstNode* parent, AstNode** result = nullptr);
    bool doCompareExpression(AstNode* parent, AstNode** result = nullptr);
    bool doBoolExpression(AstNode* parent, AstNode** result = nullptr);
    bool doExpression(AstNode* parent, AstNode** result = nullptr);
    bool doFuncDecl(AstNode* parent, AstNode** result = nullptr);
    bool doFuncDeclParameter(AstNode* parent);
    bool doFuncDeclParameters(AstNode* parent, AstNode** result = nullptr);
    bool doAttrDecl(AstNode* parent, AstNode** result = nullptr);
    bool doAttrUse(AstNode* parent, AstNode** result = nullptr);
    bool doEmbeddedInstruction(AstNode* parent, AstNode** result = nullptr);
    bool doCurlyStatement(AstNode* parent, AstNode** result = nullptr);
    bool doReturn(AstNode* parent, AstNode** result = nullptr);
    bool doUsing(AstNode* parent);

    Tokenizer   tokenizer;
    Token       token;
    SourceFile* sourceFile = nullptr;
    bool        canChangeModule;
    bool        moduleSpecified;

    Scope*       currentScope;
    AstFuncDecl* currentFct;
};
