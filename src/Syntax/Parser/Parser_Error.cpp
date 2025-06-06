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
                Diagnostic err{sourceFile, startToken, formErr(Err0645, startToken.cstr())};
                if (nextToken.is(TokenId::Identifier) && (startToken.is("function") || startToken.is("fn") || startToken.is("def")))
                    err.addNote(toNte(Nte0057));
                else if (nextToken.is(TokenId::Identifier) && nextToken.is("fn") && startToken.is("pub"))
                    err.addNote(formNte(Nte0079, "public"));
                else if (nextToken.is(TokenId::SymLeftParen))
                    err.addNote(toNte(Nte0057));
                else if (nextToken.is(TokenId::Identifier) && nextNextToken.is(TokenId::SymLeftParen))
                    err.addNote(toNte(Nte0057));
                else if (nextToken.is(TokenId::SymEqual) || nextToken.is(TokenId::SymColon))
                    err.addNote(toNte(Nte0076));
                else
                    err.addNote(SemanticError::findClosestMatchesMsg(startToken.text, {}, IdentifierSearchFor::TopLevelInstruction));
                return context->report(err);
            }

            if (startToken.is(TokenId::CompilerElse))
                msg = toErr(Err0280);
            else if (startToken.is(TokenId::CompilerElseIf))
                msg = toErr(Err0279);
            else if (startToken.is(TokenId::SymRightParen))
                msg = toErr(Err0281);
            else if (startToken.is(TokenId::SymRightCurly))
                msg = toErr(Err0285);
            else if (startToken.is(TokenId::SymRightSquare))
                msg = toErr(Err0282);
            else
            {
                msg = toErr(Err0247);
                if (startToken.is(TokenId::KwdLet))
                    note = toNte(Nte0060);
                else if (startToken.is(TokenId::CompilerInclude))
                    note = toNte(Nte0054);
                else if (startToken.is(TokenId::NativeType) && nextToken.is(TokenId::Identifier) && nextNextToken.is(TokenId::SymLeftParen))
                    note = toNte(Nte0057);
                else if (startToken.is(TokenId::NativeType) && nextToken.is(TokenId::Identifier) && nextNextToken.is(TokenId::SymEqual))
                    note = formNte(Nte0066, nextToken.token.cstr(), startToken.cstr());
                else
                    note = toNte(Nte0203);
            }

            break;

        ///////////////////////////////////////////
        case InvalidTokenError::EmbeddedInstruction:
            if (startToken.is(TokenId::SymAmpersandAmpersand))
                msg = formErr(Err0210, "and", "&&");
            else if (startToken.is(TokenId::SymVerticalVertical))
                msg = formErr(Err0210, "or", "||");
            else if (startToken.is(TokenId::KwdElse))
                msg = toErr(Err0284);
            else if (startToken.is(TokenId::KwdElif))
                msg = toErr(Err0283);
            else if (startToken.is(TokenId::CompilerElse))
                msg = toErr(Err0280);
            else if (startToken.is(TokenId::CompilerElseIf))
                msg = toErr(Err0279);
            else if (startToken.is(TokenId::SymRightParen))
                msg = toErr(Err0281);
            else if (startToken.is(TokenId::SymRightCurly))
                msg = toErr(Err0285);
            else if (startToken.is(TokenId::SymRightSquare))
                msg = toErr(Err0282);
            else if (startToken.is(TokenId::CompilerPlaceHolder))
                msg = toErr(Err0309);
            else
            {
                Diagnostic err{sourceFile, startToken, toErr(Err0630)};
                return context->report(err);
            }

            break;

        ///////////////////////////////////////////
        case InvalidTokenError::LeftExpression:
            msg = toErr(Err0620);
            break;

        ///////////////////////////////////////////
        case InvalidTokenError::PrimaryExpression:

            if (Tokenizer::isKeyword(prevTokenParse.token.id) || Tokenizer::isSymbol(prevTokenParse.token.id))
            {
                Diagnostic err{sourceFile, tokenParse, formErr(Err0184, prevTokenParse.cstr())};
                return context->report(err);
            }

            msg = toErr(Err0738);
            break;
    }

    return error(startToken, msg, note.empty() ? nullptr : note.cstr());
}

bool Parser::invalidIdentifierError(const TokenParse& myToken, const char* msg) const
{
    const Utf8 message = msg ? Utf8{msg} : toErr(Err0200);
    Diagnostic err{sourceFile, myToken, message};
    if (Tokenizer::isKeyword(myToken.token.id))
        err.addNote(formNte(Nte0130, myToken.token.cstr()));
    return context->report(err);
}

bool Parser::endOfLineError(AstNode* leftNode, bool leftAlone)
{
    if (tokenParse.is(TokenId::SymAsterisk) && getNextToken().is(TokenId::SymSlash))
        return error(tokenParse, formErr(Err0286, "left expression"));

    if (!leftAlone)
        return error(tokenParse, formErr(Err0446, "left affectation"));

    if (tokenParse.is(TokenId::SymEqualEqual))
        return error(tokenParse, toErr(Err0627));

    if (leftNode->is(AstNodeKind::IdentifierRef) &&
        leftNode->lastChild()->is(AstNodeKind::Identifier) &&
        tokenParse.is(TokenId::Identifier) &&
        !tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE))
    {
        const auto id = castAst<AstIdentifier>(leftNode->lastChild(), AstNodeKind::Identifier);
        if (!id->callParameters)
        {
            Diagnostic err{sourceFile, tokenParse, toErr(Err0644)};
            const auto nextToken = getNextToken();
            if (Tokenizer::isSymbol(nextToken.token.id) && nextToken.isNot(TokenId::SymSemiColon))
                err.addNote(formNte(Nte0078, id->token.cstr(), tokenParse.cstr()));
            else if (Tokenizer::isStartOfNewStatement(nextToken))
                err.addNote(formNte(Nte0066, tokenParse.cstr(), id->token.cstr()));
            else
                err.addNote(leftNode, toNte(Nte0208));
            return context->report(err);
        }
    }

    PushErrCxtStep ec(context, leftNode, ErrCxtStepKind::Note, [] { return toNte(Nte0210); });

    Utf8 afterMsg = "left expression";
    if (leftNode->is(AstNodeKind::IdentifierRef) && leftNode->lastChild()->is(AstNodeKind::Identifier))
        afterMsg = form("identifier [[%s]]", leftNode->lastChild()->token.cstr());
    else if (leftNode->is(AstNodeKind::MultiIdentifierTuple))
        afterMsg = "tuple unpacking";
    else if (leftNode->is(AstNodeKind::MultiIdentifier))
        afterMsg = "variable list";

    if (Tokenizer::isSymbol(tokenParse.token.id))
        return error(tokenParse, formErr(Err0659, tokenParse.cstr(), afterMsg.cstr()));

    return error(tokenParse, formErr(Err0438, afterMsg.cstr()));
}
