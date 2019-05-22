#include "pch.h"
#include "Tokenizer.h"
#include "Global.h"
#include "LanguageSpec.h"
#include "Diagnostic.h"

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
    getIdentifier(tokenSuffix);
    SWAG_CHECK(tokenSuffix.id == TokenId::NativeType || error(tokenSuffix, format("invalid literal number suffix '%s'", tokenSuffix.text.c_str())));
	SWAG_CHECK(tokenSuffix.literalType->type != NativeType::Bool || error(tokenSuffix, format("invalid literal number suffix '%s'", tokenSuffix.text.c_str())));
	SWAG_CHECK(tokenSuffix.literalType->type != NativeType::String || error(tokenSuffix, format("invalid literal number suffix '%s'", tokenSuffix.text.c_str())));

    switch (token.literalType->type)
    {
    case NativeType::F32:
        switch (tokenSuffix.literalType->type)
        {
        case NativeType::U8:
            return error(token, format("can't convert floating point number '%Lf' to 'u8'", token.literalValue.f64));
        case NativeType::U16:
            return error(token, format("can't convert floating point number '%Lf' to 'u16'", token.literalValue.f64));
        case NativeType::U32:
            return error(token, format("can't convert floating point number '%Lf' to 'u32'", token.literalValue.f64));
        case NativeType::U64:
        case NativeType::UX:
            return error(token, format("can't convert floating point number '%Lf' to 'u64'", token.literalValue.f64));
        case NativeType::S8:
            return error(token, format("can't convert floating point number '%Lf' to 's8'", token.literalValue.f64));
        case NativeType::S16:
            return error(token, format("can't convert floating point number '%Lf' to 's16'", token.literalValue.f64));
        case NativeType::S32:
            return error(token, format("can't convert floating point number '%Lf' to 's32'", token.literalValue.f64));
        case NativeType::S64:
        case NativeType::SX:
            return error(token, format("can't convert floating point number '%Lf' to 's64'", token.literalValue.f64));
        case NativeType::Bool:
            return error(token, format("can't convert floating point number '%Lf' to 'bool'", token.literalValue.f64));
        case NativeType::F32:
        case NativeType::F64:
            break;
        default:
			return error(token, format("invalid literal number conversion of '%f'", token.literalValue.f32));
        }
        break;

    case NativeType::SX:
        switch (tokenSuffix.literalType->type)
        {
        case NativeType::U8:
            if (token.literalValue.u64 > UINT8_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 'u8'", token.literalValue.u64));
            break;
        case NativeType::U16:
            if (token.literalValue.u64 > UINT16_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 'u16'", token.literalValue.u64));
            break;
        case NativeType::U32:
            if (token.literalValue.u64 > UINT32_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 'u32'", token.literalValue.u64));
            break;
		case NativeType::U64:
			break;
        case NativeType::S8:
            if (token.literalValue.s64 < INT8_MIN || token.literalValue.s64 > INT8_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 's8'", token.literalValue.u64));
            break;
        case NativeType::S16:
            if (token.literalValue.s64 < INT16_MIN || token.literalValue.s64 > INT16_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 's16'", token.literalValue.u64));
            break;
        case NativeType::S32:
            if (token.literalValue.s64 < INT32_MIN || token.literalValue.s64 > INT32_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 's32'", token.literalValue.u64));
            break;
        case NativeType::S64:
            if (token.literalValue.s64 < INT64_MIN || token.literalValue.s64 > INT64_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 's64'", token.literalValue.u64));
            break;
        case NativeType::F32:
            token.literalValue.f32 = static_cast<float>(token.literalValue.s64);
            token.literalType      = tokenSuffix.literalType;
            break;
        case NativeType::F64:
            token.literalValue.f64 = static_cast<float>(token.literalValue.s64);
            token.literalType      = tokenSuffix.literalType;
            break;
        case NativeType::Char:
            if (token.literalValue.u64 > UINT32_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 'char'", token.literalValue.u64));
            break;
        default:
			return error(token, format("invalid literal number conversion of '%I64u'", token.literalValue.u64));
        }

        break;

    case NativeType::UX:
        switch (tokenSuffix.literalType->type)
        {
        case NativeType::U8:
            if (token.literalValue.u64 > UINT8_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 'u8'", token.literalValue.u64));
            break;
        case NativeType::U16:
            if (token.literalValue.u64 > UINT16_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 'u16'", token.literalValue.u64));
            break;
        case NativeType::U32:
            if (token.literalValue.u64 > UINT32_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 'u32'", token.literalValue.u64));
            break;
        case NativeType::U64:
            break;
        case NativeType::S8:
            if (token.literalValue.u64 > UINT8_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 's8'", token.literalValue.u64));
            break;
        case NativeType::S16:
            if (token.literalValue.u64 > UINT16_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 's16'", token.literalValue.u64));
            break;
        case NativeType::S32:
            if (token.literalValue.u64 > UINT32_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 's32'", token.literalValue.u64));
            break;
		case NativeType::S64:
			break;
        case NativeType::Char:
            if (token.literalValue.u64 > UINT32_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 'char'", token.literalValue.u64));
            break;
        default:
			return error(token, format("invalid literal number conversion of '%I64u'", token.literalValue.u64));
        }

        break;
    }

	token.literalType = tokenSuffix.literalType;
    return true;
}

bool Tokenizer::doBinLiteral(Token& token)
{
    token.literalValue.u64 = 0;
    int rank           = 0;

    bool     acceptSep = false;
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
            {
                SWAG_CHECK(rank != 0 || errorNumberSyntax(token, "a digit separator can't start a literal number"));
                SWAG_CHECK(rank == 0 || errorNumberSyntax(token, "forbidden consecutive digit separators"));
            }

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
		token.text = c;
		SWAG_CHECK(error(token, format("invalid binary digit '%s'", token.text.c_str())));
	}

	// Be sure we don't have 0x without nothing
    if (rank == 0)
        SWAG_CHECK(errorNumberSyntax(token, "missing at least one digit"));
    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        SWAG_CHECK(errorNumberSyntax(token, "a digit separator can't end a literal number"));

    // Suffix
    token.id      = TokenId::LiteralNumber;
    token.literalType = &g_TypeInfoUX;
    if (c == '\'')
    {
        treatChar(c, offset);
        SWAG_CHECK(doNumberSuffix(token));
        SWAG_VERIFY(token.literalType->type != NativeType::F32, error(token, "can't convert a binary literal number to 'f32'"));
        SWAG_VERIFY(token.literalType->type != NativeType::F64, error(token, "can't convert a binary literal number to 'f64'"));
    }

    return true;
}

bool Tokenizer::doHexLiteral(Token& token)
{
    token.literalValue.u64 = 0;
    int rank           = 0;

    bool     acceptSep = false;
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
            {
                SWAG_CHECK(rank != 0 || errorNumberSyntax(token, "a digit separator can't start a literal number"));
                SWAG_CHECK(rank == 0 || errorNumberSyntax(token, "forbidden consecutive digit separators"));
            }

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
    token.id      = TokenId::LiteralNumber;
    token.literalType = &g_TypeInfoUX;
    if (c == '\'')
    {
        treatChar(c, offset);
        SWAG_CHECK(doNumberSuffix(token));
        SWAG_VERIFY(token.literalType->type != NativeType::F32, error(token, "can't convert an hexadecimal literal number to 'f32'"));
        SWAG_VERIFY(token.literalType->type != NativeType::F64, error(token, "can't convert an hexadecimal literal number to 'f64'"));
    }

    return true;
}

bool Tokenizer::doIntLiteral(char32_t c, Token& token, unsigned& fractPart)
{
    token.literalValue.u64 = 0;
    int rank           = 0;

    fractPart = 1;

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
                SWAG_CHECK(rank != 0 || errorNumberSyntax(token, "a digit separator can't start a literal number"));
                SWAG_CHECK(rank == 0 || errorNumberSyntax(token, "forbidden consecutive digit separators"));
            }

            acceptSep = false;
            c         = getCharNoSeek(offset);
            continue;
        }

        acceptSep = true;
        token.literalValue.u64 *= 10;
        fractPart *= 10;
        rank++;

        auto val = (c - '0');
        SWAG_VERIFY(token.literalValue.u64 <= 18446744073709551615 - val, error(token, "literal number is too big"));
        token.literalValue.u64 += val;

        c = getCharNoSeek(offset);
    }

    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        SWAG_CHECK(errorNumberSyntax(token, "a digit separator can't end a literal number"));

    return true;
}

bool Tokenizer::doIntFloatLiteral(bool startsWithDot, char32_t c, Token& token)
{
    unsigned fractPart = 1;
    unsigned offset    = 1;
    Token    tokenFrac;
    Token    tokenExponent;
    tokenFrac.literalValue.u64     = 0;
    tokenExponent.literalValue.u64 = 0;

    token.literalType = &g_TypeInfoSX;
    token.id      = TokenId::LiteralNumber;

    // Integer part
    if (!startsWithDot)
    {
        SWAG_CHECK(doIntLiteral(c, token, fractPart));
        c = getCharNoSeek(offset);
    }
    else
        token.literalValue.s64 = 0;

    // If there's a dot, then this is a floating point number
    if (c == '.' || startsWithDot)
    {
        token.literalType = &g_TypeInfoF32;
        if (!startsWithDot)
        {
            token.text += c;
            treatChar(c, offset);
        }

        // Fraction part
        tokenFrac.startLocation = location;
        c                       = getCharNoSeek(offset);
        SWAG_VERIFY(!SWAG_IS_NUMSEP(c), errorNumberSyntax(tokenFrac, "a digit separator can't start a fractional part"));
        if (SWAG_IS_DIGIT(c))
        {
            if (!startsWithDot)
                token.text += c;
            tokenFrac.text = c;
            treatChar(c, offset);
            SWAG_CHECK(doIntLiteral(c, tokenFrac, fractPart));
            token.text += tokenFrac.text;
            c = getCharNoSeek(offset);
        }
    }

    // If there's an exponent, then this is a floating point number
    if (c == 'e' || c == 'E')
    {
        token.literalType = &g_TypeInfoF32;
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
        SWAG_VERIFY(!SWAG_IS_NUMSEP(c), errorNumberSyntax(tokenExponent, "a digit separator can't start an exponent part"));
        SWAG_VERIFY(SWAG_IS_DIGIT(c), error(tokenExponent, "floating point number exponent must have at least one digit"));
        unsigned exponentPart;
        treatChar(c, offset);
        SWAG_CHECK(doIntLiteral(c, tokenExponent, exponentPart));
        token.text += tokenExponent.text;
        c = getCharNoSeek(offset);

        if (minus)
            tokenExponent.literalValue.s64 = -tokenExponent.literalValue.s64;
    }

    // Really compute the floating point value, with as much precision as we can
    if (token.literalType->type == NativeType::F32)
    {
        token.literalValue.f64 = (double) (token.literalValue.u64) + (tokenFrac.literalValue.u64 / (double) fractPart);
        if (tokenExponent.literalValue.s64)
            token.literalValue.f64 *= std::pow(10, tokenExponent.literalValue.s64);
    }

    if (c == '\'')
    {
        treatChar(c, offset);
        SWAG_CHECK(doNumberSuffix(token));
    }

    // Note: even for a float32 type, the value is stored in f64 in order to keep the maximum precision
    // as much as we can for possible later computations.
    return true;
}

bool Tokenizer::doNumberLiteral(char32_t c, Token& token)
{
    bool startsWithDot = false;
    token.text         = c;
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
    else if (c == '.')
    {
        unsigned offset;
        startsWithDot = true;
        c             = getCharNoSeek(offset);
        if (!SWAG_IS_DIGIT(c))
        {
			token.endLocation = location;
            token.id = TokenId::SymDot;
            return true;
        }
    }

    SWAG_CHECK(doIntFloatLiteral(startsWithDot, c, token));
	token.endLocation = location;

    return true;
}
