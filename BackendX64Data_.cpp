#include "pch.h"
#include "BackendX64.h"
#include "DataSegment.h"
#include "Module.h"
#include "ByteCode.h"

bool BackendX64::emitDataSegment(const BuildParameters& buildParameters, DataSegment* dataSegment)
{
    if (!dataSegment->buckets.size())
        return true;
    if (!dataSegment->totalCount)
        return true;
        return true;
}

bool BackendX64::emitInitDataSeg(const BuildParameters& buildParameters)
{
    return true;
}

bool BackendX64::emitInitConstantSeg(const BuildParameters& buildParameters)
{
    return true;
}
