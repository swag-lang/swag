#pragma once
#include "Pool.h"
#include "SpinLock.h"
#include "BuildPass.h"
#include "BackendParameters.h"
#include "TypeTable.h"

struct DataSegmentHeader
{
    uint8_t* buffer;
    uint32_t totalCountBefore;
    uint32_t count;
    uint32_t size;
};

struct DataSegment
{
    uint32_t                  reserve(uint32_t size);
    uint32_t                  reserveNoLock(uint32_t size);
    uint8_t*                  address(uint32_t location);
    uint8_t*                  addressNoLock(uint32_t location);
    vector<DataSegmentHeader> buckets;
    SpinLock                  mutex;

    void                             addInitString(uint32_t segOffset, uint32_t strIndex);
    void                             addInitPtr(uint32_t fromOffset, uint32_t toOffset);
    SpinLock                         mutexPtr;
    map<uint32_t, uint32_t>          initString;
    vector<pair<uint32_t, uint32_t>> initPtr;
};
