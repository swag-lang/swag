#include "pch.h"
#include "ErrorIds.h"
#include "Tokenizer.h"

bool Tokenizer::doSingleLineComment(TokenParse& tokenParse)
{
    if (tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE))
    {
        tokenParse.flags.add(TOKEN_PARSE_EOL_BEFORE_COMMENT);
        tokenParse.flags.remove(TOKEN_PARSE_EOL_BEFORE);
    }

    readChar();
    startTokenName = curBuffer;
    while (curBuffer[0] && SWAG_IS_NOT_EOL(curBuffer[0]))
        readChar();

    if (curBuffer[0])
    {
        tokenParse.flags.add(TOKEN_PARSE_EOL_BEFORE);
        readChar();
    }

    if (tokenizeFlags.has(TOKENIZER_TRACK_COMMENTS))
    {
        appendTokenName(tokenParse);
        tokenParse.comment += tokenParse.token.text;
        if (tokenParse.comment.back() != '\n')
            tokenParse.comment += "\n";

        // In case of end of line comments, skip all blanks after
        if (!tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE_COMMENT))
        {
            while (curBuffer[0] && (SWAG_IS_EOL(curBuffer[0]) || SWAG_IS_BLANK(curBuffer[0])))
            {
                if (SWAG_IS_EOL(curBuffer[0]))
                    tokenParse.flags.add(TOKEN_PARSE_EOL_BEFORE);
                readChar();
            }
        }
    }

    return true;
}

bool Tokenizer::doMultiLineComment(TokenParse& tokenParse)
{
    if (tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE))
    {
        tokenParse.flags.add(TOKEN_PARSE_EOL_BEFORE_COMMENT);
        tokenParse.flags.remove(TOKEN_PARSE_EOL_BEFORE);
    }

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
        tokenParse.comment += tokenParse.token.text;
        tokenParse.comment.removeBack();
        tokenParse.comment.removeBack();

        // In case of end of line comments, skip all blanks after
        if (!tokenParse.flags.has(TOKEN_PARSE_EOL_BEFORE_COMMENT))
        {
            while (curBuffer[0] && (SWAG_IS_EOL(curBuffer[0]) || SWAG_IS_BLANK(curBuffer[0])))
            {
                if (SWAG_IS_EOL(curBuffer[0]))
                    tokenParse.flags.add(TOKEN_PARSE_EOL_BEFORE);
                readChar();
            }
        }
    }

    return true;
}
