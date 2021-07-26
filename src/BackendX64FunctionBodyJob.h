#pragma once
#include "Job.h"
#include "BackendFunctionBodyJob.h"

struct BackendX64FunctionBodyJob : public BackendFunctionBodyJob
{
    JobResult execute() override;

    void release() override
    {
        g_Allocator.free<BackendX64FunctionBodyJob>(this);
    }
};

