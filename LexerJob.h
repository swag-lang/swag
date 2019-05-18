#pragma once
#include "Tokenizer.h"
#include "Job.h"

class LexerJob : public Job
{
public:
    bool execute() override;
    void reset() override{};
    void construct() override{};

    void setFile(class SourceFile* file)
    {
        m_file = file;
    }

private:
    bool doCompilerUnitTest();

private:
    class SourceFile* m_file = nullptr;
    Tokenizer         m_tokenizer;
    Token             m_token;
};
