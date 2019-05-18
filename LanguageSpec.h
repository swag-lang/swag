#pragma once
#include "Tokenizer.h"

class LanguageSpec
{
public:
    LanguageSpec();

public:
    map<wstring, TokenId>      m_keywords;
    map<wstring, TokenNumType> m_nativeTypes;
};

#define SWAG_IS_DIGIT(__c) (__c >= '0' && __c <= '9')
#define SWAG_IS_HEX(__c) ((__c >= 'a' && __c <= 'f') || (__c >= 'A' && __c <= 'F'))
#define SWAG_IS_ALPHA(__c) ((__c >= 'a' && __c <= 'z') || (__c >= 'A' && __c <= 'Z'))
#define SWAG_IS_NUMSEP(__c) (__c == '_')