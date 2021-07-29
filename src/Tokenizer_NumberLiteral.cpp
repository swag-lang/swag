#include "pch.h"
#include "LanguageSpec.h"
#include "TypeManager.h"
#include "SourceFile.h"
#include "ErrorIds.h"

bool Tokenizer::doBinLiteral(Token& token)
{
    int      rank      = 0;
    bool     acceptSep = true;
    unsigned offset;

    token.literalValue.u64 = 0;
    auto c                 = getCharNoSeek(offset);
    while (c == '0' || c == '1' || SWAG_IS_NUMSEP(c))
    {
        treatChar(c, offset);

        // Digit separator
        if (SWAG_IS_NUMSEP(c))
        {
            if (!acceptSep)
                return error(token, Msg0145);
            acceptSep = false;
            c         = getCharNoSeek(offset);
            continue;
        }

        acceptSep = true;
        SWAG_VERIFY(!(token.literalValue.u64 & 0x80000000'00000000), error(token, Msg0464));
        SWAG_VERIFY(rank != 64, error(token, Msg0459));
        token.literalValue.u64 <<= 1;
        rank++;

        token.literalValue.u64 += (c - '0');

        c = getCharNoSeek(offset);
    }

    // Be sure what follows is valid
    if (SWAG_IS_DIGIT(c) || SWAG_IS_ALPHA(c))
    {
        token.startLocation = location;
        token.text          = c;
        SWAG_CHECK(error(token, Utf8::format(Msg0460, token.text.c_str())));
    }

    // Be sure we don't have 0x without nothing
    if (rank == 0)
        SWAG_CHECK(error(token, Msg0151));
    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        SWAG_CHECK(error(token, Msg0152));

    // Suffix
    token.id = TokenId::LiteralNumber;
    if (token.literalValue.u64 <= UINT32_MAX)
        token.literalType = LiteralType::TT_UNTYPED_BINHEXA;
    else
        token.literalType = LiteralType::TT_U64;

    return true;
}

bool Tokenizer::doHexLiteral(Token& token)
{
    int      rank      = 0;
    bool     acceptSep = true;
    unsigned offset;

    token.literalValue.u64 = 0;
    auto c                 = getCharNoSeek(offset);
    while (SWAG_IS_ALPHAHEX(c) || SWAG_IS_DIGIT(c) || SWAG_IS_NUMSEP(c))
    {
        treatChar(c, offset);

        // Digit separator
        if (SWAG_IS_NUMSEP(c))
        {
            if (!acceptSep)
                return error(token, Msg0145);
            acceptSep = false;
            c         = getCharNoSeek(offset);
            continue;
        }

        acceptSep = true;
        SWAG_VERIFY(!(token.literalValue.u64 & 0xF0000000'00000000), error(token, Msg0464));
        SWAG_VERIFY(rank != 16, error(token, Msg0459));
        token.literalValue.u64 <<= 4;
        rank++;

        if (SWAG_IS_DIGIT(c))
            token.literalValue.u64 += (c - '0');
        else if (c >= 'a' && c <= 'f')
            token.literalValue.u64 += (10 + (c - 'a'));
        else
            token.literalValue.u64 += (10 + (c - 'A'));

        c = getCharNoSeek(offset);
    }

    // Be sure what follows is valid
    if (SWAG_IS_ALPHA(c))
    {
        token.startLocation = location;
        token.text          = c;
        SWAG_CHECK(error(token, Utf8::format(Msg0463, token.text.c_str())));
    }

    // Be sure we don't have 0x without nothing
    if (rank == 0)
        SWAG_CHECK(error(token, Msg0151));
    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        SWAG_CHECK(error(token, Msg0152));

    // Suffix
    token.id = TokenId::LiteralNumber;
    if (token.literalValue.u64 <= UINT32_MAX)
        token.literalType = LiteralType::TT_UNTYPED_BINHEXA;
    else
        token.literalType = LiteralType::TT_U64;

    return true;
}

bool Tokenizer::doFloatLiteral(uint32_t c, Token& token)
{
    int      rank      = 0;
    double   fractPart = 0.1;
    bool     acceptSep = true;
    unsigned offset    = 0;

    token.literalValue.f64 = 0;
    while (SWAG_IS_DIGIT(c) || SWAG_IS_NUMSEP(c))
    {
        if (offset)
            treatChar(c, offset);

        // Digit separator
        if (SWAG_IS_NUMSEP(c))
        {
            if (!acceptSep)
            {
                SWAG_CHECK(rank != 0 || error(token, Msg0154));
                SWAG_CHECK(rank == 0 || error(token, Msg0145));
            }

            acceptSep = false;
            c         = getCharNoSeek(offset);
            continue;
        }

        acceptSep = true;
        rank++;

        auto val = (c - '0');
        SWAG_VERIFY(token.literalValue.u64 <= 18446744073709551615 - val, error(token, Msg0464));
        token.literalValue.f64 += val * fractPart;
        fractPart *= 0.1;

        c = getCharNoSeek(offset);
    }

    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        SWAG_CHECK(error(token, Msg0152));

    return true;
}

bool Tokenizer::doIntLiteral(uint32_t c, Token& token)
{
    int      rank      = 0;
    bool     acceptSep = true;
    unsigned offset    = 0;

    token.literalValue.u64 = 0;
    while (SWAG_IS_DIGIT(c) || SWAG_IS_NUMSEP(c))
    {
        if (offset)
            treatChar(c, offset);

        // Digit separator
        if (SWAG_IS_NUMSEP(c))
        {
            if (!acceptSep)
            {
                SWAG_CHECK(rank != 0 || error(token, Msg0154));
                SWAG_CHECK(rank == 0 || error(token, Msg0145));
            }

            acceptSep = false;
            c         = getCharNoSeek(offset);
            continue;
        }

        acceptSep = true;
        token.literalValue.u64 *= 10;
        rank++;

        auto val = (c - '0');
        SWAG_VERIFY(token.literalValue.u64 <= 18446744073709551615 - val, error(token, Msg0464));
        token.literalValue.u64 += val;

        c = getCharNoSeek(offset);
    }

    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        SWAG_CHECK(error(token, Msg0152));

    return true;
}

bool Tokenizer::doIntFloatLiteral(uint32_t c, Token& token)
{
    unsigned offset = 1;
    Token    tokenFrac;
    Token    tokenExponent;

    tokenFrac.literalValue.u64     = 0;
    tokenExponent.literalValue.u64 = 0;
    token.literalType              = LiteralType::TT_S32;
    token.id                       = TokenId::LiteralNumber;

    // Integer part
    SWAG_CHECK(doIntLiteral(c, token));

    c = getCharNoSeek(offset);

    // Do this because of the slicing operator number..number. We do not want the '..' to be eaten
    bool hasDot = (c == '.') && sourceFile->curBuffer[1] != '.';

    // If there's a dot, then this is a floating point number
    if (hasDot)
    {
        token.literalType = LiteralType::TT_UNTYPED_FLOAT;
        treatChar(c, offset);

        // Fraction part
        tokenFrac.startLocation = location;
        c                       = getCharNoSeek(offset);
        SWAG_VERIFY(!SWAG_IS_NUMSEP(c), error(tokenFrac, Msg0155));
        if (SWAG_IS_DIGIT(c))
        {
            treatChar(c, offset);
            SWAG_CHECK(doFloatLiteral(c, tokenFrac));
            c = getCharNoSeek(offset);
        }
    }

    // If there's an exponent, then this is a floating point number
    if (c == 'e' || c == 'E')
    {
        token.literalType = LiteralType::TT_UNTYPED_FLOAT;
        treatChar(c, offset);
        tokenExponent.startLocation = location;

        bool minus = false;
        c          = getCharNoSeek(offset);

        if (c == '-')
        {
            minus = true;
            treatChar(c, offset);
            c = getCharNoSeek(offset);
        }
        else if (c == '+')
        {
            minus = false;
            treatChar(c, offset);
            c = getCharNoSeek(offset);
        }

        tokenExponent.startLocation = location;
        SWAG_VERIFY(!SWAG_IS_NUMSEP(c), error(tokenExponent, Msg0156));
        SWAG_VERIFY(SWAG_IS_DIGIT(c), error(tokenExponent, Msg0466));
        treatChar(c, offset);
        SWAG_CHECK(doIntLiteral(c, tokenExponent));
        c = getCharNoSeek(offset);

        if (minus)
            tokenExponent.literalValue.s64 = -tokenExponent.literalValue.s64;
    }

    // Really compute the floating point value, with as much precision as we can
    if (token.literalType == LiteralType::TT_UNTYPED_FLOAT)
    {
        auto cpt               = (unsigned) (sourceFile->curBuffer - startTokenName);
        auto ptr               = startTokenName + cpt;
        auto sc                = *ptr;
        *ptr                   = 0;
        token.literalValue.f64 = atof(startTokenName);
        *ptr                   = sc;
    }
    else if (token.literalValue.s64 < INT32_MIN || token.literalValue.s64 > INT32_MAX)
    {
        // Can be a negative number ?
        if (token.literalValue.u64 > (uint64_t) INT64_MAX + 1)
            token.literalType = LiteralType::TT_U64;
        else
            token.literalType = LiteralType::TT_S64;
    }

    if (token.literalType == LiteralType::TT_S32)
        token.literalType = LiteralType::TT_UNTYPED_INT;

    return true;
}

bool Tokenizer::doNumberLiteral(uint32_t c, Token& token)
{
    if (c == '0')
    {
        unsigned offset;
        c = getCharNoSeek(offset);

        // Hexadecimal
        if (c == 'x' || c == 'X')
        {
            treatChar(c, offset);
            SWAG_CHECK(doHexLiteral(token));
            token.endLocation = location;
            appendTokenName(token);
            return true;
        }

        // Binary
        if (c == 'b' || c == 'B')
        {
            treatChar(c, offset);
            SWAG_CHECK(doBinLiteral(token));
            token.endLocation = location;
            appendTokenName(token);
            return true;
        }

        if (SWAG_IS_ALPHA(c))
        {
            token.startLocation = location;
            treatChar(c, offset);
            token.text = c;
            return error(token, Utf8::format(Msg0467, token.text.c_str()));
        }
    }

    SWAG_CHECK(doIntFloatLiteral(c, token));
    token.endLocation = location;
    appendTokenName(token);

    return true;
}
