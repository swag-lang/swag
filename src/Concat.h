#pragma once
#pragma once
#include "Assert.h"
#include "Utf8.h"

struct ConcatBucket
{
    uint8_t*      datas      = nullptr;
    ConcatBucket* nextBucket = nullptr;
    int           countBytes = 0;
    int           capacity   = 0;
};

struct Concat
{
    void init(int size = 32 * 1024);
    void clear();
    void release();
    bool hasEnoughSpace(uint32_t numBytes);
    void ensureSpace(int numBytes);

    void addU8_safe(uint8_t v);
    void addU16_safe(uint16_t v);
    void addU32_safe(uint32_t v);
    void addU64_safe(uint64_t v);

    void      addU8(uint8_t v);
    void      addU16(uint16_t v);
    void      addU32(uint32_t v);
    void      addU64(uint64_t v);
    uint32_t* addU32Addr(uint32_t v);
    uint16_t* addU16Addr(uint16_t v);
    void      addS8(int8_t v);
    void      addS16(int16_t v);
    void      addS32(int32_t v);
    void      addS64(int64_t v);
    void      addF32(float v);
    void      addF64(double v);
    void      addBool(bool v);
    void      addPointer(void* v);
    void      addString1(const char* v);
    void      addString2(const char* v);
    void      addString3(const char* v);
    void      addString4(const char* v);
    void      addString5(const char* v);
    void      addString(const char* v, int len);
    void      addString(const char* v);

    void addString(const Utf8& v);
    void addEolIndent(int num);
    void addIndent(int num);
    void addChar(char c);
    void addEol();
    void addStringFormat(const char* format, ...);
    void addU32Str(uint32_t value);
    void addS32Str8(int value);

    bool flushToFile(const string& path);

    uint8_t* getSeekPtr()
    {
        return currentSP;
    }

    uint32_t totalCount()
    {
        if (!lastBucket)
            return 0;
        return totalCountBytes + (int) (currentSP - lastBucket->datas);
    }

    uint8_t* getPtr(int seek)
    {
        SWAG_ASSERT(firstBucket);
        auto ptr = firstBucket;
        while (true)
        {
            if (ptr == lastBucket)
            {
                SWAG_ASSERT(seek < (int) (currentSP - lastBucket->datas));
                return lastBucket->datas + seek;
            }

            if (seek < ptr->countBytes)
                return ptr->datas + seek;
            seek -= ptr->countBytes;
            ptr = ptr->nextBucket;
        }

        return nullptr;
    }

    uint32_t bucketCount(ConcatBucket* b)
    {
        if (b != lastBucket)
            return b->countBytes;
        auto count = (int) (currentSP - lastBucket->datas);
        SWAG_ASSERT(count <= bucketSize);
        return count;
    }

    ConcatBucket* firstBucket     = nullptr;
    ConcatBucket* lastBucket      = nullptr;
    uint8_t*      currentSP       = nullptr;
    int           bucketSize      = 0;
    int           eolCount        = 0;
    uint32_t      totalCountBytes = 0;
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
