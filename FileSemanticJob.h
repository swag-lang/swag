#pragma once
#include "SemanticJob.h"
struct AstNode;
struct Module;
struct SourceFile;

struct FileSemanticJob : public SemanticJob
{
    JobResult           execute() override;
    static SemanticJob* newSemanticJob(SourceFile* sourceFile);
};
