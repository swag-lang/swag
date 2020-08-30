#pragma once
#include "Job.h"
struct Backend;

struct ModuleExportJob : public Job
{
    ModuleExportJob()
    {
        flags |= JOB_IS_IO;
    }

    JobResult execute() override;
    Backend*  backend = nullptr;
};

extern thread_local Pool<ModuleExportJob> g_Pool_moduleExportJob;
