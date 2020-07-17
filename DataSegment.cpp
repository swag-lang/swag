#include "pch.h"
#include "Global.h"
#include "DataSegment.h"
#include "Log.h"
#include "RaceCondition.h"
#include "SourceFile.h"
#include "Module.h"

static const uint32_t BUCKET_SIZE = 16 * 1024;

uint32_t DataSegment::reserve(uint32_t size)
{
    scoped_lock lock(mutex);
    return reserveNoLock(size);
}

uint32_t DataSegment::reserveNoLock(uint32_t size)
{
    SWAG_ASSERT(size);
    SWAG_RACE_CONDITION_WRITE(raceCondition);
    DataSegmentHeader* last = nullptr;
    if (buckets.size())
        last = &buckets.back();

    if (last)
    {
        if (last->count + size <= last->size)
        {
            uint32_t result = last->totalCountBefore + last->count;
            last->count += size;
            totalCount += size;
            return result;
        }
    }

    DataSegmentHeader bucket;
    bucket.size             = max(size, BUCKET_SIZE);
    bucket.buffer           = (uint8_t*) calloc(1, bucket.size);
    bucket.count            = size;
    bucket.totalCountBefore = last ? last->totalCountBefore + last->count : 0;
    buckets.emplace_back(bucket);
    totalCount += size;

    return bucket.totalCountBefore;
}

uint8_t* DataSegment::address(uint32_t location)
{
    scoped_lock lock(mutex);
    return addressNoLock(location);
}

uint8_t* DataSegment::addressNoLock(uint32_t location)
{
    SWAG_ASSERT(location != UINT32_MAX);
    SWAG_RACE_CONDITION_READ(raceCondition);
    SWAG_ASSERT(buckets.size());
    for (int i = 0; i < buckets.size(); i++)
    {
        auto bucket = &buckets[i];
        if (location < bucket->totalCountBefore + bucket->count)
        {
            location -= bucket->totalCountBefore;
            return bucket->buffer + location;
        }
    }

    SWAG_ASSERT(false);
    return nullptr;
}

uint32_t DataSegment::addComputedValueNoLock(SourceFile* sourceFile, TypeInfo* typeInfo, ComputedValue& computedValue)
{
    SWAG_ASSERT(typeInfo->kind == TypeInfoKind::Native);
    SWAG_ASSERT(typeInfo->nativeType != NativeTypeKind::Any);

    if (typeInfo->nativeType == NativeTypeKind::String)
    {
        auto stringOffset     = addStringNoLock(computedValue.text);
        auto storageOffset    = reserveNoLock(2 * sizeof(uint64_t));
        auto addr             = addressNoLock(storageOffset);
        ((uint64_t*) addr)[0] = (uint64_t) computedValue.text.c_str();
        ((uint64_t*) addr)[1] = (uint32_t) computedValue.text.length();
        addInitPtr(storageOffset, stringOffset);
        return storageOffset;
    }

    switch (typeInfo->sizeOf)
    {
    case 1:
    {
        auto it = storedValues8.find(computedValue.reg.u8);
        if (it != storedValues8.end())
            return it->second;
        break;
    }

    case 2:
    {
        auto it = storedValues16.find(computedValue.reg.u16);
        if (it != storedValues16.end())
            return it->second;
        break;
    }

    case 4:
    {
        auto it = storedValues32.find(computedValue.reg.u32);
        if (it != storedValues32.end())
            return it->second;
        break;
    }
    case 8:
    {
        auto it = storedValues64.find(computedValue.reg.u64);
        if (it != storedValues64.end())
            return it->second;
        break;
    }
    }

    auto storageOffset = reserveNoLock(typeInfo->sizeOf);
    auto addr          = addressNoLock(storageOffset);
    switch (typeInfo->sizeOf)
    {
    case 1:
        *(uint8_t*) addr                    = computedValue.reg.u8;
        storedValues8[computedValue.reg.u8] = storageOffset;
        break;
    case 2:
        *(uint16_t*) addr                     = computedValue.reg.u16;
        storedValues16[computedValue.reg.u16] = storageOffset;
        break;
    case 4:
        *(uint32_t*) addr                     = computedValue.reg.u32;
        storedValues32[computedValue.reg.u32] = storageOffset;
        break;
    case 8:
        *(uint64_t*) addr                     = computedValue.reg.u64;
        storedValues64[computedValue.reg.u64] = storageOffset;
        break;
    }

    return storageOffset;
}

uint32_t DataSegment::addStringNoLock(const Utf8& str)
{
    // Same string already there ?
    auto it = mapString.find(str);
    if (it != mapString.end())
        return it->second;

    auto strLen = (uint32_t) str.length() + 1;
    auto offset = reserveNoLock(strLen);
    auto addr   = addressNoLock(offset);
    memcpy(addr, str.c_str(), strLen);
    mapString[str] = offset;

    return offset;
}

uint32_t DataSegment::addString(const Utf8& str)
{
    scoped_lock lk(mutex);
    return addStringNoLock(str);
}

void DataSegment::addInitPtr(uint32_t fromOffset, uint32_t toOffset, SegmentKind seg)
{
    DataSegmentRef ref;
    ref.sourceOffset = fromOffset;
    ref.destOffset   = toOffset;
    ref.destSeg      = seg;

    scoped_lock lk(mutexPtr);
    initPtr.push_back(ref);
}

void DataSegment::addInitPtrFunc(uint32_t offset, ByteCode* bc)
{
    scoped_lock lk(mutexPtr);
    initFuncPtr[offset] = bc;
}