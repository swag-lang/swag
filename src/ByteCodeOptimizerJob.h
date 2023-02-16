#pragma once
#include "Job.h"
#include "ByteCodeOptContext.h"
struct ByteCode;

struct ByteCodeOptimizerJob : public Job
{
    JobResult execute() override;

    void release() override
    {
        g_Allocator.free<ByteCodeOptimizerJob>(this);
    }

    ByteCodeOptContext optContext;
    int                startIndex;
    int                endIndex;
};
