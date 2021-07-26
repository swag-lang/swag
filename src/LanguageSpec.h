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

extern LanguageSpec g_LangSpec;