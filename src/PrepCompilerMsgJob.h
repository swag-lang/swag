#pragma once
#include "Job.h"
struct Module;

struct PrepCompilerMsgJob : Job
{
    JobResult execute() override;

    uint32_t pass;
    int      startIndex;
    int      endIndex;
};
