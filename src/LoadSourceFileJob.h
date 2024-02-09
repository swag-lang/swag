#pragma once
#include "Allocator.h"
#include "Job.h"
struct SourceFile;

struct LoadSourceFileJob : Job
{
    virtual ~LoadSourceFileJob() = default;

    LoadSourceFileJob()
    {
        addFlag(JOB_IS_IO);
    }

    void release() override
    {
        Allocator::free<LoadSourceFileJob>(this);
    }

    JobResult execute() override;
};
