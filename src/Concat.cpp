#include "pch.h"
#include "Concat.h"
#include "ErrorIds.h"
#include "Os.h"
#include "Report.h"

void Concat::init(int size)
{
    if (bucketSize)
    {
        lastBucket      = firstBucket;
        currentSP       = lastBucket->data;
        totalCountBytes = 0;
        return;
    }

    bucketSize            = size;
    firstBucket           = Allocator::alloc<ConcatBucket>();
    firstBucket->data     = bucketSize ? static_cast<uint8_t*>(Allocator::alloc(bucketSize)) : nullptr;
    firstBucket->capacity = size;

#ifdef SWAG_STATS
    g_Stats.memConcat += sizeof(ConcatBucket);
    g_Stats.memConcat += bucketSize;
#endif

    lastBucket = firstBucket;
    currentSP  = lastBucket->data;
}

void Concat::clear()
{
    totalCountBytes += bucketCount(lastBucket);
    lastBucket             = firstBucket;
    currentSP              = lastBucket->data;
    lastBucket->countBytes = 0;
}

void Concat::release()
{
    auto p = firstBucket;
    while (p)
    {
        Allocator::free(p->data, p->capacity);
        const auto n = p->nextBucket;
        Allocator::free(p, sizeof(ConcatBucket));
        p = n;
    }

    firstBucket     = nullptr;
    lastBucket      = nullptr;
    currentSP       = nullptr;
    totalCountBytes = 0;
}

void Concat::align(uint32_t align)
{
    while (totalCount() % align)
        addU8(0);
}

bool Concat::hasEnoughSpace(uint32_t numBytes) const
{
    const auto count = static_cast<int>(currentSP - lastBucket->data);
    return lastBucket->capacity - count >= static_cast<int>(numBytes);
}

void Concat::ensureSpace(int numBytes)
{
    const auto count = static_cast<int>(currentSP - lastBucket->data);
    if (count + numBytes <= lastBucket->capacity)
        return;

    SWAG_ASSERT(bucketSize);
    totalCountBytes += count;
    lastBucket->countBytes = count;

    // Next is already allocated
    if (lastBucket->nextBucket)
    {
        lastBucket             = lastBucket->nextBucket;
        currentSP              = lastBucket->data;
        lastBucket->countBytes = 0;
        SWAG_ASSERT(numBytes <= lastBucket->capacity);
        return;
    }

    // Need to allocate a new bucket
    const auto newBucket = Allocator::alloc<ConcatBucket>();
    SWAG_ASSERT(firstBucket && lastBucket);
    lastBucket->nextBucket = newBucket;
    lastBucket             = newBucket;

    lastBucket->capacity = max(numBytes, bucketSize);
    lastBucket->capacity = static_cast<int>(Allocator::alignSize(lastBucket->capacity));
    lastBucket->data     = static_cast<uint8_t*>(Allocator::alloc(lastBucket->capacity));

#ifdef SWAG_STATS
    g_Stats.memConcat += sizeof(ConcatBucket);
    g_Stats.memConcat += lastBucket->capacity;
#endif

    currentSP = lastBucket->data;
}

void Concat::addBool(bool v)
{
    ensureSpace(sizeof(bool));
    *reinterpret_cast<bool*>(currentSP) = v;
    currentSP += sizeof(bool);
}

void Concat::addU8(uint8_t v)
{
    ensureSpace(sizeof(uint8_t));
    *currentSP = v;
    currentSP += sizeof(uint8_t);
}

void Concat::addU16(uint16_t v)
{
    ensureSpace(sizeof(uint16_t));
    *reinterpret_cast<uint16_t*>(currentSP) = v;
    currentSP += sizeof(uint16_t);
}

void Concat::addU32(uint32_t v)
{
    ensureSpace(sizeof(uint32_t));
    *reinterpret_cast<uint32_t*>(currentSP) = v;
    currentSP += sizeof(uint32_t);
}

void Concat::addU64(uint64_t v)
{
    ensureSpace(sizeof(uint64_t));
    *reinterpret_cast<uint64_t*>(currentSP) = v;
    currentSP += sizeof(uint64_t);
}

uint32_t* Concat::addU32Addr(uint32_t v)
{
    ensureSpace(sizeof(uint32_t));
    *reinterpret_cast<uint32_t*>(currentSP) = v;
    currentSP += sizeof(uint32_t);
    return reinterpret_cast<uint32_t*>(currentSP - sizeof(uint32_t));
}

uint16_t* Concat::addU16Addr(uint16_t v)
{
    ensureSpace(sizeof(uint16_t));
    *reinterpret_cast<uint16_t*>(currentSP) = v;
    currentSP += sizeof(uint16_t);
    return reinterpret_cast<uint16_t*>(currentSP - sizeof(uint16_t));
}

void Concat::addS8(int8_t v)
{
    ensureSpace(sizeof(int8_t));
    *reinterpret_cast<int8_t*>(currentSP) = v;
    currentSP += sizeof(int8_t);
}

void Concat::addS16(int16_t v)
{
    ensureSpace(sizeof(int16_t));
    *reinterpret_cast<int16_t*>(currentSP) = v;
    currentSP += sizeof(int16_t);
}

void Concat::addS32(int32_t v)
{
    ensureSpace(sizeof(int32_t));
    *reinterpret_cast<int32_t*>(currentSP) = v;
    currentSP += sizeof(int32_t);
}

void Concat::addS64(int64_t v)
{
    ensureSpace(sizeof(int64_t));
    *reinterpret_cast<int64_t*>(currentSP) = v;
    currentSP += sizeof(int64_t);
}

void Concat::addF32(float v)
{
    ensureSpace(sizeof(float));
    *reinterpret_cast<float*>(currentSP) = v;
    currentSP += sizeof(float);
}

void Concat::addF64(double v)
{
    ensureSpace(sizeof(double));
    *reinterpret_cast<double*>(currentSP) = v;
    currentSP += sizeof(double);
}

void Concat::addPointer(void* v)
{
    ensureSpace(sizeof(void*));
    *reinterpret_cast<void**>(currentSP) = v;
    currentSP += sizeof(void*);
}

void Concat::addString(const Utf8& v)
{
    const auto len = static_cast<int>(v.length());
    ensureSpace(len);
    memcpy(currentSP, v.buffer, len);
    currentSP += len;
}

void Concat::addString1(const char* v)
{
    ensureSpace(1);
    *currentSP = *reinterpret_cast<const uint8_t*>(v);
    currentSP += 1;
}

void Concat::addString2(const char* v)
{
    ensureSpace(2);
    *reinterpret_cast<uint16_t*>(currentSP) = *reinterpret_cast<const uint16_t*>(v);
    currentSP += 2;
}

void Concat::addString3(const char* v)
{
    ensureSpace(3);
    *reinterpret_cast<uint16_t*>(currentSP) = *reinterpret_cast<const uint16_t*>(v);
    currentSP[2]                            = v[2];
    currentSP += 3;
}

void Concat::addString4(const char* v)
{
    ensureSpace(4);
    *reinterpret_cast<uint32_t*>(currentSP) = *reinterpret_cast<const uint32_t*>(v);
    currentSP += 4;
}

void Concat::addString5(const char* v)
{
    ensureSpace(5);
    *reinterpret_cast<uint32_t*>(currentSP) = *reinterpret_cast<const uint32_t*>(v);
    currentSP[4]                            = v[4];
    currentSP += 5;
}

void Concat::addString(const char* v, int len)
{
    ensureSpace(len);
    memcpy(currentSP, v, len);
    currentSP += len;
}

void Concat::addString(const char* v)
{
    const auto len = static_cast<int>(strlen(v));
    ensureSpace(len);
    memcpy(currentSP, v, len);
    currentSP += len;
}

void Concat::addChar(char c)
{
    ensureSpace(1);
    *currentSP++ = c;
}

void Concat::addEol()
{
    ensureSpace(1);
    *currentSP++ = '\n';
    eolCount++;
}

void Concat::addIndent(int num)
{
    while (num--)
        addChar('\t');
}

void Concat::addEolIndent(int num)
{
    auto p = currentSP;
    while (p != lastBucket->data)
    {
        p--;
        if (SWAG_IS_BLANK(*p))
            continue;
        if (*p == '\n')
            return;
        break;
    }

    addEol();
    addIndent(num);
}

void Concat::addStringFormat(const char* format, ...)
{
    char    buf[4096];
    va_list args;
    va_start(args, format);
    const auto len = vsnprintf(buf, 4096, format, args);
    SWAG_ASSERT(len < 4095);
    va_end(args);
    addString(buf, len);
}

void Concat::addU32Str(uint32_t value)
{
    if (value < 10)
        addChar(static_cast<char>(value + '0'));
    else
        addString(to_string(value));
}

void Concat::addS32Str8(int value)
{
    ensureSpace(8);

    currentSP += 8;
    auto pz = currentSP;

    for (int i = 0; i < 8; i++)
    {
        pz--;
        if (!value)
            *pz = '0';
        else
        {
            const auto id = value - 10 * (value / 10);
            value /= 10;
            *pz = static_cast<char>('0' + id);
        }
    }
}

bool Concat::flushToFile(const Path& path)
{
    FILE* f = nullptr;
    if (fopen_s(&f, path.string().c_str(), "wb"))
    {
        Report::errorOS(FMT(Err(Err0096), path.string().c_str()));
        return false;
    }

    auto bucket = firstBucket;
    while (bucket != lastBucket->nextBucket)
    {
        fwrite(bucket->data, 1, bucketCount(bucket), f);
        bucket = bucket->nextBucket;
    }

    fflush(f);
    fclose(f);
    OS::ensureFileIsWritten(path.string().c_str());

    clear();
    return true;
}
