#pragma once
#include "Job.h"
struct SourceFile;
struct SemanticJob;
struct AstNode;
struct Module;

struct ModuleSemanticJob : public Job
{
    JobResult    execute() override;
    bool         doSemanticNode(SourceFile* file, AstNode* node);
    SemanticJob* newSemanticJob(SourceFile* file, AstNode* node);

    Module* module = nullptr;
};
