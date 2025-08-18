#include "pch.h"
#include "Syntax/Tokenizer/Tokenizer.h"

bool Tokenizer::doSymbol(TokenParse& tokenParse, uint32_t c)
{
    switch (c)
    {
        case '\'':
            tokenParse.token.id = TokenId::SymQuote;
            return true;
        case '\\':
            tokenParse.token.id = TokenId::SymBackSlash;
            return true;
        case '(':
            tokenParse.token.id = TokenId::SymLeftParen;
            return true;
        case ')':
            tokenParse.token.id = TokenId::SymRightParen;
            return true;
        case '[':
            tokenParse.token.id = TokenId::SymLeftSquare;
            return true;
        case ']':
            tokenParse.token.id = TokenId::SymRightSquare;
            return true;
        case '{':
            lastOpenLeftCurly = tokenParse.token.startLocation;
            tokenParse.token.id = TokenId::SymLeftCurly;
            return true;
        case '}':
            tokenParse.token.id = TokenId::SymRightCurly;
            return true;
        case ';':
            tokenParse.token.id = TokenId::SymSemiColon;
            return true;
        case ',':
            tokenParse.token.id = TokenId::SymComma;
            return true;
        case '@':
            tokenParse.token.id = TokenId::SymAt;
            return true;
        case '?':
            tokenParse.token.id = TokenId::SymQuestion;
            return true;
        case '~':
            tokenParse.token.id = TokenId::SymTilde;
            return true;

        case '=':
            if (curBuffer[0] == '=')
            {
                tokenParse.token.id = TokenId::SymEqualEqual;
                curBuffer++;
                location.column++;
            }
            else if (curBuffer[0] == '>')
            {
                tokenParse.token.id = TokenId::SymEqualGreater;
                curBuffer++;
                location.column++;
            }
            else
            {
                tokenParse.token.id = TokenId::SymEqual;
            }
            return true;

        case ':':
            tokenParse.token.id = TokenId::SymColon;
            return true;

        case '!':
            if (curBuffer[0] == '=')
            {
                tokenParse.token.id = TokenId::SymExclamEqual;
                curBuffer++;
                location.column++;
            }
            else
            {
                tokenParse.token.id = TokenId::SymExclam;
            }
            return true;

        case '-':
            if (curBuffer[0] == '=')
            {
                tokenParse.token.id = TokenId::SymMinusEqual;
                curBuffer++;
                location.column++;
            }
            else if (curBuffer[0] == '>')
            {
                tokenParse.token.id = TokenId::SymMinusGreat;
                curBuffer++;
                location.column++;
            }
            else if (curBuffer[0] == '-')
            {
                tokenParse.token.id = TokenId::SymMinusMinus;
                curBuffer++;
                location.column++;
            }
            else
            {
                tokenParse.token.id = TokenId::SymMinus;
            }
            return true;

        case '+':
            if (curBuffer[0] == '=')
            {
                tokenParse.token.id = TokenId::SymPlusEqual;
                curBuffer++;
                location.column++;
            }
            else if (curBuffer[0] == '+')
            {
                tokenParse.token.id = TokenId::SymPlusPlus;
                curBuffer++;
                location.column++;
            }
            else
            {
                tokenParse.token.id = TokenId::SymPlus;
            }
            return true;

        case '*':
            if (curBuffer[0] == '=')
            {
                tokenParse.token.id = TokenId::SymAsteriskEqual;
                curBuffer++;
                location.column++;
            }
            else
            {
                tokenParse.token.id = TokenId::SymAsterisk;
            }
            return true;

        case '/':
            if (curBuffer[0] == '=')
            {
                tokenParse.token.id = TokenId::SymSlashEqual;
                curBuffer++;
                location.column++;
            }
            else
            {
                tokenParse.token.id = TokenId::SymSlash;
            }
            return true;

        case '&':
            if (curBuffer[0] == '=')
            {
                tokenParse.token.id = TokenId::SymAmpersandEqual;
                curBuffer++;
                location.column++;
            }
            else if (curBuffer[0] == '&')
            {
                tokenParse.token.id = TokenId::SymAmpersandAmpersand;
                curBuffer++;
                location.column++;
            }
            else
            {
                tokenParse.token.id = TokenId::SymAmpersand;
            }
            return true;

        case '|':
            if (curBuffer[0] == '=')
            {
                tokenParse.token.id = TokenId::SymVerticalEqual;
                curBuffer++;
                location.column++;
            }
            else if (curBuffer[0] == '|')
            {
                tokenParse.token.id = TokenId::SymVerticalVertical;
                curBuffer++;
                location.column++;
            }
            else
            {
                tokenParse.token.id = TokenId::SymVertical;
            }
            return true;

        case '^':
            if (curBuffer[0] == '=')
            {
                tokenParse.token.id = TokenId::SymCircumflexEqual;
                curBuffer++;
                location.column++;
            }
            else
            {
                tokenParse.token.id = TokenId::SymCircumflex;
            }
            return true;

        case '%':
            if (curBuffer[0] == '=')
            {
                tokenParse.token.id = TokenId::SymPercentEqual;
                curBuffer++;
                location.column++;
            }
            else
            {
                tokenParse.token.id = TokenId::SymPercent;
            }
            return true;

        case '.':
            if (curBuffer[0] == '.')
            {
                if (curBuffer[1] == '.')
                {
                    tokenParse.token.id = TokenId::SymDotDotDot;
                    curBuffer += 2;
                    location.column += 2;
                }
                else if (curBuffer[1] == '<')
                {
                    tokenParse.token.id = TokenId::SymDotDotLess;
                    curBuffer += 2;
                    location.column += 2;
                }
                else
                {
                    tokenParse.token.id = TokenId::SymDotDot;
                    curBuffer++;
                    location.column++;
                }
            }
            else
            {
                tokenParse.token.id = TokenId::SymDot;
            }
            return true;

        case '<':
            if (curBuffer[0] == '=')
            {
                if (curBuffer[1] == '>')
                {
                    tokenParse.token.id = TokenId::SymLowerEqualGreater;
                    curBuffer += 2;
                    location.column += 2;
                }
                else
                {
                    tokenParse.token.id = TokenId::SymLowerEqual;
                    curBuffer++;
                    location.column++;
                }
            }
            else if (curBuffer[0] == '<')
            {
                if (curBuffer[1] == '=')
                {
                    tokenParse.token.id = TokenId::SymLowerLowerEqual;
                    curBuffer += 2;
                    location.column += 2;
                }
                else
                {
                    tokenParse.token.id = TokenId::SymLowerLower;
                    curBuffer++;
                    location.column++;
                }
            }
            else
            {
                tokenParse.token.id = TokenId::SymLower;
            }
            return true;

        case '>':
            if (curBuffer[0] == '=')
            {
                tokenParse.token.id = TokenId::SymGreaterEqual;
                curBuffer++;
                location.column++;
            }
            else if (curBuffer[0] == '>')
            {
                if (curBuffer[1] == '=')
                {
                    tokenParse.token.id = TokenId::SymGreaterGreaterEqual;
                    curBuffer += 2;
                    location.column += 2;
                }
                else
                {
                    tokenParse.token.id = TokenId::SymGreaterGreater;
                    curBuffer++;
                    location.column++;
                }
            }
            else
            {
                tokenParse.token.id = TokenId::SymGreater;
            }
            return true;

        default:
            return false;
    }
}
