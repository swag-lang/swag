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
    void      emitFuncSignature(OutputFile& concat, TypeInfoFuncAttr* typeFunc, AstFuncDecl* funcNode, bool light = false);
    void      emitEnumSignature(OutputFile& concat, TypeInfoEnum* typeEnum, AstNode* enumNode);
    void      emitFunctions(OutputFile& concat);
    void      emitFunction(OutputFile& concat, AstNode* node);
    void      emitEnum(OutputFile& concat);

    Utf8 referencableType(TypeInfo* typeInfo);

    vector<AstNode*> nodes;
};

extern Pool<DocNodeJob> g_Pool_docNodeJob;