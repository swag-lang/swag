#pragma once
#include "Concat.h"

struct FormatConcat : Concat
{
    Utf8 getUtf8() const;

    void addBlank(bool test = true);
    void addIndent(uint32_t num);
    void addBlankLine();
    void addEol();
    void addU32Str(uint32_t value);
    void addString(const char* v, uint32_t len);
    void addString(const Utf8& v);
    void addStringFormat(const char* format, ...);
    void addChar(char c);

    uint32_t eol   = 0;
    uint32_t blank = 0;
};

#define CONCAT_FIXED_STR(__concat, __str)                                         \
    do                                                                            \
    {                                                                             \
        static constexpr int __len = (int) std::char_traits<char>::length(__str); \
        (__concat)->addString(__str, __len);                                      \
    } while (0)

#define CONCAT_STR_1(__concat, __before, __int, __after) \
    do                                                   \
    {                                                    \
        CONCAT_FIXED_STR(__concat, __before);            \
        (__concat)->addU32Str(__int);                    \
        CONCAT_FIXED_STR(__concat, __after);             \
    } while (0)

#define CONCAT_STR_2(__concat, __before, __int1, __middle, __int2, __after) \
    do                                                                      \
    {                                                                       \
        CONCAT_FIXED_STR(__concat, __before);                               \
        (__concat)->addU32Str(__int1);                                      \
        CONCAT_FIXED_STR(__concat, __middle);                               \
        (__concat)->addU32Str(__int2);                                      \
        CONCAT_FIXED_STR(__concat, __after);                                \
    } while (0)
