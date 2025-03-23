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

    void    encodeSymbolRelocationRef(const Utf8& name) override;
    void    encodeSymbolRelocationAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset) override;
    void    encodeSymbolRelocationValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset) override;
    void    encodeSymbolGlobalString(CpuReg reg, const Utf8& str) override;
    void    encodePush(CpuReg reg) override;
    void    encodePop(CpuReg reg) override;
    void    encodeNop() override;
    void    encodeRet() override;
    void    encodeCallLocal(const Utf8& symbolName) override;
    void    encodeCallExtern(const Utf8& symbolName) override;
    void    encodeCallReg(CpuReg reg) override;
    void    encodeJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries) override;
    CpuJump encodeJump(CpuCondJump jumpType, OpBits opBits) override;
    void    encodePatchJump(const CpuJump& jump) override;
    void    encodePatchJump(const CpuJump& jump, uint64_t offsetDestination) override;
    void    encodeJumpReg(CpuReg reg) override;
    void    encodeLoadRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits) override;
    void    encodeLoadRegImm(CpuReg reg, uint64_t value, OpBits opBits) override;
    void    encodeLoadRegReg(CpuReg regDst, CpuReg regSrc, OpBits opBits) override;
    void    encodeLoadRegImm64(CpuReg reg, uint64_t value) override;
    void    encodeLoadMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits) override;
    void    encodeLoadMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits) override;
    void    encodeLoadSignedExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    encodeLoadSignedExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    encodeLoadZeroExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    encodeLoadZeroExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    encodeLoadAddressMem(CpuReg reg, CpuReg memReg, uint64_t memOffset) override;
    void    encodeLoadAddressAddMul(CpuReg regDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, OpBits opBits) override;
    void    encodeCmpRegReg(CpuReg reg0, CpuReg reg1, OpBits opBits) override;
    void    encodeCmpMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits) override;
    void    encodeCmpMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits) override;
    void    encodeCmpRegImm(CpuReg reg, uint64_t value, OpBits opBits) override;
    void    encodeSetCond(CpuReg reg, CpuCondFlag setType) override;
    void    encodeClearReg(CpuReg reg, OpBits opBits) override;
    void    encodeClearMem(CpuReg memReg, uint64_t memOffset, uint32_t count) override;
    void    encodeCopy(CpuReg regDst, CpuReg regSrc, uint32_t count) override;
    void    encodeOpUnaryMem(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits) override;
    void    encodeOpUnaryReg(CpuReg reg, CpuOp op, OpBits opBits) override;
    void    encodeOpBinaryRegReg(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    encodeOpBinaryRegMem(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    void    encodeOpBinaryMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    encodeOpBinaryRegImm(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    encodeOpBinaryMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    encodeOpMulAdd(CpuReg regDst, CpuReg regMul, CpuReg regAdd, OpBits opBits) override;

    ScbeMicroInstruction* addInstruction(ScbeMicroOp op);
    void                  pushRegisters() const;
    void                  process(ScbeCpu& encoder);
    void                  encode(ScbeCpu& encoder) const;
    void                  print() const;

    bool nextIsJumpDest = false;
};
