#include "pch.h"
#include "Global.h"
#include "DataSegment.h"
#include "Log.h"
#include "RaceCondition.h"

static const uint32_t BUCKET_SIZE = 16 * 1024;

uint32_t DataSegment::reserve(uint32_t size)
{
    scoped_lock lock(mutex);
    return reserveNoLock(size);
}

uint32_t DataSegment::reserveNoLock(uint32_t size)
{
#ifdef SWAG_HAS_ASSERT
    RaceCondition rc(&raceCondition);
#endif

    DataSegmentHeader* last = nullptr;
    if (buckets.size())
        last = &buckets.back();

    if (last)
    {
        if (last->count + size <= last->size)
        {
            uint32_t result = last->totalCountBefore + last->count;
            last->count += size;
            return result;
        }
    }

    DataSegmentHeader bucket;
    bucket.size             = max(size, BUCKET_SIZE);
    bucket.buffer           = (uint8_t*) calloc(1, bucket.size);
    bucket.count            = size;
    bucket.totalCountBefore = last ? last->totalCountBefore + last->count : 0;
    buckets.emplace_back(bucket);
    return bucket.totalCountBefore;
}

uint8_t* DataSegment::address(uint32_t location)
{
    scoped_lock lock(mutex);
    return addressNoLock(location);
}

uint8_t* DataSegment::addressNoLock(uint32_t location)
{
#ifdef SWAG_HAS_ASSERT
    RaceCondition rc(&raceCondition);
#endif

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

void DataSegment::addInitString(uint32_t segOffset, uint32_t strIndex)
{
    scoped_lock lk(mutexPtr);
    initString[strIndex].push_back(segOffset);
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
