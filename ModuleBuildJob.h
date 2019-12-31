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
    Semantic,
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

    ModuleBuildPass pass = ModuleBuildPass::Dependencies;
};

extern thread_local Pool<ModuleBuildJob> g_Pool_moduleBuildJob;
