// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/SCBE/Encoder/SCBE_CPU.h"

struct SCBE_X64 final : SCBE_CPU
{
    void    emitSymbolRelocationRef(const Utf8& name) override;
    void    emitSymbolRelocationAddress(CPUReg reg, uint32_t symbolIndex, uint32_t offset) override;
    void    emitSymbolRelocationValue(CPUReg reg, uint32_t symbolIndex, uint32_t offset) override;
    void    emitSymbolGlobalString(CPUReg reg, const Utf8& str) override;
    void    emitPush(CPUReg reg) override;
    void    emitPop(CPUReg reg) override;
    void    emitNop() override;
    void    emitRet() override;
    void    emitCallLocal(const Utf8& symbolName) override;
    void    emitCallExtern(const Utf8& symbolName) override;
    void    emitCallIndirect(CPUReg reg) override;
    void    emitJumpTable(CPUReg table, CPUReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries) override;
    CPUJump emitJump(CPUCondJump jumpType, OpBits opBits) override;
    void    emitPatchJump(const CPUJump& jump, uint64_t offsetDestination) override;
    void    emitPatchJump(const CPUJump& jump) override;
    void    emitJump(CPUReg reg) override;
    void    emitLoad(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits opBits) override;
    void    emitLoad(CPUReg reg, uint64_t value, OpBits opBits) override;
    void    emitLoad(CPUReg regDst, CPUReg regSrc, OpBits opBits) override;
    void    emitLoad(CPUReg regDstSrc, OpBits opBits) override;
    void    emitLoad(CPUReg reg, uint64_t value) override;
    void    emitLoadSignedExtend(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadSignedExtend(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadZeroExtend(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadZeroExtend(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadAddress(CPUReg reg, CPUReg memReg, uint64_t memOffset) override;
    void    emitLoadAddress(CPUReg regDst, CPUReg regSrc1, CPUReg regSrc2, uint64_t mulValue, OpBits opBits) override;
    void    emitStore(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits) override;
    void    emitStore(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits) override;
    void    emitCmp(CPUReg reg0, CPUReg reg1, OpBits opBits) override;
    void    emitCmp(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits) override;
    void    emitCmp(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits) override;
    void    emitCmp(CPUReg reg, uint64_t value, OpBits opBits) override;
    void    emitSet(CPUReg reg, CPUCondFlag setType) override;
    void    emitClear(CPUReg reg, OpBits opBits) override;
    void    emitClear(CPUReg memReg, uint64_t memOffset, uint32_t count) override;
    void    emitCopy(CPUReg regDst, CPUReg regSrc, uint32_t count) override;
    void    emitOpUnary(CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits) override;
    void    emitOpUnary(CPUReg reg, CPUOp op, OpBits opBits) override;
    void    emitOpBinary(CPUReg regDst, CPUReg regSrc, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinary(CPUReg regDst, CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinary(CPUReg memReg, uint64_t memOffset, CPUReg reg, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinary(CPUReg reg, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinary(CPUReg memReg, uint64_t memOffset, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void    emitMulAdd(CPUReg regDst, CPUReg regMul, CPUReg regAdd, OpBits opBits) override;
};
