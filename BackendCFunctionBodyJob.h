#pragma once
#include "Job.h"
#include "Concat.h"
struct Module;
struct BackendC;
struct ByteCode;

struct BackendCFunctionBodyJob : public Job
{
    JobResult         execute() override;
    BackendC*         backend = nullptr;
    vector<ByteCode*> byteCodeFunc;
    Concat            concat;

    void reset()
    {
        Job::reset();
        byteCodeFunc.clear();
    }
};

extern Pool<BackendCFunctionBodyJob> g_Pool_backendCFunctionBodyJob;
