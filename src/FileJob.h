#pragma once
#include "Job.h"
struct Module;

struct CopyFileJob : public Job
{
    CopyFileJob()
    {
        flags |= JOB_IS_IO;
    }

    void release() override
    {
        g_Allocator.free<CopyFileJob>(this);
    }

    JobResult execute() override;

    string sourcePath;
    string destPath;
};

struct LoadFileJob : public Job
{
    LoadFileJob()
    {
        flags |= JOB_IS_IO;
    }

    JobResult execute() override;

    string   sourcePath;
    void*    destBuffer = nullptr;
    uint32_t sizeBuffer = 0;
};
