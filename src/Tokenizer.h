#pragma once
#include "Register.h"
#include "Utf8.h"
struct SourceFile;
struct TypeInfo;
struct ErrorContext;
enum class Intrisic;

enum class TokenId : uint16_t
{
#define DEFINE_TOKEN_ID(__id, __flags) __id,
#include "TokenIds.h"
};

extern const char*    g_TokenNames[];
extern const uint32_t g_TokenFlags[];

const uint32_t TOKEN_SYM                = 0x00000001;
const uint32_t TOKEN_INTRINSIC_NORETURN = 0x00000002;
const uint32_t TOKEN_INTRINSIC_RETURN   = 0x00000004;
const uint32_t TOKEN_KWD                = 0x00000008;
const uint32_t TOKEN_COMPILER           = 0x00000010;
const uint32_t TOKEN_LITERAL            = 0x00000020;
const uint32_t TOKEN_COMPILER_FUNC      = 0x00000040;

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
    Utf8           text;
    SourceLocation startLocation;
    SourceLocation endLocation;

    const char* ctext()
    {
        return text.c_str();
    }
};

const uint8_t TOKENPARSE_LAST_EOL           = 0x01;
const uint8_t TOKENPARSE_LAST_BLANK         = 0x02;
const uint8_t TOKENPARSE_EOL_BEFORE_COMMENT = 0x04;

struct TokenParse : public Token
{
    Register    literalValue;
    TokenId     id          = TokenId::Invalid;
    LiteralType literalType = (LiteralType) 0;
    uint8_t     flags       = 0;
};

struct Tokenizer
{
    bool error(TokenParse& token, const Utf8& msg, const Utf8& hint = "");
    void trimMultilineString(Utf8& text);
    void appendTokenName(TokenParse& token);

    uint32_t readChar();
    uint32_t peekChar(unsigned& offset);
    void     processChar(uint32_t c);
    void     eatChar(uint32_t c, unsigned offset);

    bool doMultiLineComment(TokenParse& token);
    void doIdentifier(TokenParse& token);
    bool doNumberLiteral(TokenParse& token, uint32_t c);
    bool doHexLiteral(TokenParse& token);
    bool doBinLiteral(TokenParse& token);
    bool doIntFloatLiteral(TokenParse& token, uint32_t c);
    bool doIntLiteral(TokenParse& token, uint32_t c);
    bool doFloatLiteral(TokenParse& token, uint32_t c);
    bool doSymbol(TokenParse& token, uint32_t c);
    bool doStringLiteral(TokenParse& token, bool raw, bool multiline);

    void saveState(const TokenParse& token);
    void restoreState(TokenParse& token);

    void setup(ErrorContext* errorCxt, SourceFile* file);
    bool nextToken(TokenParse& token);

    static Utf8 tokenToName(TokenId id);

    // clang-format off
    static bool isSymbol(TokenId id)            { return g_TokenFlags[(int) id] & TOKEN_SYM; }
    static bool isLiteral(TokenId id)           { return g_TokenFlags[(int) id] & TOKEN_LITERAL; }
    static bool isIntrinsicReturn(TokenId id)   { return g_TokenFlags[(int) id] & TOKEN_INTRINSIC_RETURN; }
    static bool isIntrinsicNoReturn(TokenId id) { return g_TokenFlags[(int) id] & TOKEN_INTRINSIC_NORETURN; }
    // clang-format on

    SourceLocation location;

    Utf8          comment;
    ErrorContext* errorContext        = nullptr;
    char*         curBuffer           = nullptr;
    char*         endBuffer           = nullptr;
    SourceFile*   sourceFile          = nullptr;
    char*         startTokenName      = nullptr;
    bool          forceLastTokenIsEOL = false;
    bool          realAppendName      = false;
    bool          trackComments       = false;
    bool          propagateComment    = false;
    bool          idLetters[256]      = {false};

    TokenParse     st_token;
    char*          st_curBuffer = nullptr;
    SourceLocation st_location;
    bool           st_forceLastTokenIsEOL = false;
};