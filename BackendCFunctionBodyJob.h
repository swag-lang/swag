#pragma once
#include "Job.h"
#include "Concat.h"
struct Module;
struct BackendC;
struct ByteCode;

struct BackendCFunctionBodyJob : public Job
{
    JobResult execute() override;

    void reset() override
    {
        Job::reset();
        backend         = nullptr;
        precompileIndex = 0;
        byteCodeFunc.clear();
    }

    void release() override
    {
        extern thread_local Pool<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;
        g_Pool_backendCFunctionBodyJob.release(this);
    }

    vector<ByteCode*> byteCodeFunc;
    BackendC*         backend         = nullptr;
    int               precompileIndex = 0;
};

extern thread_local Pool<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;
