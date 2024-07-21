#pragma once
#include "Backend/ByteCode/Register.h"

struct DataSegment;

struct ComputedValue
{
    bool  operator==(const ComputedValue& from) const;
    void* getStorageAddr() const;

    Utf8         text;
    Register     reg;
    DataSegment* storageSegment = nullptr;
    uint32_t     storageOffset  = UINT32_MAX;
};
