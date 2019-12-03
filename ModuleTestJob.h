#pragma once
#include "Job.h"
#include "BuildParameters.h"
struct Module;

struct ModuleTestJob : public Job
{
    JobResult execute() override;

    BuildParameters buildParameters;
};

extern thread_local Pool<ModuleTestJob> g_Pool_moduleTestJob;
