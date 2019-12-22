#pragma once
#include "Job.h"
struct Module;

struct DocModuleJob : public Job
{
    JobResult execute() override;
};

extern Pool<DocModuleJob> g_Pool_docModuleJob;