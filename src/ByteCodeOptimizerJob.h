#pragma once
#include "Job.h"
#include "ByteCodeOptContext.h"
struct ByteCode;

struct ByteCodeOptimizerJob : public Job
{
    ByteCodeOptimizerJob();
    JobResult execute() override;
    bool      optimize(bool isAsync);

    vector<function<bool(ByteCodeOptContext*)>> passes;
    int                                         startIndex, endIndex;
    ByteCodeOptContext                          optContext;

    void reset() override
    {
        Job::reset();
        optContext.reset();
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
