#include "pch.h"
#include "SymTable.h"

SymbolName* SymTable::find(const string& name)
{
    mutex.lock();
	if (mapNames.empty())
		return nullptr;
    auto it     = mapNames.find(name);
    auto result = it == mapNames.end() ? it->second : nullptr;
    mutex.unlock();
    return result;
}

SymbolName* SymTable::registerSyntaxSymbol(Pool<SymbolName>& pool, const string& name)
{
    auto symbol = find(name);
    if (!symbol)
    {
        symbol       = pool.alloc();
        symbol->name = name;
    }

    symbol->cptOverloads++;
    return symbol;
}
