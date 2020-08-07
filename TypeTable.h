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

struct TypeTable
{
    TypeTable();
    bool makeConcreteParam(JobContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, TypeInfoParam* realType, bool& shouldWait);
    bool makeConcreteSubTypeInfo(JobContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, ConcreteTypeInfo** result, TypeInfo* typeInfo, bool forceNoScope, bool& shouldWait);
    bool makeConcreteAny(JobContext* context, struct ConcreteAny* ptrAny, uint32_t storageOffset, ComputedValue& computedValue, TypeInfo* typeInfo, bool& shouldWait);
    bool makeConcreteAttributes(JobContext* context, SymbolAttributes& attributes, ConcreteSlice* result, uint32_t offset, bool& shouldWait);
    bool makeConcreteString(JobContext* context, ConcreteSlice* result, const Utf8& str, uint32_t offsetInBuffer, bool& shouldWait);
    bool makeConcreteTypeInfoNoLock(JobContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storage, bool forceNoScope, bool& shouldWait);

    bool makeConcreteTypeInfo(JobContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storagetrue, bool& shouldWait);

    Utf8& getTypeName(TypeInfo* typeInfo, bool forceNoScope);
    void  waitForTypeTableJobs(Job* job);
    void  addTypeTableJob(Job* job);
    void  typeTableJobDone();

    shared_mutex                            mutexTypes;
    shared_mutex                            mutexJobs;
    DependentJobs                           dependentJobs;
    map<Utf8Crc, pair<TypeInfo*, uint32_t>> concreteTypes;
    int                                     pendingJobs = 0;
};

#define OFFSETOF(__field) (storageOffset + (uint32_t)((uint64_t) & (__field) - (uint64_t) concreteTypeInfoValue))
