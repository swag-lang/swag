#pragma once
#include "Job.h"
#include "BackendFunctionBodyJobBase.h"

struct BackendLLVMFunctionBodyJob : public BackendFunctionBodyJobBase
{
    JobResult execute() override;

    void release() override
    {
        g_Allocator.free<BackendLLVMFunctionBodyJob>(this);
    }
};
