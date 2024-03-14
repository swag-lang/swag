#pragma once
#include "Backend/BackendParameters.h"
#include "Threading/Job.h"
struct Module;
struct Backend;
struct ByteCode;

struct BackendFunctionBodyJob final : Job
{
    JobResult execute() override;

    void release() override
    {
        Allocator::free<BackendFunctionBodyJob>(this);
    }

    BuildParameters         buildParameters;
    VectorNative<ByteCode*> byteCodeFunc;
    Backend*                backend = nullptr;
};
