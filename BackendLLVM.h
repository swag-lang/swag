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
        , llvmBuilder(llvmContext)
    {
    }

    void      setup() override;
    JobResult preCompile(const BuildParameters& buildParameters, Job* ownerJob, int preCompileIndex) override;
    bool      compile(const BuildParameters& backendParameters) override;
    bool      generateObjFile(const BuildParameters& buildParameters, int preCompileIndex);

    bool emitDataSegment(DataSegment* dataSegment, int preCompileIndex);
    bool emitMain();

    llvm::LLVMContext llvmContext;
    llvm::IRBuilder<> llvmBuilder;
    llvm::Module*     llvmModule = nullptr;

    string                bufferFiles[MAX_PRECOMPILE_BUFFERS];
    BackendPreCompilePass pass[MAX_PRECOMPILE_BUFFERS] = {BackendPreCompilePass::Init};
};