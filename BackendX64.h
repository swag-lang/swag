#pragma once
#include "Backend.h"
#include "BuildParameters.h"

struct Module;
struct BuildParameters;
struct Job;
struct DataSegment;
struct ByteCode;
struct TypeInfo;
struct ByteCodeInstruction;

enum class PatchType
{
    SymbolTableOffset,
    SymbolTableCount,
};

struct X64PerThread
{
    string                filename;
    Concat                concat;
    map<PatchType, void*> allPatches;
    BackendPreCompilePass pass = {BackendPreCompilePass::Init};
};

struct BackendX64 : public Backend
{
    BackendX64(Module* mdl)
        : Backend{mdl}
    {
    }

    JobResult               preCompile(const BuildParameters& buildParameters, Job* ownerJob) override;
    bool                    compile(const BuildParameters& backendParameters) override;
    BackendFunctionBodyJob* newFunctionJob() override;

    bool                createRuntime(const BuildParameters& buildParameters);
    llvm::FunctionType* createFunctionTypeInternal(const BuildParameters& buildParameters, TypeInfoFuncAttr* typeFuncBC);
    bool                createFunctionTypeForeign(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC, llvm::FunctionType** result);
    bool                emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc);
    void                getCallParameters(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRT, VectorNative<llvm::Value*>& params, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& pushRAParams);

    bool emitForeignCall(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRT, Module* moduleToGen, ByteCodeInstruction* ip, VectorNative<uint32_t>& pushParams);
    bool emitFuncWrapperPublic(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* one);

    void applyPatch(X64PerThread& pp, PatchType type, uint32_t value);
    void addPatch(X64PerThread& pp, PatchType type, void* addr);
    bool emitSymbolTable(const BuildParameters& buildParameters);
    bool emitStringTable(const BuildParameters& buildParameters);
    bool emitHeader(const BuildParameters& buildParameters);

    bool generateObjFile(const BuildParameters& buildParameters);

    bool emitDataSegment(const BuildParameters& buildParameters, DataSegment* dataSegment);
    bool emitInitDataSeg(const BuildParameters& buildParameters);
    bool emitInitConstantSeg(const BuildParameters& buildParameters);

    bool emitGlobalInit(const BuildParameters& buildParameters);
    bool emitGlobalDrop(const BuildParameters& buildParameters);
    bool emitMain(const BuildParameters& buildParameters);

    X64PerThread perThread[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS];
};