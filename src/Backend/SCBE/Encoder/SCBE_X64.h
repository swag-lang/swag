// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/SCBE/Encoder/SCBE_CPU.h"

struct SCBE_X64 : SCBE_CPU
{
    void      emitSymbolRelocationRef(const Utf8& name) override;
    void      emitSymbolRelocationAddr(CPUReg reg, uint32_t symbolIndex, uint32_t offset) override;
    void      emitSymbolRelocationValue(CPUReg reg, uint32_t symbolIndex, uint32_t offset) override;
    void      emitSymbolGlobalString(CPUReg reg, const Utf8& str) override;
    void      emitPush(CPUReg reg) override;
    void      emitPop(CPUReg reg) override;
    void      emitNop() override;
    void      emitRet() override;
    void      emitCallLocal(const Utf8& symbolName) override;
    void      emitCallExtern(const Utf8& symbolName) override;
    void      emitCallIndirect(CPUReg reg) override;
    void      emitCallParameters(const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& params, uint32_t offset, void* retCopyAddr = nullptr) override;
    void      emitCallParameters(const TypeInfoFuncAttr* typeFuncBc, const VectorNative<CPUPushParam>& paramsRegisters, const VectorNative<TypeInfo*>& paramsTypes, void* retCopyAddr = nullptr) override;
    void      emitStoreCallResult(CPUReg memReg, uint32_t memOffset, const TypeInfoFuncAttr* typeFuncBc) override;
    void      emitJumpTable(CPUReg table, CPUReg offset) override;
    uint8_t*  emitJumpNear(CPUCondJump jumpType) override;
    uint32_t* emitJumpLong(CPUCondJump jumpType) override;
    void      emitJump(CPUCondJump jumpType, int32_t instructionCount, int32_t jumpOffset) override;
    void      emitJump(CPUReg reg) override;
    void      emitLoad(CPUReg reg, CPUReg memReg, uint64_t memOffset, uint64_t value, bool isImmediate, CPUOp op, OpBits opBits) override;
    void      emitLoad(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits opBits) override;
    void      emitLoad(CPUReg reg, uint64_t value, OpBits opBits) override;
    void      emitLoad(CPUReg regDst, CPUReg regSrc, OpBits opBits) override;
    void      emitLoad(CPUReg regDstSrc, OpBits opBits) override;
    void      emitLoad64(CPUReg reg, uint64_t value) override;
    void      emitLoadExtend(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, bool isSigned) override;
    void      emitLoadAddress(CPUReg reg, CPUReg memReg, uint64_t memOffset) override;
    void      emitStore(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits) override;
    void      emitStore(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits) override;
    void      emitCast(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, bool isSigned) override;
    void      emitCmp(CPUReg reg0, CPUReg reg1, OpBits opBits) override;
    void      emitCmp(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits) override;
    void      emitCmp(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits) override;
    void      emitCmp(CPUReg reg, uint64_t value, OpBits opBits) override;
    void      emitSet(CPUReg reg, CPUCondFlag setType) override;
    void      emitClear(CPUReg reg, OpBits opBits) override;
    void      emitClear(CPUReg memReg, uint64_t memOffset, uint32_t count) override;
    void      emitCopy(CPUReg regDst, CPUReg regSrc, uint32_t count, uint32_t offset) override;
    void      emitOpUnary(CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits) override;
    void      emitOpUnary(CPUReg reg, CPUOp op, OpBits opBits) override;
    void      emitOpBinary(CPUReg regDst, CPUReg regSrc, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void      emitOpBinary(CPUReg memReg, uint64_t memOffset, CPUReg reg, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void      emitOpBinary(CPUReg reg, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void      emitOpBinary(CPUReg memReg, uint64_t memOffset, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void      emitMulAdd(CPUReg regDst, CPUReg regMul, CPUReg regAdd, OpBits opBits) override;
};
