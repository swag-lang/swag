#pragma once
#include "Backend/BackendParameters.h"
#include "Threading/Job.h"

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
