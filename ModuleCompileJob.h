#pragma once
#include "Job.h"
#include "BuildParameters.h"
struct Module;

struct ModuleCompileJob : public Job
{
    JobResult execute() override;

    BuildParameters     buildParameters;
    condition_variable* condVar   = nullptr;
    mutex*              mutexDone = nullptr;
};

extern thread_local Pool<ModuleCompileJob> g_Pool_moduleCompileJob;
