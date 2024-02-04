#pragma once
#include "Job.h"
struct Backend;

struct ModuleExportJob : Job
{
    JobResult execute() override;

    Backend* backend = nullptr;
};
