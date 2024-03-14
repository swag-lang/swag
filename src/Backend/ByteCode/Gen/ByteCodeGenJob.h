#pragma once
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Threading/Job.h"

struct ByteCodeGenJob final : Job
{
    void release() override;

    static JobResult       leaveJob(AstNode* node);
    JobResult              execute() override;
    JobResult              waitForDependenciesGenerated();
    static ByteCodeGenJob* newJob(Job* depJob, SourceFile* srcFile, AstNode* root);

    enum class Pass
    {
        Generate,
        WaitForDependenciesGenerated,
        ComputeDependenciesResolved,
    };

    ByteCodeGenContext context;
    Pass               pass = Pass::Generate;
};
