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
    BeforeCompilerMessages,
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
    void      release() override;
    bool      loadDependency(ModuleDependency* dep);
    void      publishFilesToTarget();
    void      publishFilesToPublic();
    void      checkMissingErrors();
    JobResult execute() override;

    ModuleBuildPass pass      = ModuleBuildPass::Init;
    bool            fromError = false;
};
