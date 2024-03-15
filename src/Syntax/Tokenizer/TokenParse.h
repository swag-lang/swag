#pragma once
#include "Backend/ByteCode/Register.h"
#include "Syntax/Tokenizer/Token.h"

struct SourceFile;
struct TypeInfo;
struct ErrorContext;

using TokenParseFlags = Flags<uint32_t>;

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

constexpr TokenParseFlags TOKEN_PARSE_EOL_BEFORE        = 0x00000001;
constexpr TokenParseFlags TOKEN_PARSE_EOL_AFTER         = 0x00000002;
constexpr TokenParseFlags TOKEN_PARSE_BLANK_BEFORE      = 0x00000004;
constexpr TokenParseFlags TOKEN_PARSE_BLANK_AFTER       = 0x00000008;
constexpr TokenParseFlags TOKEN_PARSE_BLANK_LINE_BEFORE = 0x00000010;
constexpr TokenParseFlags TOKEN_PARSE_ONE_LINE_COMMENT  = 0x00000020;

struct TokenComment
{
    Utf8            comment;
    TokenParseFlags flags = 0;
};

struct TokenFormat
{
    static Utf8 toString(const Vector<TokenComment>& other)
    {
        Utf8 result;

        bool first = true;
        for (const auto& cmt : other)
        {
            if (!first)
                result += '\n';
            result += cmt.comment;
            first = false;
        }

        return result;
    }

    Vector<TokenComment> commentBefore;
    Vector<TokenComment> commentJustBefore;
    Vector<TokenComment> commentAfterSameLine;
    TokenParseFlags      flags = 0;
};

struct TokenParse
{
    bool is(TokenId what) const { return token.id == what; }
    bool isNot(TokenId what) const { return token.id != what; }

    Token           token;
    TokenFormat     comments;
    Register        literalValue;
    TokenParseFlags flags       = 0;
    LiteralType     literalType = static_cast<LiteralType>(0);
};
