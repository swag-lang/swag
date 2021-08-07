#pragma once
#include "Job.h"
struct SourceFile;

struct LoadSourceFileJob : public Job
{
    LoadSourceFileJob()
    {
        flags |= JOB_IS_IO;
    }

    JobResult execute() override;

    SourceFile* sourceFile;
};
