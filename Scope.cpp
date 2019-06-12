#include "pch.h"
#include "Scope.h"
#include "SymTable.h"
Pool<Scope> g_Pool_scope;

void Scope::allocateSymTable()
{
    if (symTable)
        return;
    symTable = new SymTable(this);
}

const char* Scope::getNakedName(ScopeKind kind)
{
    switch (kind)
    {
    case ScopeKind::Namespace:
        return "namespace";
	case ScopeKind::Enum:
        return "enum";
    default:
        return "???";
    }
}