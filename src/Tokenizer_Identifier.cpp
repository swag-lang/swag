#include "pch.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Semantic.h"
#include "SemanticError.h"

bool Tokenizer::doIdentifier(TokenParse& tokenParse)
{
    while (idLetters[static_cast<uint8_t>(curBuffer[0])])
    {
        curBuffer++;
        location.column++;
    }

    appendTokenName(tokenParse);

    const auto it = g_LangSpec->keywords.find(tokenParse.token.text);
    if (it)
    {
        tokenParse.token.id = *it;
        if (tokenParse.token.id == TokenId::NativeType)
        {
            const auto it1 = g_LangSpec->nativeTypes.find(tokenParse.token.text);
            SWAG_ASSERT(it1);
            tokenParse.literalType = *it1;
        }
    }
    else if (tokenParse.token.text[0] == '#')
    {
        if (tokenParse.token.text.startsWith(g_LangSpec->name_atalias))
        {
            tokenParse.token.id = TokenId::Identifier;
        }
        else if (tokenParse.token.text.startsWith(g_LangSpec->name_atmixin))
        {
            tokenParse.token.id = TokenId::Identifier;
        }
        else
        {
            tokenParse.token.endLocation = location;
            Diagnostic err{sourceFile, tokenParse.token, formErr(Err0245, tokenParse.token.c_str())};

            Vector<Utf8> searchList{};
            for (int i = 0; i < static_cast<int>(g_LangSpec->keywords.allocated); i++)
            {
                const auto& k = g_LangSpec->keywords.buffer[i].key;
                if (k && k[0] == '#')
                    searchList.push_back(k);
            }

            Vector<Utf8> result;
            SemanticError::findClosestMatches(tokenParse.token.text, searchList, result);
            if (!result.empty())
                err.addNote(SemanticError::findClosestMatchesMsg(tokenParse.token.text, result));

            return errorContext->report(err);
        }
    }
    else if (tokenParse.token.text[0] == '@')
    {
        tokenParse.token.endLocation = location;
        Diagnostic err{sourceFile, tokenParse.token, formErr(Err0316, tokenParse.token.c_str())};

        Vector<Utf8> searchList{};
        for (int i = 0; i < static_cast<int>(g_LangSpec->keywords.allocated); i++)
        {
            const auto& k = g_LangSpec->keywords.buffer[i].key;
            if (k && k[0] == '@')
                searchList.push_back(k);
        }

        Vector<Utf8> result;
        SemanticError::findClosestMatches(tokenParse.token.text, searchList, result);
        if (!result.empty())
            err.addNote(SemanticError::findClosestMatchesMsg(tokenParse.token.text, result));

        return errorContext->report(err);
    }
    else
    {
        tokenParse.token.id = TokenId::Identifier;
    }

    return true;
}
