#pragma once
#include "Backend/Backend.h"
#include "Backend/BackendParameters.h"

struct BuildParameters;
struct ByteCode;
struct ByteCodeInstruction;
struct DataSegment;
struct Job;
struct LLVMDebug;
struct Module;
struct TypeInfo;
union Register;
enum class SegmentKind;
enum class SafetyMsg;

struct LLVMEncoder : BackendEncoder
{
    llvm::IRBuilder<>* builder;
    llvm::LLVMContext* llvmContext;
    llvm::Module*      llvmModule;

    llvm::GlobalVariable* bssSeg              = nullptr;
    llvm::GlobalVariable* mutableSeg          = nullptr;
    llvm::GlobalVariable* constantSeg         = nullptr;
    llvm::GlobalVariable* tlsSeg              = nullptr;
    llvm::GlobalVariable* mainContext         = nullptr;
    llvm::GlobalVariable* defaultAllocTable   = nullptr;
    llvm::GlobalVariable* processInfos        = nullptr;
    llvm::GlobalVariable* symTlsThreadLocalId = nullptr;

    llvm::Type*         errorTy        = nullptr;
    llvm::Type*         scratchTy      = nullptr;
    llvm::Type*         interfaceTy    = nullptr;
    llvm::Type*         contextTy      = nullptr;
    llvm::Type*         sliceTy        = nullptr;
    llvm::Type*         processInfosTy = nullptr;
    llvm::FunctionType* allocatorTy    = nullptr;
    llvm::FunctionType* bytecodeRunTy  = nullptr;
    llvm::FunctionType* makeCallbackTy = nullptr;

    llvm::Value* cstAi1  = nullptr;
    llvm::Value* cstBi1  = nullptr;
    llvm::Value* cstAi8  = nullptr;
    llvm::Value* cstBi8  = nullptr;
    llvm::Value* cstAi16 = nullptr;
    llvm::Value* cstAi32 = nullptr;
    llvm::Value* cstBi32 = nullptr;
    llvm::Value* cstCi32 = nullptr;
    llvm::Value* cstDi32 = nullptr;
    llvm::Value* cstEi32 = nullptr;
    llvm::Value* cstFi32 = nullptr;
    llvm::Value* cstGi32 = nullptr;
    llvm::Value* cstAi64 = nullptr;
    llvm::Value* cstBi64 = nullptr;
    llvm::Value* cstAf32 = nullptr;
    llvm::Value* cstAf64 = nullptr;
    llvm::Value* cstNull = nullptr;

    llvm::FunctionCallee fnAcosF32;
    llvm::FunctionCallee fnAcosF64;
    llvm::FunctionCallee fnAsinF32;
    llvm::FunctionCallee fnAsinF64;
    llvm::FunctionCallee fnTanF32;
    llvm::FunctionCallee fnTanF64;
    llvm::FunctionCallee fnAtanF32;
    llvm::FunctionCallee fnAtanF64;
    llvm::FunctionCallee fnSinhF32;
    llvm::FunctionCallee fnSinhF64;
    llvm::FunctionCallee fnCoshF32;
    llvm::FunctionCallee fnCoshF64;
    llvm::FunctionCallee fnTanhF32;
    llvm::FunctionCallee fnTanhF64;
    llvm::FunctionCallee fnPowF32;
    llvm::FunctionCallee fnPowF64;
    llvm::FunctionCallee fnAtan2F32;
    llvm::FunctionCallee fnAtan2F64;
    llvm::FunctionCallee fnMemCmp;
    llvm::FunctionCallee fnStrlen;
    llvm::FunctionCallee fnStrcmp;
    llvm::FunctionCallee fnMalloc;
    llvm::FunctionCallee fnReAlloc;
    llvm::FunctionCallee fnFree;

    Map<int64_t, llvm::BasicBlock*>             labels;
    Map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeInternal;
    Map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeInternalClosure;
    Map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeForeign;
    Map<TypeInfoFuncAttr*, llvm::FunctionType*> mapFctTypeForeignClosure;

    // Debug infos
    LLVMDebug* dbg = nullptr;
};

struct LLVM final : Backend
{
    explicit LLVM(Module* mdl);

    JobResult prepareOutput(const BuildParameters& buildParameters, int stage, Job* ownerJob) override;
    void      emitLocalCall(const BuildParameters& buildParameters, llvm::LLVMContext& context, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const ByteCodeInstruction* ip, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams, VectorNative<uint32_t>& pushRAParams, llvm::Value*& resultFuncCall);
    void      emitForeignCall(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const ByteCodeInstruction* ip, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams, VectorNative<uint32_t>& pushRAParams, llvm::Value*& resultFuncCall);
    bool      emitLambdaCall(const BuildParameters& buildParameters, LLVMEncoder& pp, llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Function* func, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const llvm::AllocaInst* allocT, const ByteCodeInstruction* ip, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams, VectorNative<uint32_t>& pushRAParams, llvm::Value*& resultFuncCall);
    void      emitBinOpOverflow(const BuildParameters& buildParameters, llvm::Function* func, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const ByteCodeInstruction* ip, llvm::Value* r0, llvm::Value* r1, llvm::Value* r2, llvm::Intrinsic::IndependentIntrinsics op, SafetyMsg msg);
    bool      emitFunctionBody(const BuildParameters& buildParameters, ByteCode* bc) override;

    void                createRuntime(const BuildParameters& buildParameters) const;
    static llvm::Value* getImmediateConstantA(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t opBits);
    llvm::Type*         swagTypeToLLVMType(const BuildParameters& buildParameters, TypeInfo* typeInfo);
    void                createRet(const BuildParameters& buildParameters, const TypeInfoFuncAttr* typeFunc, TypeInfo* returnType, llvm::AllocaInst* allocResult);
    llvm::FunctionType* getOrCreateFuncType(const BuildParameters& buildParameters, TypeInfoFuncAttr* typeFunc, bool closureToLambda = false);
    void                emitInternalPanic(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const AstNode* node, const char* message);
    void                setFuncAttributes(const BuildParameters& buildParameters, const AstFuncDecl* funcNode, const ByteCode* bc, llvm::Function* func) const;
    void                storeTypedValueToRegister(llvm::LLVMContext& context, const BuildParameters& buildParameters, llvm::Value* value, uint32_t reg, llvm::AllocaInst* allocR) const;
    void                storeRT2ToRegisters(llvm::LLVMContext& context, const BuildParameters& buildParameters, uint32_t reg0, uint32_t reg1, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR) const;
    void                getReturnResult(llvm::LLVMContext& context, const BuildParameters& buildParameters, TypeInfo* returnType, bool imm, const Register& reg, llvm::AllocaInst* allocR, llvm::AllocaInst* allocResult);
    void                emitByteCodeCallParameters(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const llvm::AllocaInst* allocT, VectorNative<llvm::Value*>& params, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& pushRAParams, const Vector<llvm::Value*>& values, bool closureToLambda = false);

    static void emitShiftRightArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t opBits);
    static void emitShiftRightEqArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t opBits);
    static void emitShiftLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t opBits, bool left);
    static void emitShiftEqLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t opBits, bool left);

    bool         emitCallParameters(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, TypeInfoFuncAttr* typeFuncBC, VectorNative<llvm::Value*>& params, const VectorNative<uint32_t>& pushParams, const Vector<llvm::Value*>& values, bool closureToLambda = false);
    bool         emitCallReturnValue(const BuildParameters& buildParameters, llvm::AllocaInst* allocRR, const TypeInfoFuncAttr* typeFuncBC, llvm::Value* callResult) const;
    llvm::Value* emitCall(const BuildParameters& buildParameters, const Utf8& funcName, TypeInfoFuncAttr* typeFuncBC, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const VectorNative<uint32_t>& pushParams, const Vector<llvm::Value*>& values, bool localCall);
    llvm::Value* emitCall(const BuildParameters& buildParameters, const char* name, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const Vector<uint32_t>& regs, const Vector<llvm::Value*>& values);
    void         generateObjFile(const BuildParameters& buildParameters) const;
    bool         emitDataSegment(const BuildParameters& buildParameters, DataSegment* dataSegment) const;
    bool         emitInitSeg(const BuildParameters& buildParameters, DataSegment* dataSegment, SegmentKind me) const;
    void         emitGetTypeTable(const BuildParameters& buildParameters) const;
    void         emitGlobalPreMain(const BuildParameters& buildParameters) const;
    void         emitGlobalInit(const BuildParameters& buildParameters);
    void         emitGlobalDrop(const BuildParameters& buildParameters);
    void         emitOS(const BuildParameters& buildParameters) const;
    void         emitMain(const BuildParameters& buildParameters);

    static llvm::BasicBlock* getOrCreateLabel(LLVMEncoder& pp, llvm::Function* func, int64_t ip);
    bool                     emitGetParam(llvm::LLVMContext& context, const BuildParameters& buildParameters, const llvm::Function* func, TypeInfoFuncAttr* typeFunc, uint32_t rDest, uint32_t paramIdx, llvm::AllocaInst* allocR, int sizeOf = 0, uint64_t toAdd = 0, int deRefSize = 0);
};
