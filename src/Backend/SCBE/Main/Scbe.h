#pragma once
#include "Backend/Backend.h"
#include "Backend/BackendParameters.h"
#include "Backend/SCBE/Encoder/ScbeCPU.h"
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

    static void createRuntime(ScbeCPU& pp);
    static bool buildRelocationSegment(ScbeCPU& pp, DataSegment* dataSegment, CPURelocationTable& relocationTable, SegmentKind me);

    static void emitGetTypeTable(ScbeCPU& pp);
    static void emitGlobalPreMain(ScbeCPU& pp);
    static void emitGlobalInit(ScbeCPU& pp);
    static void emitGlobalDrop(ScbeCPU& pp);
    static void emitOS(ScbeCPU& pp);
    static void emitMain(ScbeCPU& pp);

    static void emitCallCPUParams(ScbeCPU& pp, const Utf8& funcName, const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& pushCPUParams, CPUReg memRegResult, uint32_t memOffsetResult, bool localCall);
    static void emitCallRAParams(ScbeCPU& pp, const Utf8& funcName, const TypeInfoFuncAttr* typeFuncBc, bool localCall);
    static void emitInternalCallCPUParams(ScbeCPU& pp, const Utf8& funcName, const VectorNative<CPUPushParam>& pushCPUParams, CPUReg memRegResult = CPUReg::RSP, uint32_t memOffsetResult = UINT32_MAX);
    static void emitInternalCallRAParams(ScbeCPU& pp, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, CPUReg memRegResult = CPUReg::RSP, uint32_t memOffsetResult = UINT32_MAX);

    static void emitLoadParam(ScbeCPU& pp, CPUReg reg, uint32_t paramIdx, OpBits opBits);
    static void emitLoadZeroExtendParam(ScbeCPU& pp, CPUReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc);
    static void emitLoadAddressParam(ScbeCPU& pp, CPUReg reg, uint32_t paramIdx);
    static void emitGetParam(ScbeCPU& pp, uint32_t reg, uint32_t paramIdx, OpBits opBits, uint64_t toAdd = 0, OpBits derefBits = OpBits::Zero);
    static void emitLocalCall(ScbeCPU& pp);
    static void emitForeignCall(ScbeCPU& pp);
    static void emitLambdaCall(ScbeCPU& pp);

    static void emitOverflow(ScbeCPU& pp, const char* msg, bool isSigned);
    static void emitShiftRightArithmetic(ScbeCPU& pp);
    static void emitShiftLogical(ScbeCPU& pp, CPUOp op);
    static void emitShiftRightEqArithmetic(ScbeCPU& pp);
    static void emitShiftEqLogical(ScbeCPU& pp, CPUOp op);
    static void emitInternalPanic(ScbeCPU& pp, const char* msg);
    static void emitCompareOp(ScbeCPU& pp, CPUReg reg, CPUCondFlag cond);
    static void emitBinOp(ScbeCPU& pp, CPUOp op, CPUEmitFlags emitFlags = EMITF_Zero);
    static void emitBinOpOverflow(ScbeCPU& pp, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType);
    static void emitBinOpEq(ScbeCPU& pp, uint32_t offset, CPUOp op, CPUEmitFlags emitFlags = EMITF_Zero);
    static void emitBinOpEqOverflow(ScbeCPU& pp, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType);
    static void emitBinOpEqLock(ScbeCPU& pp, CPUOp op);
    static void emitBinOpEqS(ScbeCPU& pp, CPUOp op);
    static void emitAddSubMul64(ScbeCPU& pp, uint64_t mulValue, CPUOp op);
    static void emitJumpCmp(ScbeCPU& pp, CPUCondJump op, OpBits opBits);
    static void emitJumpCmpAddr(ScbeCPU& pp, CPUCondJump op, CPUReg memReg, uint64_t memOffset, OpBits opBits);
    static void emitJumpCmp2(ScbeCPU& pp, CPUCondJump op1, CPUCondJump op2, OpBits opBits);
    static void emitJumpCmp3(ScbeCPU& pp, CPUCondJump op1, CPUCondJump op2, OpBits opBits);
    static void emitIMMA(ScbeCPU& pp, CPUReg reg, OpBits opBits);
    static void emitIMMB(ScbeCPU& pp, CPUReg reg, OpBits opBits);
    static void emitIMMC(ScbeCPU& pp, CPUReg reg, OpBits opBits);
    static void emitIMMD(ScbeCPU& pp, CPUReg reg, OpBits opBits);
    static void emitIMMB(ScbeCPU& pp, CPUReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned);
    static void emitIMMC(ScbeCPU& pp, CPUReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned);
    static void emitJumpDyn(ScbeCPU& pp);
    static void emitCopyVaargs(ScbeCPU& pp);
    static void emitMakeLambda(ScbeCPU& pp);
    static void emitMakeCallback(ScbeCPU& pp);
};
