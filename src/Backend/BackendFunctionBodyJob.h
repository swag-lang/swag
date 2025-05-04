#pragma once
#include "Backend/BackendParameters.h"
#include "Threading/Job.h"

struct ScbeOptimizerJob;
struct Module;
struct Backend;
struct ByteCode;
struct CpuFunction;

struct BackendFunctionBodyJob final : Job
{
    JobResult execute() override;

    void release() override
    {
        Allocator::free<BackendFunctionBodyJob>(this);
    }

    BuildParameters            buildParameters;
    VectorNative<CpuFunction*> cpuFunc;
    VectorNative<ByteCode*>    byteCodeFunc;
    Backend*                   backend   = nullptr;
    ScbeOptimizerJob*          scbeJob   = nullptr;
    bool                       firstPass = true;
};
