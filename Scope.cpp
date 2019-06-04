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