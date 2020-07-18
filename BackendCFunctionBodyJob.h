#pragma once
#include "Job.h"
#include "BackendFunctionBodyJob.h"

struct BackendCFunctionBodyJob : public BackendFunctionBodyJob
{
    JobResult execute() override;

    void release() override
    {
        extern thread_local Pool<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;
        g_Pool_backendCFunctionBodyJob.release(this);
    }

    void reset() override
    {
        BackendFunctionBodyJob::reset();
        canSave = true;
    }

    bool canSave = true;
};

extern thread_local Pool<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;
