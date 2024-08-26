#include "pch.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

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
        if (tokenParse.is(TokenId::NativeType))
        {
            const auto it1 = g_LangSpec->nativeTypes.find(tokenParse.token.text);
            SWAG_ASSERT(it1);
            tokenParse.literalType = *it1;
        }

        return true;
    }

    if (tokenParse.token.text == '#' || tokenParse.token.text == '@')
    {
        tokenParse.token.endLocation = location;
        Diagnostic err{sourceFile, tokenParse, formErr(Err0667, tokenParse.cstr())};
        if (tokenParse.token.text == '#' && SWAG_IS_BLANK(*curBuffer))
            err.addNote(toNte(Nte0054));
        return errorContext->report(err);
    }

    if (tokenParse.token.text[0] == '#')
    {
        if (tokenParse.token.text.startsWith(g_LangSpec->name_sharp_alias))
        {
            tokenParse.token.id = TokenId::Identifier;
            return true;
        }

        if (tokenParse.token.text.startsWith(g_LangSpec->name_sharp_mix))
        {
            tokenParse.token.id = TokenId::Identifier;
            return true;
        }

        tokenParse.token.endLocation = location;
        Diagnostic err{sourceFile, tokenParse, formErr(Err0714, tokenParse.cstr())};

        Vector<Utf8> searchList{};
        for (uint32_t i = 0; i < g_LangSpec->keywords.allocated; i++)
        {
            const auto& k = g_LangSpec->keywords.buffer[i].key;
            if (k && k[0] == '#')
                searchList.push_back(k);
        }

        Vector<Utf8> result;
        SemanticError::findClosestMatchesInList(tokenParse.token.text, searchList, result);
        if (!result.empty())
            err.addNote(SemanticError::findClosestMatchesMsg(tokenParse.token.text, result));

        return errorContext->report(err);
    }

    if (tokenParse.token.text[0] == '@')
    {
        tokenParse.token.endLocation = location;
        Diagnostic err{sourceFile, tokenParse, formErr(Err0729, tokenParse.cstr())};

        Vector<Utf8> searchList{};
        for (uint32_t i = 0; i < g_LangSpec->keywords.allocated; i++)
        {
            const auto& k = g_LangSpec->keywords.buffer[i].key;
            if (k && k[0] == '@')
                searchList.push_back(k);
        }

        Vector<Utf8> result;
        SemanticError::findClosestMatchesInList(tokenParse.token.text, searchList, result);
        if (!result.empty())
            err.addNote(SemanticError::findClosestMatchesMsg(tokenParse.token.text, result));

        return errorContext->report(err);
    }

    tokenParse.token.id = TokenId::Identifier;
    return true;
}
