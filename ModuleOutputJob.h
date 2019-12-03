#pragma once
#include "Job.h"
struct Module;

struct ModuleOutputJob : public Job
{
    JobResult execute() override;
};

extern thread_local Pool<ModuleOutputJob> g_Pool_moduleOutputJob;
