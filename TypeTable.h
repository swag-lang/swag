#pragma once
#include "TypeList.h"
#include "DependentJobs.h"
struct TypeInfo;
struct JobContext;
struct AstNode;
struct ConcreteTypeInfo;
struct SymbolAttributes;
struct ConcreteStringSlice;
struct ConcreteTypeInfoParam;
struct TypeInfoParam;

struct TypeTable
{
    TypeTable();
    bool makeConcreteParam(JobContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, TypeInfoParam* realType);
    bool makeConcreteSubTypeInfo(JobContext* context, void* concreteTypeInfoValue, uint32_t storageOffset, ConcreteTypeInfo** result, TypeInfo* typeInfo);
    bool makeConcreteAny(JobContext* context, struct ConcreteAny* ptrAny, uint32_t storageOffset, ComputedValue& computedValue, TypeInfo* typeInfo);
    bool makeConcreteAttributes(JobContext* context, SymbolAttributes& attributes, ConcreteStringSlice* result, uint32_t offset);
    bool makeConcreteString(JobContext* context, ConcreteStringSlice* result, const Utf8& str, uint32_t offsetInBuffer);
    bool makeConcreteTypeInfoNoLock(JobContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storage);

    bool makeConcreteTypeInfo(JobContext* context, TypeInfo* typeInfo, TypeInfo** ptrTypeInfo, uint32_t* storagetrue);

    void waitForTypeTableJobs(Job* job);
    void addTypeTableJob(Job* job);
    void typeTableJobDone();

    TypeList concreteList;

    shared_mutex                              mutexTypes;
    shared_mutex                              mutexJobs;
    DependentJobs                             dependentJobs;
    map<TypeInfo*, pair<TypeInfo*, uint32_t>> concreteTypes;
    int                                       pendingJobs = 0;
};

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Should match bootstrap.swg
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

struct ConcreteStringSlice
{
    void*    buffer;
    uint64_t count;
};

struct ConcreteTypeInfo
{
    ConcreteStringSlice name;
    TypeInfoKind        kind;
    uint32_t            sizeOf;
};

struct ConcreteAny
{
    void*             value;
    ConcreteTypeInfo* type;
};

struct ConcreteAttributeParameter
{
    ConcreteStringSlice name;
    ConcreteAny         value;
};

struct ConcreteAttribute
{
    ConcreteStringSlice name;
    ConcreteStringSlice params;
};

struct ConcreteTypeInfoNative
{
    ConcreteTypeInfo base;
    NativeTypeKind   nativeKind;
};

struct ConcreteTypeInfoPointer
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* finalType;
    ConcreteTypeInfo* pointedType;
    uint32_t          ptrCount;
};

struct ConcreteTypeInfoReference
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
};

struct ConcreteTypeInfoParam
{
    ConcreteStringSlice name;
    ConcreteTypeInfo*   pointedType;
    void*               value;
    ConcreteStringSlice attributes;
    uint32_t            offsetOf;
    uint32_t            padding;
};

struct ConcreteTypeInfoStruct
{
    ConcreteTypeInfo    base;
    ConcreteStringSlice fields;
    ConcreteStringSlice methods;
    ConcreteStringSlice interfaces;
    ConcreteStringSlice attributes;
};

struct ConcreteTypeInfoFunc
{
    ConcreteTypeInfo    base;
    ConcreteStringSlice parameters;
    ConcreteTypeInfo*   returnType;
    ConcreteStringSlice attributes;
};

struct ConcreteTypeInfoEnum
{
    ConcreteTypeInfo    base;
    ConcreteStringSlice values;
    ConcreteTypeInfo*   rawType;
    ConcreteStringSlice attributes;
};

struct ConcreteTypeInfoArray
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
    ConcreteTypeInfo* finalType;
    uint32_t          count;
    uint32_t          totalCount;
};

struct ConcreteTypeInfoSlice
{
    ConcreteTypeInfo  base;
    ConcreteTypeInfo* pointedType;
};

#define OFFSETOF(__field) (storageOffset + (uint32_t)((uint64_t) & (__field) - (uint64_t) concreteTypeInfoValue))
