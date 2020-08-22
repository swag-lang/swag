#pragma once
#include "Backend.h"
#include "OutputFile.h"
#include "VectorNative.h"
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

struct BackendC : public Backend
{
    BackendC(Module* mdl)
        : Backend{mdl}
    {
    }

    JobResult               prepareOutput(const BuildParameters& buildParameters, Job* ownerJob) override;
    bool                    generateOutput(const BuildParameters& backendParameters) override;
    BackendFunctionBodyJob* newFunctionJob() override;

    bool emitRuntime(OutputFile& bufferC, int precompileIndex);
    bool emitDataSegment(OutputFile& bufferC, DataSegment* dataSegment, int precompileIndex);
    bool emitInitMutableSeg(OutputFile& bufferC);
    bool emitInitTypeSeg(OutputFile& bufferC);
    bool emitInitConstantSeg(OutputFile& bufferC);
    bool emitMain(OutputFile& bufferC);
    bool emitGlobalInit(OutputFile& bufferC);
    bool emitGlobalDrop(OutputFile& bufferC);

    bool        emitPublic(OutputFile& bufferC, Module* moduleToGen, Scope* scope);
    static void addCallParameters(Concat& concat, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& pushRAParams);
    static bool swagTypeToCType(Module* moduleToGen, TypeInfo* typeInfo, Utf8& cType);
    static bool emitForeignCall(Concat& concat, Module* moduleToGen, ByteCodeInstruction* ip, VectorNative<uint32_t>& pushParams);
    static bool emitForeignFuncSignature(Concat& concat, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, bool forWrapper);
    static void emitLocalFuncSignature(Concat& concat, TypeInfoFuncAttr* typeFunc, const Utf8& name, bool withNames);
    static bool emitFunctionBody(Concat& concat, Module* moduleToGen, ByteCode* bc);
    static bool emitFuncWrapperPublic(Concat& concat, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* one);

    OutputFile            bufferCFiles[MAX_PRECOMPILE_BUFFERS];
    BackendPreCompilePass pass[MAX_PRECOMPILE_BUFFERS] = {BackendPreCompilePass::Init};
    mutex                 lock;
};