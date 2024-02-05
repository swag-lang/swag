#pragma once
#include "Job.h"

struct ModuleSemanticJob : Job
{
    JobResult execute() override;
};
