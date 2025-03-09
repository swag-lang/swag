// ReSharper disable CppInconsistentNaming
#pragma once
#include "Backend/SCBE/Encoder/Micro/ScbeMicroOp.h"
#include "Backend/SCBE/Encoder/ScbeCpu.h"

enum class ScbeMicroOp : uint8_t
{
#define SCBE_MICRO_OP(__op, ...) __op,
#include "Backend/SCBE/Encoder/Micro/ScbeMicroOpList.h"
};

struct ScbeMicroOpInfo
{
    const char*     name;
    ScbeMicroOpFlag leftFlags;
    ScbeMicroOpFlag rightFlags;
};

extern ScbeMicroOpInfo g_MicroOpInfos[];

using ScbeMicroOpDetails                  = Flags<uint64_t>;
constexpr ScbeMicroOpDetails MOD_ZERO     = 0x0000000000000000;
constexpr ScbeMicroOpDetails MOD_REG_RAX  = 0x0000000000000001;
constexpr ScbeMicroOpDetails MOD_REG_RBX  = 0x0000000000000002;
constexpr ScbeMicroOpDetails MOD_REG_RCX  = 0x0000000000000004;
constexpr ScbeMicroOpDetails MOD_REG_RDX  = 0x0000000000000008;
constexpr ScbeMicroOpDetails MOD_REG_R8   = 0x0000000000000010;
constexpr ScbeMicroOpDetails MOD_REG_R9   = 0x0000000000000020;
constexpr ScbeMicroOpDetails MOD_REG_R10  = 0x0000000000000040;
constexpr ScbeMicroOpDetails MOD_REG_R11  = 0x0000000000000080;
constexpr ScbeMicroOpDetails MOD_REG_R12  = 0x0000000000000100;
constexpr ScbeMicroOpDetails MOD_REG_R13  = 0x0000000000000200;
constexpr ScbeMicroOpDetails MOD_REG_R14  = 0x0000000000000400;
constexpr ScbeMicroOpDetails MOD_REG_R15  = 0x0000000000000800;
constexpr ScbeMicroOpDetails MOD_REG_XMM0 = 0x0000000000001000;
constexpr ScbeMicroOpDetails MOD_REG_XMM1 = 0x0000000000002000;
constexpr ScbeMicroOpDetails MOD_REG_XMM2 = 0x0000000000004000;
constexpr ScbeMicroOpDetails MOD_REG_XMM3 = 0x0000000000008000;
constexpr ScbeMicroOpDetails MOD_REG_RSI  = 0x0000000000010000;
constexpr ScbeMicroOpDetails MOD_REG_RDI  = 0x0000000000020000;
constexpr ScbeMicroOpDetails MOD_REG_ALL  = 0x0000000000FFFFFF;

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
    void    emitCallIndirect(CpuReg reg) override;
    void    emitJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries) override;
    CpuJump emitJump(CpuCondJump jumpType, OpBits opBits) override;
    void    emitJumpCI(CpuCondJump jumpType, uint32_t ipDest) override;
    void    emitPatchJump(const CpuJump& jump) override;
    void    emitPatchJump(const CpuJump& jump, uint64_t offsetDestination) override;
    void    emitJumpM(CpuReg reg) override;
    void    emitLoadCallerParam(CpuReg reg, uint32_t paramIdx, OpBits opBits) override;
    void    emitLoadCallerZeroExtendParam(CpuReg reg, uint32_t paramIdx, OpBits numBitsDst, OpBits numBitsSrc) override;
    void    emitLoadCallerAddressParam(CpuReg reg, uint32_t paramIdx) override;
    void    emitStoreCallerParam(uint32_t paramIdx, CpuReg reg, OpBits opBits) override;
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
    void    emitOpBinaryRM(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags) override;
    void    emitOpBinaryMR(CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinaryRI(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    emitOpBinaryMI(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags = EMITF_Zero) override;
    void    emitMulAdd(CpuReg regDst, CpuReg regMul, CpuReg regAdd, OpBits opBits) override;

    ScbeMicroInstruction* addInstruction(ScbeMicroOp op);
    void                  process(ScbeCpu& encoder);
    void                  encode(ScbeCpu& encoder) const;
    void                  print() const;

    bool nextIsJumpDest = false;
};
