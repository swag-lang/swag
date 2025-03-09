// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/SCBE/Encoder/SCBECPU.h"

enum class SCBEMicroOp : uint8_t
{
#define SCBE_MICRO_OP(__op, ...) __op,
#include "Backend/SCBE/Encoder/Micro/SCBEMicroOpList.h"
};

using SCBEMicroOpFlag                   = Flags<uint32_t>;
constexpr SCBEMicroOpFlag MOF_ZERO      = 0x00000000;
constexpr SCBEMicroOpFlag MOF_REG_A     = 0x00000001;
constexpr SCBEMicroOpFlag MOF_REG_B     = 0x00000002;
constexpr SCBEMicroOpFlag MOF_REG_C     = 0x00000004;
constexpr SCBEMicroOpFlag MOF_VALUE_A   = 0x00000008;
constexpr SCBEMicroOpFlag MOF_VALUE_B   = 0x00000010;
constexpr SCBEMicroOpFlag MOF_VALUE_C   = 0x00000020;
constexpr SCBEMicroOpFlag MOF_OPBITS_A  = 0x00000040;
constexpr SCBEMicroOpFlag MOF_OPBITS_B  = 0x00000080;
constexpr SCBEMicroOpFlag MOF_CPU_OP    = 0x00000100;
constexpr SCBEMicroOpFlag MOF_JUMP_TYPE = 0x00000200;
constexpr SCBEMicroOpFlag MOF_NAME      = 0x00000400;
constexpr SCBEMicroOpFlag MOF_CPU_COND  = 0x00000800;

struct SCBEMicroOpInfo
{
    const char*      name;
    SCBEMicroOpFlag leftFlags;
    SCBEMicroOpFlag rightFlags;
};

extern SCBEMicroOpInfo g_MicroOpInfos[];

using SCBE_MicroInstructionFlag                   = Flags<uint8_t>;
constexpr SCBE_MicroInstructionFlag MIF_ZERO      = 0x00;
constexpr SCBE_MicroInstructionFlag MIF_JUMP_DEST = 0x01;

struct SCBEMicroInstruction
{
    Utf8     name;
    uint64_t valueA;
    uint64_t valueB;

    SCBEMicroOp op;
    CPUOp        cpuOp;
    CPUCondFlag  cpuCond;
    CPUCondJump  jumpType;

    OpBits                    opBitsA;
    OpBits                    opBitsB;
    CPUEmitFlags              emitFlags;
    SCBE_MicroInstructionFlag flags;

    uint32_t valueC;
    CPUReg   regA;
    CPUReg   regB;
    CPUReg   regC;

    bool isJump() const { return op == SCBEMicroOp::JumpM || op == SCBEMicroOp::JumpCI || op == SCBEMicroOp::JumpTable || op == SCBEMicroOp::JumpCC; }
    bool isCall() const { return op == SCBEMicroOp::CallExtern || op == SCBEMicroOp::CallIndirect || op == SCBEMicroOp::CallLocal; }
};

using SCBEMicroOpDetails                  = Flags<uint64_t>;
constexpr SCBEMicroOpDetails MOD_ZERO     = 0x0000000000000000;
constexpr SCBEMicroOpDetails MOD_REG_RAX  = 0x0000000000000001;
constexpr SCBEMicroOpDetails MOD_REG_RBX  = 0x0000000000000002;
constexpr SCBEMicroOpDetails MOD_REG_RCX  = 0x0000000000000004;
constexpr SCBEMicroOpDetails MOD_REG_RDX  = 0x0000000000000008;
constexpr SCBEMicroOpDetails MOD_REG_R8   = 0x0000000000000010;
constexpr SCBEMicroOpDetails MOD_REG_R9   = 0x0000000000000020;
constexpr SCBEMicroOpDetails MOD_REG_R10  = 0x0000000000000040;
constexpr SCBEMicroOpDetails MOD_REG_R11  = 0x0000000000000080;
constexpr SCBEMicroOpDetails MOD_REG_R12  = 0x0000000000000100;
constexpr SCBEMicroOpDetails MOD_REG_R13  = 0x0000000000000200;
constexpr SCBEMicroOpDetails MOD_REG_R14  = 0x0000000000000400;
constexpr SCBEMicroOpDetails MOD_REG_R15  = 0x0000000000000800;
constexpr SCBEMicroOpDetails MOD_REG_XMM0 = 0x0000000000001000;
constexpr SCBEMicroOpDetails MOD_REG_XMM1 = 0x0000000000002000;
constexpr SCBEMicroOpDetails MOD_REG_XMM2 = 0x0000000000004000;
constexpr SCBEMicroOpDetails MOD_REG_XMM3 = 0x0000000000008000;
constexpr SCBEMicroOpDetails MOD_REG_RSI  = 0x0000000000010000;
constexpr SCBEMicroOpDetails MOD_REG_RDI  = 0x0000000000020000;
constexpr SCBEMicroOpDetails MOD_REG_ALL  = 0x0000000000FFFFFF;

struct SCBEMicro final : SCBECPU
{
    void init(const BuildParameters& buildParameters) override;

    void    emitEnter(uint32_t sizeStack) override;
    void    emitLeave() override;
    void    emitDebug(ByteCodeInstruction* ipAddr) override;
    void    emitLabel(uint32_t instructionIndex) override;
    void    emitSymbolRelocationRef(const Utf8& name) override;
    void    emitSymbolRelocationAddress(CPUReg reg, uint32_t symbolIndex, uint32_t offset) override;
    void    emitSymbolRelocationValue(CPUReg reg, uint32_t symbolIndex, uint32_t offset) override;
    void    emitSymbolGlobalString(CPUReg reg, const Utf8& str) override;
    void    emitSymbolRelocationPtr(CPUReg reg, const Utf8& name) override;
    void    emitPush(CPUReg reg) override;
    void    emitPop(CPUReg reg) override;
    void    emitNop() override;
    void    emitRet() override;
    void    emitCallLocal(const Utf8& symbolName) override;
    void    emitCallExtern(const Utf8& symbolName) override;
    void    emitCallIndirect(CPUReg reg) override;
    void    emitJumpTable(CPUReg table, CPUReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries) override;
    CPUJump emitJump(CPUCondJump jumpType, OpBits opBits) override;
    void    emitJumpCI(CPUCondJump jumpType, uint32_t ipDest) override;
    void    emitPatchJump(const CPUJump& jump) override;
    void    emitPatchJump(const CPUJump& jump, uint64_t offsetDestination) override;
    void    emitJumpM(CPUReg reg) override;
    void    emitLoadCallerParam(CPUReg reg, uint32_t paramIdx, OpBits opBits) override;
    void    emitLoadCallerZeroExtendParam(CPUReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadCallerAddressParam(CPUReg reg, uint32_t paramIdx) override;
    void    emitStoreCallerParam(uint32_t paramIdx, CPUReg reg, OpBits opBits) override;
    void    emitLoadRM(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits opBits) override;
    void    emitLoadRI(CPUReg reg, uint64_t value, OpBits opBits) override;
    void    emitLoadRR(CPUReg regDst, CPUReg regSrc, OpBits opBits) override;
    void    emitLoadR(CPUReg regDstSrc, OpBits opBits) override;
    void    emitLoadRI64(CPUReg reg, uint64_t value) override;
    void    emitLoadSignedExtendRM(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadSignedExtendRR(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadZeroExtendRM(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadZeroExtendRR(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadAddressM(CPUReg reg, CPUReg memReg, uint64_t memOffset) override;
    void    emitLoadAddressAddMul(CPUReg regDst, CPUReg regSrc1, CPUReg regSrc2, uint64_t mulValue, OpBits opBits) override;
    void    emitStoreMR(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits) override;
    void    emitStoreMI(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits) override;
    void    emitCmpRR(CPUReg reg0, CPUReg reg1, OpBits opBits) override;
    void    emitCmpMR(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits) override;
    void    emitCmpMI(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits) override;
    void    emitCmpRI(CPUReg reg, uint64_t value, OpBits opBits) override;
    void    emitSetCC(CPUReg reg, CPUCondFlag setType) override;
    void    emitClearR(CPUReg reg, OpBits opBits) override;
    void    emitClearM(CPUReg memReg, uint64_t memOffset, uint32_t count) override;
    void    emitCopy(CPUReg regDst, CPUReg regSrc, uint32_t count) override;
    void    emitOpUnaryM(CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits) override;
    void    emitOpUnaryR(CPUReg reg, CPUOp op, OpBits opBits) override;
    void    emitOpBinaryRR(CPUReg regDst, CPUReg regSrc, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinaryRM(CPUReg regDst, CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags) override;
    void    emitOpBinaryMR(CPUReg memReg, uint64_t memOffset, CPUReg reg, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinaryRI(CPUReg reg, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinaryMI(CPUReg memReg, uint64_t memOffset, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags = EMITF_Zero) override;
    void    emitMulAdd(CPUReg regDst, CPUReg regMul, CPUReg regAdd, OpBits opBits) override;

    SCBEMicroInstruction* addInstruction(SCBEMicroOp op);
    void                   process(SCBECPU& encoder);
    void                   encode(SCBECPU& encoder) const;
    void                   print() const;

    bool nextIsJumpDest = false;
};
