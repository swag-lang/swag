#include "pch.h"
#include "LanguageSpec.h"

void Tokenizer::doIdentifier(TokenParse& token)
{
    while (SWAG_IS_ALPHA(curBuffer[0]) || SWAG_IS_DIGIT(curBuffer[0]) || curBuffer[0] == '_')
        readChar();
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
