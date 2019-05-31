#pragma once
#include "Utf8Crc.h"
#include "SymTable.h"
struct Scope;
struct SymTable;

enum class ScopeKind
{
    Workspace,
    Module,
    Namespace,
    Enum,
    Function,
};

struct Scope : public PoolElement
{
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

    bool isGlobal()
    {
        return kind == ScopeKind::Workspace || kind == ScopeKind::Module || kind == ScopeKind::Namespace;
    }

    ScopeKind kind;
    Scope*    parentScope;
    SymTable* symTable;
    Utf8Crc   name;
    Utf8Crc   fullname;
};
