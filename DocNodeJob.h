#pragma once
#include "Job.h"
#include "Utf8.h"
struct AstNode;
struct Concat;
struct TypeInfoFuncAttr;
struct TypeInfoEnum;
struct AstFuncDecl;
struct OutputFile;
struct TypeInfo;

struct DocNodeJob : public Job
{
    JobResult execute() override;
    void      emitFuncSignature(OutputFile& concat, TypeInfoFuncAttr* typeFunc, AstFuncDecl* funcNode);
    void      emitEnumSignature(OutputFile& concat, TypeInfoEnum* typeEnum, AstNode* enumNode);
    void      emitFunction(OutputFile& concat);
    void      emitEnum(OutputFile& concat);

    Utf8 referencableType(TypeInfo* typeInfo);

    vector<AstNode*> nodes;
};

extern thread_local Pool<DocNodeJob> g_Pool_docNodeJob;