#pragma once
#include "Job.h"
struct Module;

struct ModuleSemanticJob : public Job
{
    JobResult execute() override;
    bool      buildFileMode = false;
};

extern thread_local Pool<ModuleSemanticJob> g_Pool_moduleSemanticJob;
