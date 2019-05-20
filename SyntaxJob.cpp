#include "pch.h"
#include "SyntaxJob.h"
#include "SourceFile.h"
#include "Tokenizer.h"
#include "Diagnostic.h"
#include "Global.h"
#include "CommandLine.h"
#include "Workspace.h"
#include "Stats.h"
#include "SourceFile.h"
#include "Module.h"

bool SyntaxJob::doCompilerUnitTest()
{
    SWAG_CHECK(m_tokenizer.getTokenOrEOL(m_token));
	SWAG_VERIFY(m_token.id != TokenId::EndOfLine, m_file->report({ m_file, m_token, "missing #unittest parameter" }));

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
    else if (m_token.text == "module")
    {
        SWAG_VERIFY(!m_moduleSpecified, m_file->report({m_file, m_token, "#unittest module can only be specified once"}));
        SWAG_CHECK(m_tokenizer.getTokenOrEOL(m_token));
        SWAG_VERIFY(m_token.id != TokenId::EndOfLine, m_file->report({m_file, m_token, "missing module name"}));
        SWAG_VERIFY(m_token.id == TokenId::Identifier, m_file->report({m_file, m_token, format("invalid module name '%s'", m_token.text.c_str())}));
		m_moduleSpecified = true;
		if (g_CommandLine.test)
		{
			auto newModule = g_Workspace.createOrUseModule(m_token.text);
			m_file->m_module->removeFile(m_file);
			newModule->addFile(m_file);
		}
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
    if (g_CommandLine.stats)
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
                m_file->m_silent++;
                if (!m_tokenizer.getToken(m_token))
                {
                    m_file->m_silent--;
                    return false;
                }

                m_file->m_silent--;
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

	if (!result)
		return false;
    return result;
}
