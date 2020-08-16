#pragma once
#include "Job.h"
#include "Timer.h"
#include "Stats.h"
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
    ModuleOutputJob()
        : timerPrepareOutput{g_Stats.prepOutputTimeJob}
        , timerGenOutput{g_Stats.genOutputTime}
    {
    }

    JobResult           execute() override;
    ModuleOutputJobPass pass = ModuleOutputJobPass::Init;

    Timer timerPrepareOutput;
    Timer timerGenOutput;
};

extern thread_local Pool<ModuleOutputJob> g_Pool_moduleOutputJob;
