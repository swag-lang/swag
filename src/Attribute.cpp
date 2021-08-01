#include "pch.h"
#include "Attribute.h"

OneAttribute* AttributeList::getAttribute(const Utf8& fullName)
{
    for (auto& it : allAttributes)
    {
        if (it.name == fullName)
            return &it;
    }

    return nullptr;
}

const ComputedValue* AttributeList::getValue(const Utf8& fullName, const Utf8& parameter)
{
    for (auto& it : allAttributes)
    {
        if (it.name == fullName)
        {
            for (auto& param : it.parameters)
            {
                if (param.name == parameter)
                {
                    return &param.value;
                }
            }

            return nullptr;
        }
    }

    return nullptr;
}

bool AttributeList::hasAttribute(const Utf8& fullName)
{
    for (auto& it : allAttributes)
    {
        if (it.name == fullName)
            return true;
    }

    return false;
}

void AttributeList::emplace(OneAttribute& other)
{
    allAttributes.emplace_back(move(other));
}

void AttributeList::add(AttributeList& other)
{
    for (auto& p : other.allAttributes)
        allAttributes.push_back(p);
}