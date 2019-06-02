#pragma once
#include "Job.h"
struct Module;
struct PoolFactory;

struct ModuleSemanticJob : public Job
{
    ModuleSemanticJob();
    JobResult execute() override;

    Module*      module      = nullptr;
    PoolFactory* poolFactory = nullptr;
};
