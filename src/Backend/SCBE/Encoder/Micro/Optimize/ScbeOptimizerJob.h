#pragma once
#include "Threading/Job.h"

struct ScbeMicro;
struct ScbeCpu;

struct ScbeOptimizerJob final : Job
{
    JobResult execute() override;

    ScbeMicro* pp    = nullptr;
    ScbeCpu*   ppCpu = nullptr;
};
