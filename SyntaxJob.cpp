#include "pch.h"
#include "SyntaxJob.h"
#include "SourceFile.h"
#include "Tokenizer.h"
#include "Diagnostic.h"
#include "Global.h"
#include "CommandLine.h"
#include "Stats.h"

bool SyntaxJob::doCompilerUnitTest()
{
    SWAG_CHECK(m_tokenizer.getToken(m_token));
    if (m_token.text == "error")
    {
        if (g_CommandLine.test)
            m_file->m_unittestError++;
    }
    else if (m_token.text == "lexer")
    {
        if (g_CommandLine.test)
            m_file->m_doSyntax = false;
    }
    else
    {
        m_file->report({m_file, m_token, format("unknown #unittest parameter '%s'", m_token.text.c_str())});
        return false;
    }

    return true;
}

bool SyntaxJob::execute()
{
    g_Stats.numFiles++;
    m_tokenizer.setFile(m_file);

    bool canLex = true;
    bool result = true;
    while (true)
    {
        // During tests, we can cumulate more than one error during parsing
        if (!m_tokenizer.getToken(m_token))
        {
            if (!g_CommandLine.test)
                return false;
            result = false;
            while (true)
            {
                m_tokenizer.getToken(m_token);
                if (m_token.id == TokenId::CompilerUnitTest)
                    break;
                if (m_token.id == TokenId::EndOfFile)
                    return false;
            }
        }

        if (m_token.id == TokenId::EndOfFile)
            break;

        // Top level
        switch (m_token.id)
        {
        case TokenId::CompilerUnitTest:
            SWAG_CHECK(doCompilerUnitTest());
            break;
        }

        // Ask for tokenizer only
        if (!m_file->m_doSyntax)
            continue;
    }

    return result;
}
