#pragma once
#include "Job.h"
#include "ByteCodeOptContext.h"
struct ByteCode;

struct ByteCodeOptimizerJob : public Job
{
    ByteCodeOptimizerJob();

    JobResult execute() override;

    void release() override
    {
        g_Allocator.free<ByteCodeOptimizerJob>(this);
    }

    bool optimize(bool isAsync);

    vector<function<bool(ByteCodeOptContext*)>> passes;
    int                                         startIndex, endIndex;
    ByteCodeOptContext                          optContext;
};
