#pragma once
#include "Register.h"

template<typename T>
struct MapRegTo
{
	bool     here[RegisterList::MAX_REGISTERS];
	T        val[RegisterList::MAX_REGISTERS];
	uint32_t count = 0;

	MapRegTo()
	{
		memset(here, 0, sizeof(here));
	}

	void clear()
	{
		if (!count)
			return;
		count = 0;
		memset(here, 0, sizeof(here));
	}

	bool contains(uint32_t reg) const
	{
		SWAG_ASSERT(reg < RegisterList::MAX_REGISTERS);
		return here[reg];
	}

	void remove(uint32_t reg)
	{
		SWAG_ASSERT(reg < RegisterList::MAX_REGISTERS);
		if (here[reg])
		{
			SWAG_ASSERT(count);
			here[reg] = false;
			count--;
		}
	}

	void set(uint32_t reg, const T& value)
	{
		SWAG_ASSERT(reg < RegisterList::MAX_REGISTERS);
		if (!here[reg])
		{
			here[reg] = true;
			count++;
		}

		val[reg] = value;
	}

	T* find(uint32_t reg)
	{
		if (reg >= RegisterList::MAX_REGISTERS)
			return nullptr;
		if (!here[reg])
			return nullptr;
		return &val[reg];
	}
};
