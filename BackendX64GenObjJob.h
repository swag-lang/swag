#pragma once
#include "Job.h"
struct Module;
struct ModulePrepOutputJob;

struct BackendX64GenObjJob : public Job
{
    BackendX64GenObjJob()
    {
        flags |= JOB_IS_IO;
    }

    JobResult execute() override;

    ModulePrepOutputJob* prepJob;
    Module*              module;
};

extern thread_local Pool<BackendX64GenObjJob> g_Pool_backendX64GenObjJob;
