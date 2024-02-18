#pragma once
#include "BackendParameters.h"
#include "Job.h"

struct ModulePrepOutputStage1Job final : Job
{
    JobResult execute() override;

    BuildParameters buildParameters;
};

struct ModulePrepOutputStage2Job final : Job
{
    JobResult execute() override;

    BuildParameters buildParameters;
};
