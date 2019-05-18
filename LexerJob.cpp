#include "pch.h"
#include "LexerJob.h"
#include "SourceFile.h"
#include "Tokenizer.h"
#include "Diagnostic.h"
#include "Global.h"
#include "CommandLine.h"
#include "Stats.h"

bool LexerJob::doCompilerUnitTest()
{
    SWAG_CHECK(m_tokenizer.getToken(m_token));
	if (m_token.text == L"error")
	{
		if(g_CommandLine.test)
			m_file->m_unittestError++;
	}
	else if (m_token.text == L"tokenizer")
	{
		if (g_CommandLine.test)
			m_file->m_doLex = false;
	}
    else
    {
        m_file->report({m_file, m_token, format(L"unknown #unittest parameter '%s'", m_token.text.c_str())});
        return false;
    }

    return true;
}

bool LexerJob::execute()
{
    g_Stats.numFiles++;
    m_tokenizer.setFile(m_file);

    bool canLex = true;
    while (true)
    {
        SWAG_CHECK(m_tokenizer.getToken(m_token));
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
        if (!m_file->m_doLex)
            continue;
    }

    return true;
}
