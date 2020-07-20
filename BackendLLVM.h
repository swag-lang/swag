#pragma once
#include "Backend.h"
#include "BackendHelpers.h"
#include "BuildParameters.h"

struct Module;
struct BuildParameters;
struct Job;
struct DataSegment;
struct ByteCode;
struct TypeInfo;

struct BackendLLVM : public Backend
{
    BackendLLVM(Module* mdl)
        : Backend{mdl}
    {
    }

    void                    setup() override;
    JobResult               preCompile(const BuildParameters& buildParameters, Job* ownerJob) override;
    bool                    compile(const BuildParameters& backendParameters) override;
    BackendFunctionBodyJob* newFunctionJob() override;

    bool createRuntime(const BuildParameters& buildParameters);
    bool swagTypeToLLVMType(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfo* typeInfo, llvm::Type** llvmType);
    bool emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc);
    bool emitFuncWrapperPublic(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* one);

    bool generateObjFile(const BuildParameters& buildParameters);

    bool emitDataSegment(const BuildParameters& buildParameters, DataSegment* dataSegment);
    bool emitInitDataSeg(const BuildParameters& buildParameters);
    bool emitInitConstantSeg(const BuildParameters& buildParameters);

    bool emitGlobalInit(const BuildParameters& buildParameters);
    bool emitGlobalDrop(const BuildParameters& buildParameters);
    bool emitMain(const BuildParameters& buildParameters);

    struct PerType
    {
        llvm::LLVMContext*    llvmContext[MAX_PRECOMPILE_BUFFERS];
        llvm::IRBuilder<>*    llvmBuilder[MAX_PRECOMPILE_BUFFERS];
        llvm::Module*         llvmModule[MAX_PRECOMPILE_BUFFERS];
        string                bufferFiles[MAX_PRECOMPILE_BUFFERS];
        BackendPreCompilePass pass[MAX_PRECOMPILE_BUFFERS] = {BackendPreCompilePass::Init};
    } perType[BackendCompileType::Count];

    llvm::GlobalVariable* mutableSeg  = nullptr;
    llvm::GlobalVariable* constantSeg = nullptr;
};