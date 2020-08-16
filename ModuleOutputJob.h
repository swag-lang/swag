#pragma once
#include "Job.h"
struct Module;

enum class ModuleOutputJobPass
{
    Init,
    PrepareOutput,
    GenOutput,
    Done,
};

struct ModuleOutputJob : public Job
{
    JobResult           execute() override;
    ModuleOutputJobPass pass = ModuleOutputJobPass::Init;

    chrono::high_resolution_clock::time_point timeBeforePrepareOutput;
    chrono::high_resolution_clock::time_point timeBeforeGenOutput;
};

extern thread_local Pool<ModuleOutputJob> g_Pool_moduleOutputJob;
