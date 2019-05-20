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

typedef bool (*resolveFct)(SemanticJob* job, AstNode* node, SemanticResult& result);

struct SemanticJob : public Job
{
    bool execute() override;

    void reset() override
    {
        nodes.clear();
    }

    static bool resolveVarDecl(SemanticJob* job, AstNode* node, SemanticResult& result);

    resolveFct       fct;
    Module*          module;
    SourceFile*      sourceFile;
    AstNode*         astRoot;
    vector<AstNode*> nodes;
};
