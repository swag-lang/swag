#include "pch.h"
#include "Tokenizer.h"

bool Tokenizer::doSymbol(char32_t c, Token& token)
{
    unsigned offset;
    token.text = c;
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
        if (c == '?')
        {
            token.text += c;
            treatChar(c, offset);
            token.id = TokenId::SymQuestionQuestion;
        }
        return true;

    case '.':
        token.id = TokenId::SymDot;
        c        = getCharNoSeek(offset);
        if (c == '.')
        {
            token.text += c;
            treatChar(c, offset);
            token.id = TokenId::SymDotDot;

            c = getCharNoSeek(offset);
            if (c == '.')
            {
                token.id = TokenId::SymDotDotDot;
                token.text += c;
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
            token.text += c;
            treatChar(c, offset);
        }
        return true;

    case '!':
        token.id = TokenId::SymExclam;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymExclamEqual;
            token.text += c;
            treatChar(c, offset);
        }
        return true;

    case '~':
        token.id = TokenId::SymTilde;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymTildeEqual;
            token.text += c;
            treatChar(c, offset);
        }
        return true;

    case '=':
        token.id = TokenId::SymEqual;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymEqualEqual;
            token.text += c;
            treatChar(c, offset);
        }
        else if (c == '>')
        {
            token.id = TokenId::SymEqualGreater;
            token.text += c;
            treatChar(c, offset);
        }
        return true;

    case '-':
        token.id = TokenId::SymMinus;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymMinusEqual;
            token.text += c;
            treatChar(c, offset);
        }
        else if (c == '>')
        {
            token.id = TokenId::SymMinusGreat;
            token.text += c;
            treatChar(c, offset);
        }
        return true;

    case '+':
        token.id = TokenId::SymPlus;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymPlusEqual;
            token.text += c;
            treatChar(c, offset);
        }
        return true;

    case '*':
        token.id = TokenId::SymAsterisk;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymAsteriskEqual;
            token.text += c;
            treatChar(c, offset);
        }
        return true;

    case '/':
        token.id = TokenId::SymSlash;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymSlashEqual;
            token.text += c;
            treatChar(c, offset);
        }
        return true;

    case '&':
        token.id = TokenId::SymAmpersand;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymAmpersandEqual;
            token.text += c;
            treatChar(c, offset);
        }
        else if (c == '&')
        {
            token.id = TokenId::SymAmpersandAmpersand;
            token.text += c;
            treatChar(c, offset);
        }
        return true;

    case '|':
        token.id = TokenId::SymVertical;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymVerticalEqual;
            token.text += c;
            treatChar(c, offset);
        }
        else if (c == '|')
        {
            token.id = TokenId::SymVerticalVertical;
            token.text += c;
            treatChar(c, offset);
        }
        return true;

    case '^':
        token.id = TokenId::SymCircumflex;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymCircumflexEqual;
            token.text += c;
            treatChar(c, offset);
        }
        return true;

    case '%':
        token.id = TokenId::SymPercent;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymPercentEqual;
            token.text += c;
            treatChar(c, offset);
        }
        return true;

    case '<':
        token.id = TokenId::SymLower;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymLowerEqual;
            token.text += c;
            treatChar(c, offset);
        }
        else if (c == '<')
        {
            token.id = TokenId::SymLowerLower;
            token.text += c;
            treatChar(c, offset);
            c = getCharNoSeek(offset);
            if (c == '=')
            {
                token.id = TokenId::SymLowerLowerEqual;
                treatChar(c, offset);
                token.text += c;
            }
        }
        return true;

    case '>':
        token.id = TokenId::SymGreater;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymGreaterEqual;
            token.text += c;
            treatChar(c, offset);
        }
        else if (c == '>')
        {
            token.id = TokenId::SymGreaterGreater;
            token.text += c;
            treatChar(c, offset);
            c = getCharNoSeek(offset);
            if (c == '=')
            {
                token.id = TokenId::SymGreaterGreaterEqual;
                token.text += c;
                treatChar(c, offset);
            }
        }
        return true;
    }

    return false;
}
