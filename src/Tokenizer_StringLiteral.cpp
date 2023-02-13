#include "pch.h"
#include "SourceFile.h"
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
    token.id          = TokenId::LiteralString;
    token.literalType = raw ? LiteralType::TT_RAW_STRING : LiteralType::TT_STRING;
    token.text.clear();

    while (true)
    {
        startTokenName = sourceFile->curBuffer;
        while (true)
        {
            auto c = getCharNoSeek(offset);

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
                if (SWAG_IS_WIN_EOL(sourceFile->curBuffer[1]) && SWAG_IS_EOL(sourceFile->curBuffer[2]))
                {
                    appendTokenName(token);
                    processChar('\n');
                    realAppendName = true;
                    sourceFile->curBuffer += 3;
                    startTokenName    = sourceFile->curBuffer;
                    token.endLocation = location;
                    continue;
                }
                else if (SWAG_IS_EOL(sourceFile->curBuffer[1]))
                {
                    appendTokenName(token);
                    processChar('\n');
                    realAppendName = true;
                    sourceFile->curBuffer += 2;
                    startTokenName    = sourceFile->curBuffer;
                    token.endLocation = location;
                    continue;
                }
            }

            // Escape sequence
            if (!raw && c == '\\')
            {
                token.literalType = LiteralType::TT_ESCAPE_STRING;
                treatChar(c, offset);
                c = getCharNoSeek(offset);
                if (c)
                    treatChar(c, offset);
                token.endLocation = location;
                continue;
            }

            // End marker
            if (c == '"')
            {
                if (!raw && !multiline)
                {
                    appendTokenName(token);
                    treatChar(c, offset);
                    token.endLocation = location;
                    break;
                }

                if (multiline && sourceFile->curBuffer[1] == '"' && sourceFile->curBuffer[2] == '"')
                {
                    appendTokenName(token);
                    sourceFile->curBuffer += 3;
                    trimMultilineString(token.text);
                    break;
                }

                if (raw && sourceFile->curBuffer[1] == '@')
                {
                    appendTokenName(token);
                    sourceFile->curBuffer += 2;
                    trimMultilineString(token.text);
                    break;
                }
            }

            treatChar(c, offset);
            token.endLocation = location;
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
            realAppendName = true;
            treatChar(c, offset);
            continue;
        }

        break;
    }

    realAppendName = false;
    return true;
}