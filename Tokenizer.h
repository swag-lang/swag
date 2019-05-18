#pragma once
#include "SourceFile.h"
enum class TokenId
{
    Unknown,
    SymSlash,
    Identifier,
    CompilerPass,
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
    unsigned getChar();
    bool     ZapCComment(Token& token);
    void     GetIdentifier(Token& token);

private:
    SourceFile*    m_sourceFile = nullptr;
    unsigned       m_cacheChar  = 0;
    SourceLocation m_location;
    bool           m_endReached = false;
};