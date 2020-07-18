#pragma once
#include "Backend.h"
#include "BackendHelpers.h"

struct Module;
struct BuildParameters;
struct Job;
struct DataSegment;

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

    bool generateObjFile(const BuildParameters& buildParameters, int preCompileIndex);
    bool emitDataSegment(DataSegment* dataSegment, int preCompileIndex);
    bool emitMain(int precompileIndex);

    llvm::LLVMContext*    llvmContext[MAX_PRECOMPILE_BUFFERS];
    llvm::IRBuilder<>*    llvmBuilder[MAX_PRECOMPILE_BUFFERS];
    llvm::Module*         llvmModule[MAX_PRECOMPILE_BUFFERS];
    string                bufferFiles[MAX_PRECOMPILE_BUFFERS];
    BackendPreCompilePass pass[MAX_PRECOMPILE_BUFFERS] = {BackendPreCompilePass::Init};
};