#pragma once
#include "Job.h"

struct Module;

struct SaveGenJob : Job
{
    virtual ~SaveGenJob() = default;

    SaveGenJob()
    {
        addFlag(JOB_IS_IO);
    }

    void release() override
    {
        Allocator::free<SaveGenJob>(this);
    }

    static bool flush(Module* module);
    JobResult   execute() override;

    Utf8 content;
};
