#pragma once
#include "Job.h"
struct ByteCode;
struct ByteCodeOptContext;

struct ByteCodeOptimizerJob : public Job
{
    ByteCodeOptimizerJob();
    JobResult execute() override;
    bool      optimize(bool isAsync);

    vector<function<void(ByteCodeOptContext*)>> passes;
    int                                         startIndex, endIndex;

    void reset() override
    {
        Job::reset();
        startIndex = 0;
        endIndex   = 0;
    }

    void release() override
    {
        extern thread_local Pool<ByteCodeOptimizerJob> g_Pool_byteCodeOptimizerJob;
        g_Pool_byteCodeOptimizerJob.release(this);
    }
};

extern thread_local Pool<ByteCodeOptimizerJob> g_Pool_byteCodeOptimizerJob;
