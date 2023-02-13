#pragma once
#include "Utf8.h"
#include "DependentJobs.h"
#include "TypeInfo.h"
struct JobContext;
struct AstNode;
struct ConcreteTypeInfo;
struct AttributeList;
struct SwagSlice;
struct ConcreteTypeValue;
struct TypeInfoParam;
struct ComputedValue;
struct DataSegment;
struct Module;
struct TypeGenStructJob;

static uint32_t MAKE_CONCRETE_TYPE_SHOULD_WAIT    = 0x00000001;
static uint32_t MAKE_CONCRETE_TYPE_FORCE_NO_SCOPE = 0x00000002;
static uint32_t MAKE_CONCRETE_TYPE_PARTIAL        = 0x00000004;

struct TypeGen
{
    struct MapType
    {
        TypeInfo*         realType;
        TypeInfo*         newRealType;
        ConcreteTypeInfo* concreteType;
        uint32_t          storageOffset;
    };

    struct MapPerSeg
    {
        SharedMutex                                  mutex;
        unordered_map<Utf8, MapType, HashUtf8>       concreteTypes;
        unordered_map<Utf8, TypeGenStructJob*, HashUtf8> concreteTypesJob;
        unordered_map<ConcreteTypeInfo*, TypeInfo*>  concreteTypesReverse;
    };

    void  setup(const Utf8& moduleName);
    bool  makeConcreteTypeInfo(JobContext* context, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t* storageOffset, uint32_t cflags = 0, TypeInfo** ptrTypeInfo = nullptr);
    bool  makeConcreteTypeInfoNoLock(JobContext* context, ConcreteTypeInfo** result, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t* storageOffset, uint32_t cflags = 0, TypeInfo** ptrTypeInfo = nullptr);
    bool  makeConcreteTypeValue(JobContext* context, void* concreteTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, TypeInfoParam* realType, uint32_t cflags);
    bool  makeConcreteSubTypeInfo(JobContext* context, ConcreteTypeInfo** result, void* concreteTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, TypeInfo* typeInfo, uint32_t cFlags);
    void* makeConcreteSlice(JobContext* context, uint32_t sizeOf, void* concreteTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, void** result, uint32_t& storageArray);
    void* makeConcreteSlice(JobContext* context, uint32_t sizeOf, DataSegment* storageSegment, uint32_t offset, void** result, uint32_t& storageArray);
    bool  makeConcreteAny(JobContext* context, struct ConcreteAny* ptrAny, DataSegment* storageSegment, uint32_t storageOffset, ComputedValue& computedValue, TypeInfo* typeInfo, uint32_t cflags);
    bool  makeConcreteAttributes(JobContext* context, AttributeList& attributes, void* concreteTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, SwagSlice* result, uint32_t cflags);
    bool  makeConcreteString(JobContext* context, SwagSlice* result, const Utf8& str, DataSegment* storageSegment, uint32_t offsetInBuffer);
    bool  makeConcreteStruct(JobContext* context, const auto& typeName, ConcreteTypeInfo* concreteTypeInfoValue, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t storageOffset, uint32_t cflags);

    MapPerSeg& getMapPerSeg(DataSegment* segment);
    void       tableJobDone(TypeGenStructJob* job, DataSegment* segment);
    TypeInfo*  getRealType(DataSegment* segment, ConcreteTypeInfo* concreteType);
    void       initFrom(Module* module, TypeGen* other);

    Utf8                     name;
    Mutex                    mutexMapPerSeg;
    VectorNative<MapPerSeg*> mapPerSegment;
};

#define OFFSETOF(__field) (storageOffset + (uint32_t) ((uint64_t) & (__field) - (uint64_t) concreteTypeInfoValue))
#define OFFSETOFR(__field) (storageOffset + (uint32_t) ((uint64_t) __field - (uint64_t) concreteTypeInfoValue))