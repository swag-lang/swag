#pragma once
#include "Job.h"
struct Module;

struct CopyPublishJob : public Job
{
    JobResult execute() override;
    Module*   module;
};

extern Pool<CopyPublishJob> g_Pool_copyPublishJob;