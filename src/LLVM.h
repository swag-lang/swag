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
struct LLVMDebug;
enum class SegmentKind;

struct LLVMPerThread
{
    llvm::LLVMContext*    context;
    llvm::IRBuilder<>*    builder;
    llvm::Module*         module;
    Utf8                  filename;
    BackendPreCompilePass pass = {BackendPreCompilePass::Init};

    llvm::GlobalVariable* bssSeg               = nullptr;
    llvm::GlobalVariable* mutableSeg           = nullptr;
    llvm::GlobalVariable* constantSeg          = nullptr;
    llvm::GlobalVariable* tlsSeg               = nullptr;
    llvm::GlobalVariable* mainContext          = nullptr;
    llvm::GlobalVariable* defaultAllocTable    = nullptr;
    llvm::GlobalVariable* processInfos         = nullptr;
    llvm::GlobalVariable* symTls_threadLocalId = nullptr;

    llvm::Type*         errorTy        = nullptr;
    llvm::Type*         scratchTy      = nullptr;
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

    Map<int64_t, llvm::BasicBlock*>             labels;
    Map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeInternal;
    Map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeInternalClosure;
    Map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeForeign;
    Map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeForeignClosure;

    // Debug infos
    LLVMDebug* dbg = nullptr;
};

struct LLVM : public Backend
{
    LLVM(Module* mdl)
        : Backend{mdl}
    {
        memset(perThread, 0, sizeof(perThread));
    }

    JobResult prepareOutput(int stage, const BuildParameters& buildParameters, Job* ownerJob) override;
    bool      generateOutput(const BuildParameters& backendParameters) override;
    bool      emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc) override;

    bool                createRuntime(const BuildParameters& buildParameters);
    llvm::Value*        getImmediateConstantA(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint32_t numBits);
    llvm::Type*         swagTypeToLLVMType(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfo* typeInfo);
    void                createRet(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFunc, TypeInfo* returnType, llvm::AllocaInst* allocResult);
    llvm::FunctionType* getOrCreateFuncType(const BuildParameters& buildParameters, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC, bool closureToLambda = false);
    void                emitInternalPanic(const BuildParameters& buildParameters, Module* moduleToGen, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, AstNode* node, const char* message);
    void                setFuncAttributes(const BuildParameters& buildParameters, Module* moduleToGen, AstFuncDecl* funcNode, ByteCode* bc, llvm::Function* func);
    void                storeTypedValueToRegister(llvm::LLVMContext& context, const BuildParameters& buildParameters, llvm::Value* value, uint32_t reg, llvm::AllocaInst* allocR);
    void                storeRT2ToRegisters(llvm::LLVMContext& context, const BuildParameters& buildParameters, uint32_t reg0, uint32_t reg1, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR);
    void                getReturnResult(llvm::LLVMContext& context, const BuildParameters& buildParameters, Module* moduleToGen, TypeInfo* returnType, bool imm, Register& reg, llvm::AllocaInst* allocR, llvm::AllocaInst* allocResult);
    void                emitByteCodeCallParameters(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, llvm::AllocaInst* allocT, VectorNative<llvm::Value*>& params, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& pushRAParams, const Vector<llvm::Value*>& values, bool closureToLambda = false);

    void emitShiftRightArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint32_t numBits);
    void emitShiftRightEqArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint32_t numBits);
    void emitShiftLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint32_t numBits, bool left, bool isSigned);
    void emitShiftEqLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, ByteCodeInstruction* ip, uint32_t numBits, bool left, bool isSigned);

    bool         emitCallParameters(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC, VectorNative<llvm::Value*>& params, const VectorNative<uint32_t>& pushParams, const Vector<llvm::Value*>& values, bool closureToLambda = false);
    bool         emitCallReturnValue(const BuildParameters& buildParameters, llvm::AllocaInst* allocRR, Module* moduleToGen, TypeInfoFuncAttr* typeFuncBC, llvm::Value* callResult);
    llvm::Value* emitCall(const BuildParameters& buildParameters, Module* moduleToGen, const Utf8& funcName, TypeInfoFuncAttr* typeFunc, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const VectorNative<uint32_t>& pushParams, const Vector<llvm::Value*>& values, bool localCall);

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
    bool              emitGetParam(llvm::LLVMContext& context, const BuildParameters& buildParameters, llvm::Function* func, TypeInfoFuncAttr* typeFunc, uint32_t rdest, uint32_t paramIdx, llvm::AllocaInst* allocR, int sizeOf = 0, uint64_t toAdd = 0, int deRefSize = 0);
    llvm::Value*      emitCall(const BuildParameters& buildParameters, Module* moduleToGen, const char* name, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const Vector<uint32_t>& regs, const Vector<llvm::Value*>& values);

    LLVMPerThread* perThread[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS];
};