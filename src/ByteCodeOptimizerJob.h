#pragma once
#include "Job.h"
#include "ByteCodeOptContext.h"
struct ByteCode;

struct ByteCodeOptimizerJob : public Job
{
    virtual ~ByteCodeOptimizerJob() = default;
    JobResult execute() override;

    void release() override
    {
        Allocator::free<ByteCodeOptimizerJob>(this);
    }

    ByteCodeOptContext optContext;
    int                startIndex;
    int                endIndex;
};
