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
struct BackendCCompiler;

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

    bool      check() override;
    JobResult preCompile(Job* ownerJob, int preCompileIndex) override;
    bool      compile(const BuildParameters& backendParameters) override;

    bool emitRuntime(OutputFile& bufferC, int preCompileIndex);
    bool emitDataSegment(OutputFile& bufferC, DataSegment* dataSegment, int preCompileIndex);
    void emitArgcArgv(OutputFile& bufferC);
    bool emitMain(OutputFile& bufferC);
    bool emitAllFunctionBody(OutputFile& bufferC, Job* ownerJob, int preCompileIndex);
    bool emitAllFunctionBody(OutputFile& bufferC, Module* moduleToGen, Job* ownerJob, int preCompileIndex, bool full);
    bool emitAllFuncSignatureInternalC(OutputFile& bufferC);
    bool emitAllFuncSignatureInternalC(OutputFile& bufferC, Module* moduleToGen);
    bool emitGlobalInit(OutputFile& bufferC);
    bool emitGlobalDrop(OutputFile& bufferC);

    bool        emitPublic(OutputFile& bufferC, Module* moduleToGen, Scope* scope);
    static void addCallParameters(Concat& concat, TypeInfoFuncAttr* typeFuncBC, vector<uint32_t>& pushRAParams);
    static bool swagTypeToCType(Module* moduleToGen, TypeInfo* typeInfo, Utf8& cType);
    static bool emitForeignCall(Concat& concat, Module* moduleToGen, ByteCodeInstruction* ip, vector<uint32_t>& pushParams);
    static bool emitForeignFuncSignature(Concat& concat, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, bool forExport);
    static void emitFuncSignatureInternalC(Concat& concat, ByteCode* bc);
    static bool emitFunctionBody(Concat& concat, Module* moduleToGen, ByteCode* bc);
    static bool emitFuncWrapperPublic(Concat& concat, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* one);

    OutputFile             bufferCFiles[MAX_PRECOMPILE_BUFFERS];
    BackendCPreCompilePass pass[MAX_PRECOMPILE_BUFFERS] = {BackendCPreCompilePass::Init};
    mutex                  lock;
    BackendCCompiler*      compiler = nullptr;
};