#pragma once
#pragma once
#include "TypeInfo.h"
#include "Tokenizer.h"
#include "Utf8.h"
struct TypeInfoNative;

enum class Intrisic
{
    None = 0,
    Print,
    Assert,
    Prop,
};

enum class Property
{
    Invalid,
    SizeOf,
    TypeOf,
    CountOf,
    DataOf,
    KindOf,
};

enum class Intrinsic
{
    IntrinsicPrint,
    IntrinsicAssert,
    IntrinsicProp,
    IntrinsicAlloc,
    IntrinsicRealloc,
    IntrinsicFree,
    IntrinsicMemCpy,
    IntrinsicMemSet,
    IntrinsicMemCmp,
    IntrinsicGetContext,
    IntrinsicSetContext,
    IntrinsicArguments,
    IntrinsicTarget,
    IntrinsicIsByteCode,
};

struct LanguageSpec
{
    void   setup();
    string tokenToName(TokenId id);

    unordered_map<Utf8Crc, TokenId, Utf8CrcKeyHash, Utf8CrcKeyEqual>         keywords;
    unordered_map<Utf8Crc, TypeInfoNative*, Utf8CrcKeyHash, Utf8CrcKeyEqual> nativeTypes;
    unordered_map<Utf8Crc, Property, Utf8CrcKeyHash, Utf8CrcKeyEqual>        properties;
    unordered_map<Utf8Crc, Intrinsic, Utf8CrcKeyHash, Utf8CrcKeyEqual>       intrinsics;
};

#define SWAG_IS_DIGIT(__c) (__c >= '0' && __c <= '9')
#define SWAG_IS_ALPHAHEX(__c) ((__c >= 'a' && __c <= 'f') || (__c >= 'A' && __c <= 'F'))
#define SWAG_IS_HEX(__c) (SWAG_IS_ALPHAHEX(__c) || SWAG_IS_DIGIT(__c))
#define SWAG_IS_ALPHA(__c) ((__c >= 'a' && __c <= 'z') || (__c >= 'A' && __c <= 'Z'))
#define SWAG_IS_NUMSEP(__c) (__c == '_')
#define SWAG_IS_BLANK(__c) (__c == ' ' || __c == '\t' || __c == '\v' || __c == '\f' || __c == '\r')
#define SWAG_IS_EOL(__c) (__c == '\n')

extern LanguageSpec g_LangSpec;