#include "pch.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Scope.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

Utf8 SemanticError::findClosestMatchesMsg(const Utf8& searchName, const Vector<Utf8>& best)
{
    Utf8 appendMsg;
    switch (best.size())
    {
        case 1:
        {
            Utf8 a0 = searchName;
            Utf8 a1 = best[0];
            a0.makeLower();
            a1.makeLower();
            if (a0 == a1 && searchName != best[0])
                appendMsg = form("did you mean [[%s]]? (notice the capitalization)", best[0].cstr());
            else
                appendMsg = form("did you mean [[%s]]?", best[0].cstr());
            break;
        }
        case 2:
            appendMsg = form("did you mean [[%s]] or [[%s]]?", best[0].cstr(), best[1].cstr());
            break;
        case 3:
            appendMsg = form("did you mean [[%s]], [[%s]] or [[%s]]?", best[0].cstr(), best[1].cstr(), best[2].cstr());
            break;
    }

    return appendMsg;
}

Utf8 SemanticError::findClosestMatchesMsg(const Utf8& searchName, const VectorNative<CollectedScope>& scopeHierarchy, IdentifierSearchFor searchFor)
{
    Vector<Utf8> best;
    findClosestMatches(searchName, scopeHierarchy, best, searchFor);
    return findClosestMatchesMsg(searchName, best);
}

void SemanticError::findClosestMatchesInList(const Utf8& searchName, const Vector<Utf8>& searchList, Vector<Utf8>& result)
{
    uint32_t bestScore = UINT32_MAX;
    result.clear();

    // Search first for a case problem
    auto word1 = searchName;
    word1.makeUpper();
    for (const auto& it : searchList)
    {
        auto word = it;
        word.makeUpper();
        if (word == word1)
            result.push_back(it);
    }

    if(!result.empty())
        return;

    for (const auto& word : searchList)
    {
        const auto score = Utf8::fuzzyCompare(searchName, word);

        // If number of changes is too big considering the size of the text, cancel
        if (searchName.count > 1 && score > static_cast<uint32_t>(searchName.count) / 2)
            continue;
        // If too much changes, cancel
        if (score > 2)
            continue;

        if (score < bestScore)
            result.clear();
        if (score <= bestScore)
        {
            // Be sure it's not already in the best list
            bool here = false;
            for (auto& n : result)
            {
                if (n == word)
                {
                    here = true;
                    break;
                }
            }

            if (!here)
                result.push_back(word);

            bestScore = score;
        }
    }
}

void SemanticError::findClosestMatches(const Utf8& searchName, const VectorNative<CollectedScope>& scopeHierarchy, Vector<Utf8>& best, IdentifierSearchFor searchFor)
{
    Vector<Utf8> searchList;
    for (auto& as : scopeHierarchy)
    {
        const auto s = as.scope;
        for (uint32_t i = 0; i < s->symTable.mapNames.allocated; i++)
        {
            const auto one = s->symTable.mapNames.buffer[i];
            if (!one.symbolName)
                continue;
            if (one.symbolName->cptOverloadsInit == 0)
                continue;
            if (searchName == one.symbolName->name)
                continue;

            if (searchFor == IdentifierSearchFor::Whatever)
            {
                searchList.push_back(one.symbolName->name);
                continue;
            }

            switch (one.symbolName->kind)
            {
                case SymbolKind::Function:
                    if (searchFor == IdentifierSearchFor::Function)
                        searchList.push_back(one.symbolName->name);
                    break;

                case SymbolKind::Attribute:
                    if (searchFor == IdentifierSearchFor::Attribute)
                        searchList.push_back(one.symbolName->name);
                    break;

                case SymbolKind::Variable:
                    if (searchFor == IdentifierSearchFor::Variable)
                        searchList.push_back(one.symbolName->name);
                    break;

                case SymbolKind::Struct:
                    if (searchFor == IdentifierSearchFor::Struct || searchFor == IdentifierSearchFor::Type)
                        searchList.push_back(one.symbolName->name);
                    break;

                case SymbolKind::Enum:
                    if (searchFor == IdentifierSearchFor::Enum || searchFor == IdentifierSearchFor::Type)
                        searchList.push_back(one.symbolName->name);
                    break;

                case SymbolKind::TypeAlias:
                case SymbolKind::GenericType:
                case SymbolKind::Interface:
                    if (searchFor == IdentifierSearchFor::Type)
                        searchList.push_back(one.symbolName->name);
                    break;
            }
        }
    }

    for (uint32_t i = 0; i < g_LangSpec->keywords.allocated; i++)
    {
        if (!g_LangSpec->keywords.buffer[i].key)
            continue;

        switch (searchFor)
        {
            case IdentifierSearchFor::TopLevelInstruction:
                if (Tokenizer::isTopLevelInst(g_LangSpec->keywords.buffer[i].value))
                    searchList.push_back(g_LangSpec->keywords.buffer[i].key);
                break;

            case IdentifierSearchFor::Keyword:
                if (Tokenizer::isKeyword(g_LangSpec->keywords.buffer[i].value))
                    searchList.push_back(g_LangSpec->keywords.buffer[i].key);
                break;

            case IdentifierSearchFor::Type:
                if (g_LangSpec->keywords.buffer[i].value == TokenId::NativeType)
                    searchList.push_back(g_LangSpec->keywords.buffer[i].key);
                break;

            case IdentifierSearchFor::Function:
                if (searchName[0] == '@' || searchName[0] == '#')
                {
                    const auto& k = g_LangSpec->keywords.buffer[i].key;
                    if (k && k[0] == searchName[0])
                        searchList.push_back(k);
                }
                break;

            case IdentifierSearchFor::Struct:
            case IdentifierSearchFor::Enum:
            case IdentifierSearchFor::Variable:
                break;

            default:
                searchList.push_back(g_LangSpec->keywords.buffer[i].key);
                break;
        }
    }

    findClosestMatchesInList(searchName, searchList, best);
}
