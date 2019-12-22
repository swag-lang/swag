#pragma once
#include "Job.h"
struct Module;

enum class ModuleOutputJobPass
{
    Init,
    PreCompile,
    Compile,
};

struct ModuleOutputJob : public Job
{
    JobResult           execute() override;
    ModuleOutputJobPass pass = ModuleOutputJobPass::Init;
};

extern Pool<ModuleOutputJob> g_Pool_moduleOutputJob;
