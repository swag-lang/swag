#pragma once
#include "Job.h"
struct Module;

struct PrepCompilerMsgJob : public Job
{
    JobResult execute() override;

    int startIndex;
    int endIndex;
};
