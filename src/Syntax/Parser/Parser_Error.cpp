#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Syntax/Ast.h"
#include "Syntax/Parser/Parser.h"

bool Parser::error(AstNode* node, const Utf8& msg, const char* help, const char* note) const
{
    Diagnostic err{node, msg};
    err.addNote(note);
    err.addNote(help);
    return context->report(err);
}

bool Parser::error(const Token& tk, const Utf8& msg, const char* help, const char* note) const
{
    Diagnostic err{sourceFile, tk, msg};
    err.addNote(note);
    err.addNote(help);
    return context->report(err);
}

bool Parser::error(const TokenParse& tk, const Utf8& msg, const char* help, const char* note) const
{
    Diagnostic err{sourceFile, tk, msg};
    err.addNote(note);
    err.addNote(help);
    return context->report(err);
}

bool Parser::error(const SourceLocation& startLocation, const SourceLocation& endLocation, const Utf8& msg, const char* help) const
{
    Diagnostic err{sourceFile, startLocation, endLocation, msg};
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
                Diagnostic err{sourceFile, startToken, formErr(Err0437, startToken.cstr())};
                if (nextToken.is(TokenId::Identifier) && (startToken.is("function") || startToken.is("fn") || startToken.is("def")))
                    err.addNote("hint: use [[func]] to declare a function");
                else if (nextToken.is(TokenId::Identifier) && nextToken.is("fn") && startToken.is("pub"))
                    err.addNote(form("hint: did you mean [[%s]]?", "public"));
                else if (nextToken.is(TokenId::SymLeftParen))
                    err.addNote("hint: use [[func]] to declare a function");
                else if (nextToken.is(TokenId::Identifier) && nextNextToken.is(TokenId::SymLeftParen))
                    err.addNote("hint: use [[func]] to declare a function");
                else if (nextToken.is(TokenId::SymEqual) || nextToken.is(TokenId::SymColon))
                    err.addNote("hint: did you forget [[var]] or [[const]] to declare a global variable or constant?");
                else
                    err.addNote(SemanticError::findClosestMatchesMsg(startToken.text, {}, IdentifierSearchFor::TopLevelInstruction));
                return context->report(err);
            }

            if (startToken.is(TokenId::CompilerElse))
                msg = toErr(Err0259);
            else if (startToken.is(TokenId::CompilerElseIf))
                msg = toErr(Err0258);
            else if (startToken.is(TokenId::SymRightParen))
                msg = toErr(Err0751);
            else if (startToken.is(TokenId::SymRightCurly))
                msg = toErr(Err0752);
            else if (startToken.is(TokenId::SymRightSquare))
                msg = toErr(Err0384);
            else
            {
                msg = toErr(Err0521);
                if (startToken.is(TokenId::KwdLet))
                    note = "hint: use [[var]] or [[const]] instead of [[let]] to declare a global variable or constant";
                else if (startToken.is(TokenId::CompilerInclude))
                    note = "hint: use [[const Value = #include(\"path\")]] to embed an external file in a constant byte array";
                else if (startToken.is(TokenId::NativeType) && nextToken.is(TokenId::Identifier) && nextNextToken.is(TokenId::SymLeftParen))
                    note = "hint: use [[func]] to declare a function";
                else if (startToken.is(TokenId::NativeType) && nextToken.is(TokenId::Identifier) && nextNextToken.is(TokenId::SymEqual))
                    note = form("hint: use the syntax [[var %s: %s]] to declare a variable", nextToken.token.cstr(), startToken.cstr());
                else
                    note = "this is unexpected in global scope";
            }

            break;

        ///////////////////////////////////////////
        case InvalidTokenError::EmbeddedInstruction:
            if (startToken.is(TokenId::SymAmpersandAmpersand))
                msg = formErr(Err0452, "and", "&&");
            else if (startToken.is(TokenId::SymVerticalVertical))
                msg = formErr(Err0452, "or", "||");
            else if (startToken.is(TokenId::KwdElse))
                msg = toErr(Err0318);
            else if (startToken.is(TokenId::KwdElif))
                msg = toErr(Err0317);
            else if (startToken.is(TokenId::CompilerElse))
                msg = toErr(Err0259);
            else if (startToken.is(TokenId::CompilerElseIf))
                msg = toErr(Err0258);
            else if (startToken.is(TokenId::SymRightParen))
                msg = toErr(Err0751);
            else if (startToken.is(TokenId::SymRightCurly))
                msg = toErr(Err0752);
            else if (startToken.is(TokenId::SymRightSquare))
                msg = toErr(Err0384);
            else
            {
                Diagnostic err{sourceFile, startToken, toErr(Err0508)};
                return context->report(err);
            }

            break;

        ///////////////////////////////////////////
        case InvalidTokenError::LeftExpression:
            msg = toErr(Err0570);
            break;

        ///////////////////////////////////////////
        case InvalidTokenError::PrimaryExpression:

            if (Tokenizer::isKeyword(prevTokenParse.token.id) || Tokenizer::isSymbol(prevTokenParse.token.id))
            {
                Diagnostic err{sourceFile, tokenParse, formErr(Err0659, prevTokenParse.cstr())};
                return context->report(err);
            }

            msg = toErr(Err0692);
            break;
    }

    return error(startToken, msg, note.empty() ? nullptr : note.cstr());
}

bool Parser::invalidIdentifierError(const TokenParse& myToken, const char* msg) const
{
    const Utf8 message = msg ? Utf8{msg} : toErr(Err0679);
    Diagnostic err{sourceFile, myToken, message};
    if (Tokenizer::isKeyword(myToken.token.id))
        err.addNote(form("the keyword [[%s]] cannot be used as an identifier", myToken.token.cstr()));
    return context->report(err);
}

bool Parser::endOfLineError(AstNode* leftNode, bool leftAlone)
{
    if (tokenParse.is(TokenId::SymAsterisk) && getNextToken().is(TokenId::SymSlash))
        return error(tokenParse, formErr(Err0753, "left expression"));

    if (!leftAlone)
        return error(tokenParse, formErr(Err0682, "left affectation"));

    if (tokenParse.is(TokenId::SymEqualEqual))
        return error(tokenParse, toErr(Err0293));

    if (leftNode->is(AstNodeKind::IdentifierRef) &&
        leftNode->lastChild()->is(AstNodeKind::Identifier) &&
        tokenParse.is(TokenId::Identifier) &&
        !tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE))
    {
        const auto id = castAst<AstIdentifier>(leftNode->lastChild(), AstNodeKind::Identifier);
        if (!id->callParameters)
        {
            Diagnostic err{sourceFile, tokenParse, toErr(Err0401)};
            const auto nextToken = getNextToken();
            if (Tokenizer::isSymbol(nextToken.token.id) && nextToken.isNot(TokenId::SymSemiColon))
                err.addNote(form("hint: did you forget a [['.']] between [[%s]] and [[%s]]?", id->token.cstr(), tokenParse.cstr()));
            else if (Tokenizer::isStartOfNewStatement(nextToken))
                err.addNote(form("hint: use the syntax [[var %s: %s]] to declare a variable", tokenParse.cstr(), id->token.cstr()));
            else
                err.addNote(leftNode, "this should be a function call or an affectation");
            return context->report(err);
        }
    }

    PushErrCxtStep ec(context, leftNode, ErrCxtStepKind::Note, [] { return "this should be followed by an affectation"; });

    Utf8 afterMsg = "left expression";
    if (leftNode->is(AstNodeKind::IdentifierRef) && leftNode->lastChild()->is(AstNodeKind::Identifier))
        afterMsg = form("identifier [[%s]]", leftNode->lastChild()->token.cstr());
    else if (leftNode->is(AstNodeKind::MultiIdentifierTuple))
        afterMsg = "tuple unpacking";
    else if (leftNode->is(AstNodeKind::MultiIdentifier))
        afterMsg = "variable list";

    if (Tokenizer::isSymbol(tokenParse.token.id))
        return error(tokenParse, formErr(Err0513, tokenParse.cstr(), afterMsg.cstr()));

    return error(tokenParse, formErr(Err0630, afterMsg.cstr()));
}
