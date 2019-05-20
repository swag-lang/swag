#pragma once
#include "Job.h"

struct SemanticJob : public Job
{
    bool execute() override;

    vector<struct AstNode*> nodes;
};
