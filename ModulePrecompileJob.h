#pragma once
#include "Job.h"
struct Module;
struct Backend;

struct ModulePreCompileJob : public Job
{
    JobResult execute() override;
};

extern thread_local Pool<ModulePreCompileJob> g_Pool_modulePreCompileJob;
