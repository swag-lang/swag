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
#include "Pool.h"
#include "Global.h"
#include "PoolFactory.h"
#include "LanguageSpec.h"

bool SyntaxJob::syntaxError(const string& msg)
{
	string full = "syntax error";
	if (!msg.empty())
		full += ", " + msg;
    error(full);
    return false;
}

bool SyntaxJob::error(const string& msg)
{
    m_file->report({m_file, m_token, msg.c_str()});
    return false;
}

bool SyntaxJob::eatToken(TokenId id)
{
    SWAG_CHECK(m_tokenizer.getToken(m_token));
    if (m_token.id != id)
        SWAG_CHECK(syntaxError(format("'%s' expected instead of '%s'", g_LangSpec.tokenToName(id).c_str(), m_token.text.c_str())));
    return true;
}

bool SyntaxJob::doCompilerUnitTest()
{
    SWAG_CHECK(m_tokenizer.getTokenOrEOL(m_token));
    SWAG_VERIFY(m_token.id != TokenId::EndOfLine, m_file->report({m_file, m_token, "missing #unittest parameter"}));

    if (m_token.text == "error")
    {
        if (g_CommandLine.test)
            m_file->m_unittestError++;
    }
    else if (m_token.text == "pass")
    {
        SWAG_CHECK(m_tokenizer.getTokenOrEOL(m_token));
        SWAG_VERIFY(m_token.id != TokenId::EndOfLine, m_file->report({m_file, m_token, "missing pass name"}));
        if (m_token.text == "lexer")
        {
            if (g_CommandLine.test)
                m_file->m_buildPass = BuildPass::Lexer;
        }
        else if (m_token.text == "syntax")
        {
            if (g_CommandLine.test)
                m_file->m_buildPass = BuildPass::Syntax;
        }
        else if (m_token.text == "semantic")
        {
            if (g_CommandLine.test)
                m_file->m_buildPass = BuildPass::Semantic;
        }
        else
        {
            m_file->report({m_file, m_token, format("invalid pass name '%s'", m_token.text.c_str())});
            return false;
        }
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

bool SyntaxJob::recoverError()
{
    while (true)
    {
        if (m_token.id == TokenId::CompilerUnitTest)
            break;
        if (m_token.id == TokenId::SymSemiColon)
            break;
        if (m_token.id == TokenId::EndOfFile)
            return false;

        m_file->m_silent++;
        if (!m_tokenizer.getToken(m_token))
        {
            m_file->m_silent--;
            return false;
        }

        m_file->m_silent--;
	}

	return true;
}

bool SyntaxJob::execute()
{
    if (g_CommandLine.stats)
        g_Stats.numFiles++;
    m_tokenizer.setFile(m_file);

    m_file->m_astRoot = Ast::newNode(&m_file->poolFactory->astNode, AstNodeType::RootFile);

    bool result = true;
    bool ok     = true;
    while (true)
    {
        // Recover from last syntax error
        if (!ok)
        {
			// If there's an error, then we must stop at syntax pass
			m_file->m_buildPass = min(m_file->m_buildPass, BuildPass::Syntax);
			if (!recoverError())
				return false;
            result = false;
			ok = true;
        }
		else
		{
			ok = m_tokenizer.getToken(m_token);
			if (!ok)
				continue;
		}

        if (m_token.id == TokenId::EndOfFile)
            break;

        // Top level
        if (m_token.id == TokenId::CompilerUnitTest)
        {
            ok = doCompilerUnitTest();
            continue;
        }

        // Ask for lexer only
        if (m_file->m_buildPass < BuildPass::Syntax)
            continue;

        ok = doTopLevel(m_file->m_astRoot);
    }

    return result;
}
