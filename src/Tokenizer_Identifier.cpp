#include "pch.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Semantic.h"
#include "SemanticError.h"

bool Tokenizer::doIdentifier(TokenParse& token)
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
    else if (token.text[0] == '#')
    {
        if (token.text.startsWith(g_LangSpec->name_atalias))
        {
            token.id = TokenId::Identifier;
        }
        else if (token.text.startsWith(g_LangSpec->name_atmixin))
        {
            token.id = TokenId::Identifier;
        }
        else
        {
            token.endLocation = location;
            Diagnostic diag{sourceFile, token, Fmt(Err(Err2017), token.ctext())};

            Vector<Utf8> searchList{};
            for (int i = 0; i < (int) g_LangSpec->keywords.allocated; i++)
            {
                auto& k = g_LangSpec->keywords.buffer[i].key;
                if (k && k[0] == '#')
                    searchList.push_back(k);
            }

            Vector<Utf8> result;
            SemanticError::findClosestMatches(token.text, searchList, result);
            if (result.size())
                return errorContext->report(diag, Diagnostic::note(SemanticError::findClosestMatchesMsg(token.text, result)));

            return errorContext->report(diag);
        }
    }
    else if (token.text[0] == '@')
    {
        token.endLocation = location;
        Diagnostic diag{sourceFile, token, Fmt(Err(Err2027), token.ctext())};

        Vector<Utf8> searchList{};
        for (int i = 0; i < (int) g_LangSpec->keywords.allocated; i++)
        {
            auto& k = g_LangSpec->keywords.buffer[i].key;
            if (k && k[0] == '@')
                searchList.push_back(k);
        }

        Vector<Utf8> result;
        SemanticError::findClosestMatches(token.text, searchList, result);
        if (result.size())
            return errorContext->report(diag, Diagnostic::note(SemanticError::findClosestMatchesMsg(token.text, result)));

        return errorContext->report(diag);
    }
    else
    {
        token.id = TokenId::Identifier;
    }

    return true;
}
