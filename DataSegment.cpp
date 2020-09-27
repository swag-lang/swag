#include "pch.h"
#include "Global.h"
#include "DataSegment.h"
#include "Log.h"
#include "RaceCondition.h"
#include "SourceFile.h"
#include "Module.h"
#include "swag_runtime.h"

static const uint32_t BUCKET_SIZE = 16 * 1024;

uint32_t DataSegment::reserve(uint32_t size, bool setZero)
{
    scoped_lock lock(mutex);
    return reserveNoLock(size, setZero);
}

void DataSegment::initFrom(DataSegment* other)
{
    if (other->totalCount)
    {
        reserve(other->totalCount);
        memcpy(buckets[0].buffer, other->buckets[0].buffer, other->totalCount);
    }

    SWAG_ASSERT(initPtr.count == 0);
    for (auto& it : other->initPtr)
        addInitPtr(it.patchOffset, it.srcOffset, it.fromSegment);
}

uint32_t DataSegment::reserveNoLock(uint32_t size, bool setZero)
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
            if (setZero)
                memset(last->buffer + last->count, 0, size);
            last->count += size;
            totalCount += size;
            return result;
        }
    }

    DataSegmentHeader bucket;
    bucket.size   = max(size, BUCKET_SIZE);
    bucket.buffer = (uint8_t*) g_Allocator.alloc(bucket.size);
    memset(bucket.buffer, 0, bucket.size);
    bucket.count            = size;
    bucket.totalCountBefore = last ? last->totalCountBefore + last->count : 0;
    buckets.push_back(bucket);
    totalCount += size;

    if (setZero)
    {
        last = &buckets.back();
        memset(last->buffer, 0, size);
    }

    return bucket.totalCountBefore;
}

uint32_t DataSegment::offset(uint8_t* location)
{
    shared_lock lock(mutex);
    for (int i = 0; i < buckets.size(); i++)
    {
        auto bucket = &buckets[i];
        if (location >= bucket->buffer && location < bucket->buffer + bucket->count)
        {
            auto offset = (uint32_t)(location - bucket->buffer);
            offset += bucket->totalCountBefore;
            return offset;
        }
    }

    SWAG_ASSERT(false);
    return 0;
}

uint8_t* DataSegment::address(uint32_t location)
{
    shared_lock lock(mutex);
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
    swag_runtime_memcpy(addr, str.c_str(), strLen);
    mapString[str] = offset;

    return offset;
}

uint32_t DataSegment::addString(const Utf8& str)
{
    scoped_lock lk(mutex);
    return addStringNoLock(str);
}

void DataSegment::addInitPtr(uint32_t patchOffset, uint32_t srcOffset, SegmentKind seg)
{
    if (compilerOnly)
        return;

    DataSegmentRef ref;
    ref.patchOffset = patchOffset;
    ref.srcOffset   = srcOffset;
    ref.fromSegment = seg;

#ifdef SWAG_HAS_ASSERT
    // We must have at least one pointer difference with all other offsets, otherwise we will
    // have a weird memory overwrite
    for (int i = 0; i < initPtr.size(); i++)
        SWAG_ASSERT(abs((int) initPtr[i].patchOffset - (int) patchOffset) >= 8);
#endif

    scoped_lock lk(mutexPtr);
    initPtr.push_back(ref);
}

void DataSegment::addInitPtrFunc(uint32_t offset, const Utf8& funcName, RelocType relocType)
{
    if (compilerOnly)
        return;

    scoped_lock lk(mutexPtr);
    initFuncPtr[offset] = {funcName, relocType};
}

bool DataSegment::readU64(Seek& seek, uint64_t& result)
{
    if (seek.seekBucket == buckets.size())
        return false;

    auto* curBucket = &buckets[seek.seekBucket];
    if (seek.seekRead + 8 <= curBucket->count)
    {
        uint64_t* ptr = (uint64_t*) (curBucket->buffer + seek.seekRead);
        seek.seekRead += 8;
        result = *ptr;
        return true;
    }

    int cpt              = 0;
    result               = 0;
    uint8_t* ptr         = curBucket->buffer + seek.seekRead;
    uint32_t shift       = 0;
    bool     resultValid = false;
    while (cpt != 8 && seek.seekBucket != buckets.size())
    {
        while (cpt != 8 && (seek.seekRead < curBucket->count))
        {
            result |= ((uint64_t) *ptr) << shift;
            shift += 8;
            cpt++;
            ptr++;
            seek.seekRead++;
            resultValid = true;
        }

        if (seek.seekRead == curBucket->count)
        {
            seek.seekBucket++;
            if (seek.seekBucket != buckets.size())
            {
                seek.seekRead = 0;
                curBucket     = &buckets[seek.seekBucket];
                ptr           = curBucket->buffer;
            }
            else if (!resultValid)
                return false;
        }
    }

    return true;
}

void DataSegment::saveValue(void* address, uint32_t size)
{
    auto it = savedValues.find(address);
    if (it != savedValues.end())
        return;

    switch (size)
    {
    case 1:
        savedValues[address] = {(void*) (size_t) * (uint8_t*) address, size};
        break;
    case 2:
        savedValues[address] = {(void*) (size_t) * (uint16_t*) address, size};
        break;
    case 4:
        savedValues[address] = {(void*) (size_t) * (uint32_t*) address, size};
        break;
    case 8:
        savedValues[address] = {(void*) (size_t) * (uint64_t*) address, size};
        break;
    default:
        auto buf = g_Allocator.alloc(size);
        swag_runtime_memcpy(buf, address, size);
        savedValues[address] = {buf, size};
        break;
    }
}

void DataSegment::restoreAllValues()
{
    for (auto& one : savedValues)
    {
        switch (one.second.second)
        {
        case 1:
            *(uint8_t*) one.first = (uint8_t) * (uint8_t*) &one.second.first;
            break;
        case 2:
            *(uint16_t*) one.first = (uint16_t) * (uint16_t*) &one.second.first;
            break;
        case 4:
            *(uint32_t*) one.first = (uint32_t) * (uint32_t*) &one.second.first;
            break;
        case 8:
            *(uint64_t*) one.first = (uint64_t) * (uint64_t*) &one.second.first;
            break;
        default:
            swag_runtime_memcpy(one.first, one.second.first, one.second.second);
            break;
        }
    }

    savedValues.clear();
}

void DataSegment::release()
{
}