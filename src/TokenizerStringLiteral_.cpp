#include "pch.h"
#include "LanguageSpec.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "ErrorIds.h"

// This function is used to 'align' text. This is the same rule as in swift : all blanks before the end mark ("@) will be removed
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
    token.id            = TokenId::LiteralString;
    token.literalType   = raw ? LiteralType::TT_RAW_STRING : LiteralType::TT_STRING;
    token.startLocation = location;
    token.text.clear();

    while (true)
    {
        startTokenName = sourceFile->curBuffer;
        while (true)
        {
            auto c = getCharNoSeek(offset);

            // Can't have a newline inside a normal string (but this is legit in raw string literals)
            if (!raw && SWAG_IS_EOL(c))
            {
                token.startLocation = location;
                error(token, Msg0905);
                return false;
            }

            // End of file
            if (!c)
            {
                location = token.startLocation;
                error(token, Msg0905);
                return false;
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
                if (!raw)
                {
                    appendTokenName(token);
                    treatChar(c, offset);
                    break;
                }

                auto nc = sourceFile->curBuffer[1];
                if (nc == '@')
                {
                    appendTokenName(token);
                    sourceFile->curBuffer += 2;
                    postProcessRawString(token.text);
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

    return true;
}