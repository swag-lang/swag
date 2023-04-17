#pragma once
#include "Job.h"
#include "Timer.h"
#include "Statistics.h"
#include "Module.h"
struct ModuleDependency;

struct ModuleBuildJob : public Job
{
    void      release() override;
    bool      loadDependency(ModuleDependency* dep);
    bool      loadDependency(Module* depModule);
    void      publishFilesToPublic(Module* moduleToPublish);
    void      publishFilesToTarget(Module* moduleToPublish);
    void      publishFilesToPublic();
    void      publishFilesToTarget();
    void      checkMissingErrors();
    JobResult execute() override;

    ModuleBuildPass pass      = ModuleBuildPass::Init;
    bool            fromError = false;
};
