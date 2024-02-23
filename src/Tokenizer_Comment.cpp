#include "pch.h"
#include "ErrorIds.h"
#include "Tokenizer.h"

bool Tokenizer::doMultiLineComment(TokenParse& tokenParse)
{
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
            return error(tokenParse, Err(Err0681));
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
                    return true;
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
}
