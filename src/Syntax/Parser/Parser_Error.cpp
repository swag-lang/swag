#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Syntax/Ast.h"
#include "Syntax/Parser/Parser.h"
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

bool Parser::error(const TokenParse& tk, const Utf8& msg, const char* help, const char* hint) const
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

bool Parser::invalidTokenError(InvalidTokenError kind)
{
    Utf8 msg, note;

    const auto startToken    = tokenParse.token;
    const auto nextToken     = getNextToken(1);
    const auto nextNextToken = getNextToken(2);

    switch (kind)
    {
        ///////////////////////////////////////////
        case InvalidTokenError::TopLevelInstruction:

            // Identifier at global scope
            if (startToken.is(TokenId::Identifier))
            {
                Diagnostic err{sourceFile, startToken, formErr(Err0679, startToken.cstr())};
                if (nextToken.is(TokenId::Identifier) && (startToken.is("function") || startToken.is("fn") || startToken.is("def")))
                    err.addNote(toNte(Nte0060));
                else if (nextToken.is(TokenId::Identifier) && nextToken.is("fn") && startToken.is("pub"))
                    err.addNote(formNte(Nte0082, "public"));                
                else if (nextToken.is(TokenId::SymLeftParen))
                    err.addNote(toNte(Nte0060));
                else if (nextToken.is(TokenId::Identifier) && nextNextToken.is(TokenId::SymLeftParen))
                    err.addNote(toNte(Nte0060));                
                else if (nextToken.is(TokenId::SymEqual) || nextToken.is(TokenId::SymColon))
                    err.addNote(toNte(Nte0079));
                else
                    err.addNote(SemanticError::findClosestMatchesMsg(startToken.text, {}, IdentifierSearchFor::TopLevelInstruction));
                return context->report(err);
            }

            if (startToken.is(TokenId::CompilerElse))
                msg = toErr(Err0283);
            else if (startToken.is(TokenId::CompilerElseIf))
                msg = toErr(Err0282);
            else if (startToken.is(TokenId::SymRightParen))
                msg = toErr(Err0284);
            else if (startToken.is(TokenId::SymRightCurly))
                msg = toErr(Err0288);
            else if (startToken.is(TokenId::SymRightSquare))
                msg = toErr(Err0285);
            else
            {
                msg = toErr(Err0245);
                if (startToken.is(TokenId::KwdLet))
                    note = toNte(Nte0063);
                else if (startToken.is(TokenId::CompilerInclude))
                    note = toNte(Nte0057);
                else if (startToken.is(TokenId::NativeType) && nextToken.is(TokenId::Identifier) && nextNextToken.is(TokenId::SymLeftParen))
                    note = toNte(Nte0060);
                else if (startToken.is(TokenId::NativeType) && nextToken.is(TokenId::Identifier) && nextNextToken.is(TokenId::SymEqual))
                    note = formNte(Nte0069, nextToken.token.cstr(), startToken.cstr());
                else
                    note = toNte(Nte0204);
            }
        
            break;

        ///////////////////////////////////////////
        case InvalidTokenError::EmbeddedInstruction:
            if (startToken.is(TokenId::SymAmpersandAmpersand))
                msg = formErr(Err0207, "and", "&&");
            else if (startToken.is(TokenId::SymVerticalVertical))
                msg = formErr(Err0207, "or", "||");
            else if (startToken.is(TokenId::KwdElse))
                msg = toErr(Err0287);
            else if (startToken.is(TokenId::KwdElif))
                msg = toErr(Err0286);
            else if (startToken.is(TokenId::CompilerElse))
                msg = toErr(Err0283);
            else if (startToken.is(TokenId::CompilerElseIf))
                msg = toErr(Err0282);
            else if (startToken.is(TokenId::SymRightParen))
                msg = toErr(Err0284);
            else if (startToken.is(TokenId::SymRightCurly))
                msg = toErr(Err0288);
            else if (startToken.is(TokenId::SymRightSquare))
                msg = toErr(Err0285);
            else
                msg = toErr(Err0665);
            break;

        ///////////////////////////////////////////
        case InvalidTokenError::LeftExpression:
            msg = toErr(Err0655);
            break;

        ///////////////////////////////////////////
        case InvalidTokenError::PrimaryExpression:

            // Bad character syntax as an expression
            if (startToken.is(TokenId::SymQuote) && nextNextToken.is(TokenId::SymQuote))
            {
                const Diagnostic err{sourceFile, startToken.startLocation, nextNextToken.token.endLocation, formErr(Err0163, nextToken.token.cstr())};
                return context->report(err);
            }

            msg = toErr(Err0177);
            break;
    }

    return error(startToken, msg, note.empty() ? nullptr : note.cstr());
}

bool Parser::invalidIdentifierError(const TokenParse& myToken, const char* msg) const
{
    const Utf8 message = msg ? Utf8{msg} : toErr(Err0194);
    Diagnostic err{sourceFile, myToken, message};
    if (Tokenizer::isKeyword(myToken.token.id))
        err.addNote(formNte(Nte0133, myToken.token.cstr()));
    return context->report(err);
}
