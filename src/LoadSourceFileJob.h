#pragma once
#include "Job.h"
#include "Allocator.h"
struct SourceFile;

struct LoadSourceFileJob : public Job
{
    LoadSourceFileJob()
    {
        flags |= JOB_IS_IO;
    }

    void release() override
    {
        g_Allocator.free<LoadSourceFileJob>(this);
    }

    JobResult execute() override;
};
