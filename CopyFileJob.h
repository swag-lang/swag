#pragma once
#include "Job.h"
struct Module;

struct CopyFileJob : public Job
{
    JobResult execute() override;
    Module*   module;
    string    sourcePath;
    string    destPath;
};

extern Pool<CopyFileJob> g_Pool_copyPublishJob;