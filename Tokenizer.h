#pragma once
#include "Register.h"
#include "SourceLocation.h"
struct SourceFile;
struct TypeInfo;
enum class Intrisic;

enum class TokenId
{
    SymDot,
    SymDotDot,
    SymDotDotDot,
    SymLeftParen,
    SymRightParen,
    SymLeftSquare,
    SymRightSquare,
    SymLeftCurly,
    SymRightCurly,
    SymColon,
    SymColonEqual,
    SymSemiColon,
    SymQuestion,
    SymQuestionQuestion,
    SymAt,
    SymEqual,
    SymEqualEqual,
    SymEqualGreater,
    SymMinus,
    SymMinusEqual,
    SymMinusGreat,
    SymPlus,
    SymPlusEqual,
    SymAsterisk,
    SymAsteriskEqual,
    SymSlash,
    SymSlashEqual,
    SymLower,
    SymLowerEqual,
    SymLowerLower,
    SymLowerLowerEqual,
    SymGreater,
    SymGreaterEqual,
    SymGreaterGreater,
    SymGreaterGreaterEqual,
    SymAmpersand,
    SymAmpersandEqual,
    SymAmpersandAmpersand,
    SymVertical,
    SymVerticalEqual,
    SymVerticalVertical,
    SymDollar,
    SymTilde,
    SymTildeEqual,
    SymCircumflex,
    SymCircumflexEqual,
    SymPercent,
    SymPercentEqual,
    SymComma,
    SymExclam,
    SymExclamEqual,
    SymBackSlash,
    SymAttrStart,
    SymBackTick,
    SymQuote,
    SymLiteralBracket,
    SymLiteralCurly,
    SymLiteralParen,

    CompilerSkip,
    CompilerModule,
    CompilerUnitTest,
    CompilerAssert,
    CompilerPrint,
    CompilerRun,
    CompilerAst,
    CompilerFile,
    CompilerLine,
    CompilerBuildCfg,
    CompilerTarget,
    CompilerFunction,
    CompilerCallerLine,
    CompilerCallerFile,
    CompilerCallerFunction,
    CompilerBuildVersion,
    CompilerBuildRevision,
    CompilerBuildNum,
    CompilerImport,
    CompilerFuncTest,
    CompilerFuncInit,
    CompilerFuncDrop,
    CompilerFuncMain,
    CompilerIf,
    CompilerElse,
    CompilerElseIf,
    CompilerCode,
    CompilerMixin,
    CompilerInline,
    CompilerMacro,
    CompilerForeignLib,

    DocComment,

    KwdVar,
    KwdLet,
    KwdConst,
    KwdPublic,
    KwdPrivate,
    KwdInternal,
    KwdReadOnly,
    KwdReadWrite,
    KwdNamespace,
    KwdAlias,
    KwdEnum,
    KwdEnumValue,
    KwdImpl,
    KwdStruct,
    KwdInterface,
    KwdUnion,
    KwdFunc,
    KwdAttr,
    KwdTrue,
    KwdFalse,
    KwdNull,
    KwdReturn,
    KwdUsing,
    KwdCast,
    KwdAutoCast,
    KwdIf,
    KwdElse,
    KwdWhile,
    KwdBreak,
    KwdContinue,
    KwdLoop,
    KwdVisit,
    KwdMove,
    KwdNoDrop,
    KwdSwitch,
    KwdCase,
    KwdFallThrough,
    KwdDefault,
    KwdFor,
    KwdDefer,
    KwdInit,
    KwdDrop,
    KwdDeRef,
    KwdCode,
    KwdLabel,

    IntrinsicSizeOf,
    IntrinsicTypeOf,
    IntrinsicKindOf,
    IntrinsicCountOf,
    IntrinsicDataOf,
    IntrinsicSliceOf,
    IntrinsicIndex,

    IntrinsicPrint,
    IntrinsicAssert,
    IntrinsicAlloc,
    IntrinsicRealloc,
    IntrinsicFree,
    IntrinsicMemCpy,
    IntrinsicMemSet,
    IntrinsicMemCmp,
    IntrinsicGetContext,
    IntrinsicSetContext,
    IntrinsicArguments,
    IntrinsicIsByteCode,

    Identifier,
    LiteralNumber,
    LiteralString,
    LiteralCharacter,
    NativeType,

    Invalid,
    EndOfLine,
    Blank,
    EndOfFile,
};

struct Token
{
    Utf8           text;
    SourceLocation startLocation;
    SourceLocation endLocation;
    Register       literalValue;

    TypeInfo* literalType;

    TokenId id = TokenId::Invalid;
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
    int            seek               = 0;
    uint32_t       parseFlags         = 0;
    SourceLocation location;
    bool           endReached          = false;
    bool           forceLastTokenIsEOL = false;
    bool           lastTokenIsEOL      = false;
};