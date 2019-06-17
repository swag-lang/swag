#pragma once
#include "Job.h"
#include "BackendParameters.h"
struct Module;

struct ModuleCompileJob : public Job
{
    JobResult execute() override;

    Module*           module = nullptr;
    BackendParameters backendParameters;
};

extern Pool<ModuleCompileJob> g_Pool_moduleCompileJob;
