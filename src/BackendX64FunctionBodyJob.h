#pragma once
#include "Job.h"
#include "BackendFunctionBodyJob.h"

struct BackendX64FunctionBodyJob : public BackendFunctionBodyJob
{
    JobResult execute() override;

    void release() override
    {
        extern thread_local Pool<BackendX64FunctionBodyJob> g_Pool_backendX64FunctionBodyJob;
        g_Pool_backendX64FunctionBodyJob.release(this);
    }
};

extern thread_local Pool<BackendX64FunctionBodyJob> g_Pool_backendX64FunctionBodyJob;
