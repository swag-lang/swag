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

    bool operator==(const ComputedValue& from) const
    {
        if (text != from.text)
            return false;
        if (reg.u64 != from.reg.u64)
            return false;
        if (storageSegment != from.storageSegment)
            return false;
        if (storageOffset != from.storageOffset)
            return false;
        if (storageSegment2 != from.storageSegment2)
            return false;
        if (storageOffset2 != from.storageOffset2)
            return false;
        return true;
    }
};