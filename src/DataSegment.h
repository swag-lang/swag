#pragma once
#include "Pool.h"
#include "BuildPass.h"
#include "BuildParameters.h"
#include "TypeTable.h"
#include "RaceCondition.h"
struct SourceFile;
struct Module;

struct DataSegmentHeader
{
    uint8_t* buffer;
    uint32_t count;
    uint32_t size;
};

enum class SegmentKind
{
    Me,
    Data,
    Constant,
    Type,
};

struct DataSegmentRef
{
    uint32_t    patchOffset;
    uint32_t    srcOffset;
    SegmentKind fromSegment;
};

struct DataSegment
{
    void                            setup(const char* name, Module* module);
    void                            initFrom(DataSegment* other);
    uint32_t                        reserve(uint32_t size, uint32_t alignOf = 1);
    uint32_t                        reserveNoLock(TypeInfo* typeInfo);
    uint32_t                        reserveNoLock(uint32_t size, uint32_t alignOf);
    uint32_t                        reserveNoLock(uint32_t size);
    uint32_t                        offset(uint8_t* location);
    uint8_t*                        address(uint32_t location);
    uint8_t*                        addressNoLock(uint32_t location);
    void                            release();
    VectorNative<DataSegmentHeader> buckets;
    shared_mutex                    mutex;

    uint32_t                addComputedValueNoLock(SourceFile* sourceFile, TypeInfo* typeInfo, ComputedValue& computedValue);
    map<uint8_t, uint32_t>  storedValues8;
    map<uint16_t, uint32_t> storedValues16;
    map<uint32_t, uint32_t> storedValues32;
    map<uint64_t, uint32_t> storedValues64;

    enum class RelocType
    {
        Foreign,
        FileSystem,
        ByteCode
    };

    uint32_t            addString(const Utf8& str);
    uint32_t            addStringNoLock(const Utf8& str);
    void                addInitPtr(uint32_t patchOffset, uint32_t srcOffset, SegmentKind seg = SegmentKind::Me);
    void                addInitPtrFunc(uint32_t offset, const Utf8& funcName, RelocType relocType);
    shared_mutex        mutexPtr;
    map<Utf8, uint32_t> mapString;

    map<uint32_t, pair<Utf8, RelocType>> initFuncPtr;

    const char* name       = nullptr;
    Module*     module     = nullptr;
    bool        overflow   = false;
    uint32_t    totalCount = 0;

    VectorNative<DataSegmentRef> initPtr;
    SWAG_RACE_CONDITION_INSTANCE(raceCondition);

    struct Seek
    {
        uint32_t seekRead   = 0;
        uint32_t seekBucket = 0;
    };
    bool readU64(Seek& seek, uint64_t& result);

    struct SaveValue
    {
        void*    ptr;
        uint32_t size;
    };
    map<void*, SaveValue> savedValues;

    void saveValue(void* address, uint32_t size, bool zero);
    void restoreAllValues();

    bool compilerOnly = false;
};
