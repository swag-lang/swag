#pragma once
#include "Backend/BackendParameters.h"
#include "Threading/Job.h"

struct Module;

struct ModuleGenOutputJob final : Job
{
    ModuleGenOutputJob();

    JobResult execute() override;

    BuildParameters          buildParameters;
    std::condition_variable* condVar   = nullptr;
    std::mutex*              mutexDone = nullptr;
};
