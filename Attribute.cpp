#include "pch.h"
#include "Attribute.h"

OneAttribute* SymbolAttributes::getAttribute(const Utf8& fullName)
{
    for (auto& it : attributes)
    {
        if (it.name == fullName)
            return &it;
    }

    return nullptr;
}

bool SymbolAttributes::getValue(const Utf8& fullName, const Utf8& parameter, ComputedValue& value)
{
    for (auto& it : attributes)
    {
        if (it.name == fullName)
        {
            for (auto& param : it.parameters)
            {
                if (param.name == parameter)
                {
                    value = param.value;
                    return true;
                }
            }

            return false;
        }
    }

    return false;
}

bool SymbolAttributes::hasAttribute(const Utf8& fullName)
{
    for (auto& it : attributes)
    {
        if (it.name == fullName)
            return true;
    }

    return false;
}