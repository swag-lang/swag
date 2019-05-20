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
    sourceFile->report({sourceFile, token, msg.c_str()});
    return false;
}

bool SyntaxJob::eatToken(TokenId id)
{
    SWAG_CHECK(tokenizer.getToken(token));
    if (token.id != id)
        SWAG_CHECK(syntaxError(format("'%s' expected instead of '%s'", g_LangSpec.tokenToName(id).c_str(), token.text.c_str())));
    return true;
}

bool SyntaxJob::doCompilerUnitTest()
{
    SWAG_CHECK(tokenizer.getTokenOrEOL(token));
    SWAG_VERIFY(token.id != TokenId::EndOfLine, sourceFile->report({sourceFile, token, "missing #unittest parameter"}));

    if (token.text == "error")
    {
        if (g_CommandLine.test)
            sourceFile->unittestError++;
    }
    else if (token.text == "pass")
    {
        SWAG_CHECK(tokenizer.getTokenOrEOL(token));
        SWAG_VERIFY(token.id != TokenId::EndOfLine, sourceFile->report({sourceFile, token, "missing pass name"}));
        if (token.text == "lexer")
        {
            if (g_CommandLine.test)
                sourceFile->buildPass = BuildPass::Lexer;
        }
        else if (token.text == "syntax")
        {
            if (g_CommandLine.test)
                sourceFile->buildPass = BuildPass::Syntax;
        }
        else if (token.text == "semantic")
        {
            if (g_CommandLine.test)
                sourceFile->buildPass = BuildPass::Semantic;
        }
        else
        {
            sourceFile->report({sourceFile, token, format("invalid pass name '%s'", token.text.c_str())});
            return false;
        }
    }
    else if (token.text == "module")
    {
        SWAG_VERIFY(!moduleSpecified, sourceFile->report({sourceFile, token, "#unittest module can only be specified once"}));
        SWAG_CHECK(tokenizer.getTokenOrEOL(token));
        SWAG_VERIFY(token.id != TokenId::EndOfLine, sourceFile->report({sourceFile, token, "missing module name"}));
        SWAG_VERIFY(token.id == TokenId::Identifier, sourceFile->report({sourceFile, token, format("invalid module name '%s'", token.text.c_str())}));
        moduleSpecified = true;
        if (g_CommandLine.test)
        {
            auto newModule = g_Workspace.createOrUseModule(token.text);
            sourceFile->module->removeFile(sourceFile);
            newModule->addFile(sourceFile);
        }
    }
    else
    {
        sourceFile->report({sourceFile, token, format("unknown #unittest parameter '%s'", token.text.c_str())});
        return false;
    }

    return true;
}

bool SyntaxJob::recoverError()
{
    while (true)
    {
        if (token.id == TokenId::CompilerUnitTest)
            break;
        if (token.id == TokenId::SymSemiColon)
            break;
        if (token.id == TokenId::EndOfFile)
            return false;

        sourceFile->silent++;
        if (!tokenizer.getToken(token))
        {
            sourceFile->silent--;
            return false;
        }

        sourceFile->silent--;
	}

	return true;
}

bool SyntaxJob::execute()
{
    if (g_CommandLine.stats)
        g_Stats.numFiles++;
    tokenizer.setFile(sourceFile);

    sourceFile->astRoot = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::RootFile);
	sourceFile->astRoot->flags |= AST_IS_TOPLEVEL;

    bool result = true;
    bool ok     = true;
    while (true)
    {
        // Recover from last syntax error
        if (!ok)
        {
			// If there's an error, then we must stop at syntax pass
			sourceFile->buildPass = min(sourceFile->buildPass, BuildPass::Syntax);
			if (!recoverError())
				return false;
            result = false;
			ok = true;
        }
		else
		{
			ok = tokenizer.getToken(token);
			if (!ok)
				continue;
		}

        if (token.id == TokenId::EndOfFile)
            break;

        // Top level
        if (token.id == TokenId::CompilerUnitTest)
        {
            ok = doCompilerUnitTest();
            continue;
        }

        // Ask for lexer only
        if (sourceFile->buildPass < BuildPass::Syntax)
            continue;

        ok = doTopLevel(sourceFile->astRoot);
    }

    return result;
}
