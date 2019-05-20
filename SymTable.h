#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "Utf8.h"
struct PoolFactory;

struct SymbolOverload : public PoolElement
{
};

enum class SymbolType
{
    Variable,
};

struct SymbolName : public PoolElement
{
    utf8                    name;
    SymbolType              type;
    atomic<int>             cptOverloads;
    vector<SymbolOverload*> overloads;

    void reset() override
    {
        name.clear();
        cptOverloads = 0;
        overloads.clear();
    }
};

struct SymTable
{
    SymbolName* registerSyntaxSymbol(PoolFactory* factory, const utf8& name, SymbolType type);
    SymbolName* find(const utf8& name);

    SpinLock               mutex;
    map<utf8, SymbolName*> mapNames;
};
