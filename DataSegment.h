#pragma once
#include "Pool.h"
#include "BuildPass.h"
#include "BuildParameters.h"
#include "TypeTable.h"
#include "RaceCondition.h"
struct SourceFile;

struct DataSegmentHeader
{
    uint8_t* buffer;
    uint32_t totalCountBefore;
    uint32_t count;
    uint32_t size;
};

enum class SegmentKind
{
    Me,
    Data,
    Constant,
};

struct DataSegmentRef
{
    uint32_t    destOffset;
    uint32_t    srcOffset;
    SegmentKind destSeg;
};

struct DataSegment
{
    uint32_t                  reserve(uint32_t size);
    uint32_t                  reserveNoLock(uint32_t size);
    uint32_t                  offset(uint8_t* location);
    uint8_t*                  address(uint32_t location);
    uint8_t*                  addressNoLock(uint32_t location);
    vector<DataSegmentHeader> buckets;
    shared_mutex              mutex;

    uint32_t                addComputedValueNoLock(SourceFile* sourceFile, TypeInfo* typeInfo, ComputedValue& computedValue);
    map<uint8_t, uint32_t>  storedValues8;
    map<uint16_t, uint32_t> storedValues16;
    map<uint32_t, uint32_t> storedValues32;
    map<uint64_t, uint32_t> storedValues64;

    uint32_t                 addString(const Utf8& str);
    uint32_t                 addStringNoLock(const Utf8& str);
    void                     addInitPtr(uint32_t fromOffset, uint32_t toOffset, SegmentKind seg = SegmentKind::Me);
    void                     addInitPtrFunc(uint32_t offset, ByteCode* bc);
    shared_mutex             mutexPtr;
    map<Utf8, uint32_t>      mapString;
    map<uint32_t, ByteCode*> initFuncPtr;
    vector<DataSegmentRef>   initPtr;
    uint32_t                 totalCount = 0;
    SWAG_RACE_CONDITION_INSTANCE(raceCondition);

    uint32_t seekRead   = 0;
    uint32_t seekBucket = 0;
    void     rewindRead();
    bool     readU64(uint64_t& result);

    map<void*, pair<void*, uint32_t>> savedValues;

    void saveValue(void* address, uint32_t size);
    void restoreAllValues();

    bool compilerOnly = false;
};
