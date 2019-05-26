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

struct SyntaxJob : public Job
{
    JobResult execute() override;

    void reset() override
    {
        currentScope    = nullptr;
        canChangeModule = true;
        moduleSpecified = false;
    }

    bool error(const Token& tk, const Utf8& msg);
    bool syntaxError(const Token& tk, const Utf8& msg);
    bool notSupportedError(const Token& tk);
    bool eatToken(TokenId id);
    bool recoverError();

    bool doCompilerAssert(AstNode* parent);
    bool doCompilerPrint(AstNode* parent);
    bool doCompilerRun(AstNode* parent);
    bool doCompilerUnitTest();
    bool doTopLevel(AstNode* parent);
    bool doVarDecl(AstNode* parent, AstVarDecl** result = nullptr);
    bool doTypeDecl(AstNode* parent, AstNode** result = nullptr);
    bool doTypeExpression(AstNode* parent, AstNode** result = nullptr);
    bool doIdentifier(AstNode* parent, AstIdentifier** result = nullptr);
	bool doIdentifierRef(AstNode* parent, AstIdentifierRef** result = nullptr);
    bool doNamespace(AstNode* parent, AstNode** result = nullptr);
    bool doAssignmentExpression(AstNode* parent, AstNode** result = nullptr);
    bool doLiteral(AstNode* parent, AstNode** result = nullptr);
    bool doSinglePrimaryExpression(AstNode* parent, AstNode** result = nullptr);
    bool doUnaryExpression(AstNode* parent, AstNode** result = nullptr);
    bool doFactorExpression(AstNode* parent, AstNode** result = nullptr);
    bool doCompareExpression(AstNode* parent, AstNode** result = nullptr);
    bool doBoolExpression(AstNode* parent, AstNode** result = nullptr);
    bool doExpression(AstNode* parent, AstNode** result = nullptr);

    Tokenizer   tokenizer;
    Token       token;
    SourceFile* sourceFile = nullptr;
    Scope*      currentScope;
    bool        canChangeModule;
    bool        moduleSpecified;
};
