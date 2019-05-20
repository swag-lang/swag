#pragma once
#include "Tokenizer.h"
#include "Utf8.h"

struct LanguageSpec
{
    map<utf8, TokenId>      m_keywords;
    map<utf8, TokenNumType> m_nativeTypes;

    LanguageSpec();
	string tokenToName(TokenId id);
};

#define SWAG_IS_DIGIT(__c) (__c >= '0' && __c <= '9')
#define SWAG_IS_ALPHAHEX(__c) ((__c >= 'a' && __c <= 'f') || (__c >= 'A' && __c <= 'F'))
#define SWAG_IS_HEX(__c) (SWAG_IS_ALPHAHEX(__c) || SWAG_IS_DIGIT(__c))
#define SWAG_IS_ALPHA(__c) ((__c >= 'a' && __c <= 'z') || (__c >= 'A' && __c <= 'Z'))
#define SWAG_IS_NUMSEP(__c) (__c == '_')