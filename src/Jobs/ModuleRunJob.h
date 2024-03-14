#pragma once
#include "Backend/BackendParameters.h"
#include "Threading/Job.h"

struct Module;

struct ModuleRunJob final : Job
{
    JobResult execute() override;

    BuildParameters buildParameters;
};
