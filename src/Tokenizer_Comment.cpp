#include "pch.h"
#include "Tokenizer.h"
#include "ErrorIds.h"

bool Tokenizer::doMultiLineComment(TokenParse& token)
{
    int countEmb = 1;
    while (true)
    {
        auto c = readChar();
        while (c && c != '*' && c != '/')
            c = readChar();

        if (!c)
        {
            location = token.startLocation;
            location.column += 2;
            return error(token, Err(Err0681));
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