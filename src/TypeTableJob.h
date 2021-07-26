#pragma once
#include "Job.h"
struct TypeTable;
struct ConcreteTypeInfoStruct;
struct AstFuncDecl;

struct TypeTableJob : public Job
{
    JobResult execute() override;
    bool      computeStruct();

    Utf8                                 typeName;
    TypeTable*                           typeTable             = nullptr;
    ConcreteTypeInfo*                    concreteTypeInfoValue = nullptr;
    TypeInfo*                            typeInfo              = nullptr;
    DataSegment*                         segment               = nullptr;
    uint32_t                             storageOffset         = 0;
    uint32_t                             cflags                = 0;
    vector<pair<AstFuncDecl*, uint32_t>> patchMethods;
};

