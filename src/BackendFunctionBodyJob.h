#pragma once
#include "BackendParameters.h"
#include "Job.h"
struct Module;
struct Backend;
struct ByteCode;

struct BackendFunctionBodyJob : Job
{
    virtual   ~BackendFunctionBodyJob() = default;
    JobResult execute() override;

    void release() override
    {
        Allocator::free<BackendFunctionBodyJob>(this);
    }

    BuildParameters         buildParameters;
    VectorNative<ByteCode*> byteCodeFunc;
    Backend*                backend = nullptr;
};
