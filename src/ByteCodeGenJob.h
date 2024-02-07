#pragma once
#include "ByteCodeGenContext.h"
#include "Job.h"

struct ByteCodeGenJob : Job
{
    virtual ~ByteCodeGenJob() = default;
    void    release() override;

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
