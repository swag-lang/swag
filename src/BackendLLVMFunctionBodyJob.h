#pragma once
#include "Job.h"
#include "BackendFunctionBodyJob.h"

struct BackendLLVMFunctionBodyJob : public BackendFunctionBodyJob
{
    JobResult execute() override;

    void release() override
    {
        g_Allocator.free<BackendLLVMFunctionBodyJob>(this);
    }
};
