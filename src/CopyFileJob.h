#pragma once
#include "Job.h"
struct Module;

struct CopyFileJob : public Job
{
    CopyFileJob()
    {
        flags |= JOB_IS_IO;
    }

    JobResult execute() override;

    string sourcePath;
    string destPath;
};

extern thread_local Pool<CopyFileJob> g_Pool_copyFileJob;
