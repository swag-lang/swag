#pragma once
#include "Job.h"
#include "ByteCodeGenContext.h"

struct ByteCodeGenJob : public Job
{
    virtual ~ByteCodeGenJob() = default;
    void release() override;

    JobResult              execute() override;
    JobResult              waitForDependenciesGenerated();
    static ByteCodeGenJob* newJob(Job* dependentJob, SourceFile* sourceFile, AstNode* root);

    enum class Pass
    {
        Generate,
        WaitForDependenciesGenerated,
        ComputeDependenciesResolved,
    };

    ByteCodeGenContext context;
    Pass               pass = Pass::Generate;
};
