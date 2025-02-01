#pragma once
#include "Threading/Mutex.h"

#ifdef SWAG_HAS_RACE_CONDITION_TYPEGEN
#define SWAG_RACE_CONDITION_WRITE_TYPEGEN(__x)    SWAG_RACE_CONDITION_ON_WRITE(__x)
#define SWAG_RACE_CONDITION_READ_TYPEGEN(__x)     SWAG_RACE_CONDITION_ON_READ(__x)
#define SWAG_RACE_CONDITION_READ1_TYPEGEN(__x)    SWAG_RACE_CONDITION_ON_READ1(__x)
#define SWAG_RACE_CONDITION_INSTANCE_TYPEGEN(__x) SWAG_RACE_CONDITION_ON_INSTANCE(__x)
#else
#define SWAG_RACE_CONDITION_WRITE_TYPEGEN(__x)    SWAG_RACE_CONDITION_OFF_WRITE(__x)
#define SWAG_RACE_CONDITION_READ_TYPEGEN(__x)     SWAG_RACE_CONDITION_OFF_READ(__x)
#define SWAG_RACE_CONDITION_READ1_TYPEGEN(__x)    SWAG_RACE_CONDITION_OFF_READ1(__x)
#define SWAG_RACE_CONDITION_INSTANCE_TYPEGEN(__x) SWAG_RACE_CONDITION_OFF_INSTANCE(__x)
#endif

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

using GenExportFlags                                   = Flags<uint32_t>;
constexpr GenExportFlags GEN_EXPORTED_TYPE_SHOULD_WAIT = 0x00000001;
constexpr GenExportFlags GEN_EXPORTED_TYPE_PARTIAL     = 0x00000002;

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
        SWAG_RACE_CONDITION_INSTANCE_TYPEGEN(raceC);
    };

    void         setup(const Utf8& moduleName);
    void         release();
    bool         genExportedTypeInfo(JobContext* context, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t* resultStorageOffset, GenExportFlags genFlags = 0, TypeInfo** ptrTypeInfo = nullptr, ExportedTypeInfo** resultPtr = nullptr);
    bool         genExportedTypeInfoNoLock(JobContext* context, TypeInfo* typeInfo, DataSegment* storageSegment, uint32_t* resultStorageOffset, GenExportFlags genFlags, TypeInfo** ptrTypeInfo, ExportedTypeInfo** resultPtr);
    bool         genExportedTypeValue(JobContext* context, void* exportedTypeInfoValue, DataSegment* storageSegment, uint32_t storageOffset, TypeInfoParam* param, GenExportFlags genFlags, uint32_t offset = UINT32_MAX);
    bool         genExportedSubTypeInfo(JobContext* context, ExportedTypeInfo** resultPtr, void* exportedValue, DataSegment* storageSegment, uint32_t storageOffset, TypeInfo* typeInfo, GenExportFlags genFlags);
    static void* genExportedSlice(JobContext* context, uint32_t sizeOf, void* exportedValue, DataSegment* storageSegment, uint32_t storageOffset, void** result, uint32_t& storageArray);
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

#define OFFSET_OF(__field)   (storageOffset + (uint32_t) ((uint64_t) &(__field) - (uint64_t) exportedTypeInfoValue))
#define OFFSET_OF_R(__field) (storageOffset + (uint32_t) ((uint64_t) (__field) - (uint64_t) exportedValue))
