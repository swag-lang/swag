#pragma once
#include "Job.h"
struct Module;

struct CopyFileJob : Job
{
    CopyFileJob();

    void      release() override;
    JobResult execute() override;

    Path sourcePath;
    Path destPath;
};

struct LoadFileJob : Job
{
    LoadFileJob();

    JobResult execute() override;

    Path     sourcePath;
    void*    destBuffer = nullptr;
    uint32_t sizeBuffer = 0;
};
