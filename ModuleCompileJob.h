#pragma once
#include "Job.h"
#include "BuildParameters.h"
struct Module;

struct ModuleCompileJob : public Job
{
    void reset()
    {
        Job::reset();
        condVar   = nullptr;
        mutexDone = nullptr;
    }

    JobResult execute() override;

    Module*         module = nullptr;
    BuildParameters buildParameters;

    condition_variable* condVar   = nullptr;
    mutex*              mutexDone = nullptr;
};

extern Pool<ModuleCompileJob> g_Pool_moduleCompileJob;
