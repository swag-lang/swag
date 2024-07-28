#include "pch.h"
#include "Syntax/Parser/Parser.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Syntax/Ast.h"
#include "Wmf/Module.h"

bool Parser::error(AstNode* node, const Utf8& msg, const char* help, const char* hint) const
{
    Diagnostic err{node, msg};
    if (hint)
        err.hint = hint;
    if (help)
        err.addNote(help);
    return context->report(err);
}

bool Parser::error(const Token& tk, const Utf8& msg, const char* help, const char* hint) const
{
    Diagnostic err{sourceFile, tk, msg};
    if (hint)
        err.hint = hint;
    if (help)
        err.addNote(help);
    return context->report(err);
}

bool Parser::error(const SourceLocation& startLocation, const SourceLocation& endLocation, const Utf8& msg, const char* help) const
{
    Diagnostic err{sourceFile, startLocation, endLocation, msg};
    if (help)
        err.addNote(help);
    return context->report(err);
}

bool Parser::unexpectedTokenError(const Token& tk, const Utf8& msg, const char* help, const char* hint) const
{
    Diagnostic err{sourceFile, tk, msg};
    prepareExpectTokenError(err);
    if (hint)
        err.hint = hint;
    if (help)
        err.addNote(help);
    return context->report(err);
}

bool Parser::invalidTokenError(InvalidTokenError kind, const AstNode* parent)
{
    Utf8 msg, note;

    switch (kind)
    {
        ///////////////////////////////////////////
        case InvalidTokenError::TopLevelInstruction:

            // Identifier at global scope
            if (tokenParse.is(TokenId::Identifier))
            {
                const auto tokenIdentifier = tokenParse.token;
                eatToken();

                Diagnostic err{sourceFile, tokenIdentifier, formErr(Err0689, tokenIdentifier.c_str())};
                if (tokenParse.is(TokenId::Identifier))
                {
                    if (tokenIdentifier.is("function") || tokenIdentifier.is("fn") || tokenIdentifier.is("def"))
                        err.addNote(toNte(Nte0040));
                }
                else if (tokenParse.is(TokenId::SymEqual) || tokenParse.is(TokenId::SymColon))
                {
                    err.addNote(toNte(Nte0053));
                }

                if (!err.hasNotes())
                {
                    const Utf8 appendMsg = SemanticError::findClosestMatchesMsg(tokenIdentifier.text, {}, IdentifierSearchFor::TopLevelInstruction);
                    if (!appendMsg.empty())
                        err.addNote(appendMsg);
                }

                return context->report(err);
            }

            if (tokenParse.token.is(TokenId::CompilerElse))
                msg = toErr(Err0657);
            else if (tokenParse.token.is(TokenId::CompilerElseIf))
                msg = toErr(Err0656);
            else if (tokenParse.token.is(TokenId::SymRightParen))
                msg = toErr(Err0660);
            else if (tokenParse.token.is(TokenId::SymRightCurly))
                msg = toErr(Err0673);
            else if (tokenParse.token.is(TokenId::SymRightSquare))
                msg = toErr(Err0661);
            else
                msg = formErr(Err0381, tokenParse.token.c_str());

            if (tokenParse.is(TokenId::KwdLet))
                note = toNte(Nte0205);
            else
                note = toNte(Nte0167);
            break;

        ///////////////////////////////////////////
        case InvalidTokenError::EmbeddedInstruction:
            if (tokenParse.token.is(TokenId::SymAmpersandAmpersand))
                msg = formErr(Err0323, "and", "&&");
            else if (tokenParse.token.is(TokenId::SymVerticalVertical))
                msg = formErr(Err0323, "or", "||");
            else if (tokenParse.token.is(TokenId::KwdElse))
                msg = toErr(Err0665);
            else if (tokenParse.token.is(TokenId::KwdElif))
                msg = toErr(Err0664);
            else if (tokenParse.token.is(TokenId::CompilerElse))
                msg = toErr(Err0657);
            else if (tokenParse.token.is(TokenId::CompilerElseIf))
                msg = toErr(Err0656);
            else if (tokenParse.token.is(TokenId::SymRightParen))
                msg = toErr(Err0660);
            else if (tokenParse.token.is(TokenId::SymRightCurly))
                msg = toErr(Err0673);
            else if (tokenParse.token.is(TokenId::SymRightSquare))
                msg = toErr(Err0661);
            else
                msg = formErr(Err0262, tokenParse.token.c_str());
            break;

        ///////////////////////////////////////////
        case InvalidTokenError::LeftExpression:
            msg = formErr(Err0283, tokenParse.token.c_str());
            break;

        ///////////////////////////////////////////
        case InvalidTokenError::PrimaryExpression:

            // Bad character syntax as an expression
            if (tokenParse.is(TokenId::SymQuote))
            {
                tokenizer.saveState(tokenParse);
                const auto startToken = tokenParse;
                eatToken();
                const auto inToken = tokenParse;
                eatToken();
                if (tokenParse.is(TokenId::SymQuote))
                {
                    const Diagnostic err{sourceFile, startToken.token.startLocation, tokenParse.token.endLocation, formErr(Err0237, inToken.token.c_str())};
                    return context->report(err);
                }

                tokenizer.restoreState(tokenParse);
            }

            if (parent && Tokenizer::isKeyword(parent->token.id))
                msg = formErr(Err0281, form("[[%s]]", parent->token.c_str()).c_str(), tokenParse.token.c_str());
            else if (parent && Tokenizer::isCompiler(parent->token.id))
                msg = formErr(Err0281, form("the compiler directive [[%s]]", parent->token.c_str()).c_str(), tokenParse.token.c_str());
            else if (parent && Tokenizer::isSymbol(parent->token.id))
                msg = formErr(Err0281, form("the symbol [[%s]]", parent->token.c_str()).c_str(), tokenParse.token.c_str());
            else
                msg = formErr(Err0283, tokenParse.token.c_str());

            break;
    }

    return error(tokenParse.token, msg, note.empty() ? nullptr : note.c_str());
}

bool Parser::invalidIdentifierError(const TokenParse& myToken, const char* msg) const
{
    Diagnostic err{sourceFile, myToken.token, msg ? msg : formErr(Err0310, myToken.token.c_str()).c_str()};
    prepareExpectTokenError(err);
    if (Tokenizer::isKeyword(myToken.token.id))
        err.addNote(formNte(Nte0125, myToken.token.c_str()));
    return context->report(err);
}

void Parser::prepareExpectTokenError(Diagnostic& diag) const
{
    // If we expect a token, and the bad token is the first of the line, then
    // it's better to display the requested token at the end of the previous line
    if (tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE))
    {
        diag.addNote(diag.startLocation, diag.endLocation, formNte(Nte0201, tokenParse.token.c_str()));
        diag.startLocation = prevTokenParse.token.endLocation;
        diag.endLocation   = tokenParse.token.startLocation;
    }
}
