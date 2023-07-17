#pragma once
#include "Utf8.h"
#include "Register.h"
struct DataSegment;

struct ComputedValue
{
    Utf8         text;
    Register     reg;
    DataSegment* storageSegment = nullptr;
    uint32_t     storageOffset  = UINT32_MAX;

    bool  operator==(const ComputedValue& from) const;
    void* getStorageAddr() const;
};