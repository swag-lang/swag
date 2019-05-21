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
    utf8crc   name;
    utf8crc   fullname;

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
