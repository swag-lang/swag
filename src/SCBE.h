#pragma once
#include "Backend.h"
#include "BackendParameters.h"
#include "DataSegment.h"
#include "SCBE_X64.h"

struct Module;
struct BuildParameters;
struct Job;
struct DataSegment;
struct ByteCode;
struct TypeInfo;
struct ByteCodeInstruction;

struct SCBE : public Backend
{
    SCBE(Module* mdl);
    SCBE();

    void release();

    bool      createRuntime(const BuildParameters& buildParameters) const;
    JobResult prepareOutput(const BuildParameters& buildParameters, int stage, Job* ownerJob) override;
    bool      generateOutput(const BuildParameters& buildParameters) override;
    bool      emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc) override;

    bool saveObjFile(const BuildParameters& buildParameters) const;
    bool emitGetTypeTable(const BuildParameters& buildParameters);
    bool emitGlobalPreMain(const BuildParameters& buildParameters);
    bool emitGlobalInit(const BuildParameters& buildParameters);
    bool emitGlobalDrop(const BuildParameters& buildParameters);
    bool emitOS(const BuildParameters& buildParameters) const;
    bool emitMain(const BuildParameters& buildParameters);

    void        emitOverflowSigned(SCBE_X64& pp, ByteCodeInstruction* ip, const char* msg);
    void        emitOverflowUnsigned(SCBE_X64& pp, ByteCodeInstruction* ip, const char* msg);
    static void emitShiftRightArithmetic(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUBits numBits);
    static void emitShiftLogical(SCBE_X64& pp, ByteCodeInstruction* ip, CPUOp op, CPUBits numBits);
    static void emitShiftRightEqArithmetic(SCBE_X64& pp, ByteCodeInstruction* ip, CPUBits numBits);
    static void emitShiftEqLogical(SCBE_X64& pp, ByteCodeInstruction* ip, CPUOp op, CPUBits numBits);
    void        emitInternalPanic(SCBE_X64& pp, AstNode* node, const char* msg);
    static void emitBinOpFloat32(SCBE_X64& pp, ByteCodeInstruction* ip, CPUOp op);
    void        emitBinOpFloat32AtReg(SCBE_X64& pp, ByteCodeInstruction* ip, CPUOp op);
    static void emitBinOpFloat64(SCBE_X64& pp, ByteCodeInstruction* ip, CPUOp op);
    static void emitBinOpFloat64AtReg(SCBE_X64& pp, ByteCodeInstruction* ip, CPUOp op);
    static void emitBinOpIntN(SCBE_X64& pp, ByteCodeInstruction* ip, CPUOp op, CPUBits numBits);
    void        emitBinOpIntNAtReg(SCBE_X64& pp, ByteCodeInstruction* ip, CPUOp op, CPUBits numBits);
    void        emitBinOpDivIntNAtReg(SCBE_X64& pp, ByteCodeInstruction* ip, CPUOp op, CPUBits numBits);
    static void emitAddSubMul64(SCBE_X64& pp, ByteCodeInstruction* ip, uint64_t mul, CPUOp op);
    static void emitByteCodeCall(SCBE_X64& pp, const TypeInfoFuncAttr* typeFuncBc, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams);
    void        emitByteCodeCallParameters(SCBE_X64& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams);
    static void emitGetParam(SCBE_X64& pp, CPUFunction* cpuFct, int reg, uint32_t paramIdx, int sizeOf, uint64_t toAdd = 0, int derefSize = 0);
    static void emitCall(SCBE_X64& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT, bool localCall);
    void        emitCall(SCBE_X64& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, bool localCall);
    void        emitInternalCall(SCBE_X64& pp, Module* moduleToGen, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT = UINT32_MAX);
    void        emitInternalCallExt(SCBE_X64&                  pp,
                             Module*                           moduleToGen,
                             const Utf8&                       funcName,
                             const VectorNative<CPUPushParam>& pushParams,
                             uint32_t                          offsetRT = UINT32_MAX,
                             TypeInfoFuncAttr*                 typeFunc = nullptr);

    bool buildRelocSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CPURelocationTable& relocTable, SegmentKind me) const;
    void computeUnwind(const VectorNative<CPURegister>& unwindRegs,
                       const VectorNative<uint32_t>&    unwindOffsetRegs,
                       uint32_t                         sizeStack,
                       uint32_t                         offsetSubRSP,
                       VectorNative<uint16_t>&          unwind) const;
    static void initFunction(CPUFunction* fct, uint32_t startAddress, uint32_t endAddress, uint32_t sizeProlog, VectorNative<uint16_t>& unwind);

    SCBE_X64* perThread[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS];
};
