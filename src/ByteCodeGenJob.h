#pragma once
#include "ByteCodeGenContext.h"
#include "Job.h"

struct ByteCodeGenJob final : Job
{
    void release() override;

    static JobResult       leaveJob(AstNode* node);
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
