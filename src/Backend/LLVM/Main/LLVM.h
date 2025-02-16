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
    bool      emitFunctionBody(const BuildParameters& buildParameters, ByteCode* bc) override;

    static void createRuntime(LLVM_Encoder& pp);
    static void generateObjFile(LLVM_Encoder& pp);
    static bool emitDataSegment(LLVM_Encoder& pp, DataSegment* dataSegment);
    static bool emitInitSeg(LLVM_Encoder& pp, DataSegment* dataSegment, SegmentKind me);
    static void emitGetTypeTable(LLVM_Encoder& pp);
    static void emitGlobalPreMain(LLVM_Encoder& pp);
    static void emitGlobalInit(LLVM_Encoder& pp);
    static void emitGlobalDrop(LLVM_Encoder& pp);
    static void emitOS(LLVM_Encoder& pp);
    static void emitMain(LLVM_Encoder& pp);

    static void         emitLocalCall(LLVM_Encoder& pp, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams, VectorNative<uint32_t>& pushRAParams, llvm::Value*& resultFuncCall);
    static void         emitForeignCall(LLVM_Encoder& pp, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams, VectorNative<uint32_t>& pushRAParams, llvm::Value*& resultFuncCall);
    static bool         emitLambdaCall(LLVM_Encoder& pp, llvm::Function* func, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const llvm::AllocaInst* allocT, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams, VectorNative<uint32_t>& pushRAParams, llvm::Value*& resultFuncCall);
    static void         emitByteCodeCallParameters(LLVM_Encoder& pp, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const llvm::AllocaInst* allocT, VectorNative<llvm::Value*>& params, TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& pushRAParams, const Vector<llvm::Value*>& values, bool closureToLambda = false);
    static bool         emitCallParameters(LLVM_Encoder& pp, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, TypeInfoFuncAttr* typeFuncBC, VectorNative<llvm::Value*>& params, const VectorNative<uint32_t>& pushParams, const Vector<llvm::Value*>& values, bool closureToLambda = false);
    static bool         emitCallReturnValue(LLVM_Encoder& pp, llvm::AllocaInst* allocRR, const TypeInfoFuncAttr* typeFuncBC, llvm::Value* callResult);
    static llvm::Value* emitCall(LLVM_Encoder& pp, const Utf8& funcName, TypeInfoFuncAttr* typeFuncBC, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const VectorNative<uint32_t>& pushParams, const Vector<llvm::Value*>& values, bool localCall);
    static llvm::Value* emitCall(LLVM_Encoder& pp, const char* name, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const Vector<uint32_t>& regs, const Vector<llvm::Value*>& values);
    static bool         emitGetParam(LLVM_Encoder& pp, const llvm::Function* func, TypeInfoFuncAttr* typeFunc, uint32_t rDest, uint32_t paramIdx, llvm::AllocaInst* allocR, int sizeOf = 0, uint64_t toAdd = 0, int deRefSize = 0);
    static void         emitRet(LLVM_Encoder& pp, const TypeInfoFuncAttr* typeFunc, TypeInfo* returnType, llvm::AllocaInst* allocResult);

    static void emitShiftRightArithmetic(LLVM_Encoder& pp, llvm::AllocaInst* allocR, uint32_t numBits);
    static void emitShiftRightEqArithmetic(LLVM_Encoder& pp, llvm::AllocaInst* allocR, uint32_t numBits);
    static void emitShiftLogical(LLVM_Encoder& pp, llvm::AllocaInst* allocR, uint32_t numBits, bool left);
    static void emitShiftEqLogical(LLVM_Encoder& pp, llvm::AllocaInst* allocR, uint32_t numBits, bool left);
    static void emitRT2ToRegisters(LLVM_Encoder& pp, uint32_t reg0, uint32_t reg1, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR);
    static void emitTypedValueToRegister(LLVM_Encoder& pp, llvm::Value* value, uint32_t reg, llvm::AllocaInst* allocR);
    static void emitInternalPanic(LLVM_Encoder& pp, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const char* message);
    static void emitBinOpOverflow(LLVM_Encoder& pp, llvm::Function* func, llvm::Value* r0, llvm::Value* r1, llvm::Value* r2, llvm::Intrinsic::IndependentIntrinsics op, SafetyMsg msg);
    static void emitBinOpEqOverflow(LLVM_Encoder& pp, llvm::Function* func, llvm::Value* r1, llvm::Value* r2, llvm::Intrinsic::IndependentIntrinsics op, SafetyMsg msg);

    static llvm::Type*         getLLVMType(LLVM_Encoder& pp, TypeInfo* typeInfo);
    static llvm::FunctionType* getOrCreateFuncType(LLVM_Encoder& pp, TypeInfoFuncAttr* typeFunc, bool closureToLambda = false);
    static llvm::BasicBlock*   getOrCreateLabel(LLVM_Encoder& pp, llvm::Function* func, int64_t ip);
    static void                getReturnResult(LLVM_Encoder& pp, TypeInfo* returnType, bool imm, const Register& reg, llvm::AllocaInst* allocR, llvm::AllocaInst* allocResult);
    static void                setFuncAttributes(LLVM_Encoder& pp, uint32_t numPreCompileBuffers, const AstFuncDecl* funcNode, const ByteCode* bc, llvm::Function* func);
};
