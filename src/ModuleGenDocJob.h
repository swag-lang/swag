#pragma once
#include "Job.h"
#include "AstOutput.h"
#include "Concat.h"
struct Scope;
struct AstStruct;
struct AstFuncDecl;

struct ModuleGenDocJob : public Job
{
    JobResult execute() override;

    void outputNode(AstNode* node);
    void collectNodes(Scope* root);

    Concat                   concat;
    AstOutput                output;
    AstOutput::OutputContext outputCxt;
    Utf8                     helpContent;

    MapUtf8<VectorNative<AstNode*>> collect;
};
