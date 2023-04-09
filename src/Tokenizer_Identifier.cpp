#include "pch.h"
#include "LanguageSpec.h"

void Tokenizer::doIdentifier(TokenParse& token)
{
    while (idLetters[(uint32_t) curBuffer[0]])
    {
        curBuffer++;
        location.column++;
    }

    appendTokenName(token);

    auto it = g_LangSpec->keywords.find(token.text);
    if (it)
    {
        token.id = *it;
        if (token.id == TokenId::NativeType)
        {
            auto it1 = g_LangSpec->nativeTypes.find(token.text);
            SWAG_ASSERT(it1);
            token.literalType = *it1;
        }
    }
    else
    {
        token.id = TokenId::Identifier;
    }
}
