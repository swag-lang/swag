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

bool Parser::invalidTokenError(InvalidTokenError kind, const AstNode* parent)
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
                Diagnostic err{sourceFile, startToken, formErr(Err0663, startToken.c_str())};
                if (nextToken.is(TokenId::Identifier) && (startToken.is("function") || startToken.is("fn") || startToken.is("def")))
                    err.addNote(toNte(Nte0044));
                else if (nextToken.is(TokenId::SymLeftParen))
                    err.addNote(toNte(Nte0044));
                else if (nextToken.is(TokenId::SymEqual) || nextToken.is(TokenId::SymColon))
                    err.addNote(toNte(Nte0056));
                else
                    err.addNote(SemanticError::findClosestMatchesMsg(startToken.text, {}, IdentifierSearchFor::TopLevelInstruction));
                return context->report(err);
            }

            if (startToken.is(TokenId::CompilerElse))
                msg = toErr(Err0267);
            else if (startToken.is(TokenId::CompilerElseIf))
                msg = toErr(Err0266);
            else if (startToken.is(TokenId::SymRightParen))
                msg = toErr(Err0268);
            else if (startToken.is(TokenId::SymRightCurly))
                msg = toErr(Err0272);
            else if (startToken.is(TokenId::SymRightSquare))
                msg = toErr(Err0269);
            else
                msg = toErr(Err0232);

            if (startToken.is(TokenId::KwdLet))
                note = toNte(Nte0047);
            else if (startToken.is(TokenId::CompilerInclude))
                note = toNte(Nte0151);
            else if (startToken.is(TokenId::NativeType) && nextToken.is(TokenId::Identifier) && nextNextToken.is(TokenId::SymLeftParen))
                note = toNte(Nte0044);
            else if (startToken.is(TokenId::NativeType) && nextToken.is(TokenId::Identifier) && nextNextToken.is(TokenId::SymEqual))
                note = formNte(Nte0197, nextToken.token.c_str(), startToken.c_str());
            else
                note = toNte(Nte0184);
            break;

        ///////////////////////////////////////////
        case InvalidTokenError::EmbeddedInstruction:
            if (startToken.is(TokenId::SymAmpersandAmpersand))
                msg = formErr(Err0194, "and", "&&");
            else if (startToken.is(TokenId::SymVerticalVertical))
                msg = formErr(Err0194, "or", "||");
            else if (startToken.is(TokenId::KwdElse))
                msg = toErr(Err0271);
            else if (startToken.is(TokenId::KwdElif))
                msg = toErr(Err0270);
            else if (startToken.is(TokenId::CompilerElse))
                msg = toErr(Err0267);
            else if (startToken.is(TokenId::CompilerElseIf))
                msg = toErr(Err0266);
            else if (startToken.is(TokenId::SymRightParen))
                msg = toErr(Err0268);
            else if (startToken.is(TokenId::SymRightCurly))
                msg = toErr(Err0272);
            else if (startToken.is(TokenId::SymRightSquare))
                msg = toErr(Err0269);
            else
                msg = toErr(Err0650);
            break;

        ///////////////////////////////////////////
        case InvalidTokenError::LeftExpression:
            msg = toErr(Err0640);
            break;

        ///////////////////////////////////////////
        case InvalidTokenError::PrimaryExpression:

            // Bad character syntax as an expression
            if (startToken.is(TokenId::SymQuote) && nextNextToken.is(TokenId::SymQuote))
            {
                const Diagnostic err{sourceFile, startToken.startLocation, nextNextToken.token.endLocation, formErr(Err0150, nextToken.token.c_str())};
                return context->report(err);
            }

            msg = toErr(Err0165);
            break;
    }

    return error(startToken, msg, note.empty() ? nullptr : note.c_str());
}

bool Parser::invalidIdentifierError(const TokenParse& myToken, const char* msg) const
{
    const Utf8 message = msg ? Utf8{msg} : toErr(Err0181);
    Diagnostic err{sourceFile, myToken, message};
    if (Tokenizer::isKeyword(myToken.token.id))
        err.addNote(formNte(Nte0137, myToken.token.c_str()));
    return context->report(err);
}
