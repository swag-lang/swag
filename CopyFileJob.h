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

extern thread_local Pool<CopyFileJob> g_Pool_copyFileJob;