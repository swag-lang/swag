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

    void      createRuntime(const BuildParameters& buildParameters) const;
    JobResult prepareOutput(const BuildParameters& buildParameters, int stage, Job* ownerJob) override;
    bool      emitFunctionBody(const BuildParameters& buildParameters, ByteCode* bc) override;

    void saveObjFile(const BuildParameters& buildParameters) const;
    void emitGetTypeTable(const BuildParameters& buildParameters) const;
    void emitGlobalPreMain(const BuildParameters& buildParameters) const;
    void emitGlobalInit(const BuildParameters& buildParameters) const;
    void emitGlobalDrop(const BuildParameters& buildParameters) const;
    void emitOS(const BuildParameters& buildParameters) const;
    void emitMain(const BuildParameters& buildParameters) const;

    static void emitOverflow(SCBE_X64& pp, const ByteCodeInstruction* ip, const char* msg, bool isSigned);
    static void emitShiftRightArithmetic(SCBE_X64& pp, const ByteCodeInstruction* ip);
    static void emitShiftLogical(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op);
    static void emitShiftRightEqArithmetic(SCBE_X64& pp, const ByteCodeInstruction* ip);
    static void emitShiftEqLogical(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op);
    static void emitInternalPanic(SCBE_X64& pp, const AstNode* node, const char* msg);
    static void emitCompareOp(SCBE_X64& pp, const ByteCodeInstruction* ip);
    static void emitBinOpAtReg(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op);
    static void emitBinOpAtRegOverflow(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType);
    static void emitBinOpEq(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offset, CPUOp op);
    static void emitBinOpEqOverflow(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offset, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType);
    static void emitBinOpEqS(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offsetStack, CPUOp op);
    static void emitBinOpEqSS(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offsetStack, CPUOp op);
    static void emitBinOpEqLock(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op);
    static void emitAddSubMul64(SCBE_X64& pp, const ByteCodeInstruction* ip, uint64_t mul, CPUOp op);
    static void emitByteCodeCall(SCBE_X64& pp, const TypeInfoFuncAttr* typeFuncBc, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams);
    static void emitByteCodeCallParameters(SCBE_X64& pp, const TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams);
    static void emitGetParam(SCBE_X64& pp, const CPUFunction* cpuFct, uint32_t reg, uint32_t paramIdx, int sizeOf, uint64_t toAdd = 0, int deRefSize = 0);
    static void emitCall(SCBE_X64& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT, bool localCall);
    static void emitCall(SCBE_X64& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, bool localCall);
    static void emitInternalCall(SCBE_X64& pp, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT = UINT32_MAX);
    static void emitInternalCallExt(SCBE_X64& pp, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT = UINT32_MAX, TypeInfoFuncAttr* typeFunc = nullptr);
    static void emitJumpCmp(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUJumpType op, CPUBits numBits);
    static void emitJumpCmpAddr(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUJumpType op, CPUReg memReg, uint32_t memOffset, CPUBits numBits);
    static void emitJumpCmp2(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUJumpType op1, CPUJumpType op2, CPUBits numBits);
    static void emitJumpCmp3(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUJumpType op1, CPUJumpType op2, CPUBits numBits);
    static void emitIMMA(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBits);
    static void emitIMMB(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBits);
    static void emitIMMC(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBits);
    static void emitIMMD(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBits);
    static void emitIMMB(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBitsSrc, CPUBits numBitsDst, bool isSigned);
    static void emitIMMC(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBitsSrc, CPUBits numBitsDst, bool isSigned);

    bool        buildRelocationSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CPURelocationTable& relocTable, SegmentKind me) const;
    void        computeUnwind(const VectorNative<CPUReg>& unwindRegs, const VectorNative<uint32_t>& unwindOffsetRegs, uint32_t sizeStack, uint32_t offsetSubRSP, VectorNative<uint16_t>& unwind) const;
    static void emitLocalCall(SCBE_X64& pp, uint32_t offsetRT, const ByteCodeInstruction* ip, VectorNative<uint32_t>& pushRAParams, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams);
    static void emitForeignCall(SCBE_X64& pp, uint32_t offsetRT, const ByteCodeInstruction* ip, VectorNative<uint32_t>& pushRAParams, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams);
    static void emitLambdaCall(SCBE_X64& pp, const Concat& concat, uint32_t offsetRT, const ByteCodeInstruction* ip, VectorNative<uint32_t>& pushRAParams, VectorNative<std::pair<uint32_t, uint32_t>>& pushRVParams);
    static void initFunction(CPUFunction* fct, uint32_t startAddress, uint32_t endAddress, uint32_t sizeProlog, VectorNative<uint16_t>& unwind);
};
