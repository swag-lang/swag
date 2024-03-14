#pragma once
#include "Threading/Job.h"
#include "Semantic/Semantic.h"

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
