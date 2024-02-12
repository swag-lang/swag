#pragma once
#include "Allocator.h"
#include "Job.h"
struct SourceFile;

struct LoadSourceFileJob : Job
{
    virtual ~LoadSourceFileJob() = default;
    LoadSourceFileJob();

    void      release() override;
    JobResult execute() override;
};
