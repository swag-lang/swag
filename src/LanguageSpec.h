#pragma once
#pragma once
#include "TypeInfo.h"
#include "Tokenizer.h"
#include "Utf8.h"
#include "HashTable.h"
struct TypeInfoNative;

struct LanguageSpec
{
    void   setup();
    string tokenToName(TokenId id);

    HashTable<TokenId, 4096>    keywords;
    HashTable<LiteralType, 256> nativeTypes;
};

#define SWAG_IS_DIGIT(__c) (__c >= '0' && __c <= '9')
#define SWAG_IS_ALPHAHEX(__c) ((__c >= 'a' && __c <= 'f') || (__c >= 'A' && __c <= 'F'))
#define SWAG_IS_HEX(__c) (SWAG_IS_ALPHAHEX(__c) || SWAG_IS_DIGIT(__c))
#define SWAG_IS_ALPHA(__c) ((__c >= 'a' && __c <= 'z') || (__c >= 'A' && __c <= 'Z'))
#define SWAG_IS_NUMSEP(__c) (__c == '_')
#define SWAG_IS_BLANK(__c) (__c == ' ' || __c == '\t' || __c == '\v' || __c == '\f' || __c == '\r')
#define SWAG_IS_EOL(__c) (__c == '\n')

extern LanguageSpec g_LangSpec;