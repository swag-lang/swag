#pragma once
#include "SourceFile.h"
enum class TokenId
{
    Unknown,
    SymSlash,
    Identifier,
    CompilerUnitTest,
    Invalid,
    EndOfFile,
};

struct Token
{
    TokenId        id;
    SourceLocation startLocation;
    SourceLocation endLocation;
    wstring        text;
};

class Tokenizer
{
public:
    void setFile(SourceFile* file);
    bool getToken(Token& token);

private:
    unsigned getChar(bool seek = true);
    bool     ZapCComment(Token& token);
    void     GetIdentifier(Token& token);
    void     treatChar(unsigned c);
    bool     doNumberLiteral(unsigned c, Token& token);
    bool     doHexLiteral(Token& token);
	bool     error(Token& token, const wstring& msg);
    bool     errorNumberSyntax(Token& token, const wstring& msg);

private:
    SourceFile*    m_sourceFile = nullptr;
    unsigned       m_cacheChar  = 0;
    SourceLocation m_location;
    bool           m_endReached = false;
};