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
    JobResult execute() override;
    void      checkPendingJobs();

    ModuleBuildPass                           pass = ModuleBuildPass::Dependencies;
    chrono::high_resolution_clock::time_point timeBefore;
	chrono::high_resolution_clock::time_point timeAfter;
};

extern thread_local Pool<ModuleBuildJob> g_Pool_moduleBuildJob;