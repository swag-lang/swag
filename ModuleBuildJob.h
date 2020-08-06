#pragma once
#include "Job.h"
struct Module;

enum class ModuleBuildPass
{
    Dependencies,
    IncludeSwg,
    LoadDependencies,
    BuildBuildSwg,
    Publish,
    SemanticCompilerPass,
    SemanticModulePass,
    Run,
    Output,
    End,
};

struct ModuleBuildJob : public Job
{
    ModuleBuildJob()
    {
        affinity = AFFINITY_ALL ^ AFFINITY_EXECBC;
    }

    JobResult execute() override;
    void      checkPendingJobs();

    ModuleBuildPass                           pass = ModuleBuildPass::Dependencies;
    chrono::high_resolution_clock::time_point timeBeforeSemantic;
    chrono::high_resolution_clock::time_point timeBeforeRun;
    chrono::high_resolution_clock::time_point timeBeforeOutput;
};

extern thread_local Pool<ModuleBuildJob> g_Pool_moduleBuildJob;
