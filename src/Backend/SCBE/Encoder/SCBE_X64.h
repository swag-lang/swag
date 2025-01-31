// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/SCBE/Encoder/SCBE_CPU.h"

struct SCBE_X64 : SCBE_CPU
{
    void emitSymbolRelocationRef(const Utf8& name);
    void emitSymbolRelocationAddr(CPURegister reg, uint32_t symbolIndex, uint32_t offset);
    void emitSymbolRelocationValue(CPURegister reg, uint32_t symbolIndex, uint32_t offset);
    void emitSymbolGlobalString(CPURegister reg, const Utf8& str);

    void emitPush(CPURegister reg);
    void emitPop(CPURegister reg);
    void emitNop();
    void emitRet();

    void emitCall(const Utf8& symbolName);
    void emitCallFar(const Utf8& symbolName);
    void emitCallIndirect(CPURegister reg);
    void emitCallParameters(TypeInfoFuncAttr* typeFunc, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, void* retCopyAddr = nullptr);
    void emitCallParameters(TypeInfoFuncAttr* typeFunc, const VectorNative<CPUPushParam>& pushRAParams, uint32_t offsetRT, void* retCopyAddr = nullptr);
    void emitCallParameters(const TypeInfoFuncAttr* typeFuncBC, VectorNative<CPUPushParam>& paramsRegisters, VectorNative<TypeInfo*>& paramsTypes, void* retCopyAddr = nullptr);
    void emitCallResult(const TypeInfoFuncAttr* typeFunc, uint32_t offsetRT);

    void      emitJumpTable(CPURegister table, CPURegister offset);
    uint8_t*  emitNearJumpOp(CPUJumpType jumpType);
    uint32_t* emitLongJumpOp(CPUJumpType jumpType);
    void      emitJump(CPUJumpType jumpType, int32_t instructionCount, int32_t jumpOffset);
    void      emitJump(CPURegister reg);

    void emitLoadAddressIndirect(CPURegister reg, CPURegister memReg, uint32_t memOffset);
    void emitLoadImmediate(CPURegister reg, uint64_t value, CPUBits numBits, bool force64Bits = false);
    void emitStoreImmediate(CPURegister memReg, uint32_t memOffset, uint64_t value, CPUBits numBits);
    void emitStoreIndirect(CPURegister memReg, uint32_t memOffset, CPURegister reg, CPUBits numBits);
    void emitLoadIndirect(uint32_t memOffset, CPURegister reg, CPURegister memReg, CPUBits numBits);

    void emitCastU8U64(CPURegister regDst, CPURegister regSrc);
    void emitCastU16U64(CPURegister regDst, CPURegister regSrc);
    void emitCastU64F64(CPURegister regDst, CPURegister regSrc);
    void emitCopyDownUp(CPURegister reg, CPUBits numBits);
    void emitCwd();
    void emitCdq();
    void emitCqo();

    void emitLoadS16S32Indirect(uint32_t memOffset, CPURegister reg, CPURegister memReg);
    void emitLoadS16S64Indirect(uint32_t memOffset, CPURegister reg, CPURegister memReg);
    void emitLoadS32S64Indirect(uint32_t memOffset, CPURegister reg, CPURegister memReg);
    void emitLoadS8S16Indirect(uint32_t memOffset, CPURegister reg, CPURegister memReg);
    void emitLoadS8S32Indirect(uint32_t memOffset, CPURegister reg, CPURegister memReg);
    void emitLoadS8S64Indirect(uint32_t memOffset, CPURegister reg, CPURegister memReg);
    void emitLoadU16U32Indirect(uint32_t memOffset, CPURegister reg, CPURegister memReg);
    void emitLoadU16U64Indirect(uint32_t memOffset, CPURegister reg, CPURegister memReg);
    void emitLoadU8U32Indirect(uint32_t memOffset, CPURegister reg, CPURegister memReg);
    void emitLoadU8U64Indirect(uint32_t memOffset, CPURegister reg, CPURegister memReg);

    void emitCmp(CPURegister reg0, CPURegister reg1, CPUBits numBits);
    void emitCmpIndirect(uint32_t memOffset, CPURegister reg, CPURegister memReg, CPUBits numBits);
    void emitCmpIndirectDst(uint32_t memOffset, uint32_t value, CPURegister memReg, CPUBits numBits);
    void emitCmpImmediate(CPURegister reg, uint64_t value, CPUBits numBits);
    void emitSet(CPURegister reg, CPUSet setType);
    void emitClear(CPURegister reg, CPUBits numBits);
    void emitClear(CPURegister memReg, uint32_t memOffset, uint32_t count);
    void emitCopy(CPURegister regDst, CPURegister regSrc, CPUBits numBits);
    void emitCopy(CPURegister regDst, CPURegister regSrc, uint32_t count, uint32_t offset);
    void emitOp(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits, CPUBits srcBits = CPUBits::B32);
    void emitOp(uint32_t offsetStack, CPUOp op, CPUBits numBits);
    void emitOpIndirect(uint32_t memOffset, CPURegister reg, CPURegister memReg, CPUOp op, CPUBits numBits, bool lock = false);
    void emitOpImmediate(CPURegister reg, uint64_t value, CPUOp op, CPUBits numBits);
    void emitOpIndirectDst(uint32_t memOffset, uint64_t value, CPURegister memReg, CPUOp op, CPUBits numBits);
    void emitOpIndirectDst(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits);
    void emitOpIndirectDst(CPURegister reg, uint64_t value, CPUOp op, CPUBits numBits);
    void emitTest(CPURegister regDst, CPURegister regSrc, CPUBits numBits);
    void emitNot(CPURegister reg, CPUBits numBits);
    void emitNotIndirect(CPURegister memReg, uint32_t memOffset, CPUBits numBits);
    void emitIncIndirect(CPURegister memReg, uint32_t memOffset, CPUBits numBits);
    void emitDecIndirect(CPURegister memReg, uint32_t memOffset, CPUBits numBits);
    void emitNeg(CPURegister reg, CPUBits numBits);
    void emitNegIndirect(CPURegister memReg, uint32_t memOffset, CPUBits numBits);
    void emitCMov(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits);
    void emitCmpXChg(CPURegister regDst, CPURegister regSrc, CPUBits numBits);
    void emitBSwap(CPURegister reg, CPUBits numBits);
    void emitRotate(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits);
    void emitMulAdd(CPURegister regDst, CPURegister regMul, CPURegister regAdd, CPUBits numBits);
};
