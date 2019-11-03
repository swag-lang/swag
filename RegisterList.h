#pragma once
#include "Global.h"
#include "Log.h"

struct RegisterList
{
    static const int MAX_STATIC = 2;

    uint32_t         oneResult[MAX_STATIC];
    vector<uint32_t> registers;
    int              countResults = 0;
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
        SWAG_ASSERT(index < countResults);
        if (index < MAX_STATIC)
            return oneResult[index];
        return registers[index - MAX_STATIC];
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
        if (countResults >= MAX_STATIC)
            registers.push_back(r);
        else
            oneResult[countResults] = r;
        countResults++;
    }

    void clear()
    {
        countResults = 0;
        registers.clear();
    }

    operator uint32_t()
    {
        return (*this)[0];
    }
};
