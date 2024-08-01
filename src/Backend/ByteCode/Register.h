#pragma once
#include "Report/Assert.h"

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
    } mergeU64U32;

    Register() = default;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    Register(uint64_t val) :
        u64{val}
    {
    }
};

struct RegisterList
{
    static constexpr int MAX_STATIC = 2;

    uint32_t oneResult[MAX_STATIC] = {};
    uint8_t  countResults          = 0;
    bool     cannotFree            = false;
    uint8_t  padding               = 0;

    RegisterList() = default;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    RegisterList(uint32_t r)
    {
        oneResult[0] = r;
        countResults = 1;
    }

    uint32_t size() const
    {
        return countResults;
    }

    uint32_t operator[](uint32_t index) const
    {
        SWAG_ASSERT(index < countResults);
        return oneResult[index];
    }

    RegisterList& operator=(uint32_t r)
    {
        oneResult[0] = r;
        countResults = 1;
        cannotFree   = false;
        return *this;
    }

    void operator+=(const RegisterList& other)
    {
        SWAG_ASSERT(cannotFree == other.cannotFree);
        for (uint32_t i = 0; i < other.size(); i++)
            *this += other[i];
    }

    void operator+=(uint32_t r)
    {
        SWAG_ASSERT(!cannotFree);
        SWAG_ASSERT(countResults < MAX_STATIC);
        oneResult[countResults++] = r;
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

    // ReSharper disable once CppNonExplicitConversionOperator
    operator uint32_t() const
    {
        return (*this)[0];
    }
};
