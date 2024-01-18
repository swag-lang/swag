#pragma once
#include "Backend.h"
#include "BackendParameters.h"
#include "DataSegment.h"
#include "SCBEX64.h"

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

    bool      createRuntime(const BuildParameters& buildParameters);
    JobResult prepareOutput(int stage, const BuildParameters& buildParameters, Job* ownerJob) override;
    bool      generateOutput(const BuildParameters& backendParameters) override;
    bool      emitFunctionBody(const BuildParameters& buildParameters, Module* moduleToGen, ByteCode* bc) override;
    bool      saveObjFile(const BuildParameters& buildParameters);

    void emitOverflowSigned(SCBEX64& pp, ByteCodeInstruction* ip, const char* msg);
    void emitOverflowUnsigned(SCBEX64& pp, ByteCodeInstruction* ip, const char* msg);
    void emitShiftRightArithmetic(SCBEX64& pp, ByteCodeInstruction* ip, CPUBits numBits);
    void emitShiftLogical(SCBEX64& pp, ByteCodeInstruction* ip, CPUBits numBits, CPUOp op);
    void emitShiftRightEqArithmetic(SCBEX64& pp, ByteCodeInstruction* ip, CPUBits numBits);
    void emitShiftEqLogical(SCBEX64& pp, ByteCodeInstruction* ip, CPUBits numBits, CPUOp op);
    void emitInternalPanic(SCBEX64& pp, AstNode* node, const char* msg);
    void emitBinOpFloat32(SCBEX64& pp, ByteCodeInstruction* ip, CPUOp op);
    void emitBinOpFloat32AtReg(SCBEX64& pp, ByteCodeInstruction* ip, CPUOp op);
    void emitBinOpFloat64(SCBEX64& pp, ByteCodeInstruction* ip, CPUOp op);
    void emitBinOpFloat64AtReg(SCBEX64& pp, ByteCodeInstruction* ip, CPUOp op);
    void emitBinOpIntN(SCBEX64& pp, ByteCodeInstruction* ip, CPUOp op, CPUBits numBits);
    void emitBinOpIntNAtReg(SCBEX64& pp, ByteCodeInstruction* ip, CPUOp op, CPUBits numBits);
    void emitBinOpDivIntNAtReg(SCBEX64& pp, ByteCodeInstruction* ip, CPUOp op, CPUBits numBits);
    void emitAddSubMul64(SCBEX64& pp, ByteCodeInstruction* ip, uint64_t mul, CPUOp op);

    bool emitDebug(const BuildParameters& buildParameters);
    void emitByteCodeCall(SCBEX64& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams);
    void emitByteCodeCallParameters(SCBEX64& pp, TypeInfoFuncAttr* typeFuncBC, uint32_t offsetRT, VectorNative<uint32_t>& pushRAParams);

    bool emitXData(const BuildParameters& buildParameters);
    bool emitPData(const BuildParameters& buildParameters);
    bool emitDirectives(const BuildParameters& buildParameters);
    bool emitSymbolTable(const BuildParameters& buildParameters);
    bool emitStringTable(const BuildParameters& buildParameters);
    bool emitRelocationTable(Concat& concat, CoffRelocationTable& cofftable, uint32_t* sectionFlags, uint16_t* count);
    bool emitHeader(const BuildParameters& buildParameters);

    bool buildRelocSegment(const BuildParameters& buildParameters, DataSegment* dataSegment, CoffRelocationTable& relocTable, SegmentKind me);

    bool emitGetTypeTable(const BuildParameters& buildParameters);
    bool emitGlobalPreMain(const BuildParameters& buildParameters);
    bool emitGlobalInit(const BuildParameters& buildParameters);
    bool emitGlobalDrop(const BuildParameters& buildParameters);
    bool emitOS(const BuildParameters& buildParameters);
    bool emitMain(const BuildParameters& buildParameters);
    void emitSymbolRelocation(SCBEX64& pp, const Utf8& name);

    uint32_t getParamStackOffset(CoffFunction* coffFct, int paramIdx);
    void     emitGetParam(SCBEX64& pp, CoffFunction* coffFct, int reg, uint32_t paramIdx, int sizeOf, uint64_t toAdd = 0, int derefSize = 0);
    void     emitCall(SCBEX64& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT, bool localCall);
    void     emitCall(SCBEX64& pp, TypeInfoFuncAttr* typeFunc, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, bool localCall);
    void     emitInternalCall(SCBEX64& pp, Module* moduleToGen, const Utf8& funcName, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT = UINT32_MAX);
    void     emitInternalCallExt(SCBEX64& pp, Module* moduleToGen, const Utf8& funcName, const VectorNative<CPUPushParam>& pushParams, uint32_t offsetRT = UINT32_MAX, TypeInfoFuncAttr* typeFunc = nullptr);

    CoffFunction* registerFunction(SCBEX64& pp, AstNode* node, uint32_t symbolIndex);
    void          registerFunction(CoffFunction* fct, uint32_t startAddress, uint32_t endAddress, uint32_t sizeProlog, VectorNative<uint16_t>& unwind);

    SCBEX64* perThread[BackendCompileType::Count][MAX_PRECOMPILE_BUFFERS];
};