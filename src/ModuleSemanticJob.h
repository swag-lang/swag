#pragma once
#include "Job.h"
struct Module;

struct ModuleSemanticJob : Job
{
    JobResult execute() override;
};
