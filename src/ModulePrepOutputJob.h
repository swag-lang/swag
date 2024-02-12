#pragma once
#include "BackendParameters.h"
#include "Job.h"

struct ModulePrepOutputStage1Job : Job
{
    JobResult execute() override;

    BuildParameters buildParameters;
};

struct ModulePrepOutputStage2Job : Job
{
    JobResult execute() override;

    BuildParameters buildParameters;
};
