#pragma once
#include "Assert.h"
struct Utf8;
struct Path;

struct ConcatBucket
{
    uint8_t*      data       = nullptr;
    ConcatBucket* nextBucket = nullptr;
    uint32_t      countBytes = 0;
    uint32_t      capacity   = 0;
};

struct Concat
{
    void init(int size = 32 * 1024);
    void clear();
    void release();
    void ensureSpace(uint32_t numBytes);
    void align(uint32_t align);

    bool hasEnoughSpace(uint32_t numBytes) const;
    bool flushToFile(const Path& path);

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
    void      addString(const char* v, uint32_t len);
    void      addString(const char* v);
    void      addString(const Utf8& v);
    void      addEolIndent(uint32_t num);
    void      addIndent(uint32_t num);
    void      addChar(char c);
    void      addEol();
    void      addStringFormat(const char* format, ...);
    void      addU32Str(uint32_t value);
    void      addS32Str8(int value);

    template<typename T>
    T* addObj()
    {
        ensureSpace(sizeof(T));
        ::new(currentSP) T;
        auto result = reinterpret_cast<T*>(currentSP);
        currentSP += sizeof(T);
        return result;
    }

    uint8_t* getSeekPtr() const
    {
        return currentSP;
    }

    uint32_t totalCount() const
    {
        if (!lastBucket)
            return 0;
        return totalCountBytes + static_cast<uint32_t>(currentSP - lastBucket->data);
    }

    uint8_t* getPtr(uint32_t seek) const
    {
        SWAG_ASSERT(firstBucket);
        auto ptr = firstBucket;
        while (true)
        {
            if (ptr == lastBucket)
            {
                SWAG_ASSERT(seek < static_cast<uint32_t>(currentSP - lastBucket->data));
                return lastBucket->data + seek;
            }

            if (seek < ptr->countBytes)
                return ptr->data + seek;
            seek -= ptr->countBytes;
            ptr = ptr->nextBucket;
        }
    }

    uint32_t bucketCount(const ConcatBucket* b) const
    {
        if (b != lastBucket)
            return b->countBytes;
        const auto count = static_cast<uint32_t>(currentSP - lastBucket->data);
        SWAG_ASSERT(count <= bucketSize);
        return count;
    }

    ConcatBucket* firstBucket = nullptr;
    ConcatBucket* lastBucket  = nullptr;
    uint8_t*      currentSP   = nullptr;

    uint32_t bucketSize      = 0;
    uint32_t eolCount        = 0;
    uint32_t totalCountBytes = 0;
};

#define CONCAT_FIXED_STR(__concat, __str)                                    \
    do                                                                       \
    {                                                                        \
        static constexpr int __len = (int) char_traits<char>::length(__str); \
        (__concat).addString(__str, __len);                                  \
    } while (0)

#define CONCAT_STR_1(__concat, __before, __int, __after) \
    do                                                   \
    {                                                    \
        CONCAT_FIXED_STR(__concat, __before);            \
        (__concat).addU32Str(__int);                     \
        CONCAT_FIXED_STR(__concat, __after);             \
    } while (0)

#define CONCAT_STR_2(__concat, __before, __int1, __middle, __int2, __after) \
    do                                                                      \
    {                                                                       \
        CONCAT_FIXED_STR(__concat, __before);                               \
        (__concat).addU32Str(__int1);                                       \
        CONCAT_FIXED_STR(__concat, __middle);                               \
        (__concat).addU32Str(__int2);                                       \
        CONCAT_FIXED_STR(__concat, __after);                                \
    } while (0)
