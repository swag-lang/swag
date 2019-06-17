#pragma once
#include "Job.h"
#include "BackendParameters.h"
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

    Module*           module = nullptr;
    BackendParameters backendParameters;

    condition_variable* condVar   = nullptr;
    mutex*              mutexDone = nullptr;
};

extern Pool<ModuleCompileJob> g_Pool_moduleCompileJob;
