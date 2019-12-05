#pragma once
#include "Backend.h"
#include "OutputFile.h"
struct Module;
struct AstNode;
struct AstFuncDecl;
struct TypeInfoFuncAttr;
struct TypeInfo;
struct BuildParameters;
struct ByteCode;
struct ByteCodeInstruction;
struct DataSegment;
struct Utf8;
struct Job;

enum class BackendCPreCompilePass
{
    Init,
    FunctionBodies,
    End,
};

struct BackendC : public Backend
{
    BackendC(Module* mdl)
        : Backend{mdl}
    {
    }

    JobResult preCompile(Job* ownerJob) override;
    bool      compile(const BuildParameters& backendParameters) override;

    bool emitRuntime();
    bool emitDataSegment(DataSegment* dataSegment);
    void emitArgcArgv();
    bool emitMain();
    bool emitAllFunctionBody(Job* ownerJob);
    bool emitAllFunctionBody(Module* moduleToGen, Job* ownerJob);
    bool emitAllFuncSignatureInternalC();
    bool emitAllFuncSignatureInternalC(Module* moduleToGen);
    bool emitGlobalInit();
    bool emitGlobalDrop();

    bool        emitPublic(Module* moduleToGen, Scope* scope);
    static bool swagTypeToCType(Module* moduleToGen, TypeInfo* typeInfo, Utf8& cType);
    static bool emitForeignCall(Concat& concat, Module* moduleToGen, ByteCodeInstruction* ip, vector<uint32_t>& pushParams);
    static bool emitForeignFuncSignature(Module* moduleToGen, Concat& buffer, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, bool forExport);
    static void emitFuncSignatureInternalC(Concat& concat, ByteCode* bc);
    static bool emitFunctionBody(Concat& concat, Module* moduleToGen, ByteCode* bc);
    static bool emitFuncWrapperPublic(Concat& concat, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* one);

    OutputFile             bufferC;
    BackendCPreCompilePass pass = BackendCPreCompilePass::Init;
};