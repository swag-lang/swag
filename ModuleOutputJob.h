#pragma once
#include "Job.h"
struct Module;

struct ModuleOutputJob : public Job
{
    JobResult execute() override;

    Module* module = nullptr;
};

extern Pool<ModuleOutputJob> g_Pool_moduleOutputJob;
