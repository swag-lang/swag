#include "pch.h"
#include "Report/ErrorIds.h"
#include "Syntax/Tokenizer/Tokenizer.h"

bool Tokenizer::doBinLiteral(TokenParse& tokenParse)
{
    int      rank      = 0;
    bool     acceptSep = true;
    unsigned offset;

    tokenParse.literalValue.u64 = 0;
    auto c                      = peekChar(offset);
    while (c == '0' || c == '1' || SWAG_IS_NUM_SEP(c))
    {
        eatChar(c, offset);

        // Digit separator
        if (SWAG_IS_NUM_SEP(c))
        {
            if (!acceptSep)
                return error(tokenParse, toErr(Err0274));
            acceptSep = false;
            c         = peekChar(offset);
            continue;
        }

        acceptSep = true;
        SWAG_VERIFY(!(tokenParse.literalValue.u64 & 0x80000000'00000000), error(tokenParse, toErr(Err0567)));
        SWAG_VERIFY(rank != 64, error(tokenParse, toErr(Err0272)));
        tokenParse.literalValue.u64 <<= 1;
        rank++;

        tokenParse.literalValue.u64 += c - '0';

        c = peekChar(offset);
    }

    // Be sure what follows is valid
    if (SWAG_IS_DIGIT(c) || SWAG_IS_ALPHA(c))
    {
        tokenParse.token.startLocation = location;
        tokenParse.token.text          = c;
        return error(tokenParse, toErr(Err0276));
    }

    // Be sure we don't have 0x without nothing
    if (rank == 0)
        return error(tokenParse, toErr(Err0275));
    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        return error(tokenParse, toErr(Err0271));

    // Type
    tokenParse.token.id = TokenId::LiteralNumber;
    if (tokenParse.literalValue.u64 <= UINT32_MAX)
        tokenParse.literalType = LiteralType::TypeUntypedBinHexa;
    else
        tokenParse.literalType = LiteralType::TypeUnsigned64;

    return true;
}

bool Tokenizer::doHexLiteral(TokenParse& tokenParse)
{
    int      rank      = 0;
    bool     acceptSep = true;
    unsigned offset;

    tokenParse.literalValue.u64 = 0;
    auto c                      = peekChar(offset);
    while (SWAG_IS_ALPHA_HEX(c) || SWAG_IS_DIGIT(c) || SWAG_IS_NUM_SEP(c))
    {
        eatChar(c, offset);

        // Digit separator
        if (SWAG_IS_NUM_SEP(c))
        {
            if (!acceptSep)
                return error(tokenParse, toErr(Err0274));
            acceptSep = false;
            c         = peekChar(offset);
            continue;
        }

        acceptSep = true;
        SWAG_VERIFY(!(tokenParse.literalValue.u64 & 0xF0000000'00000000), error(tokenParse, toErr(Err0567)));
        SWAG_VERIFY(rank != 16, error(tokenParse, toErr(Err0273)));
        tokenParse.literalValue.u64 <<= 4;
        rank++;

        if (SWAG_IS_DIGIT(c))
            tokenParse.literalValue.u64 += c - '0';
        else if (c >= 'a' && c <= 'f')
            tokenParse.literalValue.u64 += 10 + (c - 'a');
        else
            tokenParse.literalValue.u64 += 10 + (c - 'A');

        c = peekChar(offset);
    }

    // Be sure what follows is valid
    if (SWAG_IS_ALPHA(c))
    {
        tokenParse.token.startLocation = location;
        tokenParse.token.text          = c;
        tokenParse.token.id            = TokenId::Identifier;
        return error(tokenParse, toErr(Err0281));
    }

    // Be sure we don't have 0x without nothing
    if (rank == 0)
        return error(tokenParse, toErr(Err0280));
    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        return error(tokenParse, toErr(Err0271));

    // Type
    tokenParse.token.id = TokenId::LiteralNumber;
    if (tokenParse.literalValue.u64 <= UINT32_MAX)
        tokenParse.literalType = LiteralType::TypeUntypedBinHexa;
    else
        tokenParse.literalType = LiteralType::TypeUnsigned64;

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
            SWAG_CHECK(acceptSep || rank == 0 || error(token, toErr(Err0274)));
            acceptSep = false;
            c         = peekChar(offset);
            continue;
        }

        acceptSep = true;
        rank++;

        const auto val = c - '0';
        SWAG_VERIFY(token.literalValue.u64 <= 18446744073709551615ULL - val, error(token, toErr(Err0567)));
        token.literalValue.f64 += val * fractPart;
        fractPart *= 0.1;

        c = peekChar(offset);
    }

    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        return error(token, toErr(Err0271));

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
            SWAG_CHECK(acceptSep || rank == 0 || error(token, toErr(Err0274)));
            acceptSep = false;
            c         = peekChar(offset);
            continue;
        }

        acceptSep = true;
        token.literalValue.u64 *= 10;
        rank++;

        const auto val = c - '0';
        SWAG_VERIFY(token.literalValue.u64 <= 18446744073709551615ULL - val, error(token, toErr(Err0567)));
        token.literalValue.u64 += val;

        c = peekChar(offset);
    }

    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        return error(token, toErr(Err0271));

    return true;
}

bool Tokenizer::doIntFloatLiteral(TokenParse& tokenParse, uint32_t c)
{
    unsigned   offset = 1;
    TokenParse tokenFrac;
    TokenParse tokenExponent;

    tokenFrac.literalValue.u64     = 0;
    tokenExponent.literalValue.u64 = 0;
    tokenParse.literalType         = LiteralType::TypeSigned32;
    tokenParse.token.id            = TokenId::LiteralNumber;

    // Integer part
    SWAG_CHECK(doIntLiteral(tokenParse, c));

    c = peekChar(offset);

    // Do this because of the slicing operator number..number. We do not want the '..' to be eaten
    bool hasDot = c == '.' && curBuffer[1] != '.';

    // If there's a dot, then this is a floating point number
    if (hasDot)
    {
        tokenParse.literalType = LiteralType::TypeUntypedFloat;
        eatChar(c, offset);

        // Fraction part
        tokenFrac.token.startLocation = location;
        c                             = peekChar(offset);
        SWAG_VERIFY(SWAG_IS_NOT_NUM_SEP(c), error(tokenFrac, toErr(Err0278)));
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
        tokenParse.literalType = LiteralType::TypeUntypedFloat;
        eatChar(c, offset);
        tokenExponent.token.startLocation = location;

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

        tokenExponent.token.startLocation = location;
        SWAG_VERIFY(SWAG_IS_NOT_NUM_SEP(c), error(tokenExponent, toErr(Err0277)));
        SWAG_VERIFY(SWAG_IS_DIGIT(c), error(tokenExponent, toErr(Err0279)));
        eatChar(c, offset);
        SWAG_CHECK(doIntLiteral(tokenExponent, c));
        c = peekChar(offset);

        if (minus)
            tokenExponent.literalValue.s64 = -tokenExponent.literalValue.s64;
    }

    // Really compute the floating point value, with as much precision as we can
    if (tokenParse.literalType == LiteralType::TypeUntypedFloat)
    {
        auto cpt = static_cast<unsigned>(curBuffer - startTokenName);
        auto ptr = startTokenName + cpt;
        auto sc  = *ptr;
        *ptr     = 0;
        char* end;
        tokenParse.literalValue.f64 = strtod(startTokenName, &end);
        *ptr                        = sc;
    }
    else if (tokenParse.literalValue.s64 < INT32_MIN || tokenParse.literalValue.s64 > INT32_MAX)
    {
        // Can be a negative number ?
        if (tokenParse.literalValue.u64 > static_cast<uint64_t>(INT64_MAX) + 1)
            tokenParse.literalType = LiteralType::TypeUnsigned64;
        else
            tokenParse.literalType = LiteralType::TypeSigned64;
    }

    if (tokenParse.literalType == LiteralType::TypeSigned32)
        tokenParse.literalType = LiteralType::TypeUntypedInt;

    return true;
}

bool Tokenizer::doNumberLiteral(TokenParse& tokenParse, uint32_t c)
{
    if (c == '0')
    {
        unsigned   offset;
        const auto startLoc = tokenParse.token.startLocation;

        c = peekChar(offset);

        // Hexadecimal
        if (c == 'x' || c == 'X')
        {
            eatChar(c, offset);
            SWAG_CHECK(doHexLiteral(tokenParse));
            tokenParse.token.endLocation = location;
            appendTokenName(tokenParse);
            return true;
        }

        // Binary
        if (c == 'b' || c == 'B')
        {
            eatChar(c, offset);
            SWAG_CHECK(doBinLiteral(tokenParse));
            tokenParse.token.endLocation = location;
            appendTokenName(tokenParse);
            return true;
        }

        if (SWAG_IS_ALPHA(c))
        {
            eatChar(c, offset);
            tokenParse.token.text          = c;
            tokenParse.token.startLocation = startLoc;
            return error(tokenParse, toErr(Err0282));
        }
    }

    SWAG_CHECK(doIntFloatLiteral(tokenParse, c));
    tokenParse.token.endLocation = location;
    appendTokenName(tokenParse);

    return true;
}
