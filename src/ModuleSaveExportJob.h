#pragma once
#include "Job.h"
struct Module;

struct ModuleSaveExportJob : public Job
{
    ModuleSaveExportJob()
    {
        flags |= JOB_IS_IO;
    }

    JobResult execute() override;
};

