#pragma once
#include "Utf8.h"

union Register
{
    volatile uint8_t* pointer = nullptr;
    uint64_t          u64;
    int64_t           s64;
    uint32_t          u32;
    uint32_t          offset;
    int32_t           s32;
    uint16_t          u16;
    int16_t           s16;
    uint8_t           u8;
    int8_t            s8;
    float             f32;
    double            f64;
    char32_t          ch;
    bool              b;
};

struct ComputedValue
{
    Utf8     text;
    Register reg;

    bool operator==(const ComputedValue& from) const
    {
        if (text != from.text)
            return false;
        if (reg.u64 != from.reg.u64)
            return false;
        return true;
    }
};