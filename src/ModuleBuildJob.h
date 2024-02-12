#pragma once
#include "Job.h"
#include "Module.h"

struct ModuleDependency;

struct ModuleBuildJob : Job
{
    void      release() override;
    bool      loadDependency(ModuleDependency* dep);
    bool      loadDependency(Module* depModule);
    void      publishFilesToPublic(const Module* moduleToPublish);
    void      publishFilesToTarget(const Module* moduleToPublish);
    void      publishFilesToPublic();
    void      publishFilesToTarget();
    void      checkMissingErrors() const;
    JobResult execute() override;

    ModuleBuildPass pass      = ModuleBuildPass::Init;
    bool            fromError = false;
};
