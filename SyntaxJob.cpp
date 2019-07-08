#include "pch.h"
#include "Ast.h"
#include "Module.h"
#include "SyntaxJob.h"
#include "Diagnostic.h"
#include "Global.h"
#include "CommandLine.h"
#include "Stats.h"
#include "LanguageSpec.h"
#include "SourceFile.h"
#include "Scope.h"

Pool<SyntaxJob> g_Pool_syntaxJob;

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

bool SyntaxJob::notSupportedError(const Token& tk)
{
    error(tk, "not supported (yet) ! (syntax)");
    return false;
}

bool SyntaxJob::error(const Token& tk, const Utf8& msg)
{
    sourceFile->report({sourceFile, tk, msg.c_str()});
    return false;
}

bool SyntaxJob::error(const SourceLocation& startLocation, const SourceLocation& endLocation, const Utf8& msg)
{
    sourceFile->report({sourceFile, startLocation, endLocation, msg.c_str()});
    return false;
}

bool SyntaxJob::eatToken()
{
    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::eatToken(TokenId id, const char* msg)
{
    if (token.id != id)
    {
        if (!msg)
            msg = "";
        SWAG_CHECK(syntaxError(token, format("'%s' is expected instead of '%s' %s", g_LangSpec.tokenToName(id).c_str(), token.text.c_str(), msg)));
    }

    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::eatSemiCol(const char* msg)
{
    if (token.id != TokenId::SymSemiColon && tokenizer.lastTokenIsEOL)
    {
        if (!msg)
            msg = "";
        SWAG_CHECK(syntaxError(token, format("';' or a end of line is expected instead of '%s' %s", token.text.c_str(), msg)));
    }

    if (token.id == TokenId::SymSemiColon)
        SWAG_CHECK(tokenizer.getToken(token));
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
        {
            while (token.id == TokenId::SymSemiColon || token.id == TokenId::SymRightCurly)
                tokenizer.getToken(token);
            break;
        }

        if (token.id == TokenId::EndOfFile)
            return false;
    }

    return true;
}

JobResult SyntaxJob::execute()
{
    if (g_CommandLine.stats && !sourceFile->externalBuffer)
        g_Stats.numFiles++;

    tokenizer.setFile(sourceFile);

    // One unnamed scope per file
    sourceFile->scopeRoot = Ast::newScope("", ScopeKind::File, sourceFile->module->scopeRoot);
    currentScope          = sourceFile->scopeRoot;

    // Setup root ast for file
    sourceFile->astRoot = Ast::newNode(&g_Pool_astNode, AstNodeKind::File, sourceFile->indexInModule, sourceFile->module->astRoot);
    sourceFile->astRoot->inheritOwnersAndFlags(this);

    bool result = true;
    bool ok     = tokenizer.getToken(token);
    while (true)
    {
        // Recover from last syntax error
        if (!ok)
        {
            // If there's an error, then we must stop at syntax pass
            sourceFile->buildPass = min(sourceFile->buildPass, BuildPass::Syntax);
            sourceFile->module->setBuildPass(sourceFile->buildPass);
            if (!recoverError())
                return JobResult::ReleaseJob;
            result = false;
            ok     = true;
        }

        if (token.id == TokenId::EndOfFile)
            break;

        // Top level
        switch (token.id)
        {
        case TokenId::CompilerUnitTest:
            ok = doCompilerUnitTest();
            continue;
        case TokenId::CompilerModule:
            ok = doCompilerModule();
            continue;
        }

        // Ask for lexer only
        if (sourceFile->buildPass < BuildPass::Syntax)
        {
            ok = tokenizer.getToken(token);
            continue;
        }

        canChangeModule = false;
        ok              = doTopLevelInstruction(sourceFile->astRoot);
    }

    return JobResult::ReleaseJob;
}
