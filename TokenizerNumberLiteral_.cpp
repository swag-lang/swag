#include "pch.h"
#include "Tokenizer.h"
#include "Global.h"
#include "LanguageSpec.h"
#include "Diagnostic.h"

bool Tokenizer::errorNumberSyntax(Token& token, const wstring& msg)
{
    error(token, L"invalid number syntax, " + msg);
    return false;
}

bool Tokenizer::doHexLiteral(Token& token)
{
    uint64_t value = 0;
    int      rank  = 0;

    bool acceptSep = false;
    auto c         = getChar(false);
    while (SWAG_IS_HEX(c) || SWAG_IS_DIGIT(c) || SWAG_IS_NUMSEP(c))
    {
        token.text += c;
        treatChar(c);

		// Digit separator
        if (SWAG_IS_NUMSEP(c))
        {
			if (!acceptSep)
			{
				SWAG_CHECK(rank != 0 || errorNumberSyntax(token, L"a digit separator can't start a literal number"));
				SWAG_CHECK(rank == 0 || errorNumberSyntax(token, L"forbidden consecutive digit separators"));
			}

            acceptSep = false;
            c         = getChar(false);
            continue;
        }

        acceptSep = true;
        value <<= 4;
        rank++;
		SWAG_CHECK(rank != 17 || error(token, L"literal number is too big"));

        if (SWAG_IS_DIGIT(c))
            value += (c - '0');
        else if (c >= 'a' && c <= 'f')
            value += (10 + (c - 'a'));
        else
            value += (10 + (c - 'A'));

        c = getChar(false);
    }

    // Be sure we don't have 0x without nothing
    if (rank == 0)
        SWAG_CHECK(errorNumberSyntax(token, L"missing at least one digit"));
	// Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        SWAG_CHECK(errorNumberSyntax(token, L"a digit separator can't end a literal number"));

    return true;
}

bool Tokenizer::doNumberLiteral(unsigned c, Token& token)
{
    token.text = c;
    if (c == '0')
    {
        c = getChar();
        if (c == 'x' || c == 'X')
        {
            token.text += c;
            SWAG_CHECK(doHexLiteral(token));
            return true;
        }
    }

    return true;
}
