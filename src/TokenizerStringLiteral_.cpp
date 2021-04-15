#include "pch.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "Diagnostic.h"
#include "SourceFile.h"

bool Tokenizer::getDigitHexa(Token& token, int& result, const char* errMsg)
{
    auto locationBefore = location;
    auto c              = getChar();
    if (!SWAG_IS_HEX(c))
    {
        token.startLocation = locationBefore;
        token.text          = c;
        if (c == '"')
            return error(token, format("not enough hexadecimal digit, %s", errMsg));
        else
            return error(token, format("invalid hexadecimal digit '%s', %s", token.text.c_str(), errMsg));
    }

    if (c >= 'a' && c <= 'z')
        result = 10 + (c - 'a');
    else if (c >= 'A' && c <= 'Z')
        result = 10 + (c - 'A');
    else
        result = c - '0';
    return true;
}

bool Tokenizer::isEscape(uint32_t& c, Token& token, bool& asIs)
{
    asIs                = false;
    auto locationBefore = location;
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
        int         c1, c2;
        const char* msg = "'\\x' escape code needs 2 of them";
        SWAG_CHECK(getDigitHexa(token, c1, msg));
        SWAG_CHECK(getDigitHexa(token, c2, msg));
        c    = (c1 << 4) + c2;
        asIs = true;
        return true;
    }
    case 'u':
    {
        int         c1, c2, c3, c4;
        const char* msg = "'\\u' escape code needs 4 of them";
        SWAG_CHECK(getDigitHexa(token, c1, msg));
        SWAG_CHECK(getDigitHexa(token, c2, msg));
        SWAG_CHECK(getDigitHexa(token, c3, msg));
        SWAG_CHECK(getDigitHexa(token, c4, msg));
        c = (c1 << 12) + (c2 << 8) + (c3 << 4) + c4;
        return true;
    }
    case 'U':
    {
        int         c1, c2, c3, c4, c5, c6, c7, c8;
        const char* msg = "'\\U' escape code needs 8 of them";
        SWAG_CHECK(getDigitHexa(token, c1, msg));
        SWAG_CHECK(getDigitHexa(token, c2, msg));
        SWAG_CHECK(getDigitHexa(token, c3, msg));
        SWAG_CHECK(getDigitHexa(token, c4, msg));
        SWAG_CHECK(getDigitHexa(token, c5, msg));
        SWAG_CHECK(getDigitHexa(token, c6, msg));
        SWAG_CHECK(getDigitHexa(token, c7, msg));
        SWAG_CHECK(getDigitHexa(token, c8, msg));
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

    token.startLocation = locationBefore;
    error(token, format("unrecognized character escape sequence '%s'", token.text.c_str()));
    return false;
}

// This function is used to 'align' text. This is the same rule as in swift : all blanks before the end mark "# will be removed
// from every other lines.
// This is necessary to be able to indent a multiline string in the code
void Tokenizer::postProcessRawString(Utf8& text)
{
    // Count the blanks before the end
    const char* pz    = text.c_str() + text.length() - 1;
    int         count = 0;
    while (pz != text.c_str() && SWAG_IS_BLANK(*pz))
    {
        count++;
        pz--;
    }

    if (count == 0)
        return;

    // Now remove the same amount of blank at the start of each new line
    Utf8 copyText;
    copyText.reserve(text.length());
    auto endPz = pz;

    pz = text.c_str();
    while (*pz && pz != endPz)
    {
        auto toRemove = count;
        while (SWAG_IS_BLANK(*pz) && toRemove)
        {
            pz++;
            toRemove++;
        }

        while (*pz != '\n' && *pz)
        {
            copyText += *pz++;
        }

        if (*pz == '\n')
            copyText += *pz++;
    }

    text = copyText;
}

bool Tokenizer::doStringLiteral(Token& token, bool raw)
{
    unsigned offset;
    token.id                = TokenId::LiteralString;
    token.literalType       = LiteralType::TT_STRING;
    token.literalCastedType = LiteralType::TT_MAX;
    token.startLocation     = location;

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

                bool asIs = false;
                SWAG_CHECK(isEscape(c, token, asIs));
                token.endLocation = location;
                if (asIs)
                    token.text.append((char) c);
                else
                    token.text += c;
                continue;
            }

            treatChar(c, offset);

            // End marker
            if (!raw && c == '"')
                break;

            if (raw && c == '"')
            {
                auto nc = getCharNoSeek(offset);
                if (nc == '@')
                {
                    treatChar(nc, offset);
                    postProcessRawString(token.text);
                    break;
                }
            }

            token.endLocation = location;
            token.text += c;
        }

        auto c = getCharNoSeek(offset);
        while (SWAG_IS_BLANK(c) || SWAG_IS_EOL(c))
        {
            if (SWAG_IS_EOL(c))
                forceLastTokenIsEOL = true;
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

    // Suffix
    auto c = getCharNoSeek(offset);
    if (c == '\'')
    {
        treatChar(c, offset);
        Token tokenSuffix;
        tokenSuffix.startLocation = location;

        c = getCharNoSeek(offset);
        if (!c)
            return error(tokenSuffix, "missing string literal suffix");

        getIdentifier(tokenSuffix, c, offset);
        SWAG_VERIFY(tokenSuffix.id == TokenId::NativeType, error(tokenSuffix, format("invalid literal string suffix '%s'", tokenSuffix.text.c_str())));

        VectorNative<uint32_t> uni;
        token.text.toUni32(uni);
        SWAG_VERIFY(uni.size() == 1, sourceFile->report({sourceFile, token, format("invalid character literal '%s', this is a string, not a character", token.text.c_str())}));

        switch (tokenSuffix.literalType)
        {
        case LiteralType::TT_CHAR:
            token.id              = TokenId::LiteralCharacter;
            token.literalType     = LiteralType::TT_CHAR;
            token.literalValue.ch = uni[0];
            break;

        case LiteralType::TT_U8:
            SWAG_VERIFY(uni[0] <= UINT8_MAX, sourceFile->report({sourceFile, token, format("cannot convert character literal to 'u8', value '%u' is too big", uni[0])}));
            token.id              = TokenId::LiteralNumber;
            token.literalType     = LiteralType::TT_U8;
            token.literalValue.u8 = (uint8_t) uni[0];
            break;

        case LiteralType::TT_U16:
            SWAG_VERIFY(uni[0] <= UINT16_MAX, sourceFile->report({sourceFile, token, format("cannot convert character literal to 'u16', value '%u' is too big", uni[0])}));
            token.id               = TokenId::LiteralNumber;
            token.literalType      = LiteralType::TT_U16;
            token.literalValue.u16 = (uint16_t) uni[0];
            break;

        case LiteralType::TT_U32:
            token.id               = TokenId::LiteralNumber;
            token.literalType      = LiteralType::TT_U32;
            token.literalValue.u32 = uni[0];
            break;

        case LiteralType::TT_U64:
            token.id               = TokenId::LiteralNumber;
            token.literalType      = LiteralType::TT_U64;
            token.literalValue.u64 = uni[0];
            break;

        default:
            error(tokenSuffix, format("invalid literal string suffix '%s'", tokenSuffix.text.c_str()));
            break;
        }
    }

    return true;
}

bool Tokenizer::doCharLiteral(Token& token)
{
    bool     result = true;
    unsigned offset;
    token.id = TokenId::LiteralCharacter;

    auto c                  = getCharNoSeek(offset);
    token.startLocation     = location;
    token.literalType       = LiteralType::TT_CHAR;
    token.literalCastedType = LiteralType::TT_MAX;

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

        bool asIs = false;
        result    = result && isEscape(c, token, asIs);

        token.text.clear();
        if (asIs)
            token.text.append((char) c);
        else
            token.text += c;
    }
    else
    {
        treatChar(c, offset);
        token.text = c;
    }

    token.literalValue.ch = c;
    token.endLocation     = location;
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
