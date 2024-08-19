#include "pch.h"
#include "Report/ErrorIds.h"
#include "Syntax/Tokenizer/Tokenizer.h"

bool Tokenizer::doSingleLineComment(TokenParse& tokenParse)
{
    readChar();
    startTokenName = curBuffer;

    tokenParse.flags.remove(TOKEN_PARSE_EOL_BEFORE);
    while (curBuffer[0] && SWAG_IS_NOT_WIN_EOL(curBuffer[0]) && SWAG_IS_NOT_EOL(curBuffer[0]))
        readChar();

    if (tokenizeFlags.has(TOKENIZER_TRACK_COMMENTS))
    {
        appendTokenName(tokenParse);

        TokenComment cmt;
        cmt.comment = tokenParse.token.text;
        cmt.flags   = tokenParse.flags;
        cmt.flags.add(TOKEN_PARSE_ONE_LINE_COMMENT);
        tokenParse.comments.justBefore.push_back(cmt);
    }

    tokenParse.flags.remove(TOKEN_PARSE_BLANK_LINE_BEFORE);
    return true;
}

bool Tokenizer::doMultiLineComment(TokenParse& tokenParse)
{
    readChar();
    startTokenName = curBuffer;

    tokenParse.flags.remove(TOKEN_PARSE_EOL_BEFORE);
    uint32_t countEmb = 1;

    while (true)
    {
        auto c = readChar();
        while (c && c != '*' && c != '/')
            c = readChar();

        if (!c)
        {
            location = tokenParse.token.startLocation;
            location.column += 2;
            return error(tokenParse, toErr(Err0642));
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
        tokenParse.token.text.removeBack();
        tokenParse.token.text.removeBack();
        tokenParse.token.text.replace("\r", "");

        TokenComment cmt;
        cmt.comment = tokenParse.token.text;
        cmt.flags   = tokenParse.flags;
        tokenParse.comments.justBefore.push_back(cmt);
    }

    tokenParse.flags.remove(TOKEN_PARSE_BLANK_LINE_BEFORE);
    return true;
}
