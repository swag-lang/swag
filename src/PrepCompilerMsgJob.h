#pragma once
#include "Job.h"
struct Module;

struct PrepCompilerMsgJob : public Job
{
    JobResult execute() override;

    uint32_t pass;
    int      startIndex;
    int      endIndex;
};
