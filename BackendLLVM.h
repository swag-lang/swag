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

    bool swagTypeToLLVMType(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfo* typeInfo, llvm::Type** llvmType);
    bool emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc);
    bool emitFuncWrapperPublic(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* one);

    bool generateObjFile(const BuildParameters& buildParameters);
    bool emitDataSegment(const BuildParameters& buildParameters, DataSegment* dataSegment);

    bool emitGlobalDrop(const BuildParameters& buildParameters);
    bool emitMain(const BuildParameters& buildParameters);

    llvm::LLVMContext*    llvmContext[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS] = {0};
    llvm::IRBuilder<>*    llvmBuilder[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS] = {0};
    llvm::Module*         llvmModule[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS]  = {0};
    string                bufferFiles[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS];
    BackendPreCompilePass pass[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS] = {BackendPreCompilePass::Init};
};