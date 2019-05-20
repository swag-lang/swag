#pragma once
#include "Job.h"
struct AstNode;
struct Module;
struct SourceFile;
struct SemanticJob;

enum class SemanticResult
{
    Done,
    Pending,
};

struct SemanticContext
{
    SemanticJob*   job;
    AstNode*       node;
    SemanticResult result;
};

struct SemanticJob : public Job
{
    bool execute() override;

    void reset() override
    {
        nodes.clear();
    }

    static bool resolveType(SemanticContext* context);
    static bool resolveVarDecl(SemanticContext* context);

    Module*          module;
    SourceFile*      sourceFile;
    AstNode*         astRoot;
    vector<AstNode*> nodes;
};
