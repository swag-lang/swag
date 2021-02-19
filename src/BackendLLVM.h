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
enum class SegmentKind;

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
    llvm::GlobalVariable* typeSeg           = nullptr;
    llvm::GlobalVariable* mainContext       = nullptr;
    llvm::GlobalVariable* defaultAllocTable = nullptr;
    llvm::GlobalVariable* processInfos      = nullptr;

    llvm::Type*         interfaceTy    = nullptr;
    llvm::Type*         contextTy      = nullptr;
    llvm::Type*         sliceTy        = nullptr;
    llvm::Type*         processInfosTy = nullptr;
    llvm::FunctionType* allocatorTy    = nullptr;
    llvm::FunctionType* bytecodeRunTy  = nullptr;
    llvm::FunctionType* makeCallbackTy = nullptr;

    llvm::Value* cst0_i1  = nullptr;
    llvm::Value* cst1_i1  = nullptr;
    llvm::Value* cst0_i8  = nullptr;
    llvm::Value* cst1_i8  = nullptr;
    llvm::Value* cst0_i16 = nullptr;
    llvm::Value* cst0_i32 = nullptr;
    llvm::Value* cst1_i32 = nullptr;
    llvm::Value* cst2_i32 = nullptr;
    llvm::Value* cst3_i32 = nullptr;
    llvm::Value* cst4_i32 = nullptr;
    llvm::Value* cst0_i64 = nullptr;
    llvm::Value* cst1_i64 = nullptr;
    llvm::Value* cst0_f32 = nullptr;
    llvm::Value* cst0_f64 = nullptr;
    llvm::Value* cst_null = nullptr;

    llvm::FunctionCallee fn_acosf32;
    llvm::FunctionCallee fn_acosf64;
    llvm::FunctionCallee fn_asinf32;
    llvm::FunctionCallee fn_asinf64;
    llvm::FunctionCallee fn_tanf32;
    llvm::FunctionCallee fn_tanf64;
    llvm::FunctionCallee fn_atanf32;
    llvm::FunctionCallee fn_atanf64;
    llvm::FunctionCallee fn_sinhf32;
    llvm::FunctionCallee fn_sinhf64;
    llvm::FunctionCallee fn_coshf32;
    llvm::FunctionCallee fn_coshf64;
    llvm::FunctionCallee fn_tanhf32;
    llvm::FunctionCallee fn_tanhf64;
    llvm::FunctionCallee fn_powf32;
    llvm::FunctionCallee fn_powf64;
    llvm::FunctionCallee fn_memcmp;
    llvm::FunctionCallee fn_strlen;
    llvm::FunctionCallee fn_malloc;
    llvm::FunctionCallee fn_realloc;
    llvm::FunctionCallee fn_free;

    map<int32_t, llvm::BasicBlock*>             labels;
    map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeInternal;
    map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeForeign;

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
        memset(perThread, 0, sizeof(perThread));
    }

    JobResult               prepareOutput(const BuildParameters& buildParameters, Job* ownerJob) override;
    bool                    generateOutput(const BuildParameters& backendParameters) override;
    BackendFunctionBodyJob* newFunctionJob() override;

    bool                createRuntime(const BuildParameters& buildParameters);
    bool                swagTypeToLLVMType(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfo* typeInfo, llvm::Type** llvmType);
    llvm::FunctionType* createFunctionTypeInternal(const BuildParameters& buildParameters, int numParams);
    llvm::FunctionType* createFunctionTypeInternal(const BuildParameters& buildParameters, TypeInfoFuncAttr* typeFuncBC);
    bool                createFunctionTypeForeign(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC, llvm::FunctionType** result);
    void                emitInternalPanic(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, AstNode* node, const char* message);
    void                setFuncAttributes(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc, llvm::Function* func);
    bool                emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc);
    void                getLocalCallParameters(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, llvm::AllocaInst* allocT, VectorNative<llvm::Value*>& params, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& pushRAParams, const vector<llvm::Value*>& values);

    bool getForeignCallParameters(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC, VectorNative<llvm::Value*>& params, const VectorNative<uint32_t>& pushParams);
    bool getForeignCallReturnValue(const BuildParameters& buildParameters, llvm::AllocaInst* allocRR, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC, llvm::Value* callResult);
    bool emitForeignCall(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, Module* moduleToGen, ByteCodeInstruction* ip, const VectorNative<uint32_t>& pushParams);
    bool emitFuncWrapperPublic(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, AstFuncDecl* node, ByteCode* one);

    bool generateObjFile(const BuildParameters& buildParameters);

    bool emitDataSegment(const BuildParameters& buildParameters, DataSegment* dataSegment);
    bool emitInitSeg(const BuildParameters& buildParameters, DataSegment* dataSegment, SegmentKind me);

    bool emitGlobalInit(const BuildParameters& buildParameters);
    bool emitGlobalDrop(const BuildParameters& buildParameters);
    bool emitOS(const BuildParameters& buildParameters);
    bool emitMain(const BuildParameters& buildParameters);

    llvm::BasicBlock* getOrCreateLabel(LLVMPerThread& pp, llvm::Function* func, int32_t ip);
    bool              storeLocalParam(const BuildParameters& buildParameters, llvm::Function* func, TypeInfoFuncAttr* typeFunc, int idx, llvm::Value* r0);
    void              localCall(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const char* name, const vector<uint32_t>& regs, const vector<llvm::Value*>& values);

    LLVMPerThread* perThread[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS];
};