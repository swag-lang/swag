#pragma once
#include "BackendParameters.h"
#include "Job.h"
struct Module;

struct ModuleGenOutputJob : Job
{
    ModuleGenOutputJob()
    {
        addFlag(JOB_IS_IO);
    }

    JobResult execute() override;

    BuildParameters     buildParameters;
    condition_variable* condVar   = nullptr;
    mutex*              mutexDone = nullptr;
};
