#pragma once
#include "Job.h"
struct Backend;

struct ModuleOutputExportJob : public Job
{
    JobResult execute() override;
    Backend*  backend = nullptr;
};

extern thread_local Pool<ModuleOutputExportJob> g_Pool_moduleOutputExportJob;
