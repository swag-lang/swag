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

    static bool flush(Module* module);
    JobResult   execute() override;

    Utf8 content;
};
