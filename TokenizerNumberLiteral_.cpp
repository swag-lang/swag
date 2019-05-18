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
    case TokenNumType::IntX:
        switch (tokenType.numType)
        {
        case TokenNumType::UInt8:
            if (token.numValue.u64 > UINT8_MAX)
                return error(token, format(L"can't convert literal number '%I64u' to 'u8'", token.numValue.u64));
            break;
        case TokenNumType::UInt16:
            if (token.numValue.u64 > UINT16_MAX)
                return error(token, format(L"can't convert literal number '%I64u' to 'u16'", token.numValue.u64));
            break;
        case TokenNumType::UInt32:
            if (token.numValue.u64 > UINT32_MAX)
                return error(token, format(L"can't convert literal number '%I64u' to 'u32'", token.numValue.u64));
            break;
        case TokenNumType::Int8:
            if (token.numValue.u64 > UINT8_MAX)
                return error(token, format(L"can't convert literal number '%I64u' to 's8'", token.numValue.u64));
            break;
        case TokenNumType::Int16:
            if (token.numValue.u64 > UINT16_MAX)
                return error(token, format(L"can't convert literal number '%I64u' to 's16'", token.numValue.u64));
            break;
        case TokenNumType::Int32:
            if (token.numValue.u64 > UINT32_MAX)
                return error(token, format(L"can't convert literal number '%I64u' to 's32'", token.numValue.u64));
            break;
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
        token.numValue.u64 <<= 1;
        rank++;
        SWAG_VERIFY(rank != 65, error(token, L"literal number is too big"));

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
    token.numType = TokenNumType::IntX;
    if (c == '\'')
    {
        treatChar(c, offset);
        SWAG_CHECK(doNumberSuffix(token));
        SWAG_VERIFY(token.numType != TokenNumType::Bool, error(token, L"can't convert a binary literal number to 'bool'"));
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
        token.numValue.u64 <<= 4;
        rank++;
        SWAG_CHECK(rank != 17 || error(token, L"literal number is too big"));

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
    token.numType = TokenNumType::IntX;
    if (c == '\'')
    {
        treatChar(c, offset);
        SWAG_CHECK(doNumberSuffix(token));
        SWAG_VERIFY(token.numType != TokenNumType::Bool, error(token, L"can't convert an hexadecimal literal number to 'bool'"));
        SWAG_VERIFY(token.numType != TokenNumType::Float32, error(token, L"can't convert an hexadecimal literal number to 'f32'"));
        SWAG_VERIFY(token.numType != TokenNumType::Float64, error(token, L"can't convert an hexadecimal literal number to 'f64'"));
    }

    return true;
}

bool Tokenizer::doNumberLiteral(unsigned c, Token& token)
{
    token.text = c;
    if (c == '0')
    {
        c = getChar();
        if (c == 'x' || c == 'X')
        {
            token.text += c;
            SWAG_CHECK(doHexLiteral(token));
            return true;
        }

		if (c == 'b' || c == 'B')
        {
            token.text += c;
            SWAG_CHECK(doBinLiteral(token));
            return true;
        }
    }

    return true;
}
