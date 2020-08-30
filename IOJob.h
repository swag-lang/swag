#pragma once
#include "Job.h"
struct Module;

enum class IOJobType
{
    CopyFileJob,
};

struct IOJob : public Job
{
    IOJob()
    {
        flags |= JOB_IS_IO;
    }

    JobResult execute() override;

    JobResult CopyFile();

    Module*   module;
    string    sourcePath;
    string    destPath;
    IOJobType type;
};

extern thread_local Pool<IOJob> g_Pool_ioJob;
