#include "pch.h"
#include "Tokenizer.h"

bool Tokenizer::doSymbol(uint32_t c, Token& token)
{
    unsigned offset;

    switch (c)
    {
    case '\'':
        token.id = TokenId::SymQuote;
        return true;
    case '`':
        token.id = TokenId::SymBackTick;
        return true;
    case '\\':
        token.id = TokenId::SymBackSlash;
        return true;
    case '$':
        token.id = TokenId::SymDollar;
        return true;
    case '(':
        token.id = TokenId::SymLeftParen;
        return true;
    case ')':
        token.id = TokenId::SymRightParen;
        return true;
    case '[':
        token.id = TokenId::SymLeftSquare;
        return true;
    case ']':
        token.id = TokenId::SymRightSquare;
        return true;
    case '{':
        token.id = TokenId::SymLeftCurly;
        return true;
    case '}':
        token.id = TokenId::SymRightCurly;
        return true;
    case ';':
        token.id = TokenId::SymSemiColon;
        return true;
    case ',':
        token.id = TokenId::SymComma;
        return true;
    case '@':
        token.id = TokenId::SymAt;
        return true;

    case '?':
        token.id = TokenId::SymQuestion;
        c        = getCharNoSeek(offset);
        return true;

    case '.':
        token.id = TokenId::SymDot;
        c        = getCharNoSeek(offset);
        if (c == '.')
        {
            treatChar(c, offset);
            token.id = TokenId::SymDotDot;

            c = getCharNoSeek(offset);
            if (c == '.')
            {
                token.id = TokenId::SymDotDotDot;
                treatChar(c, offset);
            }
            else if (c == '<')
            {
                token.id = TokenId::SymDotDotLess;
                treatChar(c, offset);
            }
        }
        return true;

    case ':':
        token.id = TokenId::SymColon;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymColonEqual;
            treatChar(c, offset);
        }
        return true;

    case '!':
        token.id = TokenId::SymExclam;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymExclamEqual;
            treatChar(c, offset);
        }
        return true;

    case '~':
        token.id = TokenId::SymTilde;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymTildeEqual;
            treatChar(c, offset);
        }
        return true;

    case '=':
        token.id = TokenId::SymEqual;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymEqualEqual;
            treatChar(c, offset);
        }
        else if (c == '>')
        {
            token.id = TokenId::SymEqualGreater;
            treatChar(c, offset);
        }
        return true;

    case '-':
        token.id = TokenId::SymMinus;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymMinusEqual;
            treatChar(c, offset);
        }
        else if (c == '>')
        {
            token.id = TokenId::SymMinusGreat;
            treatChar(c, offset);
        }
        return true;

    case '+':
        token.id = TokenId::SymPlus;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymPlusEqual;
            treatChar(c, offset);
        }
        return true;

    case '*':
        token.id = TokenId::SymAsterisk;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymAsteriskEqual;
            treatChar(c, offset);
        }
        return true;

    case '/':
        token.id = TokenId::SymSlash;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymSlashEqual;
            treatChar(c, offset);
        }
        return true;

    case '&':
        token.id = TokenId::SymAmpersand;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymAmpersandEqual;
            treatChar(c, offset);
        }
        else if (c == '&')
        {
            token.id = TokenId::SymAmpersandAmpersand;
            treatChar(c, offset);
        }
        return true;

    case '|':
        token.id = TokenId::SymVertical;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymVerticalEqual;
            treatChar(c, offset);
        }
        else if (c == '|')
        {
            token.id = TokenId::SymVerticalVertical;
            treatChar(c, offset);
        }
        return true;

    case '^':
        token.id = TokenId::SymCircumflex;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymCircumflexEqual;
            treatChar(c, offset);
        }
        return true;

    case '%':
        token.id = TokenId::SymPercent;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymPercentEqual;
            treatChar(c, offset);
        }
        return true;

    case '<':
        token.id = TokenId::SymLower;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymLowerEqual;
            treatChar(c, offset);
            c = getCharNoSeek(offset);
            if (c == '>')
            {
                token.id = TokenId::SymLowerEqualGreater;
                treatChar(c, offset);
            }
        }
        else if (c == '<')
        {
            token.id = TokenId::SymLowerLower;
            treatChar(c, offset);
            c = getCharNoSeek(offset);
            if (c == '=')
            {
                token.id = TokenId::SymLowerLowerEqual;
                treatChar(c, offset);
            }
        }
        return true;

    case '>':
        token.id = TokenId::SymGreater;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymGreaterEqual;
            treatChar(c, offset);
        }
        else if (c == '>')
        {
            token.id = TokenId::SymGreaterGreater;
            treatChar(c, offset);
            c = getCharNoSeek(offset);
            if (c == '=')
            {
                token.id = TokenId::SymGreaterGreaterEqual;
                treatChar(c, offset);
            }
        }
        return true;
    }

    return false;
}
