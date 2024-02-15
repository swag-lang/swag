#include "pch.h"
#include "ErrorIds.h"
#include "Tokenizer.h"

bool Tokenizer::doBinLiteral(TokenParse& token)
{
    int      rank      = 0;
    bool     acceptSep = true;
    unsigned offset;

    token.literalValue.u64 = 0;
    auto c                 = peekChar(offset);
    while (c == '0' || c == '1' || SWAG_IS_NUM_SEP(c))
    {
        eatChar(c, offset);

        // Digit separator
        if (SWAG_IS_NUM_SEP(c))
        {
            if (!acceptSep)
                return error(token, Err(Err0335));
            acceptSep = false;
            c         = peekChar(offset);
            continue;
        }

        acceptSep = true;
        SWAG_VERIFY(!(token.literalValue.u64 & 0x80000000'00000000), error(token, Err(Err0609)));
        SWAG_VERIFY(rank != 64, error(token, Err(Err0336)));
        token.literalValue.u64 <<= 1;
        rank++;

        token.literalValue.u64 += c - '0';

        c = peekChar(offset);
    }

    // Be sure what follows is valid
    if (SWAG_IS_DIGIT(c) || SWAG_IS_ALPHA(c))
    {
        token.startLocation = location;
        token.text          = c;
        return error(token, FMT(Err(Err0227), token.c_str()));
    }

    // Be sure we don't have 0x without nothing
    if (rank == 0)
        return error(token, Err(Err0123));
    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        return error(token, Err(Err0333));

    // Type
    token.id = TokenId::LiteralNumber;
    if (token.literalValue.u64 <= UINT32_MAX)
        token.literalType = LiteralType::TT_UNTYPED_BIN_HEXA;
    else
        token.literalType = LiteralType::TT_U64;

    return true;
}

bool Tokenizer::doHexLiteral(TokenParse& token)
{
    int      rank      = 0;
    bool     acceptSep = true;
    unsigned offset;

    token.literalValue.u64 = 0;
    auto c                 = peekChar(offset);
    while (SWAG_IS_ALPHA_HEX(c) || SWAG_IS_DIGIT(c) || SWAG_IS_NUM_SEP(c))
    {
        eatChar(c, offset);

        // Digit separator
        if (SWAG_IS_NUM_SEP(c))
        {
            if (!acceptSep)
                return error(token, Err(Err0335));
            acceptSep = false;
            c         = peekChar(offset);
            continue;
        }

        acceptSep = true;
        SWAG_VERIFY(!(token.literalValue.u64 & 0xF0000000'00000000), error(token, Err(Err0609)));
        SWAG_VERIFY(rank != 16, error(token, Err(Err0337)));
        token.literalValue.u64 <<= 4;
        rank++;

        if (SWAG_IS_DIGIT(c))
            token.literalValue.u64 += c - '0';
        else if (c >= 'a' && c <= 'f')
            token.literalValue.u64 += 10 + (c - 'a');
        else
            token.literalValue.u64 += 10 + (c - 'A');

        c = peekChar(offset);
    }

    // Be sure what follows is valid
    if (SWAG_IS_ALPHA(c))
    {
        token.startLocation = location;
        token.text          = c;
        return error(token, FMT(Err(Err0308), token.c_str()));
    }

    // Be sure we don't have 0x without nothing
    if (rank == 0)
        return error(token, Err(Err0125));
    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        return error(token, Err(Err0333));

    // Type
    token.id = TokenId::LiteralNumber;
    if (token.literalValue.u64 <= UINT32_MAX)
        token.literalType = LiteralType::TT_UNTYPED_BIN_HEXA;
    else
        token.literalType = LiteralType::TT_U64;

    return true;
}

bool Tokenizer::doFloatLiteral(TokenParse& token, uint32_t c)
{
    int      rank      = 0;
    double   fractPart = 0.1;
    bool     acceptSep = true;
    unsigned offset    = 0;

    token.literalValue.f64 = 0;
    while (SWAG_IS_DIGIT(c) || SWAG_IS_NUM_SEP(c))
    {
        if (offset)
            eatChar(c, offset);

        // Digit separator
        if (SWAG_IS_NUM_SEP(c))
        {
            if (!acceptSep)
            {
                SWAG_CHECK(rank != 0 || error(token, Err(Err0334)));
                SWAG_CHECK(rank == 0 || error(token, Err(Err0335)));
            }

            acceptSep = false;
            c         = peekChar(offset);
            continue;
        }

        acceptSep = true;
        rank++;

        const auto val = c - '0';
        SWAG_VERIFY(token.literalValue.u64 <= 18446744073709551615ULL - val, error(token, Err(Err0609)));
        token.literalValue.f64 += val * fractPart;
        fractPart *= 0.1;

        c = peekChar(offset);
    }

    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        return error(token, Err(Err0333));

    return true;
}

bool Tokenizer::doIntLiteral(TokenParse& token, uint32_t c)
{
    int      rank      = 0;
    bool     acceptSep = true;
    unsigned offset    = 0;

    token.literalValue.u64 = 0;
    while (SWAG_IS_DIGIT(c) || SWAG_IS_NUM_SEP(c))
    {
        if (offset)
            eatChar(c, offset);

        // Digit separator
        if (SWAG_IS_NUM_SEP(c))
        {
            if (!acceptSep)
            {
                SWAG_CHECK(rank != 0 || error(token, Err(Err0334)));
                SWAG_CHECK(rank == 0 || error(token, Err(Err0335)));
            }

            acceptSep = false;
            c         = peekChar(offset);
            continue;
        }

        acceptSep = true;
        token.literalValue.u64 *= 10;
        rank++;

        const auto val = c - '0';
        SWAG_VERIFY(token.literalValue.u64 <= 18446744073709551615ULL - val, error(token, Err(Err0609)));
        token.literalValue.u64 += val;

        c = peekChar(offset);
    }

    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        return error(token, Err(Err0333));

    return true;
}

bool Tokenizer::doIntFloatLiteral(TokenParse& token, uint32_t c)
{
    unsigned   offset = 1;
    TokenParse tokenFrac;
    TokenParse tokenExponent;

    tokenFrac.literalValue.u64     = 0;
    tokenExponent.literalValue.u64 = 0;
    token.literalType              = LiteralType::TT_S32;
    token.id                       = TokenId::LiteralNumber;

    // Integer part
    SWAG_CHECK(doIntLiteral(token, c));

    c = peekChar(offset);

    // Do this because of the slicing operator number..number. We do not want the '..' to be eaten
    bool hasDot = c == '.' && curBuffer[1] != '.';

    // If there's a dot, then this is a floating point number
    if (hasDot)
    {
        token.literalType = LiteralType::TT_UNTYPED_FLOAT;
        eatChar(c, offset);

        // Fraction part
        tokenFrac.startLocation = location;
        c                       = peekChar(offset);
        SWAG_VERIFY(SWAG_IS_NOT_NUM_SEP(c), error(tokenFrac, Err(Err0288)));
        if (SWAG_IS_DIGIT(c))
        {
            eatChar(c, offset);
            SWAG_CHECK(doFloatLiteral(tokenFrac, c));
            c = peekChar(offset);
        }
    }

    // If there's an exponent, then this is a floating point number
    if (c == 'e' || c == 'E')
    {
        token.literalType = LiteralType::TT_UNTYPED_FLOAT;
        eatChar(c, offset);
        tokenExponent.startLocation = location;

        bool minus = false;
        c          = peekChar(offset);

        if (c == '-')
        {
            minus = true;
            eatChar(c, offset);
            c = peekChar(offset);
        }
        else if (c == '+')
        {
            minus = false;
            eatChar(c, offset);
            c = peekChar(offset);
        }

        tokenExponent.startLocation = location;
        SWAG_VERIFY(SWAG_IS_NOT_NUM_SEP(c), error(tokenExponent, Err(Err0287)));
        SWAG_VERIFY(SWAG_IS_DIGIT(c), error(tokenExponent, Err(Err0286)));
        eatChar(c, offset);
        SWAG_CHECK(doIntLiteral(tokenExponent, c));
        c = peekChar(offset);

        if (minus)
            tokenExponent.literalValue.s64 = -tokenExponent.literalValue.s64;
    }

    // Really compute the floating point value, with as much precision as we can
    if (token.literalType == LiteralType::TT_UNTYPED_FLOAT)
    {
        auto cpt = (unsigned) (curBuffer - startTokenName);
        auto ptr = startTokenName + cpt;
        auto sc  = *ptr;
        *ptr     = 0;
        char* end;
        token.literalValue.f64 = strtod(startTokenName, &end);
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

bool Tokenizer::doNumberLiteral(TokenParse& token, uint32_t c)
{
    if (c == '0')
    {
        unsigned   offset;
        const auto startLoc = token.startLocation;

        c = peekChar(offset);

        // Hexadecimal
        if (c == 'x' || c == 'X')
        {
            eatChar(c, offset);
            SWAG_CHECK(doHexLiteral(token));
            token.endLocation = location;
            appendTokenName(token);
            return true;
        }

        // Binary
        if (c == 'b' || c == 'B')
        {
            eatChar(c, offset);
            SWAG_CHECK(doBinLiteral(token));
            token.endLocation = location;
            appendTokenName(token);
            return true;
        }

        if (SWAG_IS_ALPHA(c))
        {
            eatChar(c, offset);
            token.text          = c;
            token.startLocation = startLoc;
            return error(token, FMT(Err(Err0338), token.c_str()));
        }
    }

    SWAG_CHECK(doIntFloatLiteral(token, c));
    token.endLocation = location;
    appendTokenName(token);

    return true;
}
