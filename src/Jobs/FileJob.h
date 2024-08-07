#pragma once
#include "Threading/Job.h"
struct Module;

struct CopyFileJob final : Job
{
    CopyFileJob();

    void      release() override;
    JobResult execute() override;

    Path sourcePath;
    Path destPath;
};

struct LoadFileJob final : Job
{
    LoadFileJob();

    JobResult execute() override;

    Path     sourcePath;
    void*    destBuffer = nullptr;
    uint32_t sizeBuffer = 0;
};
