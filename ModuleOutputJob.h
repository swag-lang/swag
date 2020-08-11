#pragma once
#include "Job.h"
struct Module;

enum class ModuleOutputJobPass
{
    Init,
    PreCompile,
    Compile,
    Done,
};

struct ModuleOutputJob : public Job
{
    JobResult           execute() override;
    ModuleOutputJobPass pass = ModuleOutputJobPass::Init;
};

extern thread_local Pool<ModuleOutputJob> g_Pool_moduleOutputJob;
