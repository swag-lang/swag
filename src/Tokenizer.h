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

    uint32_t    getChar();
    uint32_t    getCharNoSeek(unsigned& offset);
    uint32_t    getChar(unsigned& offset, bool seek, bool useCache = true);
    bool        eatCComment(Token& token);
    static void relaxIdentifier(Token& token);
    void        getIdentifier(Token& token, uint32_t c, unsigned offset);
    void        treatChar(uint32_t c, unsigned offset);
    bool        doNumberLiteral(uint32_t c, Token& token);
    bool        doHexLiteral(Token& token);
    bool        doBinLiteral(Token& token);
    bool        doIntFloatLiteral(uint32_t c, Token& token);
    bool        doIntLiteral(uint32_t c, Token& token);
    bool        doFloatLiteral(uint32_t c, Token& token);
    bool        error(Token& token, const Utf8& msg);
    bool        doSymbol(uint32_t c, Token& token);
    void        postProcessRawString(Utf8& text);
    bool        doStringLiteral(Token& token, bool raw);
    static bool isSymbol(TokenId id);

    SourceFile*    sourceFile         = nullptr;
    uint32_t       cacheChar[2]       = {0};
    unsigned       cacheCharOffset[2] = {0};
    SourceLocation location;
    bool           endReached          = false;
    bool           forceLastTokenIsEOL = false;
    bool           lastTokenIsEOL      = false;
    bool           lastTokenIsBlank    = false;
};