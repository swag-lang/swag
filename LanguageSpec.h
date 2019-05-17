#pragma once
#include "Tokenizer.h"

class LanguageSpec
{
public:
    LanguageSpec();

public:
    map<string, TokenId> m_keywords;
};
