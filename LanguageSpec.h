#pragma once
#include "Tokenizer.h"

class LanguageSpec
{
public:
    LanguageSpec();

public:
    map<wstring, TokenId> m_keywords;
};
