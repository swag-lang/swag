#pragma once
#include "Job.h"
#include "Concat.h"
struct Module;
struct BackendC;
struct ByteCode;

struct BackendCFunctionBodyJob : public Job
{
    BackendCFunctionBodyJob()
    {
        affinity = AFFINITY_ALL ^ AFFINITY_CFCTBODY;
        jobKind = JobKind::CFCTBODY;
    }

    JobResult execute() override;

    void reset() override
    {
        Job::reset();
        backend         = nullptr;
        precompileIndex = 0;
        byteCodeFunc.clear();
        canSave = true;
    }

    void release() override
    {
        extern thread_local Pool<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;
        g_Pool_backendCFunctionBodyJob.release(this);
    }

    vector<ByteCode*> byteCodeFunc;
    BackendC*         backend         = nullptr;
    int               precompileIndex = 0;
    bool              canSave         = true;
};

extern thread_local Pool<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;
