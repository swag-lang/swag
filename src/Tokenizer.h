#pragma once
#include "Flags.h"
#include "Register.h"

struct SourceFile;
struct TypeInfo;
struct ErrorContext;

enum class TokenId : uint16_t
{
#define DEFINE_TOKEN_ID(__id, __flags) __id,
#include "TokenIds.h"
};

using TokenFlags                              = Flags<uint32_t>;
constexpr TokenFlags TOKEN_SYM                = 0x00000001;
constexpr TokenFlags TOKEN_INTRINSIC_NORETURN = 0x00000002;
constexpr TokenFlags TOKEN_INTRINSIC_RETURN   = 0x00000004;
constexpr TokenFlags TOKEN_KWD                = 0x00000008;
constexpr TokenFlags TOKEN_COMPILER           = 0x00000010;
constexpr TokenFlags TOKEN_LITERAL            = 0x00000020;
constexpr TokenFlags TOKEN_COMPILER_FUNC      = 0x00000040;
constexpr TokenFlags TOKEN_TOP_LEVEL_INST     = 0x00000080;

extern const char*      g_TokenNames[];
extern const TokenFlags g_TokenFlags[];

enum class LiteralType : uint8_t
{
    TypeUnsigned8,
    TypeUnsigned16,
    TypeUnsigned32,
    TypeUnsigned64,
    TypeSigned8,
    TypeSigned16,
    TypeSigned32,
    TypeSigned64,
    TypeBool,
    TypeString,
    TypeStringRaw,
    TypeStringMultiLine,
    TypeStringEscape,
    TypeStringMultiLineEscape,
    TypeCharacter,
    TypeCharacterEscape,
    TypeRune,
    TypeFloat32,
    TypeFloat64,
    TypeNull,
    TypeAny,
    TypeVoid,
    TypeType,
    TypeCString,
    TypeUntypedBinHexa,
    TypeUntypedInt,
    TypeUntypedFloat,
    TypeMax,
};

struct SourceLocation
{
    bool operator==(const SourceLocation& other) const { return line == other.line && column == other.column; }
    bool operator!=(const SourceLocation& other) const { return line != other.line || column != other.column; }

    uint32_t line   = 0;
    uint32_t column = 0;
};

struct Token
{
    const char* c_str() const { return text.c_str(); }

    Utf8           text;
    SourceLocation startLocation;
    SourceLocation endLocation;
};

using TokenParseFlags                                    = Flags<uint8_t>;
constexpr TokenParseFlags TOKEN_PARSE_LAST_EOL           = 0x01;
constexpr TokenParseFlags TOKEN_PARSE_LAST_BLANK         = 0x02;
constexpr TokenParseFlags TOKEN_PARSE_EOL_BEFORE_COMMENT = 0x04;

struct TokenParse : Token
{
    Register        literalValue;
    TokenId         id          = TokenId::Invalid;
    LiteralType     literalType = static_cast<LiteralType>(0);
    TokenParseFlags flags       = 0;
};

struct Tokenizer
{
    bool error(TokenParse& token, const Utf8& msg, const Utf8& hint = "") const;
    void appendTokenName(TokenParse& token) const;

    uint32_t readChar();
    uint32_t peekChar(unsigned& offset) const;
    void     processChar(uint32_t c, unsigned offset);
    void     eatChar(uint32_t c, unsigned offset);

    bool doMultiLineComment(TokenParse& token);
    bool doIdentifier(TokenParse& token);
    bool doNumberLiteral(TokenParse& token, uint32_t c);
    bool doHexLiteral(TokenParse& token);
    bool doBinLiteral(TokenParse& token);
    bool doIntFloatLiteral(TokenParse& token, uint32_t c);
    bool doIntLiteral(TokenParse& token, uint32_t c);
    bool doFloatLiteral(TokenParse& token, uint32_t c);
    bool doSymbol(TokenParse& token, uint32_t c);
    bool doCharacterLiteral(TokenParse& token);
    bool doStringLiteral(TokenParse& token);

    void saveState(const TokenParse& token);
    void restoreState(TokenParse& token);

    void setup(ErrorContext* errorCxt, SourceFile* file);
    bool nextToken(TokenParse& token);

    static TokenId tokenRelated(TokenId id);

    static bool isKeyword(TokenId id) { return g_TokenFlags[static_cast<int>(id)].has(TOKEN_KWD); }
    static bool isSymbol(TokenId id) { return g_TokenFlags[static_cast<int>(id)].has(TOKEN_SYM); }
    static bool isLiteral(TokenId id) { return g_TokenFlags[static_cast<int>(id)].has(TOKEN_LITERAL); }
    static bool isCompiler(TokenId id) { return g_TokenFlags[static_cast<int>(id)].has(TOKEN_COMPILER); }
    static bool isIntrinsicReturn(TokenId id) { return g_TokenFlags[static_cast<int>(id)].has(TOKEN_INTRINSIC_RETURN); }
    static bool isIntrinsicNoReturn(TokenId id) { return g_TokenFlags[static_cast<int>(id)].has(TOKEN_INTRINSIC_NORETURN); }
    static bool isTopLevelInst(TokenId id) { return g_TokenFlags[static_cast<int>(id)].has(TOKEN_TOP_LEVEL_INST); }

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

    Utf8           savedComment;
    TokenParse     savedToken;
    SourceLocation savedLocation;
    char*          savedCurBuffer           = nullptr;
    bool           savedForceLastTokenIsEOL = false;
};
