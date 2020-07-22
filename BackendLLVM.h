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
struct ByteCodeInstruction;

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

    llvm::BasicBlock* getOrCreateLabel(const BuildParameters& buildParameters, llvm::Function* func, ByteCodeInstruction* ip);

    struct
    {
        llvm::LLVMContext*    context;
        llvm::IRBuilder<>*    builder;
        llvm::Module*         module;
        string                filename;
        BackendPreCompilePass pass = {BackendPreCompilePass::Init};

        llvm::GlobalVariable* bssSeg            = nullptr;
        llvm::GlobalVariable* mutableSeg        = nullptr;
        llvm::GlobalVariable* constantSeg       = nullptr;
        llvm::GlobalVariable* mainContext       = nullptr;
        llvm::GlobalVariable* defaultAllocTable = nullptr;
        llvm::GlobalVariable* processInfos      = nullptr;

        llvm::Type*         interfaceTy    = nullptr;
        llvm::Type*         contextTy      = nullptr;
        llvm::Type*         sliceTy        = nullptr;
        llvm::Type*         processInfosTy = nullptr;
        llvm::FunctionType* allocatorTy    = nullptr;
        llvm::FunctionType* bytecodeRunTy  = nullptr;

        llvm::Value* cst0_i32 = nullptr;
        llvm::Value* cst1_i32 = nullptr;
        llvm::Value* cst2_i32 = nullptr;
        llvm::Value* cst1_i8  = nullptr;

        map<ByteCodeInstruction*, llvm::BasicBlock*> labels;

    } perThread[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS];
};