#include "pch.h"
#include "Global.h"
#include "LanguageSpec.h"
#include "TypeManager.h"
#include "SourceFile.h"

bool Tokenizer::errorNumberSyntax(Token& token, const Utf8& msg)
{
    Utf8 text = "invalid number syntax, ";
    text += msg;
    error(token, text);
    return false;
}

bool Tokenizer::doNumberSuffix(Token& token)
{
    Token tokenSuffix;
    tokenSuffix.startLocation = location;

    unsigned offset;
    auto     c = getCharNoSeek(offset);
    if (!c)
        return error(tokenSuffix, "missing literal number suffix");
    getIdentifier(tokenSuffix, c, offset);

    SWAG_CHECK(tokenSuffix.id == TokenId::NativeType || error(tokenSuffix, format("invalid literal number suffix '%s'", tokenSuffix.text.c_str())));
    SWAG_CHECK(tokenSuffix.literalType != LiteralType::TT_BOOL || error(tokenSuffix, format("invalid literal number suffix '%s'", tokenSuffix.text.c_str())));
    SWAG_CHECK(tokenSuffix.literalType != LiteralType::TT_STRING || error(tokenSuffix, format("invalid literal number suffix '%s'", tokenSuffix.text.c_str())));

    token.literalCastedType = tokenSuffix.literalType;
    return true;
}

bool Tokenizer::doBinLiteral(Token& token)
{
    token.literalValue.u64 = 0;
    int rank               = 0;

    bool     acceptSep = true;
    unsigned offset;
    auto     c = getCharNoSeek(offset);
    while (c == '0' || c == '1' || SWAG_IS_NUMSEP(c))
    {
        token.text += c;
        treatChar(c, offset);

        // Digit separator
        if (SWAG_IS_NUMSEP(c))
        {
            if (!acceptSep)
                return errorNumberSyntax(token, "forbidden consecutive digit separators");
            acceptSep = false;
            c         = getCharNoSeek(offset);
            continue;
        }

        acceptSep = true;
        SWAG_VERIFY(!(token.literalValue.u64 & 0x80000000'00000000), error(token, "literal number is too big"));
        SWAG_VERIFY(rank != 64, error(token, "too many digits in literal number"));
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
        SWAG_CHECK(error(token, format("invalid binary digit '%s'", token.text.c_str())));
    }

    // Be sure we don't have 0x without nothing
    if (rank == 0)
        SWAG_CHECK(errorNumberSyntax(token, "missing at least one digit"));
    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        SWAG_CHECK(errorNumberSyntax(token, "a digit separator can't end a literal number"));

    // Suffix
    token.id = TokenId::LiteralNumber;
    if (token.literalValue.u64 <= UINT32_MAX)
        token.literalType = LiteralType::TT_U32;
    else
        token.literalType = LiteralType::TT_U64;

    bool hasSuffix = false;
    if (c == '\'')
    {
        hasSuffix = true;
        treatChar(c, offset);
        SWAG_CHECK(doNumberSuffix(token));
        SWAG_VERIFY(token.literalCastedType != LiteralType::TT_F32, error(token, "cannot convert a binary literal number to 'f32'"));
        SWAG_VERIFY(token.literalCastedType != LiteralType::TT_F64, error(token, "cannot convert a binary literal number to 'f64'"));
    }

    if (!hasSuffix)
    {
        if (token.literalType == LiteralType::TT_U32)
            token.literalType = LiteralType::TT_UNTYPED_BINHEXA;
    }

    return true;
}

bool Tokenizer::doHexLiteral(Token& token)
{
    token.literalValue.u64 = 0;
    int rank               = 0;

    bool     acceptSep = true;
    unsigned offset;
    auto     c = getCharNoSeek(offset);
    while (SWAG_IS_ALPHAHEX(c) || SWAG_IS_DIGIT(c) || SWAG_IS_NUMSEP(c))
    {
        token.text += c;
        treatChar(c, offset);

        // Digit separator
        if (SWAG_IS_NUMSEP(c))
        {
            if (!acceptSep)
                return errorNumberSyntax(token, "forbidden consecutive digit separators");
            acceptSep = false;
            c         = getCharNoSeek(offset);
            continue;
        }

        acceptSep = true;
        SWAG_VERIFY(!(token.literalValue.u64 & 0xF0000000'00000000), error(token, "literal number is too big"));
        SWAG_VERIFY(rank != 16, error(token, "too many digits in literal number"));
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
        SWAG_CHECK(error(token, format("invalid hexadecimal digit '%s'", token.text.c_str())));
    }

    // Be sure we don't have 0x without nothing
    if (rank == 0)
        SWAG_CHECK(errorNumberSyntax(token, "missing at least one digit"));
    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        SWAG_CHECK(errorNumberSyntax(token, "a digit separator can't end a literal number"));

    // Suffix
    token.id = TokenId::LiteralNumber;
    if (token.literalValue.u64 <= UINT32_MAX)
        token.literalType = LiteralType::TT_U32;
    else
        token.literalType = LiteralType::TT_U64;

    bool hasSuffix = false;
    if (c == '\'')
    {
        hasSuffix = true;
        treatChar(c, offset);
        SWAG_CHECK(doNumberSuffix(token));
        SWAG_VERIFY(token.literalCastedType != LiteralType::TT_F32, error(token, "can't convert an hexadecimal literal number to 'f32'"));
        SWAG_VERIFY(token.literalCastedType != LiteralType::TT_F64, error(token, "can't convert an hexadecimal literal number to 'f64'"));
    }

    if (!hasSuffix)
    {
        if (token.literalType == LiteralType::TT_U32)
            token.literalType = LiteralType::TT_UNTYPED_BINHEXA;
    }

    return true;
}

bool Tokenizer::doFloatLiteral(uint32_t c, Token& token)
{
    token.literalValue.f64 = 0;
    int    rank            = 0;
    double fractPart       = 0.1;

    bool     acceptSep = true;
    unsigned offset    = 0;
    while (SWAG_IS_DIGIT(c) || SWAG_IS_NUMSEP(c))
    {
        if (offset)
        {
            token.text += c;
            treatChar(c, offset);
        }

        // Digit separator
        if (SWAG_IS_NUMSEP(c))
        {
            if (!acceptSep)
            {
                SWAG_CHECK(rank != 0 || errorNumberSyntax(token, "a digit separator cannot start a literal number"));
                SWAG_CHECK(rank == 0 || errorNumberSyntax(token, "forbidden consecutive digit separators"));
            }

            acceptSep = false;
            c         = getCharNoSeek(offset);
            continue;
        }

        acceptSep = true;
        rank++;

        auto val = (c - '0');
        SWAG_VERIFY(token.literalValue.u64 <= 18446744073709551615 - val, error(token, "literal number is too big"));
        token.literalValue.f64 += val * fractPart;
        fractPart *= 0.1;

        c = getCharNoSeek(offset);
    }

    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        SWAG_CHECK(errorNumberSyntax(token, "a digit separator cannot end a literal number"));

    return true;
}

bool Tokenizer::doIntLiteral(uint32_t c, Token& token)
{
    token.literalValue.u64 = 0;
    int rank               = 0;

    bool     acceptSep = true;
    unsigned offset    = 0;
    while (SWAG_IS_DIGIT(c) || SWAG_IS_NUMSEP(c))
    {
        if (offset)
        {
            token.text += c;
            treatChar(c, offset);
        }

        // Digit separator
        if (SWAG_IS_NUMSEP(c))
        {
            if (!acceptSep)
            {
                SWAG_CHECK(rank != 0 || errorNumberSyntax(token, "a digit separator cannot start a literal number"));
                SWAG_CHECK(rank == 0 || errorNumberSyntax(token, "forbidden consecutive digit separators"));
            }

            acceptSep = false;
            c         = getCharNoSeek(offset);
            continue;
        }

        acceptSep = true;
        token.literalValue.u64 *= 10;
        rank++;

        auto val = (c - '0');
        SWAG_VERIFY(token.literalValue.u64 <= 18446744073709551615 - val, error(token, "literal number is too big"));
        token.literalValue.u64 += val;

        c = getCharNoSeek(offset);
    }

    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        SWAG_CHECK(errorNumberSyntax(token, "a digit separator cannot end a literal number"));

    return true;
}

bool Tokenizer::doIntFloatLiteral(uint32_t c, Token& token)
{
    unsigned offset = 1;
    Token    tokenFrac;
    Token    tokenExponent;
    tokenFrac.literalValue.u64     = 0;
    tokenExponent.literalValue.u64 = 0;

    token.literalType = LiteralType::TT_S32;
    token.id          = TokenId::LiteralNumber;

    // Integer part
    SWAG_CHECK(doIntLiteral(c, token));

    c = getCharNoSeek(offset);

    // Do this because of the slicing operator number..number. We do not want the '..' to be eaten
    bool hasDot = false;
    if (c == '.')
    {
        unsigned offset1;
        auto     c1 = getChar(offset1, false, false);
        if (c == '.' && c1 != '.')
            hasDot = true;
    }

    // If there's a dot, then this is a floating point number
    if (hasDot)
    {
        token.literalType = LiteralType::TT_F32;
        token.text += c;
        treatChar(c, offset);

        // Fraction part
        tokenFrac.startLocation = location;
        c                       = getCharNoSeek(offset);
        SWAG_VERIFY(!SWAG_IS_NUMSEP(c), errorNumberSyntax(tokenFrac, "a digit separator cannot start a fractional part"));
        if (SWAG_IS_DIGIT(c))
        {
            tokenFrac.text = c;
            treatChar(c, offset);
            SWAG_CHECK(doFloatLiteral(c, tokenFrac));
            token.text += tokenFrac.text;
            c = getCharNoSeek(offset);
        }
    }

    // If there's an exponent, then this is a floating point number
    if (c == 'e' || c == 'E')
    {
        token.literalType = LiteralType::TT_F32;
        token.text += c;
        treatChar(c, offset);
        tokenExponent.startLocation = location;

        bool minus = false;
        c          = getCharNoSeek(offset);

        if (c == '-')
        {
            token.text += c;
            minus = true;
            treatChar(c, offset);
            c = getCharNoSeek(offset);
        }
        else if (c == '+')
        {
            token.text += c;
            minus = false;
            treatChar(c, offset);
            c = getCharNoSeek(offset);
        }

        tokenExponent.startLocation = location;
        SWAG_VERIFY(!SWAG_IS_NUMSEP(c), errorNumberSyntax(tokenExponent, "a digit separator cannot start an exponent part"));
        SWAG_VERIFY(SWAG_IS_DIGIT(c), error(tokenExponent, "floating point number exponent must have at least one digit"));
        token.text += c;
        treatChar(c, offset);
        SWAG_CHECK(doIntLiteral(c, tokenExponent));
        token.text += tokenExponent.text;
        c = getCharNoSeek(offset);

        if (minus)
            tokenExponent.literalValue.s64 = -tokenExponent.literalValue.s64;
    }

    // Really compute the floating point value, with as much precision as we can
    if (token.literalType == LiteralType::TT_F32 || token.literalType == LiteralType::TT_F64)
    {
        token.literalValue.f64 = atof(token.text.c_str());
    }
    else if (token.literalValue.s64 < INT32_MIN || token.literalValue.s64 > INT32_MAX)
    {
        // Can be a negative number ?
        if (token.literalValue.u64 > (uint64_t) INT64_MAX + 1)
            token.literalType = LiteralType::TT_U64;
        else
            token.literalType = LiteralType::TT_S64;
    }

    bool hasSuffix = false;
    if (c == '\'')
    {
        hasSuffix = true;
        treatChar(c, offset);
        SWAG_CHECK(doNumberSuffix(token));
    }

    // A type with flag TYPEINFO_UNTYPED_INTEGER of FLOAT has no real type yet, and can be casted automaticly when used
    if (!hasSuffix)
    {
        if (token.literalType == LiteralType::TT_S32)
            token.literalType = LiteralType::TT_UNTYPED_INT;
        else if (token.literalType == LiteralType::TT_F32)
            token.literalType = LiteralType::TT_UNTYPED_FLOAT;
    }

    return true;
}

bool Tokenizer::doNumberLiteral(uint32_t c, Token& token)
{
    token.literalCastedType = LiteralType::TT_MAX;
    token.text              = c;
    if (c == '0')
    {
        unsigned offset;
        c = getCharNoSeek(offset);

        // Hexadecimal
        if (c == 'x' || c == 'X')
        {
            treatChar(c, offset);
            token.text += c;
            SWAG_CHECK(doHexLiteral(token));
            token.endLocation = location;
            return true;
        }

        // Binary
        if (c == 'b' || c == 'B')
        {
            treatChar(c, offset);
            token.text += c;
            SWAG_CHECK(doBinLiteral(token));
            token.endLocation = location;
            return true;
        }

        if (SWAG_IS_ALPHA(c))
        {
            token.startLocation = location;
            treatChar(c, offset);
            token.text = c;
            return error(token, format("invalid literal number prefix '%s'", token.text.c_str()));
        }
    }

    SWAG_CHECK(doIntFloatLiteral(c, token));
    token.endLocation = location;

    return true;
}
