#pragma once

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
    SyntaxRegister,
    SyntaxInvalid,
};

static constexpr auto SYN_CODE      = "SCde";
static constexpr auto SYN_COMMENT   = "SCmt";
static constexpr auto SYN_COMPILER  = "SCmp";
static constexpr auto SYN_FUNCTION  = "SFct";
static constexpr auto SYN_CONSTANT  = "SCst";
static constexpr auto SYN_INTRINSIC = "SItr";
static constexpr auto SYN_TYPE      = "STpe";
static constexpr auto SYN_KEYWORD   = "SKwd";
static constexpr auto SYN_LOGIC     = "SLgc";
static constexpr auto SYN_NUMBER    = "SNum";
static constexpr auto SYN_STRING    = "SStr";
static constexpr auto SYN_ATTRIBUTE = "SAtr";
static constexpr auto SYN_DEFAULT   = "SDft";
static constexpr auto SYN_INVALID   = "SInv";
static constexpr auto SYN_REGISTER  = "SBcR";

enum class SyntaxColorMode
{
    ForDoc,
    ForLog,
};

struct SyntaxColorContext
{
    SyntaxColorMode mode                  = SyntaxColorMode::ForLog;
    int             multiLineCommentLevel = 0;
    bool            forByteCode           = false;
};

Utf8     syntaxColorToVTS(SyntaxColor color, SyntaxColorMode mode = SyntaxColorMode::ForLog);
uint32_t getSyntaxColorRgb(SyntaxColorMode mode, SyntaxColor color, float lum);
Utf8     doSyntaxColor(const Utf8& line, SyntaxColorContext& context, bool force = false);
