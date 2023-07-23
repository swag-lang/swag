#pragma once
#include "Assert.h"

union Register
{
    uint8_t* pointer = nullptr;
    uint64_t u64;
    int64_t  s64;
    uint32_t u32;
    int32_t  s32;
    uint16_t u16;
    int16_t  s16;
    uint8_t  u8;
    int8_t   s8;
    float    f32;
    double   f64;
    uint32_t ch;
    bool     b;

    struct
    {
        uint32_t low;
        uint32_t high;
    } u64u32;

    Register() = default;
    Register(uint64_t val)
        : u64{val}
    {
    }
};

struct RegisterList
{
    static const int MAX_STATIC    = 2;
    static const int MAX_REGISTERS = 256;

    uint32_t oneResult[MAX_STATIC] = {0};
    uint8_t  countResults          = 0;
    bool     cannotFree            = false;

    RegisterList()
    {
    }

    RegisterList(uint32_t r)
    {
        SWAG_ASSERT(r < MAX_REGISTERS);
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
        SWAG_ASSERT(r < MAX_REGISTERS);
        oneResult[0] = (uint8_t) r;
        countResults = 1;
        cannotFree   = false;
    }

    void operator+=(const RegisterList& other)
    {
        SWAG_ASSERT(cannotFree == other.cannotFree);
        for (int i = 0; i < (int) other.size(); i++)
            *this += other[i];
    }

    void operator+=(uint32_t r)
    {
        SWAG_ASSERT(!cannotFree);
        SWAG_ASSERT(r < MAX_REGISTERS);
        SWAG_ASSERT(countResults < MAX_STATIC);
        oneResult[countResults++] = (uint8_t) r;
    }

    void clear()
    {
        countResults = 0;
        cannotFree   = false;
    }

    bool operator==(const RegisterList& other) const
    {
        return isSame(other);
    }

    bool isSame(const RegisterList& other) const
    {
        if (countResults != other.countResults)
            return false;
        for (int i = 0; i < countResults; i++)
        {
            if (oneResult[i] != other.oneResult[i])
                return false;
        }

        return true;
    }

    operator uint32_t()
    {
        return (*this)[0];
    }
};
