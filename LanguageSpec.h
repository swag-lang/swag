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
    SizeOf,
	Count,
	Data,
};

struct LanguageSpec
{
    void   setup();
    string tokenToName(TokenId id);

    map<Utf8, TokenId>         keywords;
    map<Utf8, TypeInfoNative*> nativeTypes;
    map<Utf8, Property>        properties;
};

#define SWAG_IS_DIGIT(__c) (__c >= '0' && __c <= '9')
#define SWAG_IS_ALPHAHEX(__c) ((__c >= 'a' && __c <= 'f') || (__c >= 'A' && __c <= 'F'))
#define SWAG_IS_HEX(__c) (SWAG_IS_ALPHAHEX(__c) || SWAG_IS_DIGIT(__c))
#define SWAG_IS_ALPHA(__c) ((__c >= 'a' && __c <= 'z') || (__c >= 'A' && __c <= 'Z'))
#define SWAG_IS_NUMSEP(__c) (__c == '_')
#define SWAG_IS_BLANK(__c) (__c == ' ' || __c == '\t' || __c == '\v' || __c == '\f' || __c == '\r')
#define SWAG_IS_EOL(__c) (__c == '\n')

extern LanguageSpec g_LangSpec;