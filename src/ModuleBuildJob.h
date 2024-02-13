#pragma once
#include "Job.h"
#include "Module.h"

struct ModuleDependency;

struct ModuleBuildJob final : Job
{
    void      release() override;
    JobResult execute() override;

    bool loadDependency(ModuleDependency* dep);
    bool loadDependency(Module* depModule);
    void publishFilesToPublic(const Module* moduleToPublish);
    void publishFilesToTarget(const Module* moduleToPublish);
    void publishFilesToPublic();
    void publishFilesToTarget();
    void checkMissingErrors() const;

    ModuleBuildPass pass      = ModuleBuildPass::Init;
    bool            fromError = false;
};
