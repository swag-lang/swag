#pragma once
#include "Job.h"
#include "BuildParameters.h"
struct Module;
struct Backend;

struct ModulePreCompileJob : public Job
{
    JobResult       execute() override;
    BuildParameters buildParameters;
    int             precompileIndex = 0;

    void reset() override
    {
        Job::reset();
    }
};

extern thread_local Pool<ModulePreCompileJob> g_Pool_modulePreCompileJob;
