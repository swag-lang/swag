#include "pch.h"
#include "ComputedValue.h"
#include "DataSegment.h"

void* ComputedValue::getStorageAddr() const
{
    SWAG_ASSERT(storageSegment);
    SWAG_ASSERT(storageOffset != UINT32_MAX);
    return storageSegment->address(storageOffset);
}

bool ComputedValue::operator==(const ComputedValue& from) const
{
    if (text != from.text)
        return false;
    if (reg.u64 != from.reg.u64)
        return false;
    if (storageSegment != from.storageSegment)
        return false;
    if (storageOffset != from.storageOffset)
        return false;
    return true;
}
