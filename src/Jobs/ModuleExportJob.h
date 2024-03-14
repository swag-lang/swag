#pragma once
#include "Threading/Job.h"

struct Backend;

struct ModuleExportJob final : Job
{
    JobResult execute() override;

    Backend* backend = nullptr;
};
