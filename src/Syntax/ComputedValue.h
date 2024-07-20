#pragma once
#include "Backend/ByteCode/Register.h"

struct DataSegment;

struct ComputedValue
{
    [[nodiscard]] bool  operator==(const ComputedValue& from) const;
    [[nodiscard]] void* getStorageAddr() const;

    Utf8         text;
    Register     reg;
    DataSegment* storageSegment = nullptr;
    uint32_t     storageOffset  = UINT32_MAX;
};
