#pragma once
#include "Job.h"

struct ModuleSaveExportJob : Job
{
    ModuleSaveExportJob();

    JobResult execute() override;
};
