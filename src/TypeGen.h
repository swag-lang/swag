#pragma once
#include "Utf8.h"
#include "DependentJobs.h"
#include "TypeInfo.h"
#include "Map.h"
struct JobContext;
struct AstNode;
struct ExportedTypeInfo;
struct AttributeList;
struct SwagSlice;
struct ExportedTypeValue;
struct TypeInfoParam;
struct ComputedValue;
struct DataSegment;
struct Module;
struct TypeGenStructJob;

static uint32_t GEN_EXPORTED_TYPE_SHOULD_WAIT    = 0x00000001;
static uint32_t GEN_EXPORTED_TYPE_FORCE_NO_SCOPE = 0x00000002;
static uint32_t GEN_EXPORTED_TYPE_PARTIAL        = 0x00000004;

struct TypeGen
{
    struct MapType
    {
        TypeInfo*         realType;
        TypeInfo*         newRealType;
        ExportedTypeInfo* exportedType;
        uint32_t          storageOffset;
    };

    struct MapPerSeg
    {
        SharedMutex                       mutex;
        MapUtf8<MapType>                  exportedTypes;
        MapUtf8<TypeGenStructJob*>        exportedTypesJob;
        Map<ExportedTypeInfo*, TypeInfo*> exportedTypesReverse;
    };

    void  setup(const Utf8& moduleName);
    void  release();
    bool  genExportedTypeInfo(JobContext* context, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t* storageOffset, uint32_t cflags = 0, TypeInfo** ptrTypeInfo = nullptr);
    bool  genExportedTypeInfoNoLock(JobContext* context, ExportedTypeInfo** result, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t* storageOffset, uint32_t cflags = 0, TypeInfo** ptrTypeInfo = nullptr);
    bool  genExportedTypeValue(JobContext* context, void* exportedTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, TypeInfoParam* realType, uint32_t cflags);
    bool  genExportedSubTypeInfo(JobContext* context, ExportedTypeInfo** result, void* exportedTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, TypeInfo* typeInfo, uint32_t cFlags);
    void* genExportedSlice(JobContext* context, uint32_t sizeOf, void* exportedTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, void** result, uint32_t& storageArray);
    void* genExportedSlice(JobContext* context, uint32_t sizeOf, DataSegment* storageSegment, uint32_t offset, void** result, uint32_t& storageArray);
    bool  genExportedAny(JobContext* context, struct ExportedAny* ptrAny, DataSegment* storageSegment, uint32_t storageOffset, ComputedValue& computedValue, TypeInfo* typeInfo, uint32_t cflags);
    bool  genExportedAttributes(JobContext* context, AttributeList& attributes, void* exportedTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, SwagSlice* result, uint32_t cflags);
    bool  genExportedString(JobContext* context, SwagSlice* result, const Utf8& str, DataSegment* storageSegment, uint32_t offsetInBuffer);
    bool  genExportedStuct(JobContext* context, const Utf8& typeName, ExportedTypeInfo* exportedTypeInfoValue, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t storageOffset, uint32_t cflags);

    MapPerSeg& getMapPerSeg(DataSegment* segment);
    void       tableJobDone(TypeGenStructJob* job, DataSegment* segment);
    TypeInfo*  getRealType(DataSegment* segment, ExportedTypeInfo* concreteType);
    void       initFrom(Module* module, TypeGen* other);

    Utf8                     name;
    Mutex                    mutexMapPerSeg;
    VectorNative<MapPerSeg*> mapPerSegment;
};

#define OFFSETOF(__field) (storageOffset + (uint32_t) ((uint64_t) & (__field) - (uint64_t) exportedTypeInfoValue))
#define OFFSETOFR(__field) (storageOffset + (uint32_t) ((uint64_t) __field - (uint64_t) exportedTypeInfoValue))