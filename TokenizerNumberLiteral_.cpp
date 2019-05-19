#include "pch.h"
#include "Tokenizer.h"
#include "Global.h"
#include "LanguageSpec.h"
#include "Diagnostic.h"

bool Tokenizer::errorNumberSyntax(Token& token, const wstring& msg)
{
    error(token, L"invalid number syntax, " + msg);
    return false;
}

bool Tokenizer::doNumberSuffix(Token& token)
{
    Token tokenType;
    tokenType.startLocation = m_location;
    getIdentifier(tokenType);
    SWAG_CHECK(tokenType.id == TokenId::NativeType || error(tokenType, format(L"invalid literal number suffix '%s'", tokenType.text.c_str())));

    switch (token.numType)
    {
    case TokenNumType::Float32:
        switch (tokenType.numType)
        {
        case TokenNumType::UInt8:
            return error(token, format(L"can't convert floating point number '%Lf' to 'u8'", token.numValue.f64));
        case TokenNumType::UInt16:
            return error(token, format(L"can't convert floating point number '%Lf' to 'u16'", token.numValue.f64));
        case TokenNumType::UInt32:
            return error(token, format(L"can't convert floating point number '%Lf' to 'u32'", token.numValue.f64));
        case TokenNumType::UInt64:
            return error(token, format(L"can't convert floating point number '%Lf' to 'u64'", token.numValue.f64));
        case TokenNumType::Int8:
            return error(token, format(L"can't convert floating point number '%Lf' to 's8'", token.numValue.f64));
        case TokenNumType::Int16:
            return error(token, format(L"can't convert floating point number '%Lf' to 's16'", token.numValue.f64));
        case TokenNumType::Int32:
            return error(token, format(L"can't convert floating point number '%Lf' to 's32'", token.numValue.f64));
        case TokenNumType::Int64:
            return error(token, format(L"can't convert floating point number '%Lf' to 's64'", token.numValue.f64));
        case TokenNumType::Bool:
            return error(token, format(L"can't convert floating point number '%Lf' to 'bool'", token.numValue.f64));
        case TokenNumType::Float32:
            token.numValue.f32 = static_cast<float>(token.numValue.f64);
            break;
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
                return error(token, format(L"literal number '%I64u' is not in the range of 'u8'", token.numValue.u64));
            break;
        case TokenNumType::UInt16:
            if (token.numValue.u64 > UINT16_MAX)
                return error(token, format(L"literal number '%I64u' is not in the range of 'u16'", token.numValue.u64));
            break;
        case TokenNumType::UInt32:
            if (token.numValue.u64 > UINT32_MAX)
                return error(token, format(L"literal number '%I64u' is not in the range of 'u32'", token.numValue.u64));
            break;
        case TokenNumType::Int8:
            if (token.numValue.s64 < INT8_MIN || token.numValue.s64 > INT8_MAX)
                return error(token, format(L"literal number '%I64u' is not in the range of 's8'", token.numValue.u64));
            break;
        case TokenNumType::Int16:
            if (token.numValue.s64 < INT16_MIN || token.numValue.s64 > INT16_MAX)
                return error(token, format(L"literal number '%I64u' is not in the range of 's16'", token.numValue.u64));
            break;
        case TokenNumType::Int32:
            if (token.numValue.s64 < INT32_MIN || token.numValue.s64 > INT32_MAX)
                return error(token, format(L"literal number '%I64u' is not in the range of 's32'", token.numValue.u64));
            break;
        case TokenNumType::Int64:
            if (token.numValue.s64 < INT64_MIN || token.numValue.s64 > INT64_MAX)
                return error(token, format(L"literal number '%I64u' is not in the range of 's64'", token.numValue.u64));
            break;
        case TokenNumType::Float32:
            token.numValue.f32 = static_cast<float>(token.numValue.s64);
			token.numType = tokenType.numType;
            break;
        case TokenNumType::Float64:
            token.numValue.f64 = static_cast<float>(token.numValue.s64);
			token.numType = tokenType.numType;
            break;
        case TokenNumType::Bool:
            return error(token, format(L"can't convert literal number '%I64u' to 'bool'", token.numValue.u64));
        default:
            token.numType = tokenType.numType;
            break;
        }

    case TokenNumType::UIntX:
        switch (tokenType.numType)
        {
        case TokenNumType::UInt8:
            if (token.numValue.u64 > UINT8_MAX)
                return error(token, format(L"literal number '%I64u' is not in the range of 'u8'", token.numValue.u64));
            break;
        case TokenNumType::UInt16:
            if (token.numValue.u64 > UINT16_MAX)
                return error(token, format(L"literal number '%I64u' is not in the range of 'u16'", token.numValue.u64));
            break;
        case TokenNumType::UInt32:
            if (token.numValue.u64 > UINT32_MAX)
                return error(token, format(L"literal number '%I64u' is not in the range of 'u32'", token.numValue.u64));
            break;
            if (token.numValue.u64 > UINT8_MAX)
                return error(token, format(L"literal number '%I64u' is not in the range of 's8'", token.numValue.u64));
            break;
        case TokenNumType::Int16:
            if (token.numValue.u64 > UINT16_MAX)
                return error(token, format(L"literal number '%I64u' is not in the range of 's16'", token.numValue.u64));
            break;
        case TokenNumType::Int32:
            if (token.numValue.u64 > UINT32_MAX)
                return error(token, format(L"literal number '%I64u' is not in the range of 's32'", token.numValue.u64));
            break;
        case TokenNumType::Bool:
            return error(token, format(L"can't convert literal number '%I64u' to 'bool'", token.numValue.u64));
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
                SWAG_CHECK(rank != 0 || errorNumberSyntax(token, L"a digit separator can't start a literal number"));
                SWAG_CHECK(rank == 0 || errorNumberSyntax(token, L"forbidden consecutive digit separators"));
            }

            acceptSep = false;
            c         = getCharNoSeek(offset);
            continue;
        }

        acceptSep = true;
        SWAG_VERIFY(!(token.numValue.u64 & 0x80000000'00000000), error(token, L"literal number is too big"));
        SWAG_VERIFY(rank != 64, error(token, L"too many digits in literal number"));
        token.numValue.u64 <<= 1;
        rank++;

        token.numValue.u64 += (c - '0');

        c = getCharNoSeek(offset);
    }

    // Be sure we don't have 0x without nothing
    if (rank == 0)
        SWAG_CHECK(errorNumberSyntax(token, L"missing at least one digit"));
    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        SWAG_CHECK(errorNumberSyntax(token, L"a digit separator can't end a literal number"));

    // Suffix
    token.id      = TokenId::LiteralNumber;
    token.numType = TokenNumType::UIntX;
    if (c == '\'')
    {
        treatChar(c, offset);
        SWAG_CHECK(doNumberSuffix(token));
        SWAG_VERIFY(token.numType != TokenNumType::Float32, error(token, L"can't convert a binary literal number to 'f32'"));
        SWAG_VERIFY(token.numType != TokenNumType::Float64, error(token, L"can't convert a binary literal number to 'f64'"));
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
    while (SWAG_IS_HEX(c) || SWAG_IS_DIGIT(c) || SWAG_IS_NUMSEP(c))
    {
        token.text += c;
        treatChar(c, offset);

        // Digit separator
        if (SWAG_IS_NUMSEP(c))
        {
            if (!acceptSep)
            {
                SWAG_CHECK(rank != 0 || errorNumberSyntax(token, L"a digit separator can't start a literal number"));
                SWAG_CHECK(rank == 0 || errorNumberSyntax(token, L"forbidden consecutive digit separators"));
            }

            acceptSep = false;
            c         = getCharNoSeek(offset);
            continue;
        }

        acceptSep = true;
        SWAG_VERIFY(!(token.numValue.u64 & 0xF0000000'00000000), error(token, L"literal number is too big"));
        SWAG_VERIFY(rank != 16, error(token, L"too many digits in literal number"));
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

    // Be sure we don't have 0x without nothing
    if (rank == 0)
        SWAG_CHECK(errorNumberSyntax(token, L"missing at least one digit"));
    // Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        SWAG_CHECK(errorNumberSyntax(token, L"a digit separator can't end a literal number"));

    // Suffix
    token.id      = TokenId::LiteralNumber;
    token.numType = TokenNumType::UIntX;
    if (c == '\'')
    {
        treatChar(c, offset);
        SWAG_CHECK(doNumberSuffix(token));
        SWAG_VERIFY(token.numType != TokenNumType::Float32, error(token, L"can't convert an hexadecimal literal number to 'f32'"));
        SWAG_VERIFY(token.numType != TokenNumType::Float64, error(token, L"can't convert an hexadecimal literal number to 'f64'"));
    }

    return true;
}

bool Tokenizer::doIntLiteral(unsigned c, Token& token, unsigned& fractPart)
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
                SWAG_CHECK(rank != 0 || errorNumberSyntax(token, L"a digit separator can't start a literal number"));
                SWAG_CHECK(rank == 0 || errorNumberSyntax(token, L"forbidden consecutive digit separators"));
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
        SWAG_VERIFY(token.numValue.u64 <= 18446744073709551615 - val, error(token, L"literal number is too big"));
        token.numValue.u64 += val;

        c = getCharNoSeek(offset);
    }

	// Be sure we don't have a number with a separator at its end
    if (!acceptSep)
        SWAG_CHECK(errorNumberSyntax(token, L"a digit separator can't end a literal number"));

    return true;
}

bool Tokenizer::doIntFloatLiteral(unsigned c, Token& token)
{
    unsigned fractPart = 1;
    Token    tokenFrac;
    tokenFrac.numValue.u64 = 0;

    // Integer part
    token.numType = TokenNumType::IntX;
    token.id      = TokenId::LiteralNumber;
    SWAG_CHECK(doIntLiteral(c, token, fractPart));

    // If there's a dot, then this is a floating point number
    unsigned offset;
    c = getCharNoSeek(offset);
    if (c == '.')
    {
        token.numType = TokenNumType::Float32;
        treatChar(c, offset);
        tokenFrac.startLocation = m_location;

        // Fraction part
        c = getCharNoSeek(offset);
		SWAG_VERIFY(!SWAG_IS_NUMSEP(c), errorNumberSyntax(tokenFrac, L"a digit separator can't start a fractional part"));
        if (SWAG_IS_DIGIT(c))
        {
            tokenFrac.text = c;
            treatChar(c, offset);
            SWAG_CHECK(doIntLiteral(c, tokenFrac, fractPart));
            c = getCharNoSeek(offset);
        }
    }

    // If there's an exponent, then this is a floating point number
    if (c == 'e' || c == 'E')
    {
        // TODO
    }

    if (token.numType == TokenNumType::Float32)
    {
        token.numValue.f64 = (double) (token.numValue.u64) + (tokenFrac.numValue.u64 / (double) fractPart);
    }

    if (c == '\'')
    {
        treatChar(c, offset);
        SWAG_CHECK(doNumberSuffix(token));
    }

    // By default, floating point numbers are f32, not f64!
    else if (token.numType == TokenNumType::Float32)
        token.numValue.f32 = static_cast<float>(token.numValue.f64);

    return true;
}

bool Tokenizer::doNumberLiteral(unsigned c, Token& token)
{
    token.text = c;
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
            return true;
        }

		// Binary
        if (c == 'b' || c == 'B')
        {
			treatChar(c, offset);
            token.text += c;
            SWAG_CHECK(doBinLiteral(token));
            return true;
        }

		if (c != '.' && c != 'e' && c != 'E' && !SWAG_IS_DIGIT(c))
		{
            token.startLocation = m_location;
			treatChar(c, offset);
			return error(token, format(L"invalid literal number prefix '%c'", c));
		}
    }

    SWAG_CHECK(doIntFloatLiteral(c, token));
    return true;
}
