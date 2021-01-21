#pragma once
#include "Job.h"
struct Module;

struct ModuleFetchJob : public Job
{
    ModuleFetchJob()
    {
        flags |= JOB_IS_IO;
    }

    JobResult execute() override;

    Module* module;
};

extern thread_local Pool<ModuleFetchJob> g_Pool_moduleFetchJob;
