#include "pch.h"
#include "ErrorIds.h"
#include "Tokenizer.h"

bool Tokenizer::doSingleLineComment(TokenParse& tokenParse)
{
    readChar();
    startTokenName = curBuffer;

    while (curBuffer[0] && SWAG_IS_NOT_EOL(curBuffer[0]))
        readChar();

    if (tokenizeFlags.has(TOKENIZER_TRACK_COMMENTS))
    {
        appendTokenName(tokenParse);
        tokenParse.commentBefore += tokenParse.token.text;
        tokenParse.commentBefore += "\n";
    }

    return true;
}

bool Tokenizer::doMultiLineComment(TokenParse& tokenParse)
{
    readChar();
    startTokenName = curBuffer;

    int countEmb = 1;
    while (true)
    {
        auto c = readChar();
        while (c && c != '*' && c != '/')
            c = readChar();

        if (!c)
        {
            location = tokenParse.token.startLocation;
            location.column += 2;
            return error(tokenParse, toErr(Err0681));
        }

        if (c == '*')
        {
            unsigned offset;
            c = peekChar(offset);
            if (c == '/')
            {
                eatChar(c, offset);
                countEmb--;
                if (countEmb == 0)
                    break;
            }

            continue;
        }

        if (c == '/')
        {
            c = readChar();
            if (c == '*')
            {
                countEmb++;
                continue;
            }
        }
    }

    if (tokenizeFlags.has(TOKENIZER_TRACK_COMMENTS))
    {
        appendTokenName(tokenParse);
        tokenParse.commentBefore += tokenParse.token.text;
        tokenParse.commentBefore.removeBack();
        tokenParse.commentBefore.removeBack();
    }

    return true;
}
