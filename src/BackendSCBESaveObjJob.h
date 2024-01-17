#pragma once
#include "Job.h"
struct Module;
struct ModulePrepOutputStage1Job;

struct BackendSCBESaveObjJob : public Job
{
    BackendSCBESaveObjJob()
    {
        flags |= JOB_IS_IO;
    }

    JobResult execute() override;

    ModulePrepOutputStage1Job* prepJob;
};
