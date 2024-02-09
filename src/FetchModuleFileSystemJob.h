#pragma once
#include "Job.h"

struct FetchModuleFileSystemJob : Job
{
    FetchModuleFileSystemJob()
    {
        addFlag(JOB_IS_IO);
    }

    JobResult execute() override;
    bool      collectSourceFiles = true;
};
