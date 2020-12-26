#pragma once
#include "DependentJobs.h"
#include "TypeInfo.h"
struct JobContext;
struct AstNode;
struct ConcreteTypeInfo;
struct SymbolAttributes;
struct ConcreteSlice;
struct ConcreteTypeInfoParam;
struct TypeInfoParam;
struct ComputedValue;
struct DataSegment;
struct Module;
struct TypeTableJob;

static uint32_t CONCRETE_ZERO         = 0x00000000;
static uint32_t CONCRETE_SHOULD_WAIT  = 0x00000001;
static uint32_t CONCRETE_FOR_COMPILER = 0x00000002;

struct TypeTable
{
    bool makeConcreteParam(JobContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, TypeInfoParam* realType, uint32_t cflags);
    bool makeConcreteSubTypeInfo(JobContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, ConcreteTypeInfo** result, TypeInfo* typeInfo, bool forceNoScope, uint32_t cflags);
    bool makeConcreteAny(JobContext* context, struct ConcreteAny* ptrAny, uint32_t storageOffset, ComputedValue& computedValue, TypeInfo* typeInfo, uint32_t cflags);
    bool makeConcreteAttributes(JobContext* context, SymbolAttributes& attributes, ConcreteSlice* result, uint32_t offset, uint32_t cflags);
    bool makeConcreteString(JobContext* context, ConcreteSlice* result, const Utf8& str, uint32_t offsetInBuffer, uint32_t cflags);
    bool makeConcreteTypeInfoNoLock(JobContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storage, bool forceNoScope, uint32_t cflags);

    bool makeConcreteTypeInfo(JobContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storagetrue, uint32_t cflags);

    void         tableJobDone(TypeTableJob* job);
    Utf8&        getTypeName(TypeInfo* typeInfo, bool forceNoScope);
    DataSegment* getConstantSegment(Module* module, uint32_t flags);

    using mapType = map<Utf8, pair<TypeInfo*, uint32_t>>;
    mapType concreteTypes;
    mapType concreteTypesCompiler;

    using mapTypeJob = map<Utf8, TypeTableJob*>;
    mapTypeJob concreteTypesJob;
    mapTypeJob concreteTypesJobCompiler;
};

#define OFFSETOF(__field) (storageOffset + (uint32_t)((uint64_t) & (__field) - (uint64_t) concreteTypeInfoValue))
