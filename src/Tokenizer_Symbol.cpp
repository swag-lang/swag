#include "pch.h"
#include "Tokenizer.h"

bool Tokenizer::doSymbol(uint32_t c, TokenParse& token)
{
    unsigned offset;

    switch (c)
    {
    case '\'':
        token.id = TokenId::SymQuote;
        return true;
    case '\\':
        token.id = TokenId::SymBackSlash;
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
    case '~':
        token.id = TokenId::SymTilde;
        return true;

    case '.':
        c = peekChar(offset);
        if (c == '.')
        {
            eatChar(c, offset);
            token.id = TokenId::SymDotDot;

            c = peekChar(offset);
            if (c == '.')
            {
                token.id = TokenId::SymDotDotDot;
                eatChar(c, offset);
            }
            else if (c == '<')
            {
                token.id = TokenId::SymDotDotLess;
                eatChar(c, offset);
            }
        }
        else
        {
            token.id = TokenId::SymDot;
        }
        return true;

    case ':':
        c = peekChar(offset);
        if (c == '=')
        {
            token.id = TokenId::SymColonEqual;
            eatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymColon;
        }
        return true;

    case '!':
        c = peekChar(offset);
        if (c == '=')
        {
            token.id = TokenId::SymExclamEqual;
            eatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymExclam;
        }
        return true;

    case '=':
        c = peekChar(offset);
        if (c == '=')
        {
            token.id = TokenId::SymEqualEqual;
            eatChar(c, offset);
        }
        else if (c == '>')
        {
            token.id = TokenId::SymEqualGreater;
            eatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymEqual;
        }
        return true;

    case '-':
        c = peekChar(offset);
        if (c == '=')
        {
            token.id = TokenId::SymMinusEqual;
            eatChar(c, offset);
        }
        else if (c == '>')
        {
            token.id = TokenId::SymMinusGreat;
            eatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymMinus;
        }
        return true;

    case '+':
        c = peekChar(offset);
        if (c == '=')
        {
            token.id = TokenId::SymPlusEqual;
            eatChar(c, offset);
        }
        else if (c == '+')
        {
            token.id = TokenId::SymPlusPlus;
            eatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymPlus;
        }
        return true;

    case '*':
        c = peekChar(offset);
        if (c == '=')
        {
            token.id = TokenId::SymAsteriskEqual;
            eatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymAsterisk;
        }
        return true;

    case '/':
        c = peekChar(offset);
        if (c == '=')
        {
            token.id = TokenId::SymSlashEqual;
            eatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymSlash;
        }
        return true;

    case '&':
        c = peekChar(offset);
        if (c == '=')
        {
            token.id = TokenId::SymAmpersandEqual;
            eatChar(c, offset);
        }
        else if (c == '&')
        {
            token.id = TokenId::SymAmpersandAmpersand;
            eatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymAmpersand;
        }
        return true;

    case '|':
        c = peekChar(offset);
        if (c == '=')
        {
            token.id = TokenId::SymVerticalEqual;
            eatChar(c, offset);
        }
        else if (c == '|')
        {
            token.id = TokenId::SymVerticalVertical;
            eatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymVertical;
        }
        return true;

    case '^':
        c = peekChar(offset);
        if (c == '=')
        {
            token.id = TokenId::SymCircumflexEqual;
            eatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymCircumflex;
        }
        return true;

    case '%':
        c = peekChar(offset);
        if (c == '=')
        {
            token.id = TokenId::SymPercentEqual;
            eatChar(c, offset);
        }
        else
        {
            token.id = TokenId::SymPercent;
        }
        return true;

    case '<':
        c = peekChar(offset);
        if (c == '=')
        {
            token.id = TokenId::SymLowerEqual;
            eatChar(c, offset);
            c = peekChar(offset);
            if (c == '>')
            {
                token.id = TokenId::SymLowerEqualGreater;
                eatChar(c, offset);
            }
        }
        else if (c == '<')
        {
            token.id = TokenId::SymLowerLower;
            eatChar(c, offset);
            c = peekChar(offset);
            if (c == '=')
            {
                token.id = TokenId::SymLowerLowerEqual;
                eatChar(c, offset);
            }
        }
        else
        {
            token.id = TokenId::SymLower;
        }
        return true;

    case '>':
        c = peekChar(offset);
        if (c == '=')
        {
            token.id = TokenId::SymGreaterEqual;
            eatChar(c, offset);
        }
        else if (c == '>')
        {
            token.id = TokenId::SymGreaterGreater;
            eatChar(c, offset);
            c = peekChar(offset);
            if (c == '=')
            {
                token.id = TokenId::SymGreaterGreaterEqual;
                eatChar(c, offset);
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
