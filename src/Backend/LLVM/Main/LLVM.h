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

    static bool         emitCallParameters(LLVM_Encoder& pp, const TypeInfoFuncAttr* typeFuncBc, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, VectorNative<llvm::Value*>& params, const VectorNative<uint32_t>& pushParams, const Vector<llvm::Value*>& values, bool closureToLambda = false);
    static bool         emitCallResult(const LLVM_Encoder& pp, const TypeInfoFuncAttr* typeFuncBc, llvm::Value* callResult);
    static llvm::Value* emitCall(LLVM_Encoder& pp, const Utf8& funcName, const TypeInfoFuncAttr* typeFuncBc, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const VectorNative<uint32_t>& pushParams, const Vector<llvm::Value*>& values, bool localCall);
    static llvm::Value* emitCall(LLVM_Encoder& pp, const Utf8& funcName, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const Vector<uint32_t>& regs, const Vector<llvm::Value*>& values);

    static void emitByteCodeCallParameters(LLVM_Encoder& pp, TypeInfoFuncAttr* typeFuncBc, VectorNative<llvm::Value*>& params, VectorNative<uint32_t>& pushRAParams, const Vector<llvm::Value*>& values, bool closureToLambda = false);
    static bool emitGetParam(LLVM_Encoder& pp, const TypeInfoFuncAttr* typeFuncBc, uint32_t rDest, uint32_t paramIdx, int sizeOf = 0, uint64_t toAdd = 0, int deRefSize = 0);
    static void emitRet(LLVM_Encoder& pp, const TypeInfoFuncAttr* typeFuncBc, TypeInfo* returnType, llvm::AllocaInst* allocResult);
    static void emitLocalCall(LLVM_Encoder& pp, llvm::Value*& resultFuncCall);
    static void emitForeignCall(LLVM_Encoder& pp, llvm::Value*& resultFuncCall);
    static bool emitLambdaCall(LLVM_Encoder& pp, llvm::Value*& resultFuncCall);

    static void emitShiftRightArithmetic(const LLVM_Encoder& pp, uint32_t numBits);
    static void emitShiftRightEqArithmetic(const LLVM_Encoder& pp, uint32_t numBits);
    static void emitShiftLogical(const LLVM_Encoder& pp, uint32_t numBits, bool left);
    static void emitShiftEqLogical(const LLVM_Encoder& pp, uint32_t numBits, bool left);
    static void emitBinOpOverflow(LLVM_Encoder& pp, llvm::Value* r0, llvm::Value* r1, llvm::Value* r2, llvm::Intrinsic::IndependentIntrinsics op, SafetyMsg msg);
    static void emitBinOpEqOverflow(LLVM_Encoder& pp, llvm::Value* r1, llvm::Value* r2, llvm::Intrinsic::IndependentIntrinsics op, SafetyMsg msg);

    static void emitRT2ToRegisters(const LLVM_Encoder& pp, uint32_t reg0, uint32_t reg1, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR);
    static void emitTypedValueToRegister(const LLVM_Encoder& pp, llvm::Value* value, uint32_t reg);
    static void emitInternalPanic(LLVM_Encoder& pp, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const char* message);

    static llvm::Type*         getLLVMType(LLVM_Encoder& pp, TypeInfo* typeInfo);
    static llvm::FunctionType* getOrCreateFuncType(LLVM_Encoder& pp, const TypeInfoFuncAttr* typeFuncBc, bool closureToLambda = false);
    static llvm::BasicBlock*   getOrCreateLabel(LLVM_Encoder& pp, int64_t ip);
    static void                getReturnResult(LLVM_Encoder& pp, TypeInfo* returnType, bool imm, const Register& reg, llvm::AllocaInst* allocR, llvm::AllocaInst* allocResult);
    static void                setFuncAttributes(const LLVM_Encoder& pp, uint32_t numPreComp, const AstFuncDecl* funcNode, const ByteCode* bc);
};
