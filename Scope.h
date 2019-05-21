#pragma once
#include "Utf8Crc.h"
#include "SymTable.h"
struct Scope;
struct SymTable;

enum class ScopeType
{
    Module,
    Namespace,
};

struct Scope : public PoolElement
{
    ScopeType type;
    Scope*    parentScope;
    SymTable* symTable;
    Utf8Crc   name;
    Utf8Crc   fullname;

    void reset() override
    {
        parentScope = nullptr;
        symTable    = nullptr;
    }

    void allocateSymTable()
    {
        if (symTable)
            return;
        symTable = new SymTable();
    }
};
