// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/SCBE/Encoder/SCBE_CPU.h"

enum X64DispMode
{
    Disp8  = 0b01,
    Disp32 = 0b10,
    RegReg = 0b11,
};

struct SCBE_X64 : SCBE_CPU
{
    void emitSymbolRelocationRef(const Utf8& name);
    void emitSymbolRelocationAddr(CPURegister reg, uint32_t symbolIndex, uint32_t offset);
    void emitSymbolRelocationValue(CPURegister reg, uint32_t symbolIndex, uint32_t offset);
    void emitSymbolGlobalString(CPURegister reg, const Utf8& str);

    void emitPush(CPURegister reg);
    void emitPop(CPURegister reg);

    void emitCall(const Utf8& symbolName);
    void emitCallFar(const Utf8& symbolName);
    void emitCallIndirect(CPURegister reg);
    void emitCallParameters(TypeInfoFuncAttr* typeFunc, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, void* retCopyAddr = nullptr);
    void emitCallParameters(TypeInfoFuncAttr* typeFunc, const VectorNative<CPUPushParam>& pushRAParams, uint32_t offsetRT, void* retCopyAddr = nullptr);
    void emitCallParameters(const TypeInfoFuncAttr* typeFuncBC, VectorNative<CPUPushParam>& paramsRegisters, VectorNative<TypeInfo*>& paramsTypes, void* retCopyAddr = nullptr);
    void emitCallResult(const TypeInfoFuncAttr* typeFunc, uint32_t offsetRT);
    void emitRet();

    void emitClearN(CPURegister reg, CPUBits numBits);
    void emitClearX(uint32_t count, uint32_t offset, CPURegister reg);

    void emitCmpN(CPURegister regSrc, CPURegister regDst, CPUBits numBits);
    void emitCmpF32(CPURegister regSrc, CPURegister regDst);
    void emitCmpF64(CPURegister regSrc, CPURegister regDst);
    void emitCmpNIndirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUBits numBits);
    void emitCmpF32Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg);
    void emitCmpF64Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg);
    void emitCmpNImmediate(CPURegister reg, uint64_t value, CPUBits numBits);
    void emitCmpNIndirectDst(uint32_t offsetStack, uint32_t value, CPUBits numBits);

    void emitCopyN(CPURegister regDst, CPURegister regSrc, CPUBits numBits);
    void emitCopyF32(CPURegister regDst, CPURegister regSrc);
    void emitCopyF64(CPURegister regDst, CPURegister regSrc);
    void emitCopyX(CPURegister regDst, CPURegister regSrc, uint32_t count, uint32_t offset);
    void emitCopyDownUp(CPURegister reg, CPUBits numBits);

    void      emitJumpTable(CPURegister table, CPURegister offset);
    uint8_t*  emitNearJumpOp(CPUJumpType jumpType);
    uint32_t* emitLongJumpOp(CPUJumpType jumpType);
    void      emitJump(CPUJumpType jumpType, int32_t instructionCount, int32_t jumpOffset);
    void      emitJump(CPURegister reg);

    void emitExtendU16U64(CPURegister regDst, CPURegister regSrc);
    void emitExtendU8U64(CPURegister regDst, CPURegister regSrc);

    void emitLoad8Immediate(CPURegister reg, uint8_t value);
    void emitLoad16Immediate(CPURegister reg, uint16_t value);
    void emitLoad32Immediate(CPURegister reg, uint32_t value);
    void emitLoad64Immediate(CPURegister reg, uint64_t value, bool force64Bits = false);
    void emitLoadNImmediate(CPURegister reg, uint64_t value, CPUBits numBits);

    void emitLoad8Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emitLoad16Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emitLoad32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emitLoad64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emitLoadNIndirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg, CPUBits numBits);
    void emitLoadF32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emitLoadF64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);

    void emitStore8Immediate(uint32_t stackOffset, uint8_t val, CPURegister memReg);
    void emitStore16Immediate(uint32_t stackOffset, uint16_t val, CPURegister memReg);
    void emitStore32Immediate(uint32_t stackOffset, uint32_t val, CPURegister memReg);
    void emitStore64Immediate(uint32_t stackOffset, uint64_t val, CPURegister memReg);

    void emitStore8Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emitStore16Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emitStore32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emitStore64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emitStoreF32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emitStoreF64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg = RDI);
    void emitStoreNIndirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg, CPUBits numBits);

    void emitLoadAddressIndirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);

    void emitLoadS16S32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emitLoadS16S64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emitLoadS32S64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emitLoadS8S16Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emitLoadS8S32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emitLoadS8S64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emitLoadU16U32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emitLoadU16U64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emitLoadU8U32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);
    void emitLoadU8U64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg);

    void emitOpN(CPURegister regSrc, CPURegister regDst, CPUOp op, CPUBits numBits);
    void emitOpF32(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits srcBits = CPUBits::B32);
    void emitOpF64(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits srcBits = CPUBits::B32);

    void emitOpN(uint32_t offsetStack, CPUOp op, CPUBits numBits);
    void emitOpF32(uint32_t offsetStack, CPUOp op);
    void emitOpF64(uint32_t offsetStack, CPUOp op);

    void emitOpNIndirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUOp op, CPUBits numBits, bool lock = false);
    void emitOpF32Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUOp op);
    void emitOpF64Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUOp op);

    void emitOpNImmediate(CPURegister reg, uint64_t value, CPUOp op, CPUBits numBits);
    void emitOpNIndirectDst(uint32_t offsetStack, uint64_t value, CPURegister memReg, CPUOp op, CPUBits numBits);
    void emitOpNIndirectDst(CPURegister regSrc, CPURegister regDst, CPUOp op, CPUBits numBits);
    void emitOpNIndirectDst(CPURegister reg, uint64_t value, CPUOp op, CPUBits numBits);

    void emitSetA(CPURegister reg = RAX);
    void emitSetAE(CPURegister reg = RAX);
    void emitSetG(CPURegister reg = RAX);
    void emitSetB();
    void emitSetBE();
    void emitSetE();
    void emitSetEP();
    void emitSetGE();
    void emitSetL();
    void emitSetLE();
    void emitSetNA();
    void emitSetNE();
    void emitSetNEP();

    void emitCwd();
    void emitCdq();
    void emitCqo();

    void emitTestN(CPURegister regDst, CPURegister regSrc, CPUBits numBits);
    void emitNotN(CPURegister reg, CPUBits numBits);
    void emitNotNIndirect(uint32_t stackOffset, CPURegister memReg, CPUBits numBits);
    void emitIncNIndirect(uint32_t stackOffset, CPURegister memReg, CPUBits numBits);
    void emitDecNIndirect(uint32_t stackOffset, CPURegister memReg, CPUBits numBits);
    void emitNegN(CPURegister reg, CPUBits numBits);
    void emitNegNIndirect(uint32_t stackOffset, CPURegister memReg, CPUBits numBits);
    void emitCMovN(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits);
    void emitCmpXChg(CPURegister regDst, CPURegister regSrc, CPUBits numBits);
    void emitBSwapN(CPURegister reg, CPUBits numBits);
    void emitRotateN(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits);
    void emitNop();
    void emitCastU64F64(CPURegister regDst, CPURegister regSrc);
    void emitMulAddF32(CPURegister a, CPURegister b, CPURegister c);
    void emitMulAddF64(CPURegister a, CPURegister b, CPURegister c);
};
