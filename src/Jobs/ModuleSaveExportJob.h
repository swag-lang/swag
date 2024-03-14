#pragma once
#include "Threading/Job.h"

struct ModuleSaveExportJob final : Job
{
    ModuleSaveExportJob();

    JobResult execute() override;
};
