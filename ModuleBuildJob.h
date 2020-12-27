#pragma once
#include "Job.h"
#include "Timer.h"
#include "Stats.h"
struct Module;
struct ModuleDependency;

enum class ModuleBuildPass
{
    Init,
    Dependencies,
    IncludeSwg,
    AfterSemantic,
    WaitForDependencies,
    WaitForDependenciesEffective,
    OptimizeBc,
    Publish,
    SemanticModule,
    RunByteCode,
    Output,
    RunNative,
    Done,
};

struct ModuleBuildJob : public Job
{
    ModuleBuildJob()
        : timerSemanticCompiler{&g_Stats.semanticCompilerTime}
        , timerSemanticModule{&g_Stats.semanticModuleTime}
        , timerRun{&g_Stats.runTime}
        , timerOutput{&g_Stats.outputTime}
    {
        affinity = AFFINITY_ALL ^ AFFINITY_EXECBC;
    }

    bool      addDependency(ModuleDependency* dep);
    void      publishFilesToTarget();
    void      publishFilesToPublic();
    JobResult execute() override;

    ModuleBuildPass pass = ModuleBuildPass::Init;
    Timer           timerSemanticCompiler;
    Timer           timerSemanticModule;
    Timer           timerRun;
    Timer           timerOutput;
    Timer           timerOptimizeBc;
};

extern thread_local Pool<ModuleBuildJob> g_Pool_moduleBuildJob;
