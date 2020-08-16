#pragma once
#include "Job.h"
#include "BuildParameters.h"
struct Module;

struct ModuleGenOutputJob : public Job
{
    JobResult execute() override;

    BuildParameters     buildParameters;
    condition_variable* condVar   = nullptr;
    mutex*              mutexDone = nullptr;
};

extern thread_local Pool<ModuleGenOutputJob> g_Pool_moduleGenOutputJob;
