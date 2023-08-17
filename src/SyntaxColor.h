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
};

enum class SyntaxColorMode
{
    ForDoc,
    ForLog,
};

Utf8 syntaxColor(const Utf8& line, SyntaxColorMode mode);