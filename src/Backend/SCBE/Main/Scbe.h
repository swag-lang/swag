#pragma once
#include "Backend/Backend.h"
#include "Backend/BackendParameters.h"
#include "Backend/SCBE/Encoder/ScbeCpu.h"
#include "Semantic/DataSegment.h"

struct Module;
struct BuildParameters;
struct Job;
struct DataSegment;
struct ByteCode;
struct TypeInfo;
struct ByteCodeInstruction;
enum class SafetyMsg;

struct Scbe final : Backend
{
    explicit Scbe(Module* mdl);
    Scbe();

    JobResult prepareOutput(const BuildParameters& buildParameters, int stage, Job* ownerJob) override;
    bool      emitFunctionBody(const BuildParameters& buildParameters, ByteCode* bc) override;
    void      saveObjFile(const BuildParameters& buildParameters) const;

    static void createRuntime(ScbeCpu& pp);
    static bool buildRelocationSegment(ScbeCpu& pp, DataSegment* dataSegment, CpuRelocationTable& relocationTable, SegmentKind me);

    static void emitGetTypeTable(ScbeCpu& pp);
    static void emitGlobalPreMain(ScbeCpu& pp);
    static void emitGlobalInit(ScbeCpu& pp);
    static void emitGlobalDrop(ScbeCpu& pp);
    static void emitOS(ScbeCpu& pp);
    static void emitMain(ScbeCpu& pp);

    static void emitCallCPUParams(ScbeCpu& pp, const Utf8& funcName, const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CpuPushParam>& pushCPUParams, CpuReg memRegResult, uint32_t memOffsetResult, bool localCall);
    static void emitCallRAParams(ScbeCpu& pp, const Utf8& funcName, const TypeInfoFuncAttr* typeFuncBc, bool localCall);
    static void emitInternalCallCPUParams(ScbeCpu& pp, const Utf8& funcName, const VectorNative<CpuPushParam>& pushCPUParams, CpuReg memRegResult = CpuReg::Rsp, uint32_t memOffsetResult = UINT32_MAX);
    static void emitInternalCallRAParams(ScbeCpu& pp, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, CpuReg memRegResult = CpuReg::Rsp, uint32_t memOffsetResult = UINT32_MAX);

    static void emitLoadParam(ScbeCpu& pp, CpuReg reg, uint32_t paramIdx, OpBits opBits);
    static void emitLoadZeroExtendParam(ScbeCpu& pp, CpuReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc);
    static void emitLoadAddressParam(ScbeCpu& pp, CpuReg reg, uint32_t paramIdx);
    static void emitGetParam(ScbeCpu& pp, uint32_t reg, uint32_t paramIdx, OpBits opBits, uint64_t toAdd = 0, OpBits derefBits = OpBits::Zero);
    static void emitLocalCall(ScbeCpu& pp);
    static void emitForeignCall(ScbeCpu& pp);
    static void emitLambdaCall(ScbeCpu& pp);
    static void emitMakeLambda(ScbeCpu& pp);
    static void emitMakeCallback(ScbeCpu& pp);

    static void emitOverflow(ScbeCpu& pp, const char* msg, bool isSigned);
    static void emitShiftRightArithmetic(ScbeCpu& pp);
    static void emitShiftLogical(ScbeCpu& pp, CpuOp op);
    static void emitShiftRightEqArithmetic(ScbeCpu& pp);
    static void emitShiftEqLogical(ScbeCpu& pp, CpuOp op);
    static void emitInternalPanic(ScbeCpu& pp, const char* msg);
    static void emitCompareOp(ScbeCpu& pp, CpuReg reg, CpuCondFlag cond);
    static void emitBinOp(ScbeCpu& pp, CpuOp op, CpuEmitFlags emitFlags = EMITF_Zero);
    static void emitBinOpOverflow(ScbeCpu& pp, CpuOp op, SafetyMsg safetyMsg, TypeInfo* safetyType);
    static void emitBinOpEq(ScbeCpu& pp, uint32_t offset, CpuOp op, CpuEmitFlags emitFlags = EMITF_Zero);
    static void emitBinOpEqOverflow(ScbeCpu& pp, CpuOp op, SafetyMsg safetyMsg, TypeInfo* safetyType);
    static void emitBinOpEqLock(ScbeCpu& pp, CpuOp op);
    static void emitBinOpEqS(ScbeCpu& pp, CpuOp op);
    static void emitAddSubMul64(ScbeCpu& pp, uint64_t mulValue, CpuOp op);
    static void emitJumpCmp(ScbeCpu& pp, CpuCondJump op, OpBits opBits);
    static void emitJumpCmpAddr(ScbeCpu& pp, CpuCondJump op, CpuReg memReg, uint64_t memOffset, OpBits opBits);
    static void emitJumpCmp2(ScbeCpu& pp, CpuCondJump op1, CpuCondJump op2, OpBits opBits);
    static void emitJumpCmp3(ScbeCpu& pp, CpuCondJump op1, CpuCondJump op2, OpBits opBits);
    static void emitIMMA(ScbeCpu& pp, CpuReg reg, OpBits opBits);
    static void emitIMMB(ScbeCpu& pp, CpuReg reg, OpBits opBits);
    static void emitIMMC(ScbeCpu& pp, CpuReg reg, OpBits opBits);
    static void emitIMMD(ScbeCpu& pp, CpuReg reg, OpBits opBits);
    static void emitIMMB(ScbeCpu& pp, CpuReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned);
    static void emitIMMC(ScbeCpu& pp, CpuReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned);
    static void emitJumpDyn(ScbeCpu& pp);
    static void emitCopyVaargs(ScbeCpu& pp);
};
