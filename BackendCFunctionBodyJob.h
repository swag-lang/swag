#pragma once
#include "Job.h"
#include "Concat.h"
struct Module;
struct BackendC;
struct ByteCode;

struct BackendCFunctionBodyJob : public Job
{
    JobResult         execute() override;
    vector<ByteCode*> byteCodeFunc;
    BackendC*         backend = nullptr;

    void reset() override
    {
        Job::reset();
        backend = nullptr;
        byteCodeFunc.clear();
    }

    void release() override
    {
        extern thread_local Pool<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;
        g_Pool_backendCFunctionBodyJob.release(this);
    }
};

extern thread_local Pool<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;
