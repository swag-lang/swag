#pragma once
#include "Job.h"
struct AstNode;
struct Concat;
struct TypeInfoFuncAttr;
struct TypeInfoEnum;
struct AstFuncDecl;
struct OutputFile;

struct DocNodeJob : public Job
{
    JobResult execute() override;
    void      emitFuncSignature(OutputFile& concat, TypeInfoFuncAttr* typeFunc, AstFuncDecl* funcNode);
    void      emitEnumSignature(OutputFile& concat, TypeInfoEnum* typeEnum, AstNode* enumNode);
    void      emitFunction(OutputFile& concat);
    void      emitEnum(OutputFile& concat);

    AstNode* node = nullptr;
};

extern thread_local Pool<DocNodeJob> g_Pool_docNodeJob;