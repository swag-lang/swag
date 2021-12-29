#include "pch.h"
#include "LanguageSpec.h"
#include "SourceFile.h"
#include "Module.h"
#include "Version.h"
#include "Backend.h"

void Tokenizer::relaxIdentifier(Token& token)
{
    switch (token.id)
    {
    case TokenId::KwdAnd:
    case TokenId::KwdOr:
    case TokenId::KwdOrElse:
        token.id = TokenId::Identifier;
        return;
    }
}

void Tokenizer::doIdentifier(Token& token, uint32_t c, unsigned offset)
{
    while (SWAG_IS_ALPHA(c) || SWAG_IS_DIGIT(c) || c == '_')
    {
        treatChar(c, offset);
        c = getCharNoSeek(offset);
    }

    appendTokenName(token);

    auto it = g_LangSpec->keywords.find(token.text);
    if (it)
        token.id = (*it).first;
    else
        token.id = TokenId::Identifier;
    if (token.id == TokenId::NativeType)
        token.literalType = (*it).second;
}
