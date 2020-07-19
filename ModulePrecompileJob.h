#pragma once
#include "Job.h"
#include "BuildParameters.h"
#include "Backend.h"
struct Module;

struct ModulePreCompileJob : public Job
{
    JobResult       execute() override;
    BuildParameters buildParameters;
};

extern thread_local Pool<ModulePreCompileJob> g_Pool_modulePreCompileJob;
