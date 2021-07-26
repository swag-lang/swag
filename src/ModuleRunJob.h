#pragma once
#include "Job.h"
#include "BackendParameters.h"
struct Module;

struct ModuleRunJob : public Job
{
    JobResult execute() override;

    BuildParameters buildParameters;
};

