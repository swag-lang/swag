#pragma once
#include "Job.h"
struct SourceFile;
struct SemanticJob;
struct AstNode;
struct Module;

struct ModuleSemanticJob : public Job
{
    JobResult           execute() override;
    static bool         doSemanticNamespace(SourceFile* sourceFile, AstNode* node);
    static bool         doSemanticNode(SourceFile* sourceFile, AstNode* node);
    static SemanticJob* newSemanticJob(SourceFile* sourceFile);

    Module* module = nullptr;
};
