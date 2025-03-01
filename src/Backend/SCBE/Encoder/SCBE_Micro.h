// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/SCBE/Encoder/SCBE_CPU.h"

enum class SCBE_MicroOp : uint8_t
{
    SymbolRelocationRef,
    SymbolRelocationAddress,
    SymbolRelocationValue,
    SymbolGlobalString,
    SymbolRelocationPtr,
    Push,
    Pop,
    Enter,
    Leave,
    Nop,
    Ret,
    CallLocal,
    CallExtern,
    CallIndirect,
    JumpTable,
    Jump0,
    Jump1,
    Jump2,
    PatchJump,
    LoadParam,
    LoadAddressParam,
    LoadExtendParam,
    StoreParam,
    Load0,
    Load1,
    Load2,
    Load3,
    Load4,
    Load5,
    LoadExtend0,
    LoadExtend1,
    LoadAddress0,
    LoadAddress1,
    Store0,
    Store1,
    Cmp0,
    Cmp1,
    Cmp2,
    Cmp3,
    Set,
    Clear0,
    Clear1,
    Copy,
    OpUnary0,
    OpUnary1,
    OpBinary0,
    OpBinary1,
    OpBinary2,
    OpBinary3,
    OpBinary4,
    MulAdd,
    Label,
    Debug,
};

struct SCBE_MicroInstruction
{
    Utf8 name;

    SCBE_MicroOp op;
    CPUOp        cpuOp;
    CPUCondFlag  cpuCond;
    CPUCondJump  jumpType;
    CPUReg       regA;
    CPUReg       regB;
    CPUReg       regC;
    OpBits       opBitsA;
    OpBits       opBitsB;
    CPUEmitFlags emitFlags;
    bool         boolA;

    uint64_t valueA;
    uint64_t valueB;
    uint64_t valueC;
};

struct SCBE_Micro final : SCBE_CPU
{
    void init(const BuildParameters& buildParameters) override;

    void    emitDebug(ByteCodeInstruction* ipAddr) override;
    void    emitLabel(uint32_t instructionIndex) override;
    void    emitSymbolRelocationRef(const Utf8& name) override;
    void    emitSymbolRelocationAddress(CPUReg reg, uint32_t symbolIndex, uint32_t offset) override;
    void    emitSymbolRelocationValue(CPUReg reg, uint32_t symbolIndex, uint32_t offset) override;
    void    emitSymbolGlobalString(CPUReg reg, const Utf8& str) override;
    void    emitSymbolRelocationPtr(CPUReg reg, const Utf8& name) override;
    void    emitPush(CPUReg reg) override;
    void    emitPop(CPUReg reg) override;
    void    emitEnter(uint32_t sizeStack) override;
    void    emitLeave() override;
    void    emitNop() override;
    void    emitRet() override;
    void    emitCallLocal(const Utf8& symbolName) override;
    void    emitCallExtern(const Utf8& symbolName) override;
    void    emitCallIndirect(CPUReg reg) override;
    void    emitJumpTable(CPUReg table, CPUReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries) override;
    CPUJump emitJump(CPUCondJump jumpType, OpBits opBits) override;
    void    emitJump(CPUCondJump jumpType, uint32_t ipDest) override;
    void    emitPatchJump(const CPUJump& jump) override;
    void    emitPatchJump(const CPUJump& jump, uint64_t offsetDestination) override;
    void    emitJump(CPUReg reg) override;
    void    emitLoadParam(CPUReg reg, uint32_t paramIdx, OpBits opBits) override;
    void    emitLoadExtendParam(CPUReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc, bool isSigned) override;
    void    emitLoadAddressParam(CPUReg reg, uint32_t paramIdx, bool forceStack) override;
    void    emitStoreParam(uint32_t paramIdx, CPUReg reg, OpBits opBits, bool forceStack) override;
    void    emitLoad(CPUReg reg, CPUReg memReg, uint64_t memOffset, uint64_t value, bool isImmediate, CPUOp op, OpBits opBits) override;
    void    emitLoad(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits opBits) override;
    void    emitLoad(CPUReg reg, uint64_t value, OpBits opBits) override;
    void    emitLoad(CPUReg regDst, CPUReg regSrc, OpBits opBits) override;
    void    emitLoad(CPUReg regDstSrc, OpBits opBits) override;
    void    emitLoad(CPUReg reg, uint64_t value) override;
    void    emitLoadExtend(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, bool isSigned) override;
    void    emitLoadExtend(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, bool isSigned) override;
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
    void    emitOpBinary(CPUReg regDst, CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags) override;
    void    emitOpBinary(CPUReg memReg, uint64_t memOffset, CPUReg reg, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinary(CPUReg reg, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinary(CPUReg memReg, uint64_t memOffset, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void    emitMulAdd(CPUReg regDst, CPUReg regMul, CPUReg regAdd, OpBits opBits) override;

    void process();
    void print() const;
    void encode(SCBE_CPU& encoder) const;
};
