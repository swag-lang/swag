#pragma once
#include "Job.h"
struct AstNode;
struct Concat;
struct TypeInfoFuncAttr;
struct AstFuncDecl;
struct OutputFile;

struct DocNodeJob : public Job
{
    JobResult execute() override;
    bool      emitFuncSignature(OutputFile& concat, TypeInfoFuncAttr* typeFunc, AstFuncDecl* funcNode);
    void      emitFunction(OutputFile& concat);
    void      emitEnum(OutputFile& concat);

    AstNode* node = nullptr;
};

extern thread_local Pool<DocNodeJob> g_Pool_docNodeJob;