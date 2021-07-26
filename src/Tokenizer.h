#pragma once
#include "Register.h"
#include "SourceLocation.h"
struct SourceFile;
struct TypeInfo;
enum class Intrisic;

enum class TokenId : uint16_t
{
#define DEFINE_TOKEN_ID(__id, __flags) __id,
#include "TokenIds.h"
};

extern const char* g_TokenNames[];

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
    TT_UNTYPED_BINHEXA,
    TT_UNTYPED_INT,
    TT_UNTYPED_FLOAT,
    TT_UINT,
    TT_INT,
    TT_MAX,
};

struct Token
{
    Utf8           text;
    SourceLocation startLocation;
    SourceLocation endLocation;
    Register       literalValue;
    LiteralType    literalType;
    TokenId        id = TokenId::Invalid;
};

struct Tokenizer
{
    void setFile(SourceFile* file);
    bool getToken(Token& token);
    bool error(Token& token, const Utf8& msg);
    void postProcessRawString(Utf8& text);
    void setTokenName(Token& token);

    uint32_t getChar();
    uint32_t getCharNoSeek(unsigned& offset);
    void     processChar(uint32_t c);
    void     treatChar(uint32_t c, unsigned offset);

    bool doMultiLineComment(Token& token);
    void doIdentifier(Token& token, uint32_t c, unsigned offset);
    bool doNumberLiteral(uint32_t c, Token& token);
    bool doHexLiteral(Token& token);
    bool doBinLiteral(Token& token);
    bool doIntFloatLiteral(uint32_t c, Token& token);
    bool doIntLiteral(uint32_t c, Token& token);
    bool doFloatLiteral(uint32_t c, Token& token);
    bool doSymbol(uint32_t c, Token& token);
    bool doStringLiteral(Token& token, bool raw);

    static void relaxIdentifier(Token& token);
    static bool isSymbol(TokenId id);
    static bool isLiteral(TokenId id);

    SourceLocation location;
    SourceFile*    sourceFile          = nullptr;
    char*          startTokenName      = nullptr;
    bool           forceLastTokenIsEOL = false;
    bool           lastTokenIsEOL      = false;
    bool           lastTokenIsBlank    = false;
};