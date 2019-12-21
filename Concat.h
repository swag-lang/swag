#pragma once
#include "Pool.h"
#include "ConcatBucket.h"
#include "Global.h"

struct Concat
{
    Concat();
    void clear();
    void checkCount(int offset);

    void addU8(uint8_t v);
    void addU16(uint16_t v);
    void addU32(uint32_t v);
    void addU64(uint64_t v);
    void addS8(int8_t v);
    void addS16(int16_t v);
    void addS32(int32_t v);
    void addS64(int64_t v);
    void addF32(float v);
    void addF64(double v);
    void addBool(bool v);
    void addPointer(void* v);
    void addString(const char* v, int len);
    void addString(const char* v);
    void addString(const string& v);
    void addEolIndent(int num);
    void addIndent(int num);
    void addChar(char c);
    void addEol();
    void addStringFormat(const char* format, ...);

    ConcatBucket* firstBucket = nullptr;
    ConcatBucket* lastBucket  = nullptr;
    uint8_t*      currentSP   = nullptr;
    int           bucketSize  = 4 * 1024;
};

#define CONCAT_FIXED_STR(__concat, __str)                                    \
    do                                                                       \
    {                                                                        \
        static constexpr int __len = (int) char_traits<char>::length(__str); \
        __concat.addString(__str, __len);                                    \
    } while (false)
