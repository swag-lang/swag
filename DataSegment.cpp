#include "pch.h"
#include "Global.h"
#include "DataSegment.h"
#include "Log.h"

static const uint32_t BUCKET_SIZE = 1024;

DataSegmentLocation DataSegment::reserve(uint32_t size)
{
    scoped_lock lock(mutex);
    return reserveNoLock(size);
}

DataSegmentLocation DataSegment::reserveNoLock(uint32_t size)
{
    auto last = &buckets.back();
    if (last->count + size <= last->size)
    {
        last->count += size;
        return {(uint32_t) buckets.size() - 1, last->count - size};
    }

    DataSegmentHeader bucket;
    bucket.size   = max(size, BUCKET_SIZE);
    bucket.buffer = (uint8_t*) malloc(bucket.size);
    bucket.count  = size;

    buckets.emplace_back(bucket);
    last = &buckets.back();

    return {(uint32_t) buckets.size() - 1, last->count};
}

uint8_t* DataSegment::address(DataSegmentLocation& location)
{
    scoped_lock lock(mutex);
    SWAG_ASSERT(location.bucket < (uint32_t) buckets.size());
    SWAG_ASSERT(location.offset < buckets[location.bucket].count);
    return buckets[location.bucket].buffer + location.offset;
}