#pragma once
#include "Backend/Backend.h"
#include "Backend/BackendParameters.h"
#include "Backend/SCBE/Encoder/SCBE_CPU.h"
#include "Semantic/DataSegment.h"

struct Module;
struct BuildParameters;
struct Job;
struct DataSegment;
struct ByteCode;
struct TypeInfo;
struct ByteCodeInstruction;
enum class SafetyMsg;

struct SCBE final : Backend
{
    explicit SCBE(Module* mdl);
    SCBE();

    JobResult prepareOutput(const BuildParameters& buildParameters, int stage, Job* ownerJob) override;
    bool      emitFunctionBody(const BuildParameters& buildParameters, ByteCode* bc) override;
    void      saveObjFile(const BuildParameters& buildParameters) const;

    static void createRuntime(SCBE_CPU& pp);
    static bool buildRelocationSegment(SCBE_CPU& pp, DataSegment* dataSegment, CPURelocationTable& relocationTable, SegmentKind me);

    static void emitGetTypeTable(SCBE_CPU& pp);
    static void emitGlobalPreMain(SCBE_CPU& pp);
    static void emitGlobalInit(SCBE_CPU& pp);
    static void emitGlobalDrop(SCBE_CPU& pp);
    static void emitOS(SCBE_CPU& pp);
    static void emitMain(SCBE_CPU& pp);

    static void emitCallCPUParams(SCBE_CPU& pp, const Utf8& funcName, const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& pushCPUParams, uint32_t offsetRT, bool localCall);
    static void emitCallRAParams(SCBE_CPU& pp, const Utf8& funcName, const TypeInfoFuncAttr* typeFuncBc, bool localCall);
    static void emitInternalCallCPUParams(SCBE_CPU& pp, const Utf8& funcName, const VectorNative<CPUPushParam>& pushCPUParams, uint32_t offsetRT = UINT32_MAX);
    static void emitInternalCallRAParams(SCBE_CPU& pp, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT = UINT32_MAX);

    static void emitGetParam(SCBE_CPU& pp, uint32_t reg, uint32_t paramIdx, OpBits opBits, uint64_t toAdd = 0, OpBits derefBits = OpBits::Zero);
    static void emitLocalCall(SCBE_CPU& pp);
    static void emitForeignCall(SCBE_CPU& pp);
    static void emitLambdaCall(SCBE_CPU& pp);
    
    static void emitEnter(SCBE_CPU& pp, uint32_t sizeStack);
    static void emitLeave(SCBE_CPU& pp);
    
    static void emitOverflow(SCBE_CPU& pp, const char* msg, bool isSigned);
    static void emitShiftRightArithmetic(SCBE_CPU& pp);
    static void emitShiftLogical(SCBE_CPU& pp, CPUOp op);
    static void emitShiftRightEqArithmetic(SCBE_CPU& pp);
    static void emitShiftEqLogical(SCBE_CPU& pp, CPUOp op);
    static void emitInternalPanic(SCBE_CPU& pp, const char* msg);
    static void emitCompareOp(SCBE_CPU& pp, CPUReg reg, CPUCondFlag cond);
    static void emitBinOp(SCBE_CPU& pp, CPUOp op, CPUEmitFlags emitFlags = EMITF_Zero);
    static void emitBinOpOverflow(SCBE_CPU& pp, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType);
    static void emitBinOpEq(SCBE_CPU& pp, uint32_t offset, CPUOp op, CPUEmitFlags emitFlags = EMITF_Zero);
    static void emitBinOpEqOverflow(SCBE_CPU& pp, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType);
    static void emitBinOpEqLock(SCBE_CPU& pp, CPUOp op);
    static void emitBinOpEqS(SCBE_CPU& pp, CPUOp op);
    static void emitAddSubMul64(SCBE_CPU& pp, uint64_t mulValue, CPUOp op);
    static void emitJumpCmp(SCBE_CPU& pp, CPUCondJump op, OpBits opBits);
    static void emitJumpCmpAddr(SCBE_CPU& pp, CPUCondJump op, CPUReg memReg, uint64_t memOffset, OpBits opBits);
    static void emitJumpCmp2(SCBE_CPU& pp, CPUCondJump op1, CPUCondJump op2, OpBits opBits);
    static void emitJumpCmp3(SCBE_CPU& pp, CPUCondJump op1, CPUCondJump op2, OpBits opBits);
    static void emitIMMA(SCBE_CPU& pp, CPUReg reg, OpBits opBits);
    static void emitIMMB(SCBE_CPU& pp, CPUReg reg, OpBits opBits);
    static void emitIMMC(SCBE_CPU& pp, CPUReg reg, OpBits opBits);
    static void emitIMMD(SCBE_CPU& pp, CPUReg reg, OpBits opBits);
    static void emitIMMB(SCBE_CPU& pp, CPUReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned);
    static void emitIMMC(SCBE_CPU& pp, CPUReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned);
    static void emitJumpDyn(SCBE_CPU& pp);
    static void emitCopyVaargs(SCBE_CPU& pp);
    static void emitMakeLambda(SCBE_CPU& pp);
    static void emitMakeCallback(SCBE_CPU& pp);
};
