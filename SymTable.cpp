#include "pch.h"
#include "SymTable.h"
#include "PoolFactory.h"

SymbolName* SymTable::find(const utf8& name)
{
    scoped_lock<SpinLock> sl(mutex);
    if (mapNames.empty())
        return nullptr;
    auto it     = mapNames.find(name);
    auto result = it != mapNames.end() ? it->second : nullptr;
    return result;
}

SymbolName* SymTable::registerSyntaxSymbol(PoolFactory* factory, const utf8& name, SymbolType type)
{
    auto symbol = find(name);
    if (!symbol)
    {
        symbol       = factory->symName.alloc();
        symbol->name = name;
        symbol->type = type;
        mutex.lock();
        mapNames[name] = symbol;
        mutex.unlock();
    }

    symbol->cptOverloads++;
    return symbol;
}
