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
    const char* c_str() const { return text.c_str(); }

    bool is(TokenId what) const { return id == what; }
    bool isNot(TokenId what) const { return id != what; }
    bool is(const Utf8& name) const { return text == name; }
    bool isNot(const Utf8& name) const { return text != name; }

    SourceFile*    sourceFile = nullptr;
    Utf8           text;
    SourceLocation startLocation;
    SourceLocation endLocation;
    TokenId        id = TokenId::Invalid;
};
#pragma pack(pop)
