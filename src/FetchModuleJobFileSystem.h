#pragma once
#include "Job.h"
struct Module;

struct FetchModuleJobFileSystem : public Job
{
    FetchModuleJobFileSystem()
    {
        flags |= JOB_IS_IO;
    }

    JobResult execute() override;
};

