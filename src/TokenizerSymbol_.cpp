#include "pch.h"
#include "Tokenizer.h"

bool Tokenizer::doSymbol(uint32_t c, Token& token)
{
    unsigned offset;

    token.text.buffer[0] = (uint8_t) c;
    token.text.buffer[1] = 0;
    token.text.count     = 1;

#define ADDC2(__c)                        \
    token.text.buffer[1] = (uint8_t) __c; \
    token.text.buffer[2] = 0;             \
    token.text.count     = 2;

#define ADDC3(__c)                        \
    token.text.buffer[2] = (uint8_t) __c; \
    token.text.buffer[3] = 0;             \
    token.text.count     = 3;

#define ADDC4(__c)                        \
    token.text.buffer[3] = (uint8_t) __c; \
    token.text.buffer[4] = 0;             \
    token.text.count     = 4;

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
            ADDC2(c);
            treatChar(c, offset);
            token.id = TokenId::SymQuestionQuestion;
        }
        return true;

    case '.':
        token.id = TokenId::SymDot;
        c        = getCharNoSeek(offset);
        if (c == '.')
        {
            ADDC2(c);
            treatChar(c, offset);
            token.id = TokenId::SymDotDot;

            c = getCharNoSeek(offset);
            if (c == '.')
            {
                token.id = TokenId::SymDotDotDot;
                ADDC3(c);
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
            ADDC2(c);
            treatChar(c, offset);
        }
        return true;

    case '!':
        token.id = TokenId::SymExclam;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymExclamEqual;
            ADDC2(c);
            treatChar(c, offset);
        }
        return true;

    case '~':
        token.id = TokenId::SymTilde;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymTildeEqual;
            ADDC2(c);
            treatChar(c, offset);
        }
        return true;

    case '=':
        token.id = TokenId::SymEqual;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymEqualEqual;
            ADDC2(c);
            treatChar(c, offset);
        }
        else if (c == '>')
        {
            token.id = TokenId::SymEqualGreater;
            ADDC2(c);
            treatChar(c, offset);
        }
        return true;

    case '-':
        token.id = TokenId::SymMinus;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymMinusEqual;
            ADDC2(c);
            treatChar(c, offset);
        }
        else if (c == '>')
        {
            token.id = TokenId::SymMinusGreat;
            ADDC2(c);
            treatChar(c, offset);
        }
        else if (c == '%')
        {
            token.id = TokenId::SymMinusPercent;
            ADDC2(c);
            treatChar(c, offset);
            c = getCharNoSeek(offset);
            if (c == '=')
            {
                token.id = TokenId::SymMinusPercentEqual;
                ADDC3(c);
                treatChar(c, offset);
            }
        }
        return true;

    case '+':
        token.id = TokenId::SymPlus;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymPlusEqual;
            ADDC2(c);
            treatChar(c, offset);
        }
        else if (c == '%')
        {
            token.id = TokenId::SymPlusPercent;
            ADDC2(c);
            treatChar(c, offset);
            c = getCharNoSeek(offset);
            if (c == '=')
            {
                token.id = TokenId::SymPlusPercentEqual;
                ADDC3(c);
                treatChar(c, offset);
            }
        }
        return true;

    case '*':
        token.id = TokenId::SymAsterisk;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymAsteriskEqual;
            ADDC2(c);
            treatChar(c, offset);
        }
        else if (c == '%')
        {
            token.id = TokenId::SymAsteriskPercent;
            ADDC2(c);
            treatChar(c, offset);
            c = getCharNoSeek(offset);
            if (c == '=')
            {
                token.id = TokenId::SymAsteriskPercentEqual;
                ADDC3(c);
                treatChar(c, offset);
            }
        }
        return true;

    case '/':
        token.id = TokenId::SymSlash;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymSlashEqual;
            ADDC2(c);
            treatChar(c, offset);
        }
        return true;

    case '&':
        token.id = TokenId::SymAmpersand;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymAmpersandEqual;
            ADDC2(c);
            treatChar(c, offset);
        }
        else if (c == '&')
        {
            token.id = TokenId::SymAmpersandAmpersand;
            ADDC2(c);
            treatChar(c, offset);
        }
        return true;

    case '|':
        token.id = TokenId::SymVertical;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymVerticalEqual;
            ADDC2(c);
            treatChar(c, offset);
        }
        else if (c == '|')
        {
            token.id = TokenId::SymVerticalVertical;
            ADDC2(c);
            treatChar(c, offset);
        }
        return true;

    case '^':
        token.id = TokenId::SymCircumflex;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymCircumflexEqual;
            ADDC2(c);
            treatChar(c, offset);
        }
        return true;

    case '%':
        token.id = TokenId::SymPercent;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymPercentEqual;
            ADDC2(c);
            treatChar(c, offset);
        }
        return true;

    case '<':
        token.id = TokenId::SymLower;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymLowerEqual;
            ADDC2(c);
            treatChar(c, offset);
            c = getCharNoSeek(offset);
            if (c == '>')
            {
                token.id = TokenId::SymLowerEqualGreater;
                treatChar(c, offset);
                ADDC3(c);
            }
        }
        else if (c == '<')
        {
            token.id = TokenId::SymLowerLower;
            ADDC2(c);
            treatChar(c, offset);
            c = getCharNoSeek(offset);
            if (c == '=')
            {
                token.id = TokenId::SymLowerLowerEqual;
                treatChar(c, offset);
                ADDC3(c);
            }
            else if (c == '%')
            {
                token.id = TokenId::SymLowerLowerPercent;
                ADDC3(c);
                treatChar(c, offset);

                c = getCharNoSeek(offset);
                if (c == '=')
                {
                    token.id = TokenId::SymLowerLowerPercentEqual;
                    ADDC4(c);
                    treatChar(c, offset);
                }
            }
        }
        return true;

    case '>':
        token.id = TokenId::SymGreater;
        c        = getCharNoSeek(offset);
        if (c == '=')
        {
            token.id = TokenId::SymGreaterEqual;
            ADDC2(c);
            treatChar(c, offset);
        }
        else if (c == '>')
        {
            token.id = TokenId::SymGreaterGreater;
            ADDC2(c);
            treatChar(c, offset);
            c = getCharNoSeek(offset);
            if (c == '=')
            {
                token.id = TokenId::SymGreaterGreaterEqual;
                ADDC3(c);
                treatChar(c, offset);
            }
            else if (c == '%')
            {
                token.id = TokenId::SymGreaterGreaterPercent;
                ADDC3(c);
                treatChar(c, offset);

                c = getCharNoSeek(offset);
                if (c == '=')
                {
                    token.id = TokenId::SymGreaterGreaterPercentEqual;
                    ADDC4(c);
                    treatChar(c, offset);
                }
            }
        }
        return true;
    }

    return false;
}
