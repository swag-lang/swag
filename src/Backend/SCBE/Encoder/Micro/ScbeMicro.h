// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/SCBE/Encoder/ScbeCpu.h"

enum class ScbeMicroOp : uint8_t
{
#define SCBE_MICRO_OP(__op, ...) __op,
#include "Backend/SCBE/Encoder/Micro/ScbeMicroOpList.h"
};

struct ScbeMicro final : ScbeCpu
{
    void init(const BuildParameters& buildParameters) override;

    void emitEnter(uint32_t sizeStack) override;
    void emitLeave() override;
    void emitDebug(ByteCodeInstruction* ipAddr) override;
    void emitLabel(uint32_t instructionIndex) override;
    void emitLoadCallerParam(CpuReg reg, uint32_t paramIdx, OpBits opBits) override;
    void emitLoadCallerZeroExtendParam(CpuReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc) override;
    void emitLoadCallerAddressParam(CpuReg reg, uint32_t paramIdx) override;
    void emitStoreCallerParam(uint32_t paramIdx, CpuReg reg, OpBits opBits) override;
    void emitSymbolRelocationPtr(CpuReg reg, const Utf8& name) override;
    void emitJumpCondImm(CpuCondJump jumpType, uint32_t ipDest) override;

    void encodeSymbolRelocationRef(const Utf8& name, CpuEmitFlags emitFlags) override;
    void encodeSymbolRelocationAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags) override;
    void encodeSymbolRelocationValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags) override;
    void encodeSymbolGlobalString(CpuReg reg, const Utf8& str, CpuEmitFlags emitFlags) override;
    void encodePush(CpuReg reg, CpuEmitFlags emitFlags) override;
    void encodePop(CpuReg reg, CpuEmitFlags emitFlags) override;
    void encodeNop(CpuEmitFlags emitFlags) override;
    void encodeRet(CpuEmitFlags emitFlags) override;
    void encodeCallLocal(const Utf8& symbolName, CpuEmitFlags emitFlags) override;
    void encodeCallExtern(const Utf8& symbolName, CpuEmitFlags emitFlags) override;
    void encodeCallReg(CpuReg reg, CpuEmitFlags emitFlags) override;
    void encodeJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries, CpuEmitFlags emitFlags) override;
    void encodeJump(CpuJump& jump, CpuCondJump jumpType, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodePatchJump(const CpuJump& jump, CpuEmitFlags emitFlags) override;
    void encodePatchJump(const CpuJump& jump, uint64_t offsetDestination, CpuEmitFlags emitFlags) override;
    void encodeJumpReg(CpuReg reg, CpuEmitFlags emitFlags) override;
    void encodeLoadRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeLoadRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeLoadRegReg(CpuReg regDst, CpuReg regSrc, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeLoadRegImm64(CpuReg reg, uint64_t value, CpuEmitFlags emitFlags) override;
    void encodeLoadMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeLoadMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeLoadSignedExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags) override;
    void encodeLoadSignedExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags) override;
    void encodeLoadZeroExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags) override;
    void encodeLoadZeroExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags) override;
    void encodeLoadAddressMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, CpuEmitFlags emitFlags) override;
    void encodeLoadAddressAddMul(CpuReg regDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeCmpRegReg(CpuReg reg0, CpuReg reg1, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeCmpMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeCmpMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeCmpRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeSetCond(CpuReg reg, CpuCondFlag setType, CpuEmitFlags emitFlags) override;
    void encodeClearReg(CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeClearMem(CpuReg memReg, uint64_t memOffset, uint32_t count, CpuEmitFlags emitFlags) override;
    void encodeCopy(CpuReg regDst, CpuReg regSrc, uint32_t count, CpuEmitFlags emitFlags) override;
    void encodeOpUnaryMem(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeOpUnaryReg(CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeOpBinaryRegReg(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeOpBinaryRegMem(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeOpBinaryMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeOpBinaryRegImm(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeOpBinaryMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    void encodeOpMulAdd(CpuReg regDst, CpuReg regMul, CpuReg regAdd, OpBits opBits, CpuEmitFlags emitFlags) override;

    ScbeMicroInstruction* addInstruction(ScbeMicroOp op, CpuEmitFlags emitFlags);
    void                  pushRegisters() const;
    void                  process(ScbeCpu& encoder);
    void                  encode(ScbeCpu& encoder) const;
    void                  print() const;

    bool nextIsJumpDest = false;
};
