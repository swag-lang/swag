#pragma once
#include "Backend/Backend.h"
#include "Backend/BackendParameters.h"
#include "Backend/LLVM/Encoder/LLVM_Encoder.h"

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

struct LLVM final : Backend
{
    explicit LLVM(Module* mdl);

    JobResult prepareOutput(const BuildParameters& buildParameters, int stage, Job* ownerJob) override;

    void         emitLocalCall(const BuildParameters& buildParameters, llvm::LLVMContext& context, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const ByteCodeInstruction* ip, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams, VectorNative<uint32_t>& pushRAParams, llvm::Value*& resultFuncCall);
    void         emitForeignCall(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const ByteCodeInstruction* ip, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams, VectorNative<uint32_t>& pushRAParams, llvm::Value*& resultFuncCall);
    bool         emitLambdaCall(const BuildParameters& buildParameters, LLVMEncoder& pp, llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Function* func, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const llvm::AllocaInst* allocT, const ByteCodeInstruction* ip, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams, VectorNative<uint32_t>& pushRAParams, llvm::Value*& resultFuncCall);
    bool         emitFunctionBody(const BuildParameters& buildParameters, ByteCode* bc) override;
    void         emitByteCodeCallParameters(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const llvm::AllocaInst* allocT, VectorNative<llvm::Value*>& params, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& pushRAParams, const Vector<llvm::Value*>& values, bool closureToLambda = false);
    bool         emitCallParameters(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, TypeInfoFuncAttr* typeFuncBC, VectorNative<llvm::Value*>& params, const VectorNative<uint32_t>& pushParams, const Vector<llvm::Value*>& values, bool closureToLambda = false);
    bool         emitCallReturnValue(const BuildParameters& buildParameters, llvm::AllocaInst* allocRR, const TypeInfoFuncAttr* typeFuncBC, llvm::Value* callResult) const;
    llvm::Value* emitCall(const BuildParameters& buildParameters, const Utf8& funcName, TypeInfoFuncAttr* typeFuncBC, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const VectorNative<uint32_t>& pushParams, const Vector<llvm::Value*>& values, bool localCall);
    llvm::Value* emitCall(const BuildParameters& buildParameters, const char* name, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const Vector<uint32_t>& regs, const Vector<llvm::Value*>& values);
    bool         emitGetParam(llvm::LLVMContext& context, const BuildParameters& buildParameters, const llvm::Function* func, TypeInfoFuncAttr* typeFunc, uint32_t rDest, uint32_t paramIdx, llvm::AllocaInst* allocR, int sizeOf = 0, uint64_t toAdd = 0, int deRefSize = 0);

    static void emitShiftRightArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t numBits);
    static void emitShiftRightEqArithmetic(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t numBits);
    static void emitShiftLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t numBits, bool left);
    static void emitShiftEqLogical(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::AllocaInst* allocR, const ByteCodeInstruction* ip, uint32_t numBits, bool left);
    static void emitRT2ToRegisters(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, uint32_t reg0, uint32_t reg1, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR);
    static void emitTypedValueToRegister(llvm::LLVMContext& context, llvm::IRBuilder<>& builder, llvm::Value* value, uint32_t reg, llvm::AllocaInst* allocR);
    void        emitInternalPanic(const BuildParameters& buildParameters, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const AstNode* node, const char* message);
    void        emitBinOpOverflow(const BuildParameters& buildParameters, llvm::Function* func, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const ByteCodeInstruction* ip, llvm::Value* r0, llvm::Value* r1, llvm::Value* r2, llvm::Intrinsic::IndependentIntrinsics op, SafetyMsg msg);
    void        emitBinOpEqOverflow(const BuildParameters& buildParameters, llvm::Function* func, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const ByteCodeInstruction* ip, llvm::Value* r0, llvm::Value* r1, llvm::Value* r2, llvm::Intrinsic::IndependentIntrinsics op, SafetyMsg msg);

    void                     createRuntime(const BuildParameters& buildParameters) const;
    llvm::Type*              swagTypeToLLVMType(const BuildParameters& buildParameters, TypeInfo* typeInfo);
    void                     createRet(const BuildParameters& buildParameters, const TypeInfoFuncAttr* typeFunc, TypeInfo* returnType, llvm::AllocaInst* allocResult);
    llvm::FunctionType*      getOrCreateFuncType(const BuildParameters& buildParameters, TypeInfoFuncAttr* typeFunc, bool closureToLambda = false);
    static llvm::BasicBlock* getOrCreateLabel(LLVMEncoder& pp, llvm::Function* func, int64_t ip);
    void                     setFuncAttributes(const BuildParameters& buildParameters, const AstFuncDecl* funcNode, const ByteCode* bc, llvm::Function* func) const;
    void                     getReturnResult(llvm::LLVMContext& context, const BuildParameters& buildParameters, TypeInfo* returnType, bool imm, const Register& reg, llvm::AllocaInst* allocR, llvm::AllocaInst* allocResult);

    void generateObjFile(const BuildParameters& buildParameters) const;
    bool emitDataSegment(const BuildParameters& buildParameters, DataSegment* dataSegment) const;
    bool emitInitSeg(const BuildParameters& buildParameters, DataSegment* dataSegment, SegmentKind me) const;
    void emitGetTypeTable(const BuildParameters& buildParameters) const;
    void emitGlobalPreMain(const BuildParameters& buildParameters) const;
    void emitGlobalInit(const BuildParameters& buildParameters);
    void emitGlobalDrop(const BuildParameters& buildParameters);
    void emitOS(const BuildParameters& buildParameters) const;
    void emitMain(const BuildParameters& buildParameters);
};
