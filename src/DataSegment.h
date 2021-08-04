#pragma once
#include "VectorNative.h"
#include "RaceCondition.h"
struct SourceFile;
struct Module;
struct TypeInfo;
struct ComputedValue;
struct Utf8;
struct AstFuncDecl;
struct JobContext;

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

struct DataSegment
{
    struct Bucket
    {
        uint8_t* buffer;
        uint32_t count;
        uint32_t size;
    };

    struct InitPtrRef
    {
        uint32_t    patchOffset;
        uint32_t    srcOffset;
        SegmentKind fromSegment;
    };

    struct PatchPtrRef
    {
        int64_t* addr;
        int64_t  value;
    };

    struct CacheValue
    {
        uint32_t offset;
        uint8_t* addr;
    };

    enum class RelocType
    {
        Foreign,
        Local,
        ByteCode
    };

    struct Seek
    {
        uint32_t seekRead   = 0;
        uint32_t seekBucket = 0;
    };

    struct SaveValue
    {
        void*    ptr;
        uint32_t size;
    };

    void setup(SegmentKind kind, Module* module);
    void initFrom(DataSegment* other);
    void release();

    void     align(uint32_t alignOf);
    void     alignNoLock(uint32_t alignOf);
    void     makeLinear();
    uint32_t reserve(uint32_t size, uint8_t** resultPtr = nullptr, uint32_t alignOf = 1);
    uint32_t reserveNoLock(uint32_t size, uint32_t alignOf, uint8_t** resultPtr = nullptr);
    uint32_t reserveNoLock(uint32_t size, uint8_t** resultPtr = nullptr);
    uint32_t offset(uint8_t* location);
    uint8_t* address(DataSegment* lockedSegment, uint32_t location);
    uint8_t* address(uint32_t location);
    uint8_t* addressNoLock(uint32_t location);

    uint32_t addComputedValue(SourceFile* sourceFile, TypeInfo* typeInfo, ComputedValue& computedValue, uint8_t** resultPtr = nullptr);
    uint32_t addString(const Utf8& str, uint8_t** resultPtr = nullptr);
    uint32_t addStringNoLock(const Utf8& str, uint8_t** resultPtr = nullptr);
    void     addInitPtr(uint32_t patchOffset, uint32_t srcOffset, SegmentKind seg = SegmentKind::Me);
    void     addInitPtrFunc(uint32_t offset, const Utf8& funcName, RelocType relocType);
    void     addPatchPtr(int64_t* addr, int64_t value);
    void     applyPatchPtr();
    void     addPatchMethod(AstFuncDecl* funcDecl, uint32_t storageOffset);
    void     doPatchMethods(JobContext* context);

    bool readU64(Seek& seek, uint64_t& result);
    void saveValue(void* address, uint32_t size, bool zero);
    void restoreAllValues();

    shared_mutex mutex;
    shared_mutex mutexPatchMethod;
    shared_mutex mutexPtr;

    VectorNative<Bucket> buckets;

    map<Utf8, CacheValue>     storedStrings;
    map<uint8_t, CacheValue>  storedValues8;
    map<uint16_t, CacheValue> storedValues16;
    map<uint32_t, CacheValue> storedValues32;
    map<uint64_t, CacheValue> storedValues64;

    VectorNative<InitPtrRef>             initPtr;
    map<uint32_t, pair<Utf8, RelocType>> initFuncPtr;
    map<void*, SaveValue>                savedValues;
    vector<PatchPtrRef>                  patchPtr;
    vector<pair<AstFuncDecl*, uint32_t>> patchMethods;

    const char* name   = nullptr;
    Module*     module = nullptr;

    SegmentKind kind        = SegmentKind::Me;
    uint32_t    granularity = 0;
    uint32_t    totalCount  = 0;

    bool overflow = false;

    SWAG_RACE_CONDITION_INSTANCE(raceC);
};
