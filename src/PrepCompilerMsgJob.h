#pragma once
#include "Job.h"

struct PrepCompilerMsgJob final : Job
{
    JobResult execute() override;

    uint32_t pass;
    uint32_t startIndex;
    uint32_t endIndex;
};
