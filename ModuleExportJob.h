#pragma once
#include "Job.h"
struct Backend;

struct ModuleExportJob : public Job
{
    JobResult execute() override;
    Backend*  backend = nullptr;
};

extern thread_local Pool<ModuleExportJob> g_Pool_moduleExportJob;
