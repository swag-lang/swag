#pragma once
#include "Utf8.h"

union Register {
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
    char32_t ch;
    bool     b;
    uint8_t* pointer;
};

struct ComputedValue
{
    Register reg;
    Utf8     text;
};