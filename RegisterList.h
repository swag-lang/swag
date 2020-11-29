#pragma once
#include "Global.h"
#include "Log.h"
#include "VectorNative.h"

struct RegisterList
{
    static const int MAX_STATIC = 2;
    uint32_t         oneResult[MAX_STATIC];
    uint8_t          countResults = 0;
    bool             canFree      = true;

    RegisterList()
    {
    }

    RegisterList(uint32_t r)
    {
        oneResult[0] = r;
        countResults = 1;
    }

    int size() const
    {
        return countResults;
    }

    uint32_t operator[](int index) const
    {
        SWAG_ASSERT(index < MAX_STATIC);
        return oneResult[index];
    }

    void operator=(uint32_t r)
    {
        oneResult[0] = r;
        countResults = 1;
    }

    void operator+=(const RegisterList& other)
    {
        for (int i = 0; i < (int) other.size(); i++)
            *this += other[i];
    }

    void operator+=(uint32_t r)
    {
        SWAG_ASSERT(countResults < MAX_STATIC);
        oneResult[countResults++] = r;
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
