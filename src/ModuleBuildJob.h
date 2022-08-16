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
    BeforeCompilerMessagesPass0,
    CompilerMessagesPass0,
    BeforeCompilerMessagesPass1,
    AfterSemantic,
    WaitForDependencies,
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
    void      publishFilesToTarget(Module* moduleToPublish);
    void      publishFilesToPublic();
    void      checkMissingErrors();
    JobResult execute() override;

    ModuleBuildPass pass      = ModuleBuildPass::Init;
    bool            fromError = false;
};
