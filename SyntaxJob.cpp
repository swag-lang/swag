#include "pch.h"
#include "Module.h"
#include "SyntaxJob.h"
#include "Diagnostic.h"
#include "Global.h"
#include "CommandLine.h"
#include "Stats.h"
#include "PoolFactory.h"
#include "LanguageSpec.h"

bool SyntaxJob::syntaxError(const Token& tk, const Utf8& msg)
{
    Utf8 full = "syntax error";
    if (!msg.empty())
    {
        full += ", ";
        full += msg;
    }

    error(tk, full);
    return false;
}

bool SyntaxJob::error(const Token& tk, const Utf8& msg)
{
    sourceFile->report({sourceFile, tk, msg.c_str()});
    return false;
}

bool SyntaxJob::eatToken(TokenId id)
{
    SWAG_CHECK(tokenizer.getToken(token));
    if (token.id != id)
        SWAG_CHECK(syntaxError(token, format("'%s' expected instead of '%s'", g_LangSpec.tokenToName(id).c_str(), token.text.c_str())));
    return true;
}

bool SyntaxJob::recoverError()
{
    while (true)
    {
        sourceFile->silent++;
        if (!tokenizer.getToken(token))
        {
            sourceFile->silent--;
            return false;
        }

        sourceFile->silent--;
        if (token.id == TokenId::CompilerUnitTest)
            break;
        if (token.id == TokenId::SymSemiColon)
            break;
        if (token.id == TokenId::EndOfFile)
            return false;
    }

    return true;
}

bool SyntaxJob::execute()
{
    if (g_CommandLine.stats)
        g_Stats.numFiles++;
    tokenizer.setFile(sourceFile);

    // Setup root ast for file
    sourceFile->astRoot = Ast::newNode(&sourceFile->poolFactory->astNode, AstNodeType::File, sourceFile->module->astRoot);

    // Setup current scope as being the module root one
    currentScope = sourceFile->module->scopeRoot;

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
            ok     = true;
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
#ifdef SWAG_TEST_CPP
        sourceFile->buildPass = BuildPass::Syntax;
        continue;
#endif

        canChangeModule = false;
        ok              = doTopLevel(sourceFile->astRoot);
    }

    return result;
}
