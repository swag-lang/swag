// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/SCBE/Encoder/ScbeCpu.h"
struct ScbeMicroInstruction;

struct ScbeX64 final : ScbeCpu
{
    ScbeMicroOpDetails getInstructionDetails(ScbeMicroInstruction* inst) const override;

    void    emitSymbolRelocationRef(const Utf8& name) override;
    void    emitSymbolRelocationAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset) override;
    void    emitSymbolRelocationValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset) override;
    void    emitSymbolGlobalString(CpuReg reg, const Utf8& str) override;
    void    emitPush(CpuReg reg) override;
    void    emitPop(CpuReg reg) override;
    void    emitNop() override;
    void    emitRet() override;
    void    emitCallLocal(const Utf8& symbolName) override;
    void    emitCallExtern(const Utf8& symbolName) override;
    void    emitCallIndirect(CpuReg reg) override;
    void    emitJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries) override;
    CpuJump emitJump(CpuCondJump jumpType, OpBits opBits) override;
    void    emitPatchJump(const CpuJump& jump, uint64_t offsetDestination) override;
    void    emitPatchJump(const CpuJump& jump) override;
    void    emitJumpM(CpuReg reg) override;
    void    emitLoadRM(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits) override;
    void    emitLoadRI(CpuReg reg, uint64_t value, OpBits opBits) override;
    void    emitLoadRR(CpuReg regDst, CpuReg regSrc, OpBits opBits) override;
    void    emitLoadR(CpuReg regDstSrc, OpBits opBits) override;
    void    emitLoadRI64(CpuReg reg, uint64_t value) override;
    void    emitLoadSignedExtendRM(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadSignedExtendRR(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadZeroExtendRM(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadZeroExtendRR(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadAddressM(CpuReg reg, CpuReg memReg, uint64_t memOffset) override;
    void    emitLoadAddressAddMul(CpuReg regDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, OpBits opBits) override;
    void    emitStoreMR(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits) override;
    void    emitStoreMI(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits) override;
    void    emitCmpRR(CpuReg reg0, CpuReg reg1, OpBits opBits) override;
    void    emitCmpMR(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits) override;
    void    emitCmpMI(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits) override;
    void    emitCmpRI(CpuReg reg, uint64_t value, OpBits opBits) override;
    void    emitSetCC(CpuReg reg, CpuCondFlag setType) override;
    void    emitClearR(CpuReg reg, OpBits opBits) override;
    void    emitClearM(CpuReg memReg, uint64_t memOffset, uint32_t count) override;
    void    emitCopy(CpuReg regDst, CpuReg regSrc, uint32_t count) override;
    void    emitOpUnaryM(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits) override;
    void    emitOpUnaryR(CpuReg reg, CpuOp op, OpBits opBits) override;
    void    emitOpBinaryRR(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinaryRM(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinaryMR(CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinaryRI(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinaryMI(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    emitMulAdd(CpuReg regDst, CpuReg regMul, CpuReg regAdd, OpBits opBits) override;
};
