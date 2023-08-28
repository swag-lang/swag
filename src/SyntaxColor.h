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
    SyntaxInvalid,
};

static const char* SYN_CODE      = "SCde";
static const char* SYN_COMMENT   = "SCmt";
static const char* SYN_COMPILER  = "SCmp";
static const char* SYN_FUNCTION  = "SFct";
static const char* SYN_CONSTANT  = "SCst";
static const char* SYN_INTRINSIC = "SItr";
static const char* SYN_TYPE      = "STpe";
static const char* SYN_KEYWORD   = "SKwd";
static const char* SYN_LOGIC     = "SLgc";
static const char* SYN_NUMBER    = "SNum";
static const char* SYN_STRING    = "SStr";
static const char* SYN_ATTRIBUTE = "SAtr";
static const char* SYN_DEFAULT   = "SDft";
static const char* SYN_INVALID   = "SInv";

enum class SyntaxColorMode
{
    ForDoc,
    ForLog,
};

uint32_t getSyntaxColor(SyntaxColorMode mode, SyntaxColor color, float lum);
Utf8     syntaxColor(const Utf8& line, SyntaxColorMode mode);