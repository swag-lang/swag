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
struct BackendLLVMDbg;

struct LLVMPerThread
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

    llvm::Value* cst0_i1  = nullptr;
    llvm::Value* cst1_i1  = nullptr;
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
    BackendLLVMDbg* dbg = nullptr;
};

#define CST_RA32 builder.getInt32(ip->a.u32)
#define CST_RB32 builder.getInt32(ip->b.u32)
#define CST_RC32 builder.getInt32(ip->c.u32)
#define CST_RD32 builder.getInt32(ip->d.u32)
#define CST_RA64 builder.getInt64(ip->a.u64)
#define CST_RB64 builder.getInt64(ip->b.u64)
#define CST_RC64 builder.getInt64(ip->c.u64)
#define GEP_I32(__data, __i32) (__i32 ? builder.CreateInBoundsGEP(__data, builder.getInt32(__i32)) : __data)
#define TO_PTR_PTR_I8(__r) builder.CreatePointerCast(__r, llvm::Type::getInt8PtrTy(context)->getPointerTo())
#define TO_PTR_PTR_I16(__r) builder.CreatePointerCast(__r, llvm::Type::getInt16PtrTy(context)->getPointerTo())
#define TO_PTR_PTR_I32(__r) builder.CreatePointerCast(__r, llvm::Type::getInt32PtrTy(context)->getPointerTo())
#define TO_PTR_PTR_I64(__r) builder.CreatePointerCast(__r, llvm::Type::getInt64PtrTy(context)->getPointerTo())
#define TO_PTR_PTR_F32(__r) builder.CreatePointerCast(__r, llvm::Type::getFloatPtrTy(context)->getPointerTo())
#define TO_PTR_PTR_F64(__r) builder.CreatePointerCast(__r, llvm::Type::getDoublePtrTy(context)->getPointerTo())
#define TO_PTR_PTR_PTR_I8(__r) builder.CreatePointerCast(__r, llvm::Type::getInt8PtrTy(context)->getPointerTo()->getPointerTo())
#define TO_PTR_I64(__r) builder.CreatePointerCast(__r, llvm::Type::getInt64PtrTy(context))
#define TO_PTR_I32(__r) builder.CreatePointerCast(__r, llvm::Type::getInt32PtrTy(context))
#define TO_PTR_I16(__r) builder.CreatePointerCast(__r, llvm::Type::getInt16PtrTy(context))
#define TO_PTR_I8(__r) builder.CreatePointerCast(__r, llvm::Type::getInt8PtrTy(context))
#define TO_PTR_F64(__r) builder.CreatePointerCast(__r, llvm::Type::getDoublePtrTy(context))
#define TO_PTR_F32(__r) builder.CreatePointerCast(__r, llvm::Type::getFloatPtrTy(context))
#define TO_BOOL(__r) builder.CreateIntCast(__r, llvm::Type::getInt1Ty(context), false)

struct BackendLLVM : public Backend
{
    BackendLLVM(Module* mdl)
        : Backend{mdl}
    {
    }

    JobResult               preCompile(const BuildParameters& buildParameters, Job* ownerJob) override;
    bool                    compile(const BuildParameters& backendParameters) override;
    BackendFunctionBodyJob* newFunctionJob() override;

    bool                createRuntime(const BuildParameters& buildParameters);
    bool                swagTypeToLLVMType(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfo* typeInfo, llvm::Type** llvmType);
    llvm::FunctionType* createFunctionTypeInternal(const BuildParameters& buildParameters, TypeInfoFuncAttr* typeFuncBC);
    bool                createFunctionTypeForeign(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC, llvm::FunctionType** result);
    void                createAssert(const BuildParameters& buildParameters, llvm::Value* toTest, ByteCodeInstruction* ip, const char* msg);
    bool                emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc);
    void                getCallParameters(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRT, VectorNative<llvm::Value*>& params, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& pushRAParams);

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

    LLVMPerThread perThread[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS];
};