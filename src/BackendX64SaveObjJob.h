#pragma once
#include "Job.h"
struct Module;
struct ModulePrepOutputStage1Job;

struct BackendX64SaveObjJob : public Job
{
    BackendX64SaveObjJob()
    {
        flags |= JOB_IS_IO;
    }

    JobResult execute() override;

    ModulePrepOutputStage1Job* prepJob;
};
