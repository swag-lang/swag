#pragma once
#include "Pool.h"
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
    Type,
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
    uint32_t reserve(uint32_t size, uint32_t alignOf = 1);
    uint32_t reserveNoLock(TypeInfo* typeInfo);
    void     align(uint32_t alignOf);
    void     alignNoLock(uint32_t alignOf);
    uint32_t reserveNoLock(uint32_t size, uint32_t alignOf);
    uint32_t reserveNoLock(uint32_t size);
    bool     tryOffset(uint8_t* location, uint32_t& offset);
    uint32_t offset(uint8_t* location);
    uint8_t* address(uint32_t location);
    uint8_t* addressNoLock(uint32_t location);
    void     release();
    void     makeLinear();

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
        Local,
        ByteCode
    };

    uint32_t addString(const Utf8& str);
    uint32_t addStringNoLock(const Utf8& str);
    void     addInitPtr(uint32_t patchOffset, uint32_t srcOffset, SegmentKind seg = SegmentKind::Me);
    void     addInitPtrFunc(uint32_t offset, const Utf8& funcName, RelocType relocType);
    void     addPatchPtr(int64_t* addr, int64_t value);
    void     applyPatchPtr();
    void     addPatchMethod(AstFuncDecl* funcDecl, uint32_t storageOffset);
    void     doPatchMethods(JobContext* context);

    shared_mutex                         mutexPatchMethod;
    shared_mutex                         mutexPtr;
    map<Utf8, uint32_t>                  mapString;
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
