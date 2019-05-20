#pragma once
#include "Job.h"
#include "Module.h"

struct ModuleJob : public Job
{
    bool execute() override;
    bool semanticNode(struct SourceFile* file, struct AstNode* node);

    Module* module = nullptr;
};
