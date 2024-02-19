#include "pch.h"
#include "ErrorIds.h"
#include "Tokenizer.h"

namespace
{
    // This function is used to 'align' text. This is the same rule as in swift : all blanks before the end mark ("@) will be removed
    // from every other lines.
    // This is necessary to be able to indent a multiline string in the code
    void trimMultilineString(Utf8& text)
    {
        // Count the blanks before the end
        const char* pz    = text.buffer + text.length() - 1;
        int         count = 0;
        while (pz != text.buffer && SWAG_IS_BLANK(*pz))
        {
            count++;
            pz--;
        }

        if (count == 0)
            return;

        // Now remove the same amount of blank at the start of each new line
        Utf8 copyText;
        copyText.reserve(text.length());
        const auto endPz = pz;

        pz     = text.buffer;
        auto i = text.count;
        while (i && pz != endPz)
        {
            auto toRemove = count;
            while (i && SWAG_IS_BLANK(*pz) && toRemove)
            {
                pz++;
                toRemove--;
                i--;
            }

            while (i && SWAG_IS_NOT_EOL(*pz))
            {
                copyText += *pz++;
                i--;
            }

            if (SWAG_IS_EOL(*pz))
            {
                copyText += *pz++;
                i--;
            }
        }

        text = copyText;
    }
}

bool Tokenizer::doStringLiteral(TokenParse& token)
{
    bool multiline = false;
    bool verbatim  = false;

    if (token.literalType == LiteralType::TypeStringMultiLine)
    {
        multiline = true;
    }
    else if (token.literalType != LiteralType::TypeString)
    {
        verbatim  = true;
        multiline = true;
    }

    token.id = TokenId::LiteralString;
    token.text.clear();

    unsigned offset;
    startTokenName = curBuffer;

    while (true)
    {
        auto c = peekChar(offset);

        // Can't have a newline inside a normal string (but this is legit in raw string literals)
        if (!multiline && SWAG_IS_EOL(c))
        {
            token.startLocation = location;
            error(token, Err(Err0132));
            return false;
        }

        // End of file
        if (!c)
        {
            location = token.startLocation;
            error(token, Err(Err0132));
            return false;
        }

        // Skip next eol
        if (multiline && c == '\\')
        {
            // Window \r\n
            if (SWAG_IS_WIN_EOL(curBuffer[1]) && SWAG_IS_EOL(curBuffer[2]))
            {
                appendTokenName(token);
                processChar('\n', 1);
                realAppendName = true;
                curBuffer += 3;
                startTokenName    = curBuffer;
                token.endLocation = location;
                continue;
            }
            if (SWAG_IS_EOL(curBuffer[1]))
            {
                appendTokenName(token);
                processChar('\n', 1);
                realAppendName = true;
                curBuffer += 2;
                startTokenName    = curBuffer;
                token.endLocation = location;
                continue;
            }
        }

        // Escape sequence
        if (!verbatim && c == '\\')
        {
            token.literalType = multiline ? LiteralType::TypeStringMultiLineEscape : LiteralType::TypeStringEscape;
            eatChar(c, offset);
            c = peekChar(offset);
            if (c)
                eatChar(c, offset);
            token.endLocation = location;
            continue;
        }

        // End marker
        if (c == '"')
        {
            if (!verbatim && !multiline)
            {
                appendTokenName(token);
                eatChar(c, offset);
                token.endLocation = location;
                break;
            }

            if (multiline && curBuffer[1] == '"' && curBuffer[2] == '"')
            {
                appendTokenName(token);
                curBuffer += 3;
                location.column += 3;
                trimMultilineString(token.text);
                break;
            }

            if (verbatim && curBuffer[1] == '#')
            {
                appendTokenName(token);
                curBuffer += 2;
                location.column += 2;
                trimMultilineString(token.text);
                break;
            }
        }

        eatChar(c, offset);
        token.endLocation = location;
    }

    realAppendName = false;
    return true;
}

bool Tokenizer::doCharacterLiteral(TokenParse& token)
{
    unsigned offset;
    token.id          = TokenId::LiteralCharacter;
    token.literalType = LiteralType::TypeCharacter;
    token.text.clear();

    startTokenName = curBuffer;
    while (true)
    {
        auto c = peekChar(offset);

        // Can't have a newline inside a normal string (but this is legit in raw string literals)
        if (SWAG_IS_EOL(c))
        {
            token.startLocation = location;
            error(token, Err(Err0124));
            return false;
        }

        // End of file
        if (!c)
        {
            location = token.startLocation;
            error(token, Err(Err0124));
            return false;
        }

        // Escape sequence
        if (c == '\\')
        {
            token.literalType = LiteralType::TypeCharacterEscape;
            eatChar(c, offset);
            c = peekChar(offset);
            if (c)
                eatChar(c, offset);
            token.endLocation = location;
            continue;
        }

        // End marker
        if (c == '`')
        {
            appendTokenName(token);
            eatChar(c, offset);
            token.endLocation = location;
            break;
        }

        eatChar(c, offset);
        token.endLocation = location;
    }

    return true;
}
