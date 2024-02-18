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

void AttributeList::getAttributes(VectorNative<const OneAttribute*>& res, const Utf8& fullName) const
{
	SWAG_RACE_CONDITION_READ(raceCond);

	res.clear();
	for (auto& it : allAttributes)
	{
		if (it.name == fullName)
			res.push_back(&it);
	}
}

const AttributeParameter* OneAttribute::getParam(const Utf8& paramName) const
{
	for (auto& param : parameters)
	{
		if (param.token.text == paramName)
		{
			return &param;
		}
	}

	return nullptr;
}

const ComputedValue* OneAttribute::getValue(const Utf8& paramName) const
{
	for (auto& param : parameters)
	{
		if (param.token.text == paramName)
		{
			return &param.value;
		}
	}

	return nullptr;
}

const AttributeParameter* AttributeList::getParam(const Utf8& fullName, const Utf8& parameter) const
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

const ComputedValue* AttributeList::getValue(const Utf8& fullName, const Utf8& parameter) const
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

bool AttributeList::hasAttribute(const Utf8& fullName) const
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
	allAttributes.emplace_back(std::move(other));
}

void AttributeList::add(const AttributeList& other)
{
	SWAG_RACE_CONDITION_WRITE(raceCond);
	SWAG_RACE_CONDITION_READ1(other.raceCond);

	for (auto& p : other.allAttributes)
		allAttributes.push_back(p);
}
