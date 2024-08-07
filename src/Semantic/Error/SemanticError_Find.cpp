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
                appendMsg = form("do you mean [[%s]]? (notice the capitalization)", best[0].c_str());
            else
                appendMsg = form("do you mean [[%s]]?", best[0].c_str());
            break;
        }
        case 2:
            appendMsg = form("do you mean [[%s]] or [[%s]]?", best[0].c_str(), best[1].c_str());
            break;
        case 3:
            appendMsg = form("do you mean [[%s]], [[%s]] or [[%s]]?", best[0].c_str(), best[1].c_str(), best[2].c_str());
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

void SemanticError::findClosestMatches(const Utf8& searchName, const Vector<Utf8>& searchList, Vector<Utf8>& result)
{
    uint32_t bestScore = UINT32_MAX;
    result.clear();

    const auto searchName1 = searchName;
    for (const auto& i : searchList)
    {
        const auto& searchName2 = i;
        const auto  score       = Utf8::fuzzyCompare(searchName1, searchName2);

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
                if (n == i)
                {
                    here = true;
                    break;
                }
            }

            if (!here)
            {
                result.push_back(i);
            }

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

            // Filter to try to be as relevant as possible
            if (searchFor == IdentifierSearchFor::Function &&
                one.symbolName->isNot(SymbolKind::Function))
                continue;
            if (searchFor == IdentifierSearchFor::Attribute &&
                one.symbolName->isNot(SymbolKind::Attribute))
                continue;
            if (searchFor != IdentifierSearchFor::Function &&
                one.symbolName->is(SymbolKind::Function))
                continue;
            if (searchFor != IdentifierSearchFor::Function &&
                one.symbolName->is(SymbolKind::Attribute))
                continue;
            if (searchFor == IdentifierSearchFor::Type &&
                one.symbolName->isNot(SymbolKind::TypeAlias) &&
                one.symbolName->isNot(SymbolKind::Enum) &&
                one.symbolName->isNot(SymbolKind::GenericType) &&
                one.symbolName->isNot(SymbolKind::Struct) &&
                one.symbolName->isNot(SymbolKind::Interface))
                continue;
            if (searchFor == IdentifierSearchFor::Struct &&
                one.symbolName->isNot(SymbolKind::Struct))
                continue;
            if (one.symbolName->cptOverloadsInit == 0)
                continue;

            searchList.push_back(one.symbolName->name);
        }
    }

    for (uint32_t i = 0; i < g_LangSpec->keywords.allocated; i++)
    {
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

            default:
                if (g_LangSpec->keywords.buffer[i].key)
                    searchList.push_back(g_LangSpec->keywords.buffer[i].key);
                break;
        }
    }

    findClosestMatches(searchName, searchList, best);
}
