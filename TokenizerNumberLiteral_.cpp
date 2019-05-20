#include "pch.h"
#include "Tokenizer.h"
#include "Global.h"
#include "LanguageSpec.h"
#include "Diagnostic.h"

bool Tokenizer::errorNumberSyntax(Token& token, const utf8& msg)
{
    utf8 text = "invalid number syntax, ";
    text += msg;
    error(token, text);
    return false;
}

bool Tokenizer::doNumberSuffix(Token& token)
{
    Token tokenType;
    tokenType.startLocation = m_location;
    getIdentifier(tokenType);
    SWAG_CHECK(tokenType.id == TokenId::NativeType || error(tokenType, format("invalid literal number suffix '%s'", tokenType.text.c_str())));

    switch (token.numType)
    {
    case TokenNumType::Float32:
        switch (tokenType.numType)
        {
        case TokenNumType::UInt8:
            return error(token, format("can't convert floating point number '%Lf' to 'u8'", token.numValue.f64));
        case TokenNumType::UInt16:
            return error(token, format("can't convert floating point number '%Lf' to 'u16'", token.numValue.f64));
        case TokenNumType::UInt32:
            return error(token, format("can't convert floating point number '%Lf' to 'u32'", token.numValue.f64));
        case TokenNumType::UInt64:
            return error(token, format("can't convert floating point number '%Lf' to 'u64'", token.numValue.f64));
        case TokenNumType::Int8:
            return error(token, format("can't convert floating point number '%Lf' to 's8'", token.numValue.f64));
        case TokenNumType::Int16:
            return error(token, format("can't convert floating point number '%Lf' to 's16'", token.numValue.f64));
        case TokenNumType::Int32:
            return error(token, format("can't convert floating point number '%Lf' to 's32'", token.numValue.f64));
        case TokenNumType::Int64:
            return error(token, format("can't convert floating point number '%Lf' to 's64'", token.numValue.f64));
        case TokenNumType::Bool:
            return error(token, format("can't convert floating point number '%Lf' to 'bool'", token.numValue.f64));
        case TokenNumType::Float32:
        case TokenNumType::Float64:
            token.numType = tokenType.numType;
            break;
        default:
            assert(false);
            break;
        }
        break;

    case TokenNumType::IntX:
        switch (tokenType.numType)
        {
        case TokenNumType::UInt8:
            if (token.numValue.u64 > UINT8_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 'u8'", token.numValue.u64));
            break;
        case TokenNumType::UInt16:
            if (token.numValue.u64 > UINT16_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 'u16'", token.numValue.u64));
            break;
        case TokenNumType::UInt32:
            if (token.numValue.u64 > UINT32_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 'u32'", token.numValue.u64));
            break;
        case TokenNumType::Int8:
            if (token.numValue.s64 < INT8_MIN || token.numValue.s64 > INT8_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 's8'", token.numValue.u64));
            break;
        case TokenNumType::Int16:
            if (token.numValue.s64 < INT16_MIN || token.numValue.s64 > INT16_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 's16'", token.numValue.u64));
            break;
        case TokenNumType::Int32:
            if (token.numValue.s64 < INT32_MIN || token.numValue.s64 > INT32_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 's32'", token.numValue.u64));
            break;
        case TokenNumType::Int64:
            if (token.numValue.s64 < INT64_MIN || token.numValue.s64 > INT64_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 's64'", token.numValue.u64));
            break;
        case TokenNumType::Float32:
            token.numValue.f32 = static_cast<float>(token.numValue.s64);
            token.numType      = tokenType.numType;
            break;
        case TokenNumType::Float64:
            token.numValue.f64 = static_cast<float>(token.numValue.s64);
            token.numType      = tokenType.numType;
            break;
        case TokenNumType::Bool:
            return error(token, format("can't convert literal number '%I64u' to 'bool'", token.numValue.u64));
        default:
            token.numType = tokenType.numType;
            break;
        }

    case TokenNumType::UIntX:
        switch (tokenType.numType)
        {
        case TokenNumType::UInt8:
            if (token.numValue.u64 > UINT8_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 'u8'", token.numValue.u64));
            break;
        case TokenNumType::UInt16:
            if (token.numValue.u64 > UINT16_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 'u16'", token.numValue.u64));
            break;
        case TokenNumType::UInt32:
            if (token.numValue.u64 > UINT32_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 'u32'", token.numValue.u64));
            break;
            if (token.numValue.u64 > UINT8_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 's8'", token.numValue.u64));
            break;
        case TokenNumType::Int16:
            if (token.numValue.u64 > UINT16_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 's16'", token.numValue.u64));
            break;
        case TokenNumType::Int32:
            if (token.numValue.u64 > UINT32_MAX)
                return error(token, format("literal number '%I64u' is not in the range of 's32'", token.numValue.u64));
            break;
        case TokenNumType::Bool:
            return error(token, format("can't convert literal number '%I64u' to 'bool'", token.numValue.u64));
        default:
            token.numType = tokenType.numType;
            break;
        }

        break;
    }

    return true;
}

bool Tokenizer::doBinLiteral(Token& token)
{
    token.numValue.u64 = 0;
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
        SWAG_VERIFY(!(token.numValue.u64 & 0x80000000'00000000), error(token, "literal number is too big"));
        SWAG_VERIFY(rank != 64, error(token, "too many digits in literal number"));
        token.numValue.u64 <<= 1;
        rank++;

        token.numValue.u64 += (c - '0');

        c = getCharNoSeek(offset);
    }

	// Be sure what follows is valid
	if (SWAG_IS_DIGIT(c) || SWAG_IS_ALPHA(c))
	{
		token.startLocation = m_location;
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
    token.numType = TokenNumType::UIntX;
    if (c == '\'')
    {
        treatChar(c, offset);
        SWAG_CHECK(doNumberSuffix(token));
        SWAG_VERIFY(token.numType != TokenNumType::Float32, error(token, "can't convert a binary literal number to 'f32'"));
        SWAG_VERIFY(token.numType != TokenNumType::Float64, error(token, "can't convert a binary literal number to 'f64'"));
    }

    return true;
}

bool Tokenizer::doHexLiteral(Token& token)
{
    token.numValue.u64 = 0;
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
        SWAG_VERIFY(!(token.numValue.u64 & 0xF0000000'00000000), error(token, "literal number is too big"));
        SWAG_VERIFY(rank != 16, error(token, "too many digits in literal number"));
        token.numValue.u64 <<= 4;
        rank++;

        if (SWAG_IS_DIGIT(c))
            token.numValue.u64 += (c - '0');
        else if (c >= 'a' && c <= 'f')
            token.numValue.u64 += (10 + (c - 'a'));
        else
            token.numValue.u64 += (10 + (c - 'A'));

        c = getCharNoSeek(offset);
    }

	// Be sure what follows is valid
    if (SWAG_IS_ALPHA(c))
    {
        token.startLocation = m_location;
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
    token.numType = TokenNumType::UIntX;
    if (c == '\'')
    {
        treatChar(c, offset);
        SWAG_CHECK(doNumberSuffix(token));
        SWAG_VERIFY(token.numType != TokenNumType::Float32, error(token, "can't convert an hexadecimal literal number to 'f32'"));
        SWAG_VERIFY(token.numType != TokenNumType::Float64, error(token, "can't convert an hexadecimal literal number to 'f64'"));
    }

    return true;
}

bool Tokenizer::doIntLiteral(char32_t c, Token& token, unsigned& fractPart)
{
    token.numValue.u64 = 0;
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
        token.numValue.u64 *= 10;
        fractPart *= 10;
        rank++;

        auto val = (c - '0');
        SWAG_VERIFY(token.numValue.u64 <= 18446744073709551615 - val, error(token, "literal number is too big"));
        token.numValue.u64 += val;

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
    tokenFrac.numValue.u64     = 0;
    tokenExponent.numValue.u64 = 0;

    token.numType = TokenNumType::IntX;
    token.id      = TokenId::LiteralNumber;

    // Integer part
    if (!startsWithDot)
    {
        SWAG_CHECK(doIntLiteral(c, token, fractPart));
        c = getCharNoSeek(offset);
    }
    else
        token.numValue.s64 = 0;

    // If there's a dot, then this is a floating point number
    if (c == '.' || startsWithDot)
    {
        token.numType = TokenNumType::Float32;
        if (!startsWithDot)
        {
            token.text += c;
            treatChar(c, offset);
        }

        // Fraction part
        tokenFrac.startLocation = m_location;
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
        token.numType = TokenNumType::Float32;
        token.text += c;
        treatChar(c, offset);
        tokenExponent.startLocation = m_location;

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

        tokenExponent.startLocation = m_location;
        SWAG_VERIFY(!SWAG_IS_NUMSEP(c), errorNumberSyntax(tokenExponent, "a digit separator can't start an exponent part"));
        SWAG_VERIFY(SWAG_IS_DIGIT(c), error(tokenExponent, "floating point number exponent must has at least one digit"));
        unsigned exponentPart;
        treatChar(c, offset);
        SWAG_CHECK(doIntLiteral(c, tokenExponent, exponentPart));
        token.text += tokenExponent.text;
        c = getCharNoSeek(offset);

        if (minus)
            tokenExponent.numValue.s64 = -tokenExponent.numValue.s64;
    }

    // Really compute the floating point value, with as much precision as we can
    if (token.numType == TokenNumType::Float32)
    {
        token.numValue.f64 = (double) (token.numValue.u64) + (tokenFrac.numValue.u64 / (double) fractPart);
        if (tokenExponent.numValue.s64)
            token.numValue.f64 *= std::pow(10, tokenExponent.numValue.s64);
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
			token.endLocation = m_location;
            return true;
        }

        // Binary
        if (c == 'b' || c == 'B')
        {
            treatChar(c, offset);
            token.text += c;
            SWAG_CHECK(doBinLiteral(token));
			token.endLocation = m_location;
            return true;
        }

        if (SWAG_IS_ALPHA(c))
        {
            token.startLocation = m_location;
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
			token.endLocation = m_location;
            token.id = TokenId::SymDot;
            return true;
        }
    }

    SWAG_CHECK(doIntFloatLiteral(startsWithDot, c, token));
	token.endLocation = m_location;

    return true;
}
