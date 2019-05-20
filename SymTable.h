#pragma once
#include "Pool.h"
#include "SpinLock.h"

struct SymbolOverload : public PoolElement
{
};

enum class SymbolType
{
    Variable,
};

struct SymbolName : public PoolElement
{
    string                  name;
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
    SymbolName* registerSyntaxSymbol(Pool<SymbolName>& pool, const string& name, SymbolType type);
    SymbolName* find(const string& name);

    SpinLock                 mutex;
    map<string, SymbolName*> mapNames;
};
