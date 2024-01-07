#pragma once
#include "Job.h"
#include "ByteCodeGenContext.h"

struct ByteCodeGenJob : public Job
{
    virtual ~ByteCodeGenJob() = default;
    void release() override;

    JobResult execute() override;
    JobResult waitForDependenciesGenerated();

    ByteCodeGenContext context;
};
