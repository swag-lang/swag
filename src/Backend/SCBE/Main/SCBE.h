#pragma once
#include "Backend/Backend.h"
#include "Backend/BackendParameters.h"
#include "Backend/SCBE/Encoder/SCBE_X64.h"
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

    static void createRuntime(SCBE_X64& pp);
    static void initFunction(CPUFunction* fct, uint32_t startAddress, uint32_t endAddress, uint32_t sizeProlog, VectorNative<uint16_t>& unwind);
    static bool buildRelocationSegment(SCBE_X64& pp, DataSegment* dataSegment, CPURelocationTable& relocTable, SegmentKind me);

    static void emitGetTypeTable(SCBE_X64& pp);
    static void emitGlobalPreMain(SCBE_X64& pp);
    static void emitGlobalInit(SCBE_X64& pp);
    static void emitGlobalDrop(SCBE_X64& pp);
    static void emitOS(SCBE_X64& pp);
    static void emitMain(SCBE_X64& pp);

    static void emitByteCodeCall(SCBE_X64& pp, const TypeInfoFuncAttr* typeFuncBc, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams);
    static void emitByteCodeCallParameters(SCBE_X64& pp, const TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams);
    static void emitGetParam(SCBE_X64& pp, const CPUFunction* cpuFct, uint32_t reg, uint32_t paramIdx, OpBits opBits, uint64_t toAdd = 0, OpBits derefBits = OpBits::INVALID);
    static void emitCall(SCBE_X64& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT, bool localCall);
    static void emitCall(SCBE_X64& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, bool localCall);
    static void emitInternalCall(SCBE_X64& pp, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT = UINT32_MAX);
    static void emitInternalCallExt(SCBE_X64& pp, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT = UINT32_MAX, TypeInfoFuncAttr* typeFunc = nullptr);
    static void emitLocalCall(SCBE_X64& pp, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams);
    static void emitForeignCall(SCBE_X64& pp, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams);
    static void emitLambdaCall(SCBE_X64& pp, const Concat& concat, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams);
    static void computeUnwind(SCBE_X64& pp, const VectorNative<CPUReg>& unwindRegs, const VectorNative<uint32_t>& unwindOffsetRegs, uint32_t sizeStack, uint32_t offsetSubRSP, VectorNative<uint16_t>& unwind);

    static void emitOverflow(SCBE_X64& pp, const char* msg, bool isSigned);
    static void emitShiftRightArithmetic(SCBE_X64& pp);
    static void emitShiftLogical(SCBE_X64& pp, CPUOp op);
    static void emitShiftRightEqArithmetic(SCBE_X64& pp);
    static void emitShiftEqLogical(SCBE_X64& pp, CPUOp op);
    static void emitInternalPanic(SCBE_X64& pp, const AstNode* node, const char* msg);
    static void emitCompareOp(SCBE_X64& pp);
    static void emitBinOp(SCBE_X64& pp, CPUOp op, CPUEmitFlags emitFlags = EMITF_Zero);
    static void emitBinOpOverflow(SCBE_X64& pp, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType);
    static void emitBinOpEq(SCBE_X64& pp, uint32_t offset, CPUOp op, CPUEmitFlags emitFlags = EMITF_Zero);
    static void emitBinOpEqOverflow(SCBE_X64& pp, uint32_t offset, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType);
    static void emitBinOpEqLock(SCBE_X64& pp, CPUOp op);
    static void emitBinOpEqS(SCBE_X64& pp, uint32_t offsetStack, CPUOp op);
    static void emitAddSubMul64(SCBE_X64& pp, uint64_t mulValue, CPUOp op);
    static void emitJumpCmp(SCBE_X64& pp, CPUCondJump op, OpBits opBits);
    static void emitJumpCmpAddr(SCBE_X64& pp, CPUCondJump op, CPUReg memReg, uint64_t memOffset, OpBits opBits);
    static void emitJumpCmp2(SCBE_X64& pp, CPUCondJump op1, CPUCondJump op2, OpBits opBits);
    static void emitJumpCmp3(SCBE_X64& pp, CPUCondJump op1, CPUCondJump op2, OpBits opBits);
    static void emitIMMA(SCBE_X64& pp, CPUReg reg, OpBits opBits);
    static void emitIMMB(SCBE_X64& pp, CPUReg reg, OpBits opBits);
    static void emitIMMC(SCBE_X64& pp, CPUReg reg, OpBits opBits);
    static void emitIMMD(SCBE_X64& pp, CPUReg reg, OpBits opBits);
    static void emitIMMB(SCBE_X64& pp, CPUReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned);
    static void emitIMMC(SCBE_X64& pp, CPUReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned);
};
