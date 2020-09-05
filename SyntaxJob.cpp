#include "pch.h"
#include "Ast.h"
#include "Module.h"
#include "SyntaxJob.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Scoped.h"

thread_local Pool<SyntaxJob> g_Pool_syntaxJob;

bool SyntaxJob::verifyError(const Token& tk, bool expr, const Utf8& msg)
{
    if (!expr)
        return syntaxError(tk, msg);
    return true;
}

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

bool SyntaxJob::syntaxError(AstNode* node, const Utf8& msg)
{
    Utf8 full = "syntax error";
    if (!msg.empty())
    {
        full += ", ";
        full += msg;
    }

    error(node, full);
    return false;
}

bool SyntaxJob::invalidTokenError(InvalidTokenError kind)
{
    if (Ast::lastGeneratedNode)
    {
        switch (token.id)
        {
        case TokenId::KwdNoDrop:
            if (Ast::lastGeneratedNode->token.id == TokenId::KwdMove)
                return syntaxError(token, "'raw' instruction must be placed before 'move'");
            if (Ast::lastGeneratedNode->token.id == TokenId::KwdNoDrop)
                return syntaxError(token, "'raw' instruction defined twice");
            break;
        case TokenId::KwdMove:
            if (Ast::lastGeneratedNode->token.id == TokenId::KwdMove)
                return syntaxError(token, "'move' instruction defined twice");
            break;
        }
    }

    switch (token.id)
    {
    case TokenId::KwdElse:
        if (kind == InvalidTokenError::EmbeddedInstruction)
            return syntaxError(token, "'else' without a corresponding 'if'");
        break;
    case TokenId::CompilerElse:
        if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
            return syntaxError(token, "'#else' without a corresponding '#if'");
        break;
    case TokenId::CompilerElseIf:
        if (kind == InvalidTokenError::EmbeddedInstruction || kind == InvalidTokenError::TopLevelInstruction)
            return syntaxError(token, "'#elif' without a corresponding '#if'");
        break;
    }

    Utf8 msg;
    switch (token.id)
    {
    case TokenId::Identifier:
        msg = format("identifier '%s' ", token.text.c_str());
        break;
    default:
        msg = format("'%s' ", token.text.c_str());
        break;
    }

    switch (kind)
    {
    case InvalidTokenError::TopLevelInstruction:
        msg += "is invalid as a top level instruction";
        break;
    case InvalidTokenError::EmbeddedInstruction:
        msg += "is invalid as an embedded instruction";
        break;
    case InvalidTokenError::LeftExpression:
        msg += "is invalid as a left expression";
        break;
    case InvalidTokenError::LeftExpressionVar:
        msg += "is invalid as a left expression for variable declaration";
        break;
    case InvalidTokenError::PrimaryExpression:
        msg += "is invalid as an expression";
        break;
    }

    switch (token.id)
    {
    case TokenId::Identifier:
        if (kind == InvalidTokenError::TopLevelInstruction)
        {
            Token nextToken;
            tokenizer.getToken(nextToken);
            if (nextToken.id == TokenId::SymEqual || nextToken.id == TokenId::SymColonEqual || nextToken.id == TokenId::SymColon)
            {
                msg += ", do you miss 'var', 'let' or 'const' to declare a global variable ?";
            }
        }
        break;
    }

    return syntaxError(token, msg);
}

bool SyntaxJob::error(AstNode* node, const Utf8& msg)
{
    sourceFile->report({node, msg.c_str()});
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
        if (token.id == TokenId::EndOfFile)
            SWAG_CHECK(syntaxError(token, format("missing '%s' %s", g_LangSpec.tokenToName(id).c_str(), msg)));
        else
            SWAG_CHECK(syntaxError(token, format("'%s' is expected instead of '%s' %s", g_LangSpec.tokenToName(id).c_str(), token.text.c_str(), msg)));
    }

    SWAG_CHECK(tokenizer.getToken(token));
    return true;
}

bool SyntaxJob::eatSemiCol(const char* msg)
{
    if (token.id != TokenId::SymSemiColon && token.id != TokenId::EndOfFile && !tokenizer.lastTokenIsEOL)
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

bool SyntaxJob::constructEmbedded(const Utf8& content, AstNode* parent, AstNode* fromNode, CompilerAstKind kind)
{
    SourceFile* tmpFile      = g_Allocator.alloc<SourceFile>();
    tmpFile->externalContent = content;
    tmpFile->externalBuffer  = (uint8_t*) tmpFile->externalContent.c_str();
    tmpFile->externalSize    = (uint32_t) tmpFile->externalContent.length();
    tmpFile->module          = parent->sourceFile->module;
    tmpFile->name            = "generated.swg";
    tmpFile->path            = "generated.swg";
    tmpFile->sourceNode      = fromNode;
    sourceFile               = tmpFile;
    currentScope             = parent->ownerScope;
    currentStructScope       = parent->ownerStructScope;
    currentMainNode          = parent->ownerMainNode;
    currentFct               = parent->ownerFct;

    tokenizer.setFile(sourceFile);

    ScopedFlags scopedFlags(this, AST_GENERATED | (parent->flags & (AST_RUN_BLOCK | AST_NO_BACKEND)));
    SWAG_CHECK(tokenizer.getToken(token));
    while (true)
    {
        if (token.id == TokenId::EndOfFile)
            break;
        switch (kind)
        {
        case CompilerAstKind::EmbeddedInstruction:
            SWAG_CHECK(doEmbeddedInstruction(parent));
            break;
        case CompilerAstKind::TopLevelInstruction:
            SWAG_CHECK(doTopLevelInstruction(parent));
            break;
        case CompilerAstKind::StructVarDecl:
            SWAG_CHECK(doVarDecl(parent, nullptr, AstNodeKind::VarDecl));
            break;
        case CompilerAstKind::EnumValue:
            SWAG_CHECK(doEnumValue(parent));
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
    }

    return true;
}

JobResult SyntaxJob::execute()
{
    baseContext        = &context;
    context.job        = this;
    context.sourceFile = sourceFile;
    g_Stats.numFiles++;
    Ast::lastGeneratedNode = nullptr;

    tokenizer.setFile(sourceFile);

    // One named scope per file
    Utf8 scopeName = sourceFile->name;
    SWAG_ASSERT(scopeName.buffer[scopeName.length() - 4] == '.'); // ".swg"
    scopeName.buffer[scopeName.length() - 4] = 0;
    scopeName.count -= 4;
    Ast::normalizeIdentifierName(scopeName);

    module                   = sourceFile->module;
    sourceFile->scopePrivate = Ast::newScope(nullptr, scopeName, ScopeKind::File, module->scopeRoot);
    sourceFile->scopePrivate->flags |= SCOPE_PRIVATE;

    // By default, everything is private if it comes from the test folder
    if (sourceFile->fromTests)
        currentScope = sourceFile->scopePrivate;
    else
        currentScope = module->scopeRoot;

    // Setup root ast for file
    sourceFile->astRoot             = Ast::newNode<AstNode>(this, AstNodeKind::File, sourceFile, module->astRoot);
    sourceFile->scopePrivate->owner = sourceFile->astRoot;
    sourceFile->scopePrivate->owner = sourceFile->astRoot;

    bool result = true;
    bool ok     = tokenizer.getToken(token);
    bool skip   = false;
    while (!skip)
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
        case TokenId::CompilerSkip:
            skip = true;
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
