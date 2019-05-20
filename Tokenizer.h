#pragma once
#include "SourceFile.h"
#include "Register.h"

enum class TokenId
{
    SymDot,
    SymLeftParen,
    SymRightParen,
    SymLeftSquare,
    SymRightSquare,
    SymLeftCurly,
    SymRightCurly,
    SymColon,
    SymSemiColon,
    SymQuestion,
    SymAt,
    SymEqual,
    SymEqualEqual,
    SymMinus,
    SymMinusEqual,
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
    SymVertical,
    SymVerticalEqual,
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

    CompilerUnitTest,

	KwdVar,

    Identifier,
    LiteralNumber,
    LiteralString,
    LiteralCharacter,
    NativeType,

    Invalid,
    EndOfLine,
    EndOfFile,
};

enum class TokenNumType
{
    IntX,
    UIntX,
    Int8,
    Int16,
    Int32,
    Int64,
    UInt8,
    UInt16,
    UInt32,
    UInt64,
    Float32,
    Float64,
    Bool,
    Char,
};

struct Token
{
    TokenId        id;
    SourceLocation startLocation;
    SourceLocation endLocation;
    utf8           text;
    TokenNumType   numType;
    Register       numValue;
};

class Tokenizer
{
public:
    void setFile(SourceFile* file);
    bool getToken(Token& token, bool skipEOL = true);

    bool getTokenOrEOL(Token& token)
    {
        return getToken(token, false);
    }

private:
    char32_t getChar();
    char32_t getCharNoSeek(unsigned& offset);
    char32_t getChar(unsigned& offset, bool seek);
    bool     eatCComment(Token& token);
    void     getIdentifier(Token& token);
    void     treatChar(char32_t c, unsigned offset);
    bool     doNumberLiteral(char32_t c, Token& token);
    bool     doHexLiteral(Token& token);
    bool     doBinLiteral(Token& token);
    bool     doNumberSuffix(Token& token);
    bool     doIntFloatLiteral(bool hasIntegralPart, char32_t c, Token& token);
    bool     doIntLiteral(char32_t c, Token& token, unsigned& fractPart);
    bool     error(Token& token, const utf8& msg);
    bool     errorNumberSyntax(Token& token, const utf8& msg);
    bool     doSymbol(char32_t c, Token& token);
    bool     doStringLiteral(Token& token, bool raw);
    bool     isEscape(char32_t& c, Token& token);
    bool     getDigitHexa(Token& token, int& result);
    bool     doCharLiteral(Token& token);

private:
    SourceFile*    m_sourceFile  = nullptr;
    char32_t       m_cacheChar   = 0;
    unsigned       m_cacheOffset = 0;
    SourceLocation m_location;
    bool           m_endReached = false;
};