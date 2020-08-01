#pragma once
#include "Job.h"
struct Module;

enum class ModuleOutputJobPass
{
    Init,
    PreCompile,
    Compile,
    Run,
};

struct ModuleOutputJob : public Job
{
    JobResult           execute() override;
    ModuleOutputJobPass pass = ModuleOutputJobPass::Init;
};

extern thread_local Pool<ModuleOutputJob> g_Pool_moduleOutputJob;
