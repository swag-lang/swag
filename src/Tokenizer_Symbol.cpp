#include "pch.h"
#include "Tokenizer.h"

bool Tokenizer::doSymbol(TokenParse& token, uint32_t c)
{
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

    case '=':
        if (curBuffer[0] == '=')
        {
            token.id = TokenId::SymEqualEqual;
            curBuffer++;
            location.column++;
        }
        else if (curBuffer[0] == '>')
        {
            token.id = TokenId::SymEqualGreater;
            curBuffer++;
            location.column++;
        }
        else
        {
            token.id = TokenId::SymEqual;
        }
        return true;

    case '-':
        if (curBuffer[0] == '=')
        {
            token.id = TokenId::SymMinusEqual;
            curBuffer++;
            location.column++;
        }
        else if (curBuffer[0] == '>')
        {
            token.id = TokenId::SymMinusGreat;
            curBuffer++;
            location.column++;
        }
        else
        {
            token.id = TokenId::SymMinus;
        }
        return true;

    case ':':
        token.id = TokenId::SymColon;
        return true;

    case '!':
        if (curBuffer[0] == '=')
        {
            token.id = TokenId::SymExclamEqual;
            curBuffer++;
            location.column++;
        }
        else
        {
            token.id = TokenId::SymExclam;
        }
        return true;

    case '+':
        if (curBuffer[0] == '=')
        {
            token.id = TokenId::SymPlusEqual;
            curBuffer++;
            location.column++;
        }
        else if (curBuffer[0] == '+')
        {
            token.id = TokenId::SymPlusPlus;
            curBuffer++;
            location.column++;
        }
        else
        {
            token.id = TokenId::SymPlus;
        }
        return true;

    case '*':
        if (curBuffer[0] == '=')
        {
            token.id = TokenId::SymAsteriskEqual;
            curBuffer++;
            location.column++;
        }
        else
        {
            token.id = TokenId::SymAsterisk;
        }
        return true;

    case '/':
        if (curBuffer[0] == '=')
        {
            token.id = TokenId::SymSlashEqual;
            curBuffer++;
            location.column++;
        }
        else
        {
            token.id = TokenId::SymSlash;
        }
        return true;

    case '&':
        if (curBuffer[0] == '=')
        {
            token.id = TokenId::SymAmpersandEqual;
            curBuffer++;
            location.column++;
        }
        else if (curBuffer[0] == '&')
        {
            token.id = TokenId::SymAmpersandAmpersand;
            curBuffer++;
            location.column++;
        }
        else
        {
            token.id = TokenId::SymAmpersand;
        }
        return true;

    case '|':
        if (curBuffer[0] == '=')
        {
            token.id = TokenId::SymVerticalEqual;
            curBuffer++;
            location.column++;
        }
        else if (curBuffer[0] == '|')
        {
            token.id = TokenId::SymVerticalVertical;
            curBuffer++;
            location.column++;
        }
        else
        {
            token.id = TokenId::SymVertical;
        }
        return true;

    case '^':
        if (curBuffer[0] == '=')
        {
            token.id = TokenId::SymCircumflexEqual;
            curBuffer++;
            location.column++;
        }
        else
        {
            token.id = TokenId::SymCircumflex;
        }
        return true;

    case '%':
        if (curBuffer[0] == '=')
        {
            token.id = TokenId::SymPercentEqual;
            curBuffer++;
            location.column++;
        }
        else
        {
            token.id = TokenId::SymPercent;
        }
        return true;

    case '.':
        if (curBuffer[0] == '.')
        {
            if (curBuffer[1] == '.')
            {
                token.id = TokenId::SymDotDotDot;
                curBuffer += 2;
                location.column += 2;
            }
            else if (curBuffer[1] == '<')
            {
                token.id = TokenId::SymDotDotLess;
                curBuffer += 2;
                location.column += 2;
            }
            else
            {
                token.id = TokenId::SymDotDot;
                curBuffer++;
                location.column++;
            }
        }
        else
        {
            token.id = TokenId::SymDot;
        }
        return true;

    case '<':
        if (curBuffer[0] == '=')
        {
            if (curBuffer[1] == '>')
            {
                token.id = TokenId::SymLowerEqualGreater;
                curBuffer += 2;
                location.column += 2;
            }
            else
            {
                token.id = TokenId::SymLowerEqual;
                curBuffer++;
                location.column++;
            }
        }
        else if (curBuffer[0] == '<')
        {
            if (curBuffer[1] == '=')
            {
                token.id = TokenId::SymLowerLowerEqual;
                curBuffer += 2;
                location.column += 2;
            }
            else
            {
                token.id = TokenId::SymLowerLower;
                curBuffer++;
                location.column++;
            }
        }
        else
        {
            token.id = TokenId::SymLower;
        }
        return true;

    case '>':
        if (curBuffer[0] == '=')
        {
            token.id = TokenId::SymGreaterEqual;
            curBuffer++;
            location.column++;
        }
        else if (curBuffer[0] == '>')
        {
            if (curBuffer[1] == '=')
            {
                token.id = TokenId::SymGreaterGreaterEqual;
                curBuffer += 2;
                location.column += 2;
            }
            else
            {
                token.id = TokenId::SymGreaterGreater;
                curBuffer++;
                location.column++;
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
