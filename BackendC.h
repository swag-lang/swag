#pragma once
#include "Backend.h"
#include "OutputFile.h"
#include "VectorNative.h"
#include "BackendHelpers.h"
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

struct BackendC : public Backend
{
    BackendC(Module* mdl)
        : Backend{mdl}
    {
    }

    void                    setup();
    JobResult               preCompile(const BuildParameters& buildParameters, Job* ownerJob) override;
    bool                    compile(const BuildParameters& backendParameters) override;
    BackendFunctionBodyJob* newFunctionJob() override;

    bool emitRuntime(OutputFile& bufferC, int precompileIndex);
    bool emitDataSegment(OutputFile& bufferC, DataSegment* dataSegment, int precompileIndex);
    bool emitInitDataSeg(OutputFile& bufferC);
    bool emitInitConstantSeg(OutputFile& bufferC);
    bool emitMain(OutputFile& bufferC);
    bool emitGlobalInit(OutputFile& bufferC);
    bool emitGlobalDrop(OutputFile& bufferC);

    bool        emitPublic(OutputFile& bufferC, Module* moduleToGen, Scope* scope);
    static void addCallParameters(Concat& concat, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& pushRAParams);
    static bool swagTypeToCType(Module* moduleToGen, TypeInfo* typeInfo, Utf8& cType);
    static bool emitForeignCall(Concat& concat, Module* moduleToGen, ByteCodeInstruction* ip, VectorNative<uint32_t>& pushParams);
    static bool emitForeignFuncSignature(Concat& concat, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, bool forWrapper);
    static void emitFuncSignatureInternalC(Concat& concat, ByteCode* bc, bool forDecl);
    static bool emitFunctionBody(Concat& concat, Module* moduleToGen, ByteCode* bc);
    static bool emitFuncWrapperPublic(Concat& concat, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* one);

    OutputFile            bufferCFiles[MAX_PRECOMPILE_BUFFERS];
    BackendPreCompilePass pass[MAX_PRECOMPILE_BUFFERS] = {BackendPreCompilePass::Init};
    mutex                 lock;
    BackendCCompiler*     compiler = nullptr;
};