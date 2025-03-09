#pragma once
#include "Threading/Job.h"

struct ModulePrepOutputStage1Job;

struct ScbeSaveObjJob final : Job
{
    ScbeSaveObjJob()
    {
        addFlag(JOB_IS_IO);
    }

    JobResult execute() override;

    ModulePrepOutputStage1Job* prepJob;
};
