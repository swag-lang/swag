#pragma once
#include "Job.h"
struct TypeGen;
struct ConcreteTypeInfo;
struct AstFuncDecl;
struct DataSegment;

struct TypeGenStructJob : public Job
{
    JobResult execute() override;
    bool      computeStruct();

    Utf8                                 typeName;
    Vector<pair<AstFuncDecl*, uint32_t>> patchMethods;

    TypeGen*          typeTable             = nullptr;
    ConcreteTypeInfo* concreteTypeInfoValue = nullptr;
    TypeInfo*         typeInfo              = nullptr;
    DataSegment*      storageSegment        = nullptr;

    uint32_t storageOffset = 0;
    uint32_t cflags        = 0;
};
