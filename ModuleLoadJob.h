#pragma once
#include "Job.h"
struct ModuleLoadJob : public Job
{
    JobResult execute() override;
};

extern thread_local Pool<ModuleLoadJob> g_Pool_moduleLoadJob;
