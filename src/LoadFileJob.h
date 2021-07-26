#pragma once
#include "Job.h"
struct Module;

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

