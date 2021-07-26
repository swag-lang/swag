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

