#pragma once
#include "Job.h"
#include "AstOutput.h"
#include "Concat.h"
struct Scope;
struct AstStruct;
struct AstFuncDecl;

struct ModuleGenDocJob : public Job
{
    struct OneRef
    {
        Utf8                   fullName;
        Utf8                   displayName;
        VectorNative<AstNode*> nodes;
    };

    JobResult execute() override;

    void collectNode(AstNode* node);
    void collectScopes(Scope* root);

    Utf8 outputNode(AstNode* node);
    void outputUserLine(const Utf8& user);
    void outputUserBlock(const Utf8& user);
    void outputCode(const Utf8& code);
    void outputStyles();
    void outputTitle(OneRef& c);

    void generateToc();

    Concat                   concat;
    AstOutput                output;
    AstOutput::OutputContext outputCxt;
    Utf8                     helpContent;

    MapUtf8<VectorNative<AstNode*>> collect;
    Vector<OneRef>                  allNodes;
};
