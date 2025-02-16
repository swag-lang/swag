// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/SCBE/Encoder/SCBE_CPU.h"

struct SCBE_X64 : SCBE_CPU
{
    void emitSymbolRelocationRef(const Utf8& name);
    void emitSymbolRelocationAddr(CPUReg reg, uint32_t symbolIndex, uint32_t offset);
    void emitSymbolRelocationValue(CPUReg reg, uint32_t symbolIndex, uint32_t offset);
    void emitSymbolGlobalString(CPUReg reg, const Utf8& str);

    void emitPush(CPUReg reg);
    void emitPop(CPUReg reg);
    void emitNop();
    void emitRet();

    void emitCall(const Utf8& symbolName);
    void emitCallFar(const Utf8& symbolName);
    void emitCall(CPUReg reg);
    void emitCallParameters(const TypeInfoFuncAttr* typeFunc, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, void* retCopyAddr = nullptr);
    void emitCallParameters(const TypeInfoFuncAttr* typeFunc, const VectorNative<CPUPushParam>& pushRAParams, uint32_t offsetRT, void* retCopyAddr = nullptr);
    void emitCallParameters(const TypeInfoFuncAttr* typeFuncBC, VectorNative<CPUPushParam>& paramsRegisters, VectorNative<TypeInfo*>& paramsTypes, void* retCopyAddr = nullptr);
    void emitCallResult(const TypeInfoFuncAttr* typeFunc, uint32_t offsetRT);

    void      emitJumpTable(CPUReg table, CPUReg offset);
    uint8_t*  emitJumpNear(CPUCondJump jumpType);
    uint32_t* emitJumpLong(CPUCondJump jumpType);
    void      emitJump(CPUCondJump jumpType, int32_t instructionCount, int32_t jumpOffset);
    void      emitJump(CPUReg reg);

    void emitSetAddress(CPUReg reg, CPUReg memReg, uint64_t memOffset);
    void emitLoad(CPUReg reg, CPUReg memReg, uint64_t memOffset, uint64_t value, bool isImmediate, CPUOp op, OpBits opBits);
    void emitLoad(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits opBits);
    void emitLoad(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, bool isSigned);
    void emitLoad(CPUReg reg, uint64_t value, OpBits opBits);
    void emitLoad(CPUReg regDst, CPUReg regSrc, OpBits opBits);
    void emitLoad(CPUReg reg, OpBits opBits);
    void emitStore0Load64(CPUReg reg);
    void emitStore(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits);
    void emitStore(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits);

    void emitCast(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, bool isSigned);

    void emitCmp(CPUReg reg0, CPUReg reg1, OpBits opBits);
    void emitCmp(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits);
    void emitCmp(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits);
    void emitCmp(CPUReg reg, uint64_t value, OpBits opBits);
    void emitSet(CPUReg reg, CPUCondFlag setType);
    void emitClear(CPUReg reg, OpBits opBits);
    void emitClear(CPUReg memReg, uint64_t memOffset, uint32_t count);
    void emitCopy(CPUReg regDst, CPUReg regSrc, uint32_t count, uint32_t offset);
    void emitOpUnary(CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits);
    void emitOpUnary(CPUReg reg, CPUOp op, OpBits opBits);
    void emitOpBinary(CPUReg regDst, CPUReg regSrc, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero);
    void emitOpBinary(CPUReg memReg, uint64_t memOffset, CPUReg reg, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero);
    void emitOpBinary(CPUReg reg, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero);
    void emitOpBinary(CPUReg memReg, uint64_t memOffset, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero);
    void emitMulAdd(CPUReg regDst, CPUReg regMul, CPUReg regAdd, OpBits opBits);
};
