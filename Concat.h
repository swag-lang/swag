#pragma once
#include "Pool.h"
#include "Global.h"

struct ConcatBucket
{
    uint8_t*      datas      = nullptr;
    ConcatBucket* nextBucket = nullptr;
    int           count      = 0;
};

struct Concat
{
    Concat();
    void clear();
    void checkCount(int offset);

    void  addU8(uint8_t v);
    void  addU16(uint16_t v);
    void  addU32(uint32_t v);
    void* addU32Addr(uint32_t v);
    void  addU64(uint64_t v);
    void  addS8(int8_t v);
    void  addS16(int16_t v);
    void  addS32(int32_t v);
    void  addS64(int64_t v);
    void  addF32(float v);
    void  addF64(double v);
    void  addBool(bool v);
    void  addPointer(void* v);
    void  addString(const char* v, int len);
    void  addString(const char* v);
    void  addString(const Utf8& v);
    void  addEolIndent(int num);
    void  addIndent(int num);
    void  addChar(char c);
    void  addEol();
    void  addStringFormat(const char* format, ...);
    void  addU32Str(uint32_t value);
    void  addS32Str8(int value);

    ConcatBucket* firstBucket = nullptr;
    ConcatBucket* lastBucket  = nullptr;
    uint8_t*      currentSP   = nullptr;
    int           bucketSize  = 4 * 1024;
    uint32_t      totalCount  = 0;
};

#define CONCAT_FIXED_STR(__concat, __str)                                    \
    do                                                                       \
    {                                                                        \
        static constexpr int __len = (int) char_traits<char>::length(__str); \
        __concat.addString(__str, __len);                                    \
    } while (false)

#define CONCAT_STR_1(__concat, __before, __int, __after) \
    do                                                   \
    {                                                    \
        CONCAT_FIXED_STR(__concat, __before);            \
        __concat.addU32Str(__int);                       \
        CONCAT_FIXED_STR(__concat, __after);             \
    } while (false)

#define CONCAT_STR_2(__concat, __before, __int1, __middle, __int2, __after) \
    do                                                                      \
    {                                                                       \
        CONCAT_FIXED_STR(__concat, __before);                               \
        __concat.addU32Str(__int1);                                         \
        CONCAT_FIXED_STR(__concat, __middle);                               \
        __concat.addU32Str(__int2);                                         \
        CONCAT_FIXED_STR(__concat, __after);                                \
    } while (false)
