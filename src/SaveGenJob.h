#pragma once
#include "Job.h"
#include "Utf8.h"
struct Module;

struct SaveGenJob : public Job
{
    SaveGenJob()
    {
        flags |= JOB_IS_IO;
    }

    void release() override
    {
        g_Allocator.free<SaveGenJob>(this);
    }

    JobResult execute() override;

    Utf8 content;
};
