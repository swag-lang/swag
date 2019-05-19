#pragma once
#include "SourceFile.h"
#include "Register.h"

enum class TokenId
{
    Unknown,
    SymSlash,
	SymDot,
    Identifier,
    CompilerUnitTest,
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
    wstring        text;
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
    bool     ZapCComment(Token& token);
    void     getIdentifier(Token& token);
    void     treatChar(unsigned c, unsigned offset);
    bool     doNumberLiteral(unsigned c, Token& token);
    bool     doHexLiteral(Token& token);
    bool     doBinLiteral(Token& token);
    bool     doNumberSuffix(Token& token);
    bool     doIntFloatLiteral(unsigned c, Token& token);
    bool     doIntLiteral(unsigned c, Token& token, unsigned& fractPart);
    bool     error(Token& token, const wstring& msg);
    bool     errorNumberSyntax(Token& token, const wstring& msg);

private:
    SourceFile*    m_sourceFile  = nullptr;
    unsigned       m_cacheChar   = 0;
    unsigned       m_cacheOffset = 0;
    SourceLocation m_location;
    bool           m_endReached = false;
};