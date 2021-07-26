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
        return true;

    case '.':
        c = getCharNoSeek(offset);
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
        else
        {
            token.id = TokenId::SymDot;
        }
        return true;

    case ':':
        c = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymColonEqual;
            treatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymColon;
        }
        return true;

    case '!':
        c = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymExclamEqual;
            treatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymExclam;
        }
        return true;

    case '~':
        c = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymTildeEqual;
            treatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymTilde;
        }
        return true;

    case '=':
        c = getCharNoSeek(offset);
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
        else
        {
            token.id = TokenId::SymEqual;
        }
        return true;

    case '-':
        c = getCharNoSeek(offset);
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
        else
        {
            token.id = TokenId::SymMinus;
        }
        return true;

    case '+':
        c = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymPlusEqual;
            treatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymPlus;
        }
        return true;

    case '*':
        c = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymAsteriskEqual;
            treatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymAsterisk;
        }
        return true;

    case '/':
        c = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymSlashEqual;
            treatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymSlash;
        }
        return true;

    case '&':
        c = getCharNoSeek(offset);
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
        else
        {
            token.id = TokenId::SymAmpersand;
        }
        return true;

    case '|':
        c = getCharNoSeek(offset);
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
        else
        {
            token.id = TokenId::SymVertical;
        }
        return true;

    case '^':
        c = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymCircumflexEqual;
            treatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymCircumflex;
        }
        return true;

    case '%':
        c = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymPercentEqual;
            treatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymPercent;
        }
        return true;

    case '<':
        c = getCharNoSeek(offset);
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
        else
        {
            token.id = TokenId::SymLower;
        }
        return true;

    case '>':
        c = getCharNoSeek(offset);
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
        else
        {
            token.id = TokenId::SymGreater;
        }
        return true;
    }

    return false;
}
