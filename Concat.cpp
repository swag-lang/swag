#include "pch.h"
#include "Concat.h"
#include "Allocator.h"
#include "SwagRuntime.h"

Concat::Concat()
{
    bucketSize = 32 * 1024;
}

void Concat::clear()
{
    auto bucket = firstBucket;
    while (bucket)
    {
        bucket->count = 0;
        bucket        = bucket->nextBucket;
    }

    lastBucket = firstBucket;
    if (lastBucket)
        currentSP = lastBucket->datas;
}

void Concat::checkCount(int offset)
{
    totalCount += offset;

    if (lastBucket)
    {
        if (lastBucket->count + offset < bucketSize)
        {
            lastBucket->count += offset;
            return;
        }

        // Next is already allocated
        if (lastBucket->nextBucket)
        {
            lastBucket        = lastBucket->nextBucket;
            currentSP         = lastBucket->datas;
            lastBucket->count = offset;
            return;
        }
    }

    // Need to allocate a new bucket
    auto newBucket = g_Allocator.alloc<ConcatBucket>();
    if (!firstBucket)
        firstBucket = newBucket;
    if (lastBucket)
        lastBucket->nextBucket = newBucket;

    lastBucket = newBucket;
    SWAG_ASSERT(offset < bucketSize);
    lastBucket->datas = (uint8_t*) malloc(bucketSize);
    currentSP         = newBucket->datas;
    newBucket->count  = offset;
}

void Concat::addBool(bool v)
{
    checkCount(sizeof(bool));
    *(bool*) currentSP = v;
    currentSP += sizeof(bool);
}

void Concat::addU8(uint8_t v)
{
    checkCount(sizeof(uint8_t));
    *(uint8_t*) currentSP = v;
    currentSP += sizeof(uint8_t);
}

void Concat::addU16(uint16_t v)
{
    checkCount(sizeof(uint16_t));
    *(uint16_t*) currentSP = v;
    currentSP += sizeof(uint16_t);
}

void Concat::addU32(uint32_t v)
{
    checkCount(sizeof(uint32_t));
    *(uint32_t*) currentSP = v;
    currentSP += sizeof(uint32_t);
}

void Concat::addU64(uint64_t v)
{
    checkCount(sizeof(uint64_t));
    *(uint64_t*) currentSP = v;
    currentSP += sizeof(uint64_t);
}

uint32_t* Concat::addU32Addr(uint32_t v)
{
    checkCount(sizeof(uint32_t));
    *(uint32_t*) currentSP = v;
    currentSP += sizeof(uint32_t);
    return (uint32_t*) (currentSP - sizeof(uint32_t));
}

uint16_t* Concat::addU16Addr(uint16_t v)
{
    checkCount(sizeof(uint16_t));
    *(uint16_t*) currentSP = v;
    currentSP += sizeof(uint16_t);
    return (uint16_t*) (currentSP - sizeof(uint16_t));
}

void Concat::addS8(int8_t v)
{
    checkCount(sizeof(int8_t));
    *(int8_t*) currentSP = v;
    currentSP += sizeof(int8_t);
}

void Concat::addS16(int16_t v)
{
    checkCount(sizeof(int16_t));
    *(int16_t*) currentSP = v;
    currentSP += sizeof(int16_t);
}

void Concat::addS32(int32_t v)
{
    checkCount(sizeof(int32_t));
    *(int32_t*) currentSP = v;
    currentSP += sizeof(int32_t);
}

void Concat::addS64(int64_t v)
{
    checkCount(sizeof(int64_t));
    *(int64_t*) currentSP = v;
    currentSP += sizeof(int64_t);
}

void Concat::addF32(float v)
{
    checkCount(sizeof(float));
    *(float*) currentSP = v;
    currentSP += sizeof(float);
}

void Concat::addF64(double v)
{
    checkCount(sizeof(double));
    *(double*) currentSP = v;
    currentSP += sizeof(double);
}

void Concat::addPointer(void* v)
{
    checkCount(sizeof(void*));
    *(void**) currentSP = v;
    currentSP += sizeof(void*);
}

void Concat::addString(const Utf8& v)
{
    auto len = (int) v.length();
    checkCount(len);
    swag_runtime_memcpy(currentSP, v.c_str(), len);
    currentSP += len;
}

void Concat::addString1(const char* v)
{
    checkCount(1);
    *(uint8_t*) currentSP = *(uint8_t*) v;
    currentSP += 1;
}

void Concat::addString2(const char* v)
{
    checkCount(2);
    *(uint16_t*) currentSP = *(uint16_t*) v;
    currentSP += 2;
}

void Concat::addString3(const char* v)
{
    checkCount(3);
    *(uint16_t*)currentSP = *(uint16_t*)v;
    currentSP[2] = v[2];
    currentSP += 3;
}

void Concat::addString4(const char* v)
{
    checkCount(4);
    *(uint32_t*) currentSP = *(uint32_t*) v;
    currentSP += 4;
}

void Concat::addString(const char* v, int len)
{
    checkCount(len);
    swag_runtime_memcpy(currentSP, v, len);
    currentSP += len;
}

void Concat::addString(const char* v)
{
    auto len = (int) strlen(v);
    checkCount(len);
    swag_runtime_memcpy(currentSP, v, len);
    currentSP += len;
}

void Concat::addChar(char c)
{
    checkCount(1);
    *currentSP++ = c;
}

void Concat::addEol()
{
    checkCount(1);
    *currentSP++ = '\n';
}

void Concat::addIndent(int num)
{
    while (num--)
        addChar('\t');
}

void Concat::addEolIndent(int num)
{
    addEol();
    addIndent(num);
}

void Concat::addStringFormat(const char* format, ...)
{
    char    buf[4096];
    va_list args;
    va_start(args, format);
    auto len = vsnprintf(buf, 4096, format, args);
    SWAG_ASSERT(len < 4095);
    va_end(args);
    addString(buf, len);
}

void Concat::addU32Str(uint32_t value)
{
    if (value < 10)
        addChar((char) (value + '0'));
    else
        addString(to_string(value));
}

void Concat::addS32Str8(int value)
{
    checkCount(8);

    currentSP += 8;
    auto pz = currentSP;

    for (int i = 0; i < 8; i++)
    {
        pz--;
        if (!value)
            *pz = '0';
        else
        {
            auto id = value - (10 * (value / 10));
            value /= 10;
            *pz = (char) ('0' + id);
        }
    }
}
