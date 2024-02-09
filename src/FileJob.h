#pragma once
#include "Job.h"
struct Module;

struct CopyFileJob : Job
{
    virtual ~CopyFileJob() = default;

    CopyFileJob()
    {
        addFlag(JOB_IS_IO);
    }

    void release() override
    {
        Allocator::free<CopyFileJob>(this);
    }

    JobResult execute() override;

    Path sourcePath;
    Path destPath;
};

struct LoadFileJob : Job
{
    LoadFileJob()
    {
        addFlag(JOB_IS_IO);
    }

    JobResult execute() override;

    Path     sourcePath;
    void*    destBuffer = nullptr;
    uint32_t sizeBuffer = 0;
};
