#pragma once
#include "Job.h"

struct PrepCompilerMsgJob final : Job
{
    JobResult execute() override;

    uint32_t pass;
    int      startIndex;
    int      endIndex;
};
