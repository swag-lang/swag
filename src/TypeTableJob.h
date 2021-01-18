#pragma once
#include "Job.h"
struct TypeTable;
struct ConcreteTypeInfoStruct;

struct TypeTableJob : public Job
{
    JobResult execute() override;
    bool      computeStruct();

    Utf8              typeName;
    TypeTable*        typeTable             = nullptr;
    ConcreteTypeInfo* concreteTypeInfoValue = nullptr;
    TypeInfo*         typeInfo              = nullptr;
    uint32_t          storageOffset         = 0;
    uint32_t          cflags                = 0;
};

extern thread_local Pool<TypeTableJob> g_Pool_typeTableJob;
