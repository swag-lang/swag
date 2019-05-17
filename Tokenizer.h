#pragma once
#include "SourceFile.h"
enum class TokenId
{
	Unknown,
	SymSlash,
	Identifier,
	EndOfFile,
};

struct Token
{
	TokenId id;
	SourceLocation location;
	string  text;
};

enum class TokenizerResult
{
	Pending,
	EndOfFile,
	Error,
};

class Tokenizer
{
public:
	void setFile(SourceFile* file);
	TokenizerResult getToken(Token& token);

private:
	unsigned getChar();
	void ZapCComment();
	void GetIdentifier(Token& token);

private:
	SourceFile* m_sourceFile = nullptr;
	unsigned m_cacheChar = 0;
	SourceLocation m_location;
};