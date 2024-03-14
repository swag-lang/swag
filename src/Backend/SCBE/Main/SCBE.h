#pragma once
#include "Backend/Backend.h"
#include "Backend/BackendParameters.h"
#include "Semantic/DataSegment.h"
#include "Backend/SCBE/Encoder/SCBE_X64.h"

struct Module;
struct BuildParameters;
struct Job;
struct DataSegment;
struct ByteCode;
struct TypeInfo;
struct ByteCodeInstruction;

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

    static void emitOverflowSigned(SCBE_X64& pp, const ByteCodeInstruction* ip, const char* msg);
    static void emitOverflowUnsigned(SCBE_X64& pp, const ByteCodeInstruction* ip, const char* msg);
    static void emitShiftRightArithmetic(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUBits numBits);
    static void emitShiftLogical(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits);
    static void emitShiftRightEqArithmetic(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUBits numBits);
    static void emitShiftEqLogical(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits);
    static void emitInternalPanic(SCBE_X64& pp, const AstNode* node, const char* msg);
    static void emitBinOpFloat32(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op);
    static void emitBinOpFloat32AtReg(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op);
    static void emitBinOpFloat64(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op);
    static void emitBinOpFloat64AtReg(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op);
    static void emitBinOpIntN(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits);
    static void emitBinOpIntNAtReg(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits);
    static void emitBinOpDivIntNAtReg(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits);
    static void emitAddSubMul64(SCBE_X64& pp, const ByteCodeInstruction* ip, uint64_t mul, CPUOp op);
    static void emitByteCodeCall(SCBE_X64& pp, const TypeInfoFuncAttr* typeFuncBc, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams);
    static void emitByteCodeCallParameters(SCBE_X64& pp, const TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams);
    static void emitGetParam(SCBE_X64& pp, const CPUFunction* cpuFct, uint32_t reg, uint32_t paramIdx, int sizeOf, uint64_t toAdd = 0, int deRefSize = 0);
    static void emitCall(SCBE_X64& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT, bool localCall);
    static void emitCall(SCBE_X64& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, bool localCall);
    static void emitInternalCall(SCBE_X64& pp, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT = UINT32_MAX);
    static void emitInternalCallExt(SCBE_X64& pp, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT = UINT32_MAX, TypeInfoFuncAttr* typeFunc = nullptr);

    bool        buildRelocationSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CPURelocationTable& relocTable, SegmentKind me) const;
    void        computeUnwind(const VectorNative<CPURegister>& unwindRegs, const VectorNative<uint32_t>& unwindOffsetRegs, uint32_t sizeStack, uint32_t offsetSubRSP, VectorNative<uint16_t>& unwind) const;
    static void initFunction(CPUFunction* fct, uint32_t startAddress, uint32_t endAddress, uint32_t sizeProlog, VectorNative<uint16_t>& unwind);
};
