#pragma once
#include "Threading/Job.h"

struct ModuleSemanticJob final : Job
{
    JobResult execute() override;
};
