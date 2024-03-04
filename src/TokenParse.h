#pragma once
#include "Flags.h"
#include "Register.h"
#include "Token.h"

struct SourceFile;
struct TypeInfo;
struct ErrorContext;

using TokenParseFlags = Flags<uint8_t>;

enum class LiteralType : uint8_t
{
    TypeUnsigned8,
    TypeUnsigned16,
    TypeUnsigned32,
    TypeUnsigned64,
    TypeSigned8,
    TypeSigned16,
    TypeSigned32,
    TypeSigned64,
    TypeBool,
    TypeString,
    TypeStringRaw,
    TypeStringMultiLine,
    TypeStringEscape,
    TypeStringMultiLineEscape,
    TypeCharacter,
    TypeCharacterEscape,
    TypeRune,
    TypeFloat32,
    TypeFloat64,
    TypeNull,
    TypeAny,
    TypeVoid,
    TypeType,
    TypeCString,
    TypeUntypedBinHexa,
    TypeUntypedInt,
    TypeUntypedFloat,
    TypeMax,
};

constexpr TokenParseFlags TOKEN_PARSE_EOL_BEFORE        = 0x01;
constexpr TokenParseFlags TOKEN_PARSE_EOL_AFTER         = 0x02;
constexpr TokenParseFlags TOKEN_PARSE_BLANK_BEFORE      = 0x04;
constexpr TokenParseFlags TOKEN_PARSE_BLANK_AFTER       = 0x08;
constexpr TokenParseFlags TOKEN_PARSE_BLANK_LINE_BEFORE = 0x10;

struct TokenComments
{
    Vector<std::pair<Utf8, bool>> before;
    Vector<std::pair<Utf8, bool>> justBefore;
    Vector<std::pair<Utf8, bool>> afterSameLine;

    static Utf8 toString(const Vector<std::pair<Utf8, bool>>& other)
    {
        Utf8 result;

        bool first = true;
        for (const auto& key : other | std::views::keys)
        {
            if (!first)
                result += '\n';
            result += key;
            first = false;
        }

        return result;
    }
};

struct TokenParse
{
    bool is(TokenId what) const { return token.id == what; }
    bool isNot(TokenId what) const { return token.id != what; }

    Token           token;
    TokenComments   comments;
    Register        literalValue;
    LiteralType     literalType = static_cast<LiteralType>(0);
    TokenParseFlags flags       = 0;
};
