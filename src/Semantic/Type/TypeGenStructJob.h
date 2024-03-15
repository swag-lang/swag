#pragma once
#include "Threading/Job.h"

struct AstFuncDecl;
struct DataSegment;
struct ExportedTypeInfo;
struct TypeGen;
using GenExportFlags = Flags<uint32_t>;

struct TypeGenStructJob final : Job
{
    JobResult execute() override;
    bool      computeStruct();

    Utf8                                      typeName;
    Vector<std::pair<AstFuncDecl*, uint32_t>> patchMethods;

    TypeGen*          typeGen               = nullptr;
    ExportedTypeInfo* exportedTypeInfoValue = nullptr;
    TypeInfo*         typeInfo              = nullptr;
    DataSegment*      storageSegment        = nullptr;

    uint32_t       storageOffset = 0;
    GenExportFlags genFlags      = 0;
};
