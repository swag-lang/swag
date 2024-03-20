#pragma once
#include "Semantic/Semantic.h"
#include "Threading/Job.h"

struct SourceFile;
struct AstNode;

struct SemanticJob final : Job
{
    void release() override;

    bool                spawnJob();
    JobResult           execute() override;
    static SemanticJob* newJob(Job* depJob, SourceFile* file, AstNode* rootNode, bool run);

    SemanticContext context;
};
