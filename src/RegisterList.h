#pragma once
#include "Global.h"
#include "Log.h"
#include "VectorNative.h"

struct RegisterList
{
    static const int MAX_STATIC = 2;

    // To optimize memory, register cannot have a value > 255. This should be fine as we are recycling
    // registers. But perhaps one day an assert will trigger (if we do not correctly free a register for example).
    // For now, stick to 8 bits max.
    uint8_t oneResult[MAX_STATIC];
    uint8_t countResults = 0;
    bool    canFree      = true;

    RegisterList()
    {
    }

    RegisterList(uint32_t r)
    {
        SWAG_ASSERT(r <= 255);
        oneResult[0] = (uint8_t) r;
        countResults = 1;
    }

    int size() const
    {
        return countResults;
    }

    uint32_t operator[](int index) const
    {
        SWAG_ASSERT(index < countResults);
        return oneResult[index];
    }

    void operator=(uint32_t r)
    {
        SWAG_ASSERT(r <= 255);
        oneResult[0] = (uint8_t) r;
        countResults = 1;
        canFree      = true;
    }

    void operator+=(const RegisterList& other)
    {
        for (int i = 0; i < (int) other.size(); i++)
            *this += other[i];
    }

    void operator+=(uint32_t r)
    {
        SWAG_ASSERT(r <= 255);
        SWAG_ASSERT(countResults < MAX_STATIC);
        oneResult[countResults++] = (uint8_t) r;
    }

    void clear()
    {
        countResults = 0;
    }

    operator uint32_t()
    {
        return (*this)[0];
    }
};
