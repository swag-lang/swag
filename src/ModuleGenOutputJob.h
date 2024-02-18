#pragma once
#include "BackendParameters.h"
#include "Job.h"
struct Module;

struct ModuleGenOutputJob final : Job
{
    ModuleGenOutputJob();

    JobResult execute() override;

    BuildParameters     buildParameters;
    condition_variable* condVar   = nullptr;
    mutex*              mutexDone = nullptr;
};
