#pragma once
#include "Job.h"
struct Module;

struct ModuleSaveExportJob : Job
{
    ModuleSaveExportJob()
    {
        addFlag(JOB_IS_IO);
    }

    JobResult execute() override;
};
