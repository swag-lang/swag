#pragma once
#include "RaceCondition.h"
#include "Register.h"

struct AstFuncDecl;
struct ComputedValue;
struct JobContext;
struct Module;
struct SourceFile;
struct TypeInfo;
struct Utf8;

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
        uint32_t    fromOffset;
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
        Register value;
        uint32_t size;
    };

    void setup(SegmentKind myKind, Module* myModule);
    void initFrom(DataSegment* other);
    void release();

    void     align(uint32_t alignOf);
    void     alignNoLock(uint32_t alignOf);
    void     makeLinear();
    uint32_t reserve(uint32_t size, uint8_t** resultPtr = nullptr, uint32_t alignOf = 1);
    uint32_t reserveNoLock(uint32_t size, uint32_t alignOf, uint8_t** resultPtr = nullptr);
    uint32_t reserveNoLock(uint32_t size, uint8_t** resultPtr = nullptr);
    uint32_t tryOffset(const uint8_t* location);
    uint32_t offset(const uint8_t* location);
    uint8_t* address(uint32_t location);
    uint8_t* addressNoLock(uint32_t location);

    uint32_t addComputedValue(const TypeInfo* typeInfo, ComputedValue& computedValue, uint8_t** resultPtr);
    uint32_t addString(const Utf8& str, uint8_t** resultPtr = nullptr);
    uint32_t addStringNoLock(const Utf8& str, uint8_t** resultPtr = nullptr);
    void     addInitPtr(uint32_t patchOffset, uint32_t srcOffset, SegmentKind seg = SegmentKind::Me);
    void     addInitPtrFunc(uint32_t offset, const Utf8& funcName);
    void     addPatchPtr(int64_t* addr, int64_t value);
    void     applyPatchPtr() const;
    void     addPatchMethod(AstFuncDecl* funcDecl, uint32_t storageOffset);
    void     doPatchMethods(JobContext* context);

    bool readU64(Seek& seek, uint64_t& result);
    void saveValue(uint8_t* address, uint32_t size, bool zero);
    void restoreAllValues();

    SharedMutex mutex;
    SharedMutex mutexPatchMethod;
    SharedMutex mutexPtr;

    VectorNative<Bucket> buckets;

    MapUtf8<CacheValue>       storedStrings;
    Map<uint8_t, CacheValue>  storedValues8;
    Map<uint16_t, CacheValue> storedValues16;
    Map<uint32_t, CacheValue> storedValues32;
    Map<uint64_t, CacheValue> storedValues64;

    VectorNative<InitPtrRef>                  initPtr;
    Map<uint32_t, Utf8>                       initFuncPtr;
    Map<uint8_t*, SaveValue>                  savedValues;
    Vector<PatchPtrRef>                       patchPtr;
    Vector<std::pair<AstFuncDecl*, uint32_t>> patchMethods;

    const char* name   = nullptr;
    Module*     module = nullptr;

    SegmentKind kind              = SegmentKind::Me;
    uint32_t    granularity       = 0;
    uint32_t    totalCount        = 0;
    uint32_t    compilerThreadIdx = UINT32_MAX;

    bool overflow = false;
    bool deleted  = false;

    SWAG_RACE_CONDITION_INSTANCE(raceC);
};
