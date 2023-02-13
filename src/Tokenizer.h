#pragma once
#include "Register.h"
#include "Utf8.h"
struct SourceFile;
struct TypeInfo;
enum class Intrisic;

enum class TokenId : uint16_t
{
#define DEFINE_TOKEN_ID(__id, __flags) __id,
#include "TokenIds.h"
};

extern const char*    g_TokenNames[];
extern const uint32_t g_TokenFlags[];

static const uint32_t TOKEN_SYM                = 0x00000001;
static const uint32_t TOKEN_INTRINSIC_NORETURN = 0x00000002;
static const uint32_t TOKEN_INTRINSIC_RETURN   = 0x00000004;
static const uint32_t TOKEN_KWD                = 0x00000008;
static const uint32_t TOKEN_COMPILER           = 0x00000010;
static const uint32_t TOKEN_LITERAL            = 0x00000020;
static const uint32_t TOKEN_COMPILER_FUNC      = 0x00000040;

enum class LiteralType : uint8_t
{
    TT_U8,
    TT_U16,
    TT_U32,
    TT_U64,
    TT_S8,
    TT_S16,
    TT_S32,
    TT_S64,
    TT_BOOL,
    TT_STRING,
    TT_RAW_STRING,
    TT_ESCAPE_STRING,
    TT_RUNE,
    TT_F32,
    TT_F64,
    TT_NULL,
    TT_ANY,
    TT_VOID,
    TT_TYPE,
    TT_CSTRING,
    TT_UNTYPED_BINHEXA,
    TT_UNTYPED_INT,
    TT_UNTYPED_FLOAT,
    TT_MAX,
};

struct SourceLocation
{
    uint32_t line   = 0;
    uint32_t column = 0;

    bool operator==(const SourceLocation& other)
    {
        return line == other.line && column == other.column;
    }
};

struct Token
{
    TokenId id = TokenId::Invalid;

    Utf8           text;
    SourceLocation startLocation;
    SourceLocation endLocation;

    const char* ctext()
    {
        return text.c_str();
    }
};

struct TokenParse : public Token
{
    Register    literalValue;
    LiteralType literalType      = (LiteralType) 0;
    bool        lastTokenIsEOL   = false;
    bool        lastTokenIsBlank = false;
};

struct Tokenizer
{
    void setFile(SourceFile* file);
    bool getToken(TokenParse& token);
    bool error(TokenParse& token, const Utf8& msg, const Utf8& hint = "");
    void trimMultilineString(Utf8& text);
    void appendTokenName(TokenParse& token);
    void saveState(const TokenParse& token);
    void restoreState(TokenParse& token);

    uint32_t getChar();
    uint32_t getCharNoSeek(unsigned& offset);
    void     processChar(uint32_t c);
    void     treatChar(uint32_t c, unsigned offset);

    bool doMultiLineComment(TokenParse& token);
    void doIdentifier(TokenParse& token, uint32_t c, unsigned offset);
    bool doNumberLiteral(uint32_t c, TokenParse& token);
    bool doHexLiteral(TokenParse& token);
    bool doBinLiteral(TokenParse& token);
    bool doIntFloatLiteral(uint32_t c, TokenParse& token);
    bool doIntLiteral(uint32_t c, TokenParse& token);
    bool doFloatLiteral(uint32_t c, TokenParse& token);
    bool doSymbol(uint32_t c, TokenParse& token);
    bool doStringLiteral(TokenParse& token, bool raw, bool multiline);

    // clang-format off
    static bool isSymbol(TokenId id)            { return g_TokenFlags[(int) id] & TOKEN_SYM; }
    static bool isLiteral(TokenId id)           { return g_TokenFlags[(int) id] & TOKEN_LITERAL; }
    static bool isIntrinsicReturn(TokenId id)   { return g_TokenFlags[(int) id] & TOKEN_INTRINSIC_RETURN; }
    static bool isIntrinsicNoReturn(TokenId id) { return g_TokenFlags[(int) id] & TOKEN_INTRINSIC_NORETURN; }
    // clang-format on

    SourceLocation location;
    SourceFile*    sourceFile          = nullptr;
    char*          startTokenName      = nullptr;
    bool           forceLastTokenIsEOL = false;
    bool           realAppendName      = false;

    TokenParse     st_token;
    char*          st_curBuffer = nullptr;
    SourceLocation st_location;
    bool           st_forceLastTokenIsEOL = false;
};