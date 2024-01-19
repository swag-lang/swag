#pragma once
#include "Job.h"

struct ModulePrepOutputStage1Job;

struct SCBE_SaveObjJob : public Job
{
    SCBE_SaveObjJob()
    {
        flags |= JOB_IS_IO;
    }

    JobResult execute() override;

    ModulePrepOutputStage1Job* prepJob;
};
