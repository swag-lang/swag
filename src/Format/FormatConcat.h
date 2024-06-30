#pragma once
#include "Core/Concat.h"

struct FormatConcat : Concat
{
    void     clear();
    Utf8     getUtf8() const;
    uint32_t length() const;

    void addBlank();
    void alignToColumn(uint32_t destCol);
    void addIndent(uint32_t num);
    void addBlankLine();
    void addEol();
    void addU32Str(uint32_t value);
    void addString(const std::string_view& view);
    void addString(const char* v, uint32_t len);
    void addStringFormat(const char* format, ...);
    void addChar(char c);
    bool removeLastChar(char c);

    uint32_t totalEol = 0;
    uint32_t eol      = 1;
    uint32_t blank    = 0;
    uint32_t column   = 0;
    uint32_t indent   = 0;
    uint32_t noEol    = 0;
};
