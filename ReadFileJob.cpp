#include "pch.h"
#include "ReadFileJob.h"
#include "SourceFile.h"
#include "Tokenizer.h"
#include "Diagnostic.h"
#include "Global.h"
#include "Stats.h"

bool ReadFileJob::doCompilerUnitTest()
{
	SWAG_CHECK(m_tokenizer.getToken(m_token));
	if (m_token.text == L"error")
	{
		m_file->m_unittestError++;
	}
	else
	{
		m_file->report({ m_file, m_token, format(L"unknown #unittest parameter '%s'", m_token.text.c_str()) });
		return false;
	}

	return true;
}

bool ReadFileJob::execute()
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
    }

    return true;
}
