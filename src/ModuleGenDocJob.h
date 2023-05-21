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

    void collectNode(AstNode* node);
    void collectScopes(Scope* root);

    Utf8 outputNode(AstNode* node);
    void outputCode(const Utf8& code);

    void generateToc();

    struct OneRef
    {
        Utf8                   name;
        VectorNative<AstNode*> nodes;
    };

    Concat                   concat;
    AstOutput                output;
    AstOutput::OutputContext outputCxt;
    Utf8                     helpContent;

    MapUtf8<VectorNative<AstNode*>> collect;
    Vector<OneRef>                  allNodes;
};
