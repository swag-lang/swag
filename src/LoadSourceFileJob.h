#pragma once
#include "Job.h"

struct SourceFile;

struct LoadSourceFileJob : Job
{
    virtual ~LoadSourceFileJob() = default;
    LoadSourceFileJob();

    void      release() override;
    JobResult execute() override;
};
