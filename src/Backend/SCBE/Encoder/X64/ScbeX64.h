// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/SCBE/Encoder/ScbeCpu.h"
struct ScbeMicroInstruction;

struct ScbeX64 final : ScbeCpu
{
    ScbeMicroOpDetails getInstructionDetails(ScbeMicroInstruction* inst) const override;

    CpuResultFlags encodeSymbolRelocationRef(const Utf8& name, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeSymbolRelocationAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeSymbolRelocationValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeSymbolGlobalString(CpuReg reg, const Utf8& str, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodePush(CpuReg reg, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodePop(CpuReg reg, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeNop(CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeRet(CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeCallLocal(const Utf8& symbolName, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeCallExtern(const Utf8& symbolName, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeCallReg(CpuReg reg, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeJump(CpuJump& jump, CpuCondJump jumpType, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodePatchJump(const CpuJump& jump, uint64_t offsetDestination, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodePatchJump(const CpuJump& jump, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeJumpReg(CpuReg reg, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeLoadRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeLoadRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeLoadRegReg(CpuReg regDst, CpuReg regSrc, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeLoadRegImm64(CpuReg reg, uint64_t value, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeLoadSignedExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeLoadSignedExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeLoadZeroExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeLoadZeroExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeLoadAddressMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeLoadAddressAddMul(CpuReg regDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeLoadMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeLoadMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeCmpRegReg(CpuReg reg0, CpuReg reg1, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeCmpMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeCmpMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeCmpRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeSetCond(CpuReg reg, CpuCondFlag setType, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeClearReg(CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeClearMem(CpuReg memReg, uint64_t memOffset, uint32_t count, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeCopy(CpuReg memRegDst, CpuReg memRegSrc, uint32_t count, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeOpUnaryMem(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeOpUnaryReg(CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeOpBinaryRegReg(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeOpBinaryRegMem(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeOpBinaryMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeOpBinaryRegImm(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeOpBinaryMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    CpuResultFlags encodeOpMulAdd(CpuReg regDst, CpuReg regMul, CpuReg regAdd, OpBits opBits, CpuEmitFlags emitFlags) override;
};
