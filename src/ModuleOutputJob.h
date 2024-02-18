#pragma once
#include "Job.h"

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

struct ModuleOutputJob final : Job
{
    JobResult           execute() override;
    ModuleOutputJobPass pass = ModuleOutputJobPass::Init;
};
