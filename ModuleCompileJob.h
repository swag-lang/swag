#pragma once
#include "Job.h"
#include "BuildParameters.h"
struct Module;

struct ModuleCompileJob : public Job
{
    JobResult execute() override;

    BuildParameters     buildParameters;
    Module*             module    = nullptr;
    condition_variable* condVar   = nullptr;
    mutex*              mutexDone = nullptr;
};

extern Pool<ModuleCompileJob> g_Pool_moduleCompileJob;
