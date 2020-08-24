#pragma once
#include "Job.h"
#include "Timer.h"
#include "Stats.h"
struct Module;

enum class ModuleBuildPass
{
    Dependencies,
    IncludeSwg,
    WaitForDependencies,
    Publish,
    SemanticCompilerPass,
    SemanticModulePass,
    RunByteCode,
    Output,
    RunNative,
    Done,
};

struct ModuleBuildJob : public Job
{
    ModuleBuildJob()
        : timerSemanticCompiler{g_Stats.semanticCompilerTime}
        , timerSemanticModule{g_Stats.semanticModuleTime}
        , timerRun{g_Stats.runTime}
        , timerOutput{g_Stats.outputTime}
    {
        affinity = AFFINITY_ALL ^ AFFINITY_EXECBC;
    }

    JobResult execute() override;

    ModuleBuildPass pass = ModuleBuildPass::Dependencies;
    Timer           timerSemanticCompiler;
    Timer           timerSemanticModule;
    Timer           timerRun;
    Timer           timerOutput;
};

extern thread_local Pool<ModuleBuildJob> g_Pool_moduleBuildJob;
