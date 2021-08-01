#pragma once
#include "DependentJobs.h"
#include "TypeInfo.h"
struct JobContext;
struct AstNode;
struct ConcreteTypeInfo;
struct AttributeList;
struct SwagSlice;
struct ConcreteTypeInfoParam;
struct TypeInfoParam;
struct ComputedValue;
struct DataSegment;
struct Module;
struct TypeTableJob;

static uint32_t MAKE_CONCRETE_SHOULD_WAIT    = 0x00000001;
static uint32_t MAKE_CONCRETE_FORCE_NO_SCOPE = 0x00000002;
static uint32_t MAKE_CONCRETE_NATIVE         = 0x00000004;

struct TypeTable
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
        shared_mutex                      mutex;
        map<Utf8, MapType>                concreteTypes;
        map<ConcreteTypeInfo*, TypeInfo*> concreteTypesReverse;
        map<Utf8, TypeTableJob*>          concreteTypesJob;
        vector<pair<TypeInfo*, MapType*>> nativeConcreteTypes;
    };

    bool  makeConcreteTypeInfo(JobContext* context, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t* storageOffset, uint32_t cflags = 0, TypeInfo** ptrTypeInfo = nullptr);
    bool  makeConcreteTypeInfoNoLock(JobContext* context, ConcreteTypeInfo** result, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t* storageOffset, uint32_t cflags = 0, TypeInfo** ptrTypeInfo = nullptr);
    bool  makeConcreteParam(JobContext* context, void* concreteTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, TypeInfoParam* realType, uint32_t cflags);
    bool  makeConcreteSubTypeInfo(JobContext* context, ConcreteTypeInfo** result, void* concreteTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, TypeInfo* typeInfo, uint32_t cFlags);
    void* makeConcreteSlice(JobContext* context, uint32_t sizeOf, void* concreteTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, void** result, uint32_t& storageArray);
    void* makeConcreteSlice(JobContext* context, uint32_t sizeOf, DataSegment* storageSegment, uint32_t offset, void** result, uint32_t& storageArray);
    bool  makeConcreteAny(JobContext* context, struct ConcreteAny* ptrAny, DataSegment* storageSegment, uint32_t storageOffset, ComputedValue& computedValue, TypeInfo* typeInfo, uint32_t cflags);
    bool  makeConcreteAttributes(JobContext* context, AttributeList& attributes, void* concreteTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, SwagSlice* result, uint32_t cflags);
    bool  makeConcreteString(JobContext* context, SwagSlice* result, const Utf8& str, DataSegment* storageSegment, uint32_t offsetInBuffer);

    MapType*   getBasicType(DataSegment* segment, TypeInfo* typeInfo);
    void       registerBasicTypes(DataSegment* segment);
    MapPerSeg& getMapPerSeg(DataSegment* segment);
    Utf8&      getTypeName(TypeInfo* typeInfo, bool forceNoScope);
    void       tableJobDone(TypeTableJob* job, DataSegment* segment);
    TypeInfo*  getRealType(DataSegment* segment, ConcreteTypeInfo* concreteType);

    MapPerSeg mapPerSegment[2];
};

#define OFFSETOF(__field) (storageOffset + (uint32_t)((uint64_t) & (__field) - (uint64_t) concreteTypeInfoValue))
#define OFFSETOFR(__field) (storageOffset + (uint32_t)((uint64_t) __field - (uint64_t) concreteTypeInfoValue))