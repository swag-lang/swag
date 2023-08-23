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

static const char* SYN_CODE      = "SyntaxCode";
static const char* SYN_COMMENT   = "SyntaxComment";
static const char* SYN_COMPILER  = "SyntaxCompiler";
static const char* SYN_FUNCTION  = "SyntaxFunction";
static const char* SYN_CONSTANT  = "SyntaxConstant";
static const char* SYN_INTRINSIC = "SyntaxIntrinsic";
static const char* SYN_TYPE      = "SyntaxType";
static const char* SYN_KEYWORD   = "SyntaxKeyword";
static const char* SYN_LOGIC     = "SyntaxLogic";
static const char* SYN_NUMBER    = "SyntaxNumber";
static const char* SYN_STRING    = "SyntaxString";
static const char* SYN_ATTRIBUTE = "SyntaxAttribute";
static const char* SYN_DEFAULT   = "SyntaxDefault";

enum class SyntaxColorMode
{
    ForDoc,
    ForLog,
};

uint32_t getSyntaxColor(SyntaxColor color, float lum);
Utf8     syntaxColor(const Utf8& line, SyntaxColorMode mode);