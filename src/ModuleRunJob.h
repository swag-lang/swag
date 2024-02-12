#pragma once
#include "BackendParameters.h"
#include "Job.h"

struct Module;

struct ModuleRunJob : Job
{
    JobResult execute() override;

    BuildParameters buildParameters;
};
