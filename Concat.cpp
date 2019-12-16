#include "pch.h"
#include "Concat.h"

thread_local Pool<ConcatBucket> g_Pool_concatBucket;

Concat::Concat()
{
    bucketSize = 32 * 1024;
}

void Concat::clear()
{
    firstBucket = nullptr;
    lastBucket  = nullptr;
}

void Concat::checkCount(int offset)
{
    if (lastBucket && lastBucket->count + offset < bucketSize)
    {
        lastBucket->count += offset;
        return;
    }

    if (lastBucket)
        flushBucket(lastBucket);

    auto newBucket = g_Pool_concatBucket.alloc();
    if (!firstBucket)
        firstBucket = newBucket;
    if (lastBucket)
        lastBucket->nextBucket = newBucket;

    lastBucket        = newBucket;
    lastBucket->datas = (uint8_t*) malloc(max(offset, bucketSize));
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

void Concat::addString(const string& v)
{
    auto len = (int) v.length();
    checkCount(len);
    memcpy(currentSP, v.c_str(), len);
    currentSP += len;
}

void Concat::addString(const char* v, int len)
{
    checkCount(len);
    memcpy(currentSP, v, len);
    currentSP += len;
}

void Concat::addString(const char* v)
{
    auto len = (int) strlen(v);
    checkCount(len);
    memcpy(currentSP, v, len);
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
        addString("\t");
}

void Concat::addEolIndent(int num)
{
    addString("\n");
    addIndent(num);
}

void Concat::addStringFormat(const char* format, ...)
{
    static char         buf[4096];
    static shared_mutex lock;
    scoped_lock         lk(lock);
    va_list             args;
    va_start(args, format);
    auto len = vsnprintf(buf, 4096, format, args);
    SWAG_ASSERT(len < 4095);
    va_end(args);
    addString(buf, len);
}