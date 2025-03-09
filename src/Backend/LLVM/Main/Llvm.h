#pragma once
#include "Backend/Backend.h"
#include "Backend/BackendParameters.h"
#include "Backend/LLVM/Encoder/LlvmEncoder.h"

struct BuildParameters;
struct ByteCode;
struct ByteCodeInstruction;
struct DataSegment;
struct Job;
struct LlvmDebug;
struct Module;
struct TypeInfo;
union Register;
enum class SegmentKind;
enum class SafetyMsg;

struct Llvm final : Backend
{
    explicit Llvm(Module* mdl);

    JobResult prepareOutput(const BuildParameters& buildParameters, int stage, Job* ownerJob) override;
    bool      emitFunctionBody(const BuildParameters& buildParameters, ByteCode* bc) override;

    static void createRuntime(LlvmEncoder& pp);
    static void generateObjFile(LlvmEncoder& pp);
    static bool emitDataSegment(LlvmEncoder& pp, DataSegment* dataSegment);
    static bool emitInitSeg(const LlvmEncoder& pp, DataSegment* dataSegment, SegmentKind me);
    static void emitGetTypeTable(const LlvmEncoder& pp);
    static void emitGlobalPreMain(const LlvmEncoder& pp);
    static void emitGlobalInit(LlvmEncoder& pp);
    static void emitGlobalDrop(LlvmEncoder& pp);
    static void emitOS(const LlvmEncoder& pp);
    static void emitMain(LlvmEncoder& pp);

    static bool         emitCallParameters(LlvmEncoder& pp, const TypeInfoFuncAttr* typeFuncBc, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, VectorNative<llvm::Value*>& params, const VectorNative<uint32_t>& pushParams, const Vector<llvm::Value*>& values, bool closureToLambda = false);
    static bool         emitCallResult(const LlvmEncoder& pp, const TypeInfoFuncAttr* typeFuncBc, llvm::Value* callResult);
    static llvm::Value* emitCall(LlvmEncoder& pp, const Utf8& funcName, const TypeInfoFuncAttr* typeFuncBc, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR, const VectorNative<uint32_t>& pushParams, const Vector<llvm::Value*>& values, bool localCall);
    static llvm::Value* emitCall(LlvmEncoder& pp, const Utf8& funcName, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const Vector<uint32_t>& regs, const Vector<llvm::Value*>& values);

    static void emitByteCodeCallParameters(LlvmEncoder& pp, TypeInfoFuncAttr* typeFuncBc, VectorNative<llvm::Value*>& params, VectorNative<uint32_t>& pushRAParams, const Vector<llvm::Value*>& values, bool closureToLambda = false);
    static bool emitGetParam(LlvmEncoder& pp, const TypeInfoFuncAttr* typeFuncBc, uint32_t rDest, uint32_t paramIdx, int sizeOf = 0, uint64_t toAdd = 0, int deRefSize = 0);
    static void emitRet(LlvmEncoder& pp, const TypeInfoFuncAttr* typeFuncBc, TypeInfo* returnType, llvm::AllocaInst* allocResult);
    static void emitLocalCall(LlvmEncoder& pp, llvm::Value*& resultFuncCall);
    static void emitForeignCall(LlvmEncoder& pp, llvm::Value*& resultFuncCall);
    static bool emitLambdaCall(LlvmEncoder& pp, llvm::Value*& resultFuncCall);

    static void emitShiftRightArithmetic(const LlvmEncoder& pp, uint32_t numBits);
    static void emitShiftRightEqArithmetic(const LlvmEncoder& pp, uint32_t numBits);
    static void emitShiftLogical(const LlvmEncoder& pp, uint32_t numBits, bool left);
    static void emitShiftEqLogical(const LlvmEncoder& pp, uint32_t numBits, bool left);
    static void emitBinOpOverflow(LlvmEncoder& pp, llvm::Value* r0, llvm::Value* r1, llvm::Value* r2, llvm::Intrinsic::IndependentIntrinsics op, SafetyMsg msg);
    static void emitBinOpEqOverflow(LlvmEncoder& pp, llvm::Value* r1, llvm::Value* r2, llvm::Intrinsic::IndependentIntrinsics op, SafetyMsg msg);

    static void emitRT2ToRegisters(const LlvmEncoder& pp, uint32_t reg0, uint32_t reg1, llvm::AllocaInst* allocR, llvm::AllocaInst* allocRR);
    static void emitTypedValueToRegister(const LlvmEncoder& pp, llvm::Value* value, uint32_t reg);
    static void emitInternalPanic(LlvmEncoder& pp, llvm::AllocaInst* allocR, llvm::AllocaInst* allocT, const char* message);
    static void emitCopyVaargs(LlvmEncoder& pp);

    static llvm::Type*         getLlvmType(LlvmEncoder& pp, TypeInfo* typeInfo);
    static llvm::FunctionType* getOrCreateFuncType(LlvmEncoder& pp, const TypeInfoFuncAttr* typeFuncBc, bool closureToLambda = false);
    static llvm::BasicBlock*   getOrCreateLabel(LlvmEncoder& pp, int64_t ip);
    static void                getReturnResult(LlvmEncoder& pp, TypeInfo* returnType, bool imm, const Register& reg, llvm::AllocaInst* allocR, llvm::AllocaInst* allocResult);
    static void                setFuncAttributes(const LlvmEncoder& pp, uint32_t numPreComp, const AstFuncDecl* funcNode, const ByteCode* bc);
};
