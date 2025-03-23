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

    void    emitEnter(uint32_t sizeStack) override;
    void    emitLeave() override;
    void    emitDebug(ByteCodeInstruction* ipAddr) override;
    void    emitLabel(uint32_t instructionIndex) override;
    void    emitSymbolRelocationRef(const Utf8& name) override;
    void    emitSymbolRelocationAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset) override;
    void    emitSymbolRelocationValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset) override;
    void    emitSymbolGlobalString(CpuReg reg, const Utf8& str) override;
    void    emitSymbolRelocationPtr(CpuReg reg, const Utf8& name) override;
    void    emitPush(CpuReg reg) override;
    void    emitPop(CpuReg reg) override;
    void    emitNop() override;
    void    emitRet() override;
    void    emitCallLocal(const Utf8& symbolName) override;
    void    emitCallExtern(const Utf8& symbolName) override;
    void    emitCallReg(CpuReg reg) override;
    void    emitJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries) override;
    CpuJump emitJump(CpuCondJump jumpType, OpBits opBits) override;
    void    emitJumpCondImm(CpuCondJump jumpType, uint32_t ipDest) override;
    void    emitPatchJump(const CpuJump& jump) override;
    void    emitPatchJump(const CpuJump& jump, uint64_t offsetDestination) override;
    void    emitJumpReg(CpuReg reg) override;
    void    emitLoadCallerParam(CpuReg reg, uint32_t paramIdx, OpBits opBits) override;
    void    emitLoadCallerZeroExtendParam(CpuReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadCallerAddressParam(CpuReg reg, uint32_t paramIdx) override;
    void    emitStoreCallerParam(uint32_t paramIdx, CpuReg reg, OpBits opBits) override;
    void    emitLoadRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits) override;
    void    emitLoadRegImm(CpuReg reg, uint64_t value, OpBits opBits) override;
    void    emitLoadRegReg(CpuReg regDst, CpuReg regSrc, OpBits opBits) override;
    void    emitLoadRegImm64(CpuReg reg, uint64_t value) override;
    void    emitLoadMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits) override;
    void    emitLoadMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits) override;
    void    emitLoadSignedExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadSignedExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadZeroExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadZeroExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadAddressMem(CpuReg reg, CpuReg memReg, uint64_t memOffset) override;
    void    emitLoadAddressAddMul(CpuReg regDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, OpBits opBits) override;
    void    emitCmpRegReg(CpuReg reg0, CpuReg reg1, OpBits opBits) override;
    void    emitCmpMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits) override;
    void    emitCmpMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits) override;
    void    emitCmpRegImm(CpuReg reg, uint64_t value, OpBits opBits) override;
    void    emitSetCond(CpuReg reg, CpuCondFlag setType) override;
    void    emitClearReg(CpuReg reg, OpBits opBits) override;
    void    emitClearMem(CpuReg memReg, uint64_t memOffset, uint32_t count) override;
    void    emitCopy(CpuReg regDst, CpuReg regSrc, uint32_t count) override;
    void    emitOpUnaryMem(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits) override;
    void    emitOpUnaryReg(CpuReg reg, CpuOp op, OpBits opBits) override;
    void    emitOpBinaryRegReg(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinaryRegMem(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    void    emitOpBinaryMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinaryRegImm(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinaryMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpMulAdd(CpuReg regDst, CpuReg regMul, CpuReg regAdd, OpBits opBits) override;

    ScbeMicroInstruction* addInstruction(ScbeMicroOp op);
    void                  pushRegisters() const;
    void                  process(ScbeCpu& encoder);
    void                  encode(ScbeCpu& encoder) const;
    void                  print() const;

    bool nextIsJumpDest = false;
};
