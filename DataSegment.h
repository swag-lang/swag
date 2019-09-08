#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "BuildPass.h"
#include "BackendParameters.h"
#include "TypeTable.h"

struct DataSegmentHeader
{
    uint8_t* buffer;
    uint32_t count;
    uint32_t size;
};

struct DataSegmentLocation
{
    uint32_t bucket;
    uint32_t offset;
};

struct DataSegment
{
    vector<DataSegmentHeader> buckets;
    SpinLock                  mutex;

    DataSegmentLocation reserve(uint32_t size);
    DataSegmentLocation reserveNoLock(uint32_t size);
    uint8_t*            address(DataSegmentLocation& location);
};
