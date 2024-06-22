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
    SyntaxBcRegister,
    SyntaxBcKeyword,
    SyntaxBcConstant,
    SyntaxInvalid,
};

static auto SYN_CODE        = "SCde";
static auto SYN_COMMENT     = "SCmt";
static auto SYN_COMPILER    = "SCmp";
static auto SYN_FUNCTION    = "SFct";
static auto SYN_CONSTANT    = "SCst";
static auto SYN_INTRINSIC   = "SItr";
static auto SYN_TYPE        = "STpe";
static auto SYN_KEYWORD     = "SKwd";
static auto SYN_LOGIC       = "SLgc";
static auto SYN_NUMBER      = "SNum";
static auto SYN_STRING      = "SStr";
static auto SYN_ATTRIBUTE   = "SAtr";
static auto SYN_DEFAULT     = "SDft";
static auto SYN_INVALID     = "SInv";
static auto SYN_BC_REGISTER = "SBcR";
static auto SYN_BC_KEYWORD  = "SBcK";
static auto SYN_BC_CONSTANT = "SBcC";

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

uint32_t getSyntaxColor(SyntaxColorMode mode, SyntaxColor color, float lum);
Utf8     syntaxColor(const Utf8& line, SyntaxColorContext& context);
