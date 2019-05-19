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

    CompilerUnitTest,

    Identifier,
    LiteralNumber,
    NativeType,

    Invalid,
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
    bool getToken(Token& token);

private:
    unsigned getChar();
    unsigned getCharNoSeek(unsigned& offset);
    unsigned getChar(unsigned& offset, bool seek);
    bool     eatCComment(Token& token);
    void     getIdentifier(Token& token);
    void     treatChar(unsigned c, unsigned offset);
    bool     doNumberLiteral(unsigned c, Token& token);
    bool     doHexLiteral(Token& token);
    bool     doBinLiteral(Token& token);
    bool     doNumberSuffix(Token& token);
    bool     doIntFloatLiteral(bool hasIntegralPart, unsigned c, Token& token);
    bool     doIntLiteral(unsigned c, Token& token, unsigned& fractPart);
    bool     error(Token& token, const utf8& msg);
    bool     errorNumberSyntax(Token& token, const utf8& msg);
    bool     doSymbol(unsigned c, Token& token);
    bool     doStringLiteral(Token& token);

private:
    SourceFile*    m_sourceFile  = nullptr;
    unsigned       m_cacheChar   = 0;
    unsigned       m_cacheOffset = 0;
    SourceLocation m_location;
    bool           m_endReached = false;
};