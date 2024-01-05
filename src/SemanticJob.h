#pragma once
#include "Job.h"
#include "Semantic.h"
struct SourceFile;
struct AstNode;

struct SemanticJob : public Job
{
    JobResult execute() override;

    virtual ~SemanticJob() = default;
    void                release() override;
    static SemanticJob* newJob(Job* dependentJob, SourceFile* sourceFile, AstNode* rootNode, bool run);

    SemanticContext context;
    Semantic        sem;
};
