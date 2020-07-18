#pragma once
#include "Backend.h"
#include "BackendHelpers.h"

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
    JobResult               preCompile(const BuildParameters& buildParameters, Job* ownerJob, int preCompileIndex) override;
    bool                    compile(const BuildParameters& backendParameters) override;
    BackendFunctionBodyJob* newFunctionJob() override;

    bool swagTypeToLLVMType(Module* moduleToGen, TypeInfo* typeInfo, int precompileIndex, llvm::Type** llvmType);
    bool emitFunctionBody(Module* moduleToGen, ByteCode* bc, int preCompileIndex);
    bool emitFuncWrapperPublic(Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* one, int preCompileIndex);

    bool generateObjFile(const BuildParameters& buildParameters, int preCompileIndex);
    bool emitDataSegment(DataSegment* dataSegment, int preCompileIndex);

    bool emitGlobalDrop(const BuildParameters& buildParameters, int precompileIndex);
    bool emitMain(const BuildParameters& buildParameters, int precompileIndex);

    llvm::LLVMContext*    llvmContext[MAX_PRECOMPILE_BUFFERS];
    llvm::IRBuilder<>*    llvmBuilder[MAX_PRECOMPILE_BUFFERS];
    llvm::Module*         llvmModule[MAX_PRECOMPILE_BUFFERS];
    string                bufferFiles[MAX_PRECOMPILE_BUFFERS];
    BackendPreCompilePass pass[MAX_PRECOMPILE_BUFFERS] = {BackendPreCompilePass::Init};
};