#pragma once
#include "Backend.h"
#include "BackendParameters.h"

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

    llvm::GlobalVariable* bssSeg               = nullptr;
    llvm::GlobalVariable* mutableSeg           = nullptr;
    llvm::GlobalVariable* constantSeg          = nullptr;
    llvm::GlobalVariable* tlsSeg               = nullptr;
    llvm::GlobalVariable* mainContext          = nullptr;
    llvm::GlobalVariable* defaultAllocTable    = nullptr;
    llvm::GlobalVariable* processInfos         = nullptr;
    llvm::GlobalVariable* symTls_threadLocalId = nullptr;

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
    llvm::Value* cst5_i32 = nullptr;
    llvm::Value* cst6_i32 = nullptr;
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
    llvm::FunctionCallee fn_atan2f32;
    llvm::FunctionCallee fn_atan2f64;
    llvm::FunctionCallee fn_memcmp;
    llvm::FunctionCallee fn_strlen;
    llvm::FunctionCallee fn_strcmp;
    llvm::FunctionCallee fn_malloc;
    llvm::FunctionCallee fn_realloc;
    llvm::FunctionCallee fn_free;

    map<int64_t, llvm::BasicBlock*>             labels;
    map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeInternal;
    map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeInternalClosure;
    map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeForeign;
    map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeForeignClosure;

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
#define TO_PTR_IX(__r, __n) builder.CreatePointerCast(__r, llvm::Type::getIntNPtrTy(context, __n))
#define TO_PTR_F64(__r) builder.CreatePointerCast(__r, llvm::Type::getDoublePtrTy(context))
#define TO_PTR_F32(__r) builder.CreatePointerCast(__r, llvm::Type::getFloatPtrTy(context))
#define TO_PTR_I_N(__r, __numBits) builder.CreatePointerCast(__r, getIntPtrType(context, __numBits))
#define TO_PTR_PTR_I_N(__r, __numBits) builder.CreatePointerCast(__r, getIntPtrType(context, __numBits)->getPointerTo())
#define TO_BOOL(__r) builder.CreateIntCast(__r, llvm::Type::getInt1Ty(context), false)

struct BackendLLVM : public Backend
{
    BackendLLVM(Module* mdl)
        : Backend{mdl}
    {
        memset(perThread, 0, sizeof(perThread));
    }

    JobResult                   prepareOutput(int stage, const BuildParameters& buildParameters, Job* ownerJob) override;
    bool                        generateOutput(const BuildParameters& backendParameters) override;
    BackendFunctionBodyJobBase* newFunctionJob() override;

    bool                createRuntime(const BuildParameters& buildParameters);
    llvm::Value*        getImmediateConstantA(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint8_t numBits);
    llvm::Type*         getIntType(llvm::LLVMContext& context, uint8_t numBits);
    llvm::Type*         getIntPtrType(llvm::LLVMContext& context, uint8_t numBits);
    llvm::Type*         swagTypeToLLVMType(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfo* typeInfo);
    llvm::FunctionType* getOrCreateFuncType(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC, bool closureToLambda = false);
    void                emitInternalPanic(const BuildParameters& buildParameters, Module* moduleToGen, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, AstNode* node, const char* message);
    void                setFuncAttributes(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc, llvm::Function* func);
    void                storeTypedValueToRegister(llvm::LLVMContext& context, const BuildParameters& buildParameters, llvm::Value* value, uint32_t reg, llvm::AllocaInst* allocR);
    void                storeRT2ToRegisters(llvm::LLVMContext& context, const BuildParameters& buildParameters, uint32_t reg0, uint32_t reg1, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR);
    void                getReturnResult(llvm::LLVMContext& context, const BuildParameters& buildParameters, Module* moduleToGen, TypeInfo* returnType, bool imm, Register& reg, llvm::AllocaInst* allocR, llvm::AllocaInst* allocResult);
    bool                emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc);
    void                emitByteCodeCallParameters(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, llvm::AllocaInst* allocT, VectorNative<llvm::Value*>& params, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& pushRAParams, const vector<llvm::Value*>& values, bool closureToLambda = false);

    void emitShiftArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint8_t numBits);
    void emitShiftLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint8_t numBits, bool left);
    void emitShiftEqArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint8_t numBits);
    void emitShiftEqLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint8_t numBits, bool left);

    bool         emitCallParameters(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC, VectorNative<llvm::Value*>& params, const VectorNative<uint32_t>& pushParams, const vector<llvm::Value*>& values, bool closureToLambda = false);
    bool         emitCallReturnValue(const BuildParameters& buildParameters, llvm::AllocaInst* allocRR, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC, llvm::Value* callResult);
    llvm::Value* emitCall(const BuildParameters& buildParameters, Module* moduleToGen, const Utf8& funcName, TypeInfoFuncAttr* typeFunc, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const VectorNative<uint32_t>& pushParams, const vector<llvm::Value*>& values, bool localCall);

    bool generateObjFile(const BuildParameters& buildParameters);

    bool emitDataSegment(const BuildParameters& buildParameters, DataSegment* dataSegment);
    bool emitInitSeg(const BuildParameters& buildParameters, DataSegment* dataSegment, SegmentKind me);

    bool emitGetTypeTable(const BuildParameters& buildParameters);
    bool emitGlobalPreMain(const BuildParameters& buildParameters);
    bool emitGlobalInit(const BuildParameters& buildParameters);
    bool emitGlobalDrop(const BuildParameters& buildParameters);
    bool emitOS(const BuildParameters& buildParameters);
    bool emitMain(const BuildParameters& buildParameters);

    llvm::BasicBlock* getOrCreateLabel(LLVMPerThread& pp, llvm::Function* func, int64_t ip);
    bool              emitGetParam(llvm::LLVMContext& context, const BuildParameters& buildParameters, llvm::Function* func, TypeInfoFuncAttr* typeFunc, int idx, llvm::Value* r0, int sizeOf = 0, uint64_t toAdd = 0, int deRefSize = 0);
    llvm::Value*      emitCall(const BuildParameters& buildParameters, Module* moduleToGen, const char* name, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const vector<uint32_t>& regs, const vector<llvm::Value*>& values);

    LLVMPerThread* perThread[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS];
};