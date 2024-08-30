#pragma once
#include "Syntax/Tokenizer/Token.h"
#include "Syntax/Tokenizer/TokenParse.h"

struct SourceFile;
struct TypeInfo;
struct ErrorContext;

using TokenFlags     = Flags<uint32_t>;
using TokenizerFlags = Flags<uint32_t>;

constexpr TokenFlags TOKEN_SYM                = 0x00000001;
constexpr TokenFlags TOKEN_INTRINSIC_NORETURN = 0x00000002;
constexpr TokenFlags TOKEN_INTRINSIC_RETURN   = 0x00000004;
constexpr TokenFlags TOKEN_KWD                = 0x00000008;
constexpr TokenFlags TOKEN_COMPILER           = 0x00000010;
constexpr TokenFlags TOKEN_LITERAL            = 0x00000020;
constexpr TokenFlags TOKEN_COMPILER_FUNC      = 0x00000040;
constexpr TokenFlags TOKEN_TOP_LEVEL_INST     = 0x00000080;
constexpr TokenFlags TOKEN_MODIFIER           = 0x00000100;

constexpr TokenizerFlags TOKENIZER_TRACK_COMMENTS = 0x00000001;
constexpr TokenizerFlags TOKENIZER_TRACK_FORMAT   = 0x00000002;

constexpr const char* TOKEN_NAMES[] =
{
#undef DEFINE_TOKEN_ID
#define DEFINE_TOKEN_ID(__id, __flags) #__id,
#include "TokenIds.h"

};

constexpr TokenFlags TOKEN_FLAGS[] =
{
#undef DEFINE_TOKEN_ID
#define DEFINE_TOKEN_ID(__id, __flags) __flags,
#include "TokenIds.h"

};

struct TokenizerState
{
    TokenParse     token;
    SourceLocation location;
    char*          buffer = nullptr;
};

struct Tokenizer
{
    bool error(TokenParse& tokenParse, const Utf8& msg, const Utf8& hint = "") const;
    void appendTokenName(TokenParse& tokenParse) const;

    uint32_t readChar();
    uint32_t peekChar(unsigned& offset) const;
    void     processChar(uint32_t c, unsigned offset);
    void     eatChar(uint32_t c, unsigned offset);

    bool doSingleLineComment(TokenParse& tokenParse);
    bool doMultiLineComment(TokenParse& tokenParse);
    bool doIdentifier(TokenParse& tokenParse);
    bool doNumberLiteral(TokenParse& tokenParse, uint32_t c);
    bool doHexLiteral(TokenParse& tokenParse);
    bool doBinLiteral(TokenParse& tokenParse);
    bool doIntFloatLiteral(TokenParse& tokenParse, uint32_t c);
    bool doIntLiteral(TokenParse& token, uint32_t c);
    bool doFloatLiteral(TokenParse& token, uint32_t c);
    bool doSymbol(TokenParse& tokenParse, uint32_t c);
    bool doCharacterLiteral(TokenParse& tokenParse);
    bool doStringLiteral(TokenParse& tokenParse);

    void saveState(const TokenParse& token);
    void restoreState(TokenParse& token);

    void trimMultilineString(Utf8& text) const;
    void setup(ErrorContext* errorCxt, SourceFile* file);
    bool doAfterToken(TokenParse& tokenParse);
    bool nextToken(TokenParse& tokenParse);

    static TokenId tokenRelated(TokenId id);

    static bool isKeyword(TokenId id) { return TOKEN_FLAGS[static_cast<int>(id)].has(TOKEN_KWD); }
    static bool isSymbol(TokenId id) { return TOKEN_FLAGS[static_cast<int>(id)].has(TOKEN_SYM); }
    static bool isLiteral(TokenId id) { return TOKEN_FLAGS[static_cast<int>(id)].has(TOKEN_LITERAL); }
    static bool isCompiler(TokenId id) { return TOKEN_FLAGS[static_cast<int>(id)].has(TOKEN_COMPILER); }
    static bool isModifier(TokenId id) { return TOKEN_FLAGS[static_cast<int>(id)].has(TOKEN_MODIFIER); }
    static bool isIntrinsicReturn(TokenId id) { return TOKEN_FLAGS[static_cast<int>(id)].has(TOKEN_INTRINSIC_RETURN); }
    static bool isIntrinsicNoReturn(TokenId id) { return TOKEN_FLAGS[static_cast<int>(id)].has(TOKEN_INTRINSIC_NORETURN); }
    static bool isTopLevelInst(TokenId id) { return TOKEN_FLAGS[static_cast<int>(id)].has(TOKEN_TOP_LEVEL_INST); }
    static bool isStartOfNewStatement(const TokenParse& token);
    static bool isJustAfterPrevious(const TokenParse& token);

    SourceLocation location;

    SourceLocation lastOpenLeftCurly;

    ErrorContext*  errorContext   = nullptr;
    char*          curBuffer      = nullptr;
    char*          endBuffer      = nullptr;
    SourceFile*    sourceFile     = nullptr;
    char*          startTokenName = nullptr;
    TokenizerFlags tokenizeFlags  = 0;
    bool           realAppendName = false;
    bool           idLetters[256] = {false};

    Vector<TokenizerState> savedState;
};
