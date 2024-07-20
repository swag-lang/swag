#pragma once
struct SourceFile;

enum class TokenId : uint16_t
{
#define DEFINE_TOKEN_ID(__id, __flags) __id,
#include "TokenIds.h"
};

struct SourceLocation
{
    bool operator==(const SourceLocation& other) const { return line == other.line && column == other.column; }
    bool operator!=(const SourceLocation& other) const { return line != other.line || column != other.column; }

    uint32_t line   = 0;
    uint32_t column = 0;
};

#pragma pack(push, 2)
struct Token
{
    [[nodiscard]] const char* c_str() const { return text.c_str(); }

    [[nodiscard]] bool is(TokenId what) const { return id == what; }
    [[nodiscard]] bool isNot(TokenId what) const { return id != what; }

    SourceFile*    sourceFile = nullptr;
    Utf8           text;
    SourceLocation startLocation;
    SourceLocation endLocation;
    TokenId        id = TokenId::Invalid;
};
#pragma pack(pop)
