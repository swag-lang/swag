#include "pch.h"
#include "Scope.h"
#include "SymTable.h"

void Scope::allocateSymTable()
{
    if (symTable)
        return;
    symTable = new SymTable(this);
}