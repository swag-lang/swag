#pragma once
#include "Job.h"
struct Module;

struct ModuleOutputJob : public Job
{
    JobResult execute() override;
};

extern Pool<ModuleOutputJob> g_Pool_moduleOutputJob;
