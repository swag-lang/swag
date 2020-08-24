#pragma once
#include "Register.h"
#include "SourceLocation.h"
struct SourceFile;
struct TypeInfo;
enum class Intrisic;

enum class TokenId
{
#define DEFINE_TOKEN_ID(__id) __id,
#include "TokenizerIds.h"
};

extern const char* g_TokenNames[];

enum class LiteralType
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
    TT_CHAR,
    TT_F32,
    TT_F64,
    TT_NULL,
    TT_ANY,
    TT_VOID,
    TT_UNTYPED_BINHEXA,
    TT_UNTYPED_INT,
    TT_UNTYPED_FLOAT,
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

static const uint32_t TOKENIZER_KEEP_EOL              = 0x00000001;
static const uint32_t TOKENIZER_KEEP_BLANKS           = 0x00000002;
static const uint32_t TOKENIZER_KEEP_CPP_COMMENTS     = 0x00000004;
static const uint32_t TOKENIZER_KEEP_KEYWORDS         = 0x00000008;
static const uint32_t TOKENIZER_NO_LITERAL_CONVERSION = 0x00000010;

struct Tokenizer
{
    void setFile(SourceFile* file);
    bool getToken(Token& token);

    char32_t getChar();
    char32_t getCharNoSeek(unsigned& offset);
    char32_t getChar(unsigned& offset, bool seek, bool useCache = true);
    bool     eatCComment(Token& token);
    void     getIdentifier(Token& token, char32_t c, unsigned offset);
    void     treatChar(char32_t c, unsigned offset);
    bool     doNumberLiteral(char32_t c, Token& token);
    bool     doHexLiteral(Token& token);
    bool     doBinLiteral(Token& token);
    bool     doNumberSuffix(Token& token);
    bool     doIntFloatLiteral(char32_t c, Token& token);
    bool     doIntLiteral(char32_t c, Token& token);
    bool     doFloatLiteral(char32_t c, Token& token);
    bool     error(Token& token, const Utf8& msg);
    bool     errorNumberSyntax(Token& token, const Utf8& msg);
    bool     doSymbol(char32_t c, Token& token);
    void     postProcessRawString(Utf8& text);
    bool     doStringLiteral(Token& token, bool raw);
    bool     isEscape(char32_t& c, Token& token);
    bool     getDigitHexa(Token& token, int& result);
    bool     doCharLiteral(Token& token);
    void     doDocComment(Token& token);

    SourceFile*    sourceFile         = nullptr;
    char32_t       cacheChar[2]       = {0};
    unsigned       cacheCharOffset[2] = {0};
    uint32_t       parseFlags         = 0;
    SourceLocation location;
    bool           endReached          = false;
    bool           forceLastTokenIsEOL = false;
    bool           lastTokenIsEOL      = false;
};