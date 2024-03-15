#pragma once
#include "Threading/Mutex.h"

struct AstNode;
struct AttributeList;
struct ComputedValue;
struct DataSegment;
struct ExportedTypeInfo;
struct ExportedTypeValue;
struct JobContext;
struct Module;
struct SwagAny;
struct SwagSlice;
struct TypeGenStructJob;
struct TypeInfo;
struct TypeInfoParam;

using GenExportFlags                                      = Flags<uint32_t>;
constexpr GenExportFlags GEN_EXPORTED_TYPE_SHOULD_WAIT    = 0x00000001;
constexpr GenExportFlags GEN_EXPORTED_TYPE_FORCE_NO_SCOPE = 0x00000002;
constexpr GenExportFlags GEN_EXPORTED_TYPE_PARTIAL        = 0x00000004;

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
        SharedMutex                             mutex;
        MapUtf8<MapType>                        exportedTypes;
        MapUtf8<TypeGenStructJob*>              exportedTypesJob;
        Map<const ExportedTypeInfo*, TypeInfo*> exportedTypesReverse;
    };

    void         setup(const Utf8& moduleName);
    void         release();
    bool         genExportedTypeInfo(JobContext* context, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t* storage, GenExportFlags genFlags = 0, TypeInfo** ptrTypeInfo = nullptr);
    bool         genExportedTypeInfoNoLock(JobContext* context, ExportedTypeInfo** result, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t* storage, GenExportFlags genFlags = 0, TypeInfo** ptrTypeInfo = nullptr);
    bool         genExportedTypeValue(JobContext* context, void* exportedTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, TypeInfoParam* realType, GenExportFlags genFlags);
    bool         genExportedSubTypeInfo(JobContext* context, ExportedTypeInfo** result, void* exportedTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, TypeInfo* typeInfo, GenExportFlags genFlags);
    static void* genExportedSlice(JobContext* context, uint32_t sizeOf, void* exportedTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, void** result, uint32_t& storageArray);
    static void* genExportedSlice(JobContext* context, uint32_t sizeOf, DataSegment* storageSegment, uint32_t storageOffset, void** result, uint32_t& storageArray);
    bool         genExportedAny(JobContext* context, SwagAny* ptrAny, DataSegment* storageSegment, uint32_t storageOffset, ComputedValue& computedValue, TypeInfo* typeInfo, GenExportFlags genFlags);
    bool         genExportedAttributes(JobContext* context, AttributeList& attributes, void* exportedTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, SwagSlice* result, GenExportFlags genFlags);
    static bool  genExportedString(JobContext* context, SwagSlice* result, const Utf8& str, DataSegment* storageSegment, uint32_t offsetInBuffer);
    bool         genExportedStruct(const JobContext* context, const Utf8& typeName, ExportedTypeInfo* exportedTypeInfoValue, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t storageOffset, GenExportFlags genFlags);

    MapPerSeg& getMapPerSeg(const DataSegment* segment);
    void       tableJobDone(const TypeGenStructJob* job, DataSegment* segment);
    TypeInfo*  getRealType(const DataSegment* segment, const ExportedTypeInfo* concreteType);
    void       initFrom(Module* module, TypeGen* other);

    Utf8                     name;
    Mutex                    mutexMapPerSeg;
    VectorNative<MapPerSeg*> mapPerSegment;
};

#define OFFSET_OF(__field)   (storageOffset + (uint32_t) ((uint64_t) & (__field) - (uint64_t) exportedTypeInfoValue))
#define OFFSET_OF_R(__field) (storageOffset + (uint32_t) ((uint64_t) (__field) - (uint64_t) exportedTypeInfoValue))
