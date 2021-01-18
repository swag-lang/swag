#pragma once
#include "Job.h"
#include "BuildParameters.h"
struct Module;

struct ModuleRunJob : public Job
{
    JobResult execute() override;

    BuildParameters buildParameters;
};

extern thread_local Pool<ModuleRunJob> g_Pool_moduleRunJob;
