#pragma once
#include "Tokenizer.h"
#include "Job.h"
struct SourceFile;
struct AstType;
struct AstVarDecl;
struct AstNode;
struct AstScope;
struct utf8;

struct SyntaxJob : public Job
{
    bool execute() override;

    void reset() override
    {
        currentScope    = nullptr;
        canChangeModule = true;
        moduleSpecified = false;
    }

    bool error(const Token& tk, const utf8& msg);
    bool syntaxError(const Token& tk, const utf8& msg);
    bool eatToken(TokenId id);
    bool recoverError();

    bool doCompilerUnitTest();
    bool doTopLevel(AstNode* parent);
    bool doVarDecl(AstNode* parent, AstVarDecl** result = nullptr);
    bool doType(AstNode* parent, AstType** result = nullptr);
    bool doNamespace(AstNode* parent, AstScope** result = nullptr);

    Tokenizer   tokenizer;
    Token       token;
    SourceFile* sourceFile = nullptr;
    AstScope*   currentScope;
    bool        canChangeModule;
    bool        moduleSpecified;
};
