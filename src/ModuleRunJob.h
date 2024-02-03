#pragma once
#include "BackendParameters.h"
#include "Job.h"
struct Module;

struct ModuleRunJob : public Job
{
    JobResult execute() override;

    BuildParameters buildParameters;
};
