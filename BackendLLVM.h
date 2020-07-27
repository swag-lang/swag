#pragma once
#include "Backend.h"
#include "BuildParameters.h"
#include "BackendLLVMDbg.h"

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

    void                    intialize() override;
    JobResult               preCompile(const BuildParameters& buildParameters, Job* ownerJob) override;
    bool                    compile(const BuildParameters& backendParameters) override;
    BackendFunctionBodyJob* newFunctionJob() override;

    bool                createRuntime(const BuildParameters& buildParameters);
    bool                swagTypeToLLVMType(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfo* typeInfo, llvm::Type** llvmType);
    llvm::FunctionType* createFunctionTypeInternal(const BuildParameters& buildParameters, TypeInfoFuncAttr* typeFuncBC);
    bool                createFunctionTypeForeign(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC, llvm::FunctionType** result);
    void                createAssert(const BuildParameters& buildParameters, llvm::Value* toTest, ByteCodeInstruction* ip, const char* msg);
    bool                emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc);
    void                getCallParameters(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRT, vector<llvm::Value*>& params, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& pushRAParams);

    bool emitForeignCall(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRT, Module* moduleToGen, ByteCodeInstruction* ip, VectorNative<uint32_t>& pushParams);
    bool emitFuncWrapperPublic(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* one);

    bool generateObjFile(const BuildParameters& buildParameters);

    bool emitDataSegment(const BuildParameters& buildParameters, DataSegment* dataSegment);
    bool emitInitDataSeg(const BuildParameters& buildParameters);
    bool emitInitConstantSeg(const BuildParameters& buildParameters);

    bool emitGlobalInit(const BuildParameters& buildParameters);
    bool emitGlobalDrop(const BuildParameters& buildParameters);
    bool emitMain(const BuildParameters& buildParameters);

    llvm::BasicBlock* getOrCreateLabel(const BuildParameters& buildParameters, llvm::Function* func, int32_t ip);

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

        llvm::Value* cst0_i8  = nullptr;
        llvm::Value* cst1_i8  = nullptr;
        llvm::Value* cst0_i16 = nullptr;
        llvm::Value* cst0_i32 = nullptr;
        llvm::Value* cst1_i32 = nullptr;
        llvm::Value* cst2_i32 = nullptr;
        llvm::Value* cst3_i32 = nullptr;
        llvm::Value* cst0_i64 = nullptr;
        llvm::Value* cst0_f32 = nullptr;
        llvm::Value* cst0_f64 = nullptr;
        llvm::Value* cst_null = nullptr;

        map<int32_t, llvm::BasicBlock*> labels;

        // Debug infos
        BackendLLVMDbg dbg;

    } perThread[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS];
};