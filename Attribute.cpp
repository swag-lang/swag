#include "pch.h"
#include "Attribute.h"

bool SymbolAttributes::getValue(const string& fullName, ComputedValue& value)
{
    auto it = values.find(fullName);
    if (it == values.end())
        return false;
    value = it->second;
    return true;
}