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
    void emitCallIndirect(CPUReg reg);
    void emitCallParameters(TypeInfoFuncAttr* typeFunc, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, void* retCopyAddr = nullptr);
    void emitCallParameters(TypeInfoFuncAttr* typeFunc, const VectorNative<CPUPushParam>& pushRAParams, uint32_t offsetRT, void* retCopyAddr = nullptr);
    void emitCallParameters(const TypeInfoFuncAttr* typeFuncBC, VectorNative<CPUPushParam>& paramsRegisters, VectorNative<TypeInfo*>& paramsTypes, void* retCopyAddr = nullptr);
    void emitCallResult(const TypeInfoFuncAttr* typeFunc, uint32_t offsetRT);

    void      emitJumpTable(CPUReg table, CPUReg offset);
    uint8_t*  emitJumpNear(CPUJumpType jumpType);
    uint32_t* emitJumpLong(CPUJumpType jumpType);
    void      emitJump(CPUJumpType jumpType, int32_t instructionCount, int32_t jumpOffset);
    void      emitJump(CPUReg reg);

    void emitLoadAddressIndirect(CPUReg reg, CPUReg memReg, uint32_t memOffset);
    void emitLoadIndirect(CPUReg reg, CPUReg memReg, uint32_t memOffset, CPUBits numBits);
    void emitLoadIndirect(CPUReg reg, CPUReg memReg, uint32_t memOffset, CPUSignedType srcType, CPUSignedType dstType);
    void emitLoadImmediate(CPUReg reg, uint64_t value, CPUBits numBits, bool force64Bits = false);
    void emitStoreIndirect(CPUReg memReg, uint32_t memOffset, CPUReg reg, CPUBits numBits);
    void emitStoreImmediate(CPUReg memReg, uint32_t memOffset, uint64_t value, CPUBits numBits);

    void emitCast(CPUReg regDst, CPUReg regSrc, CPUSignedType dstType, CPUSignedType srcType);
    void emitCopyDownUp(CPUReg reg, CPUBits numBits);
    void emitCwd();
    void emitCdq();
    void emitCqo();

    void emitCmp(CPUReg reg0, CPUReg reg1, CPUBits numBits);
    void emitCmpIndirect(CPUReg memReg, uint32_t memOffset, CPUReg reg, CPUBits numBits);
    void emitCmpIndirectDst(CPUReg memReg, uint32_t memOffset, uint32_t value, CPUBits numBits);
    void emitCmpImmediate(CPUReg reg, uint64_t value, CPUBits numBits);
    void emitSet(CPUReg reg, CPUSet setType);
    void emitClear(CPUReg reg, CPUBits numBits);
    void emitClear(CPUReg memReg, uint32_t memOffset, uint32_t count);
    void emitCopy(CPUReg regDst, CPUReg regSrc, CPUBits numBits);
    void emitCopy(CPUReg regDst, CPUReg regSrc, uint32_t count, uint32_t offset);
    void emitOp(CPUReg regDst, CPUReg regSrc, CPUOp op, CPUBits numBits, CPUBits srcBits = CPUBits::B32);
    void emitOpIndirectSrc(CPUReg reg, CPUReg memReg, uint32_t memOffset, CPUOp op, CPUBits numBits);
    void emitOpIndirect(CPUReg memReg, uint32_t memOffset, CPUReg reg, CPUOp op, CPUBits numBits, bool lock = false);
    void emitOpImmediate(CPUReg reg, uint64_t value, CPUOp op, CPUBits numBits);
    void emitOpIndirectDst(CPUReg memReg, uint32_t memOffset, uint64_t value, CPUOp op, CPUBits numBits);
    void emitOpIndirectDst(CPUReg regDst, CPUReg regSrc, CPUOp op, CPUBits numBits);
    void emitOpIndirectDst(CPUReg reg, uint64_t value, CPUOp op, CPUBits numBits);
    void emitTest(CPUReg regDst, CPUReg regSrc, CPUBits numBits);
    void emitNot(CPUReg reg, CPUBits numBits);
    void emitNotIndirect(CPUReg memReg, uint32_t memOffset, CPUBits numBits);
    void emitIncIndirect(CPUReg memReg, uint32_t memOffset, CPUBits numBits);
    void emitDecIndirect(CPUReg memReg, uint32_t memOffset, CPUBits numBits);
    void emitNeg(CPUReg reg, CPUBits numBits);
    void emitNegIndirect(CPUReg memReg, uint32_t memOffset, CPUBits numBits);
    void emitCMov(CPUReg regDst, CPUReg regSrc, CPUOp op, CPUBits numBits);
    void emitCmpXChg(CPUReg regDst, CPUReg regSrc, CPUBits numBits);
    void emitBSwap(CPUReg reg, CPUBits numBits);
    void emitRotate(CPUReg regDst, CPUReg regSrc, CPUOp op, CPUBits numBits);
    void emitMulAdd(CPUReg regDst, CPUReg regMul, CPUReg regAdd, CPUBits numBits);
};
