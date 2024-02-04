#pragma once
#include "Job.h"
#include "Semantic.h"

struct SourceFile;
struct AstNode;

struct SemanticJob : public Job
{
    virtual ~SemanticJob() = default;
    void    release() override;

    bool                spawnJob();
    JobResult           execute() override;
    static SemanticJob* newJob(Job* dependentJob, SourceFile* sourceFile, AstNode* rootNode, bool run);

    SemanticContext context;
};
