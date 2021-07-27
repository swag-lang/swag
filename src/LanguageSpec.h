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

    Utf8 name_self;
    Utf8 name_opInitGenerated;
    Utf8 name_opDropGenerated;
    Utf8 name_opRelocGenerated;
    Utf8 name_opPostCopyGenerated;
    Utf8 name_opPostMoveGenerated;
    Utf8 name_SystemAllocator;
};

extern LanguageSpec g_LangSpec;