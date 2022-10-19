#include "pch.h"
#include "Attribute.h"

OneAttribute* AttributeList::getAttribute(const Utf8& fullName)
{
    SWAG_RACE_CONDITION_READ(raceCond);
    for (auto& it : allAttributes)
    {
        if (it.name == fullName)
            return &it;
    }

    return nullptr;
}

const AttributeParameter* AttributeList::getParam(const Utf8& fullName, const Utf8& parameter)
{
    SWAG_RACE_CONDITION_READ(raceCond);
    for (auto& it : allAttributes)
    {
        if (it.name == fullName)
        {
            for (auto& param : it.parameters)
            {
                if (param.token.text == parameter)
                {
                    return &param;
                }
            }

            return nullptr;
        }
    }

    return nullptr;
}

const ComputedValue* AttributeList::getValue(const Utf8& fullName, const Utf8& parameter)
{
    SWAG_RACE_CONDITION_READ(raceCond);
    for (auto& it : allAttributes)
    {
        if (it.name == fullName)
        {
            for (auto& param : it.parameters)
            {
                if (param.token.text == parameter)
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
    SWAG_RACE_CONDITION_READ(raceCond);
    for (auto& it : allAttributes)
    {
        if (it.name == fullName)
            return true;
    }

    return false;
}

void AttributeList::emplace(OneAttribute& other)
{
    SWAG_RACE_CONDITION_WRITE(raceCond);
    allAttributes.emplace_back(move(other));
}

void AttributeList::add(AttributeList& other)
{
    SWAG_RACE_CONDITION_WRITE(raceCond);
    SWAG_RACE_CONDITION_READ1(other.raceCond);

    for (auto& p : other.allAttributes)
        allAttributes.push_back(p);
}