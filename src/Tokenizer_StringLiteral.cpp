#include "pch.h"
#include "Tokenizer.h"
#include "ErrorIds.h"

// This function is used to 'align' text. This is the same rule as in swift : all blanks before the end mark ("@) will be removed
// from every other lines.
// This is necessary to be able to indent a multiline string in the code
void Tokenizer::trimMultilineString(Utf8& text)
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
    auto endPz = pz;

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

        while (i && *pz != '\n')
        {
            copyText += *pz++;
            i--;
        }

        if (*pz == '\n')
        {
            copyText += *pz++;
            i--;
        }
    }

    text = copyText;
}

bool Tokenizer::doStringLiteral(TokenParse& token, bool raw, bool multiline)
{
    unsigned offset;
    token.id = TokenId::LiteralString;

    if (raw)
        token.literalType = LiteralType::TT_STRING_RAW;
    else if (multiline)
        token.literalType = LiteralType::TT_STRING_MULTILINE;
    else
        token.literalType = LiteralType::TT_STRING;

    token.text.clear();

    while (true)
    {
        startTokenName = curBuffer;
        while (true)
        {
            auto c = peekChar(offset);

            // Can't have a newline inside a normal string (but this is legit in raw string literals)
            if (!multiline && SWAG_IS_EOL(c))
            {
                token.startLocation = location;
                error(token, Err(Tkn0018));
                return false;
            }

            // End of file
            if (!c)
            {
                location = token.startLocation;
                error(token, Err(Tkn0017));
                return false;
            }

            // Skip next eol
            if (multiline && c == '\\')
            {
                // Window \r\n
                if (SWAG_IS_WIN_EOL(curBuffer[1]) && SWAG_IS_EOL(curBuffer[2]))
                {
                    appendTokenName(token);
                    processChar('\n');
                    realAppendName = true;
                    curBuffer += 3;
                    startTokenName    = curBuffer;
                    token.endLocation = location;
                    continue;
                }
                else if (SWAG_IS_EOL(curBuffer[1]))
                {
                    appendTokenName(token);
                    processChar('\n');
                    realAppendName = true;
                    curBuffer += 2;
                    startTokenName    = curBuffer;
                    token.endLocation = location;
                    continue;
                }
            }

            // Escape sequence
            if (!raw && c == '\\')
            {
                token.literalType = multiline ? LiteralType::TT_STRING_MULTILINE_ESCAPE : LiteralType::TT_STRING_ESCAPE;
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
                if (!raw && !multiline)
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
                    trimMultilineString(token.text);
                    break;
                }

                if (raw && curBuffer[1] == '@')
                {
                    appendTokenName(token);
                    curBuffer += 2;
                    trimMultilineString(token.text);
                    break;
                }
            }

            eatChar(c, offset);
            token.endLocation = location;
        }

        auto c = peekChar(offset);
        while (SWAG_IS_BLANK(c) || SWAG_IS_EOL(c))
        {
            if (SWAG_IS_EOL(c))
                forceLastTokenIsEOL = true;
            eatChar(c, offset);
            c = peekChar(offset);
        }

        if (!raw && c == '"')
        {
            realAppendName = true;
            eatChar(c, offset);
            continue;
        }

        break;
    }

    realAppendName = false;
    return true;
}

bool Tokenizer::doCharacterLiteral(TokenParse& token)
{
    unsigned offset;
    token.id          = TokenId::LiteralCharacter;
    token.literalType = LiteralType::TT_CHARACTER;
    token.text.clear();

    startTokenName = curBuffer;
    while (true)
    {
        auto c = peekChar(offset);

        // Can't have a newline inside a normal string (but this is legit in raw string literals)
        if (SWAG_IS_EOL(c))
        {
            token.startLocation = location;
            error(token, Err(Tkn0026));
            return false;
        }

        // End of file
        if (!c)
        {
            location = token.startLocation;
            error(token, Err(Tkn0017));
            return false;
        }

        // Escape sequence
        if (c == '\\')
        {
            token.literalType = LiteralType::TT_CHARACTER_ESCAPE;
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