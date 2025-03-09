#pragma once
#include "Backend/Backend.h"
#include "Backend/BackendParameters.h"
#include "Backend/SCBE/Encoder/SCBECPU.h"
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

    static void createRuntime(SCBECPU& pp);
    static bool buildRelocationSegment(SCBECPU& pp, DataSegment* dataSegment, CPURelocationTable& relocationTable, SegmentKind me);

    static void emitGetTypeTable(SCBECPU& pp);
    static void emitGlobalPreMain(SCBECPU& pp);
    static void emitGlobalInit(SCBECPU& pp);
    static void emitGlobalDrop(SCBECPU& pp);
    static void emitOS(SCBECPU& pp);
    static void emitMain(SCBECPU& pp);

    static void emitCallCPUParams(SCBECPU& pp, const Utf8& funcName, const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& pushCPUParams, CPUReg memRegResult, uint32_t memOffsetResult, bool localCall);
    static void emitCallRAParams(SCBECPU& pp, const Utf8& funcName, const TypeInfoFuncAttr* typeFuncBc, bool localCall);
    static void emitInternalCallCPUParams(SCBECPU& pp, const Utf8& funcName, const VectorNative<CPUPushParam>& pushCPUParams, CPUReg memRegResult = CPUReg::RSP, uint32_t memOffsetResult = UINT32_MAX);
    static void emitInternalCallRAParams(SCBECPU& pp, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, CPUReg memRegResult = CPUReg::RSP, uint32_t memOffsetResult = UINT32_MAX);

    static void emitLoadParam(SCBECPU& pp, CPUReg reg, uint32_t paramIdx, OpBits opBits);
    static void emitLoadZeroExtendParam(SCBECPU& pp, CPUReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc);
    static void emitLoadAddressParam(SCBECPU& pp, CPUReg reg, uint32_t paramIdx);
    static void emitGetParam(SCBECPU& pp, uint32_t reg, uint32_t paramIdx, OpBits opBits, uint64_t toAdd = 0, OpBits derefBits = OpBits::Zero);
    static void emitLocalCall(SCBECPU& pp);
    static void emitForeignCall(SCBECPU& pp);
    static void emitLambdaCall(SCBECPU& pp);

    static void emitOverflow(SCBECPU& pp, const char* msg, bool isSigned);
    static void emitShiftRightArithmetic(SCBECPU& pp);
    static void emitShiftLogical(SCBECPU& pp, CPUOp op);
    static void emitShiftRightEqArithmetic(SCBECPU& pp);
    static void emitShiftEqLogical(SCBECPU& pp, CPUOp op);
    static void emitInternalPanic(SCBECPU& pp, const char* msg);
    static void emitCompareOp(SCBECPU& pp, CPUReg reg, CPUCondFlag cond);
    static void emitBinOp(SCBECPU& pp, CPUOp op, CPUEmitFlags emitFlags = EMITF_Zero);
    static void emitBinOpOverflow(SCBECPU& pp, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType);
    static void emitBinOpEq(SCBECPU& pp, uint32_t offset, CPUOp op, CPUEmitFlags emitFlags = EMITF_Zero);
    static void emitBinOpEqOverflow(SCBECPU& pp, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType);
    static void emitBinOpEqLock(SCBECPU& pp, CPUOp op);
    static void emitBinOpEqS(SCBECPU& pp, CPUOp op);
    static void emitAddSubMul64(SCBECPU& pp, uint64_t mulValue, CPUOp op);
    static void emitJumpCmp(SCBECPU& pp, CPUCondJump op, OpBits opBits);
    static void emitJumpCmpAddr(SCBECPU& pp, CPUCondJump op, CPUReg memReg, uint64_t memOffset, OpBits opBits);
    static void emitJumpCmp2(SCBECPU& pp, CPUCondJump op1, CPUCondJump op2, OpBits opBits);
    static void emitJumpCmp3(SCBECPU& pp, CPUCondJump op1, CPUCondJump op2, OpBits opBits);
    static void emitIMMA(SCBECPU& pp, CPUReg reg, OpBits opBits);
    static void emitIMMB(SCBECPU& pp, CPUReg reg, OpBits opBits);
    static void emitIMMC(SCBECPU& pp, CPUReg reg, OpBits opBits);
    static void emitIMMD(SCBECPU& pp, CPUReg reg, OpBits opBits);
    static void emitIMMB(SCBECPU& pp, CPUReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned);
    static void emitIMMC(SCBECPU& pp, CPUReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned);
    static void emitJumpDyn(SCBECPU& pp);
    static void emitCopyVaargs(SCBECPU& pp);
    static void emitMakeLambda(SCBECPU& pp);
    static void emitMakeCallback(SCBECPU& pp);
};
