#pragma once
#include "Job.h"
struct Module;

struct ModuleSemanticJob : public Job
{
    JobResult execute() override;
    Module*   module;
    bool      buildFileMode = false;
};

extern Pool<ModuleSemanticJob> g_Pool_moduleSemanticJob;