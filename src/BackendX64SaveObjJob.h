#pragma once
#include "Job.h"
struct Module;
struct ModulePrepOutputJob;

struct BackendX64SaveObjJob : public Job
{
    BackendX64SaveObjJob()
    {
        flags |= JOB_IS_IO;
    }

    JobResult execute() override;

    ModulePrepOutputJob* prepJob;
};

