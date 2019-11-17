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

    CompilerModule,
    CompilerUnitTest,
    CompilerAssert,
    CompilerPrint,
    CompilerRun,
    CompilerFile,
    CompilerLine,
    CompilerConfiguration,
    CompilerPlatform,
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

    Intrinsic,

    KwdVar,
    KwdLet,
    KwdConst,
    KwdPublic,
    KwdPrivate,
    KwdNamespace,
    KwdTypeAlias,
    KwdEnum,
    KwdEnumValue,
    KwdImpl,
    KwdStruct,
    KwdUnion,
    KwdFunc,
	KwdMacro,
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
    KwdIndex,
    KwdMove,
    KwdSwitch,
    KwdCase,
    KwdDefault,
    KwdFor,
    KwdDefer,
    KwdIs,
    KwdInit,
    KwdDrop,
    KwdDeRef,

    Identifier,
    LiteralNumber,
    LiteralString,
    LiteralCharacter,
    NativeType,

    Invalid,
    EndOfLine,
    EndOfFile,
};

struct Token
{
    Utf8           text;
    SourceLocation startLocation;
    SourceLocation endLocation;
    Register       literalValue;
    TokenId        id;
    TypeInfo*      literalType;
};

struct Tokenizer
{
    void setFile(SourceFile* file);
    bool getToken(Token& token, bool skipEOL = true);

    char32_t getChar();
    char32_t getCharNoSeek(unsigned& offset);
    char32_t getChar(unsigned& offset, bool seek);
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
    bool     doStringLiteral(Token& token, bool raw);
    bool     isEscape(char32_t& c, Token& token);
    bool     getDigitHexa(Token& token, int& result);
    bool     doCharLiteral(Token& token);

    SourceFile*    sourceFile      = nullptr;
    char32_t       cacheChar       = 0;
    unsigned       cacheCharOffset = 0;
    int            seek            = 0;
    SourceLocation location;
    bool           endReached          = false;
    bool           forceLastTokenIsEOL = false;
    bool           lastTokenIsEOL      = false;
};