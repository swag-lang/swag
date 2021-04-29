#pragma once
#include "DependentJobs.h"
#include "TypeInfo.h"
struct JobContext;
struct AstNode;
struct ConcreteTypeInfo;
struct SymbolAttributes;
struct SwagSlice;
struct ConcreteTypeInfoParam;
struct TypeInfoParam;
struct ComputedValue;
struct DataSegment;
struct Module;
struct TypeTableJob;

static uint32_t CONCRETE_ZERO           = 0x00000000;
static uint32_t CONCRETE_SHOULD_WAIT    = 0x00000001;
static uint32_t CONCRETE_FOR_COMPILER   = 0x00000002;
static uint32_t CONCRETE_FORCE_NO_SCOPE = 0x00000004;

struct TypeTable
{
    bool  makeConcreteParam(JobContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, TypeInfoParam* realType, uint32_t cflags);
    bool  makeConcreteSubTypeInfo(JobContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, int64_t* result, TypeInfo* typeInfo, uint32_t cflags);
    bool  makeConcreteSubTypeInfo(JobContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, ConcreteTypeInfo** result, TypeInfo* typeInfo, uint32_t cflags);
    void* makeConcreteSlice(JobContext* context, uint32_t sizeOf, void* concreteTypeInfoValue, uint32_t storageOffset, int64_t* result, uint32_t cflags, uint32_t& storageArray);
    void* makeConcreteSlice(JobContext* context, uint32_t sizeOf, uint32_t offset, int64_t* result, uint32_t cflags, uint32_t& storageArray);
    bool  makeConcreteAny(JobContext* context, struct ConcreteAny* ptrAny, uint32_t storageOffset, ComputedValue& computedValue, TypeInfo* typeInfo, uint32_t cflags);
    bool  makeConcreteAttributes(JobContext* context, SymbolAttributes& attributes, void* concreteTypeInfoValue, uint32_t storageOffset, ConcreteRelativeSlice* result, uint32_t cflags);
    bool  makeConcreteString(JobContext* context, SwagSlice* result, const Utf8& str, uint32_t offsetInBuffer, uint32_t cflags);
    bool  makeConcreteTypeInfoNoLock(JobContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storage, uint32_t cflags);

    bool makeConcreteTypeInfo(JobContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storagetrue, uint32_t cflags);

    void         tableJobDone(TypeTableJob* job, DataSegment* segment);
    Utf8&        getTypeName(TypeInfo* typeInfo, bool forceNoScope);
    DataSegment* getSegmentStorage(Module* module, uint32_t flags);

    struct MapType
    {
        TypeInfo*         realType;
        TypeInfo*         newRealType;
        ConcreteTypeInfo* concreteType;
        uint32_t          storageOffset;
    };

    using mapType = map<Utf8, MapType>;
    mapType concreteTypes;
    mapType concreteTypesCompiler;

    using mapTypeJob = map<Utf8, TypeTableJob*>;
    mapTypeJob concreteTypesJob;
    mapTypeJob concreteTypesJobCompiler;
};

#define OFFSETOF(__field) (storageOffset + (uint32_t)((uint64_t) & (__field) - (uint64_t) concreteTypeInfoValue))
#define OFFSETOFR(__field) (storageOffset + ((int64_t) __field - (int64_t) concreteTypeInfoValue))
#define RELATIVE_PTR(__addr) (ConcreteTypeInfo*) (((uint8_t*) __addr) + *(int64_t*) __addr)