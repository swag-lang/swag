#pragma once
#include "Job.h"

struct ModuleSemanticJob final : Job
{
    JobResult execute() override;
};
