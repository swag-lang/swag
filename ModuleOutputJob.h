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
    ModuleOutputJob()
        : timerPrepareOutput{g_Stats.prepOutputTimePass}
        , timerGenOutput{g_Stats.genOutputTimePass}
    {
    }

    JobResult           execute() override;
    ModuleOutputJobPass pass = ModuleOutputJobPass::Init;

    Timer timerPrepareOutput;
    Timer timerGenOutput;
};

extern thread_local Pool<ModuleOutputJob> g_Pool_moduleOutputJob;
