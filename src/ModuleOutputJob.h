#pragma once
#include "Job.h"
#include "Timer.h"
#include "Stats.h"
struct Module;

enum class ModuleOutputJobPass
{
    Init,
    PrepareOutput,
    WaitForDependencies,
    GenOutput,
    Done,
};

struct ModuleOutputJob : public Job
{
    JobResult           execute() override;
    ModuleOutputJobPass pass = ModuleOutputJobPass::Init;
};

