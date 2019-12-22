#pragma once
#include "Job.h"
#include "BuildParameters.h"
struct Module;

struct ModuleTestJob : public Job
{
    JobResult execute() override;

    BuildParameters buildParameters;
};

extern Pool<ModuleTestJob> g_Pool_moduleTestJob;
