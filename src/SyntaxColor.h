#pragma once
#include "Log.h"
#include "Utf8.h"

enum class SyntaxColor
{
    SyntaxCode,
    SyntaxComment,
    SyntaxCompiler,
    SyntaxFunction,
    SyntaxConstant,
    SyntaxIntrinsic,
    SyntaxType,
    SyntaxKeyword,
    SyntaxLogic,
    SyntaxNumber,
    SyntaxString,
    SyntaxAttribute,
    SyntaxDefault,
};

enum class SyntaxColorMode
{
    ForDoc,
    ForLog,
};

uint32_t getSyntaxColor(SyntaxColor color, float lum);
Utf8     syntaxColor(const Utf8& line, SyntaxColorMode mode);