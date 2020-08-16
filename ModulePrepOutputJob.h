#pragma once
#include "Job.h"
#include "BuildParameters.h"
#include "Backend.h"
struct Module;

struct ModulePrepOutputJob : public Job
{
    JobResult       execute() override;
    BuildParameters buildParameters;
};

extern thread_local Pool<ModulePrepOutputJob> g_Pool_modulePrepOutputJob;
