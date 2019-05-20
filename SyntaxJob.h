#pragma once
#include "Tokenizer.h"
#include "Job.h"

struct SyntaxContext
{
    AstNode* parent = nullptr;
};

class SyntaxJob : public Job
{
public:
    bool execute() override;
    void reset() override{};
    void construct() override{};

    void setFile(struct SourceFile* file)
    {
        m_file = file;
    }

private:
    bool error(const string& msg);
    bool syntaxError(const string& msg);
    bool eatToken(TokenId id);
    bool recoverError();

    bool doCompilerUnitTest();
    bool doTopLevel(AstNode* parent);
    bool doVarDecl(AstNode* parent, struct AstVarDecl** result = nullptr);
    bool doType(AstNode* parent, struct AstType** result = nullptr);

private:
    struct SourceFile* m_file = nullptr;
    Tokenizer          m_tokenizer;
    Token              m_token;
    bool               m_moduleSpecified = false;
};
