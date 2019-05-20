#pragma once
#include "Job.h"
struct SourceFile;
struct SemanticJob;
struct AstNode;
struct Module;

struct ModuleSemanticJob : public Job
{
    bool         execute() override;
    bool         semanticNode(SourceFile* file, AstNode* node);
    SemanticJob* newSemanticJob(SourceFile* file, AstNode* node);

    Module* module = nullptr;
};
