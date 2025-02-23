#include "pch.h"
#include "Core/Concat.h"
#include "Main/Statistics.h"
#include "Os/Os.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"

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
    firstBucket->data     = bucketSize ? Allocator::allocN<uint8_t>(bucketSize) : nullptr;
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
    const auto count = static_cast<uint32_t>(currentSP - lastBucket->data);
    return lastBucket->capacity - count >= numBytes;
}

void Concat::ensureSpace(uint32_t numBytes)
{
    const auto count = static_cast<uint32_t>(currentSP - lastBucket->data);
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

    lastBucket->capacity = std::max(numBytes, bucketSize);
    lastBucket->capacity = Allocator::alignSize(lastBucket->capacity);
    lastBucket->data     = Allocator::allocN<uint8_t>(lastBucket->capacity);

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

void Concat::addStringX(const char* v)
{
    const auto len = static_cast<uint32_t>(strlen(v));
    ensureSpace(len);
    std::copy_n(v, len, currentSP);
    currentSP += len;
}

void Concat::addStringN(const char* v, uint32_t len)
{
    ensureSpace(len);
    std::copy_n(v, len, currentSP);
    currentSP += len;
}

uint8_t* Concat::getSeekPtr() const
{
    return currentSP;
}

uint32_t Concat::totalCount() const
{
    if (!lastBucket)
        return 0;
    return totalCountBytes + static_cast<uint32_t>(currentSP - lastBucket->data);
}

uint8_t* Concat::getPtr(uint32_t seek) const
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

uint32_t Concat::bucketCount(const ConcatBucket* b) const
{
    if (b != lastBucket)
        return b->countBytes;
    const auto count = static_cast<uint32_t>(currentSP - lastBucket->data);
    SWAG_ASSERT(count <= bucketSize);
    return count;
}

bool Concat::flushToFile(const Path& path)
{
    FILE* f = nullptr;
    if (fopen_s(&f, path, "wb"))
    {
        Report::errorOS(formErr(Err0732, path.cstr()));
        return false;
    }

    auto bucket = firstBucket;
    if (bucket)
    {
        while (bucket != lastBucket->nextBucket)
        {
            (void) fwrite(bucket->data, 1, bucketCount(bucket), f);
            bucket = bucket->nextBucket;
        }
    }

    (void) fflush(f);
    (void) fclose(f);
    OS::ensureFileIsWritten(path);

    clear();
    return true;
}

void Concat::makeLinear()
{
    if (firstBucket == lastBucket)
        return;

    auto bucket = firstBucket;

    firstBucket           = Allocator::alloc<ConcatBucket>();
    firstBucket->capacity = totalCount();
    firstBucket->data     = Allocator::allocN<uint8_t>(firstBucket->capacity);

    auto ptr = bucket->data;
    while (bucket != lastBucket->nextBucket)
    {
        std::copy_n(bucket->data, bucket->countBytes, ptr);
        ptr += bucket->countBytes;

        const auto next = bucket->nextBucket;
        Allocator::free(bucket->data, bucket->capacity);
        Allocator::free(bucket, sizeof(ConcatBucket));
        bucket = next;
    }

    lastBucket = firstBucket;
}
