#pragma once
#include "BackendParameters.h"
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
    Tls,
    Compiler,
    Bss,
    Global,
    String,
};

struct SegmentInitPtrRef
{
    uint32_t    patchOffset;
    uint32_t    srcOffset;
    SegmentKind fromSegment;
};

struct SegmentPatchPtrRef
{
    int64_t* addr;
    int64_t  value;
};

struct DataSegment
{
    void     setup(SegmentKind kind, Module* module);
    void     initFrom(DataSegment* other);
    void     align(uint32_t alignOf);
    void     alignNoLock(uint32_t alignOf);
    uint32_t reserve(uint32_t size, uint8_t** resultPtr = nullptr, uint32_t alignOf = 1);
    uint32_t reserveNoLock(uint32_t size, uint32_t alignOf, uint8_t** resultPtr = nullptr);
    uint32_t reserveNoLock(uint32_t size, uint8_t** resultPtr = nullptr);
    uint32_t offset(uint8_t* location);
    uint8_t* address(DataSegment* lockedSegment, uint32_t location);
    uint8_t* address(uint32_t location);
    uint8_t* addressNoLock(uint32_t location);
    void     release();
    void     makeLinear();

    VectorNative<DataSegmentHeader> buckets;
    shared_mutex                    mutex;

    struct CacheValue
    {
        uint32_t offset;
        uint8_t* addr;
    };

    uint32_t                  addComputedValueNoLock(SourceFile* sourceFile, TypeInfo* typeInfo, ComputedValue& computedValue, uint8_t** resultPtr = nullptr);
    map<uint8_t, CacheValue>  storedValues8;
    map<uint16_t, CacheValue> storedValues16;
    map<uint32_t, CacheValue> storedValues32;
    map<uint64_t, CacheValue> storedValues64;

    enum class RelocType
    {
        Foreign,
        Local,
        ByteCode
    };

    uint32_t addString(const Utf8& str, uint8_t** resultPtr = nullptr);
    uint32_t addStringNoLock(const Utf8& str, uint8_t** resultPtr = nullptr);
    void     addInitPtr(uint32_t patchOffset, uint32_t srcOffset, SegmentKind seg = SegmentKind::Me);
    void     addInitPtrFunc(uint32_t offset, const Utf8& funcName, RelocType relocType);
    void     addPatchPtr(int64_t* addr, int64_t value);
    void     applyPatchPtr();
    void     addPatchMethod(AstFuncDecl* funcDecl, uint32_t storageOffset);
    void     doPatchMethods(JobContext* context);

    shared_mutex                         mutexPatchMethod;
    shared_mutex                         mutexPtr;
    map<Utf8, CacheValue>                mapString;
    map<uint32_t, pair<Utf8, RelocType>> initFuncPtr;
    vector<SegmentPatchPtrRef>           patchPtr;
    vector<pair<AstFuncDecl*, uint32_t>> patchMethods;

    SegmentKind kind       = SegmentKind::Me;
    const char* name       = nullptr;
    Module*     module     = nullptr;
    bool        overflow   = false;
    uint32_t    totalCount = 0;

    VectorNative<SegmentInitPtrRef> initPtr;

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

    SWAG_RACE_CONDITION_INSTANCE(raceC);
};
