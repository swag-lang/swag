#pragma once
#include "Tokenizer.h"
#include "Job.h"
struct SourceFile;
struct AstType;
struct AstVarDecl;
struct AstNode;
struct AstScopeNode;
struct utf8;

struct SyntaxJob : public Job
{
    bool execute() override;
    void reset() override{};
    void construct() override{};

    bool error(const utf8& msg);
    bool syntaxError(const utf8& msg);
    bool eatToken(TokenId id);
    bool recoverError();

    bool doCompilerUnitTest();
    bool doTopLevel(AstNode* parent);
    bool doVarDecl(AstNode* parent, AstVarDecl** result = nullptr);
    bool doType(AstNode* parent, AstType** result = nullptr);

    AstScopeNode* currentScope = nullptr;
    SourceFile*   sourceFile   = nullptr;
    Tokenizer     tokenizer;
    Token         token;
    bool          canChangeModule = true;
    bool          moduleSpecified = false;
};
