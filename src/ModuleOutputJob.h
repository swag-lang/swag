#pragma once
#include "Job.h"
#include "Statistics.h"
#include "Timer.h"
struct Module;

enum class ModuleOutputJobPass
{
    Init,
    PrepareOutputStage1,
    PrepareOutputStage2,
    WaitForDependencies,
    GenOutput,
    Done,
};

struct ModuleOutputJob : Job
{
    JobResult           execute() override;
    ModuleOutputJobPass pass = ModuleOutputJobPass::Init;
};
