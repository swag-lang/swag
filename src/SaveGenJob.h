#pragma once
#include "Job.h"

struct Module;

struct SaveGenJob : public Job
{
    virtual ~SaveGenJob() = default;

    SaveGenJob()
    {
        flags |= JOB_IS_IO;
    }

    void release() override
    {
        Allocator::free<SaveGenJob>(this);
    }

    static bool flush(Module* module);
    JobResult   execute() override;

    Utf8 content;
};
