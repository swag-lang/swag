#include "pch.h"
#include "Tokenizer.h"
#include "Global.h"
#include "LanguageSpec.h"
#include "Diagnostic.h"

bool Tokenizer::getDigitHexa(Token& token, int& result)
{
    auto c = getChar();
    SWAG_VERIFY(SWAG_IS_HEX(c), error(token, "invalid hexadecimal digit"));
    if (c >= 'a' && c <= 'z')
        result = 10 + (c - 'a');
    else if (c >= 'A' && c <= 'Z')
        result = 10 + (c - 'A');
    else
        result = c - '0';
    return true;
}

bool Tokenizer::isEscape(char32_t& c, Token& token)
{
    token.startLocation = location;
    c                   = getChar();
    switch (c)
    {
    case '0':
        c = '\0';
        return true;
    case 'a':
        c = '\a';
        return true;
    case 'b':
        c = '\b';
        return true;
    case '\\':
        c = '\\';
        return true;
    case 't':
        c = '\t';
        return true;
    case 'n':
        c = '\n';
        return true;
    case 'f':
        c = '\f';
        return true;
    case 'r':
        c = '\r';
        return true;
    case 'v':
        c = '\v';
        return true;
    case '\'':
        c = '\'';
        return true;
    case '\"':
        c = '\"';
        return true;
    case 'x':
    {
        int c1, c2;
        SWAG_CHECK(getDigitHexa(token, c1));
        SWAG_CHECK(getDigitHexa(token, c2));
        c = (c1 << 4) + c2;
        return true;
    }
    case 'u':
    {
        int c1, c2, c3, c4;
        SWAG_CHECK(getDigitHexa(token, c1));
        SWAG_CHECK(getDigitHexa(token, c2));
        SWAG_CHECK(getDigitHexa(token, c3));
        SWAG_CHECK(getDigitHexa(token, c4));
        c = (c1 << 12) + (c2 << 8) + (c3 << 4) + c4;
        return true;
    }
    case 'U':
    {
        int c1, c2, c3, c4, c5, c6, c7, c8;
        SWAG_CHECK(getDigitHexa(token, c1));
        SWAG_CHECK(getDigitHexa(token, c2));
        SWAG_CHECK(getDigitHexa(token, c3));
        SWAG_CHECK(getDigitHexa(token, c4));
        SWAG_CHECK(getDigitHexa(token, c5));
        SWAG_CHECK(getDigitHexa(token, c6));
        SWAG_CHECK(getDigitHexa(token, c7));
        SWAG_CHECK(getDigitHexa(token, c8));
        c = (c1 << 28) + (c2 << 24) + (c3 << 20) + (c4 << 16) + (c5 << 12) + (c6 << 8) + (c7 << 4) + c8;
        return true;
    }
    }

    if (c == '\n')
        token.text = "<eol>";
    else if (c < 32)
        token.text = "<blank>";
    else
        token.text = c;
    error(token, format("unrecognized character escape sequence '%s'", token.text.c_str()));
    return false;
}

bool Tokenizer::doStringLiteral(Token& token, bool raw)
{
    bool     result = true;
    unsigned offset;
    token.id          = TokenId::LiteralString;
    token.literalType = &g_TypeInfoString;

    while (true)
    {
        while (true)
        {
            auto c = getCharNoSeek(offset);

            // Can't have a newline inside a normal string (but this is legit in raw string literals)
            if (!raw && SWAG_IS_EOL(c))
            {
                token.startLocation = location;
                token.endLocation   = token.startLocation;
                sourceFile->report({sourceFile, token, "unexpected end of line found in string literal"});
                return false;
            }

            // End of file
            if (!c)
            {
                token.endLocation = token.startLocation;
                sourceFile->report({sourceFile, token, "unexpected end of file found in string literal"});
                return false;
            }

            // Escape sequence
            if (!raw && c == '\\')
            {
                treatChar(c, offset);
                result = result && isEscape(c, token);
                token.text += c;
                continue;
            }

            treatChar(c, offset);

            // End marker
            if (!raw && c == '"')
                break;
            if (raw && c == '`')
                break;

            token.text += c;
        }

        auto c = getCharNoSeek(offset);
        while (SWAG_IS_BLANK(c) || SWAG_IS_EOL(c))
        {
            treatChar(c, offset);
            c = getCharNoSeek(offset);
        }

		if (!raw && c == '"')
		{
			treatChar(c, offset);
			continue;
		}

		if (raw && c == '`')
		{
			treatChar(c, offset);
			continue;
		}

		break;
    }

    return result;
}

bool Tokenizer::doCharLiteral(Token& token)
{
    bool     result = true;
    unsigned offset;
    token.id = TokenId::LiteralCharacter;

    auto c              = getCharNoSeek(offset);
    token.startLocation = location;
    token.literalType   = &g_TypeInfoChar;

    // Can't have a newline inside a character
    if (c == '\n')
    {
        token.startLocation = location;
        token.endLocation   = token.startLocation;
        sourceFile->report({sourceFile, token, "unexpected end of line found in character literal"});
        return false;
    }

    // End of file
    if (!c)
    {
        token.endLocation = token.startLocation;
        sourceFile->report({sourceFile, token, "unexpected end of file found in character literal"});
        return false;
    }

    // Escape sequence
    if (c == '\\')
    {
        treatChar(c, offset);
        result     = result && isEscape(c, token);
        token.text = c;
    }
    else
    {
        treatChar(c, offset);
        token.text = c;
    }

    token.literalValue.ch = c;
    token.startLocation   = location;
    c                     = getCharNoSeek(offset);
    if (c != '\'')
    {
        result = false;
        error(token, "missing character literal last '\''");
        while (c != '\'' && c && c != '\n')
        {
            treatChar(c, offset);
            c = getCharNoSeek(offset);
        }
    }

    treatChar(c, offset);
    return result;
}
