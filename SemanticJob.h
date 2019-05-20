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
        m_nodes.clear();
    }

    Module*          m_Module;
    SourceFile*      m_SourceFile;
    AstNode*         m_astRoot;
    vector<AstNode*> m_nodes;
};
