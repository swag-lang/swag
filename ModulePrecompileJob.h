#pragma once
#include "Job.h"
struct Module;
struct Backend;

struct ModulePreCompileJob : public Job
{
    JobResult execute() override;
};

extern Pool<ModulePreCompileJob> g_Pool_modulePreCompileJob;
