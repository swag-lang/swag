#pragma once
#include "Utf8.h"
#include "Register.h"
struct DataSegment;

struct ComputedValue
{
    Utf8         text;
    Register     reg;
    DataSegment* storageSegment  = nullptr;
    DataSegment* storageSegment2 = nullptr;
    uint32_t     storageOffset   = UINT32_MAX;
    uint32_t     storageOffset2  = UINT32_MAX;

    bool  operator==(const ComputedValue& from) const;
    void* getStorageAddr() const;
};