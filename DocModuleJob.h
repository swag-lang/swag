#pragma once
#include "Job.h"
struct Module;

struct DocModuleJob : public Job
{
    JobResult execute() override;
};

extern thread_local Pool<DocModuleJob> g_Pool_docModuleJob;
extern bool                            docFileAlreadyDone(const string& path);
