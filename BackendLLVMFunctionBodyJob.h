#pragma once
#include "Job.h"
#include "BackendFunctionBodyJob.h"

struct BackendLLVMFunctionBodyJob : public BackendFunctionBodyJob
{
    JobResult execute() override;

    void release() override
    {
        extern thread_local Pool<BackendLLVMFunctionBodyJob> g_Pool_backendLLVMFunctionBodyJob;
        g_Pool_backendLLVMFunctionBodyJob.release(this);
    }
};

extern thread_local Pool<BackendLLVMFunctionBodyJob> g_Pool_backendLLVMFunctionBodyJob;
