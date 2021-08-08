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
    Syntax,
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
    bool      loadDependency(ModuleDependency* dep);
    void      publishFilesToTarget();
    void      publishFilesToPublic();
    JobResult execute() override;

    ModuleBuildPass pass      = ModuleBuildPass::Init;
    bool            fromError = false;
};
