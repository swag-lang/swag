#pragma once
#include "Threading/Job.h"

struct ModulePrepOutputStage1Job;

struct SCBE_SaveObjJob final : Job
{
    SCBE_SaveObjJob()
    {
        addFlag(JOB_IS_IO);
    }

    JobResult execute() override;

    ModulePrepOutputStage1Job* prepJob;
};
