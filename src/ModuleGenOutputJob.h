#pragma once
#include "Job.h"
#include "BackendParameters.h"
struct Module;

struct ModuleGenOutputJob : public Job
{
    ModuleGenOutputJob()
    {
        flags |= JOB_IS_IO;
    }

    JobResult execute() override;

    BuildParameters     buildParameters;
    condition_variable* condVar   = nullptr;
    mutex*              mutexDone = nullptr;
};

