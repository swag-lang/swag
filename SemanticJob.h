#pragma once
#include "Job.h"
struct AstNode;
struct Module;
struct SourceFile;

struct SemanticJob : public Job
{
    bool execute() override;

    void reset() override
    {
        nodes.clear();
    }

    Module*          module;
    SourceFile*      sourceFile;
    AstNode*         astRoot;
    vector<AstNode*> nodes;
};
