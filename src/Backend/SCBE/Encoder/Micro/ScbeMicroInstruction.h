#pragma once
#include "Backend/SCBE/Encoder/Micro/ScbeMicroOp.h"
#include "Backend/SCBE/Encoder/ScbeCpu.h"
#include "ScbeMicro.h"

enum class OpBits : uint8_t;
enum class CpuCondJump : uint8_t;
enum class CpuCond : uint8_t;
enum class CpuOp : uint8_t;
enum class ScbeMicroOp : uint8_t;

using ScbeMicroInstructionFlags                   = Flags<uint8_t>;
constexpr ScbeMicroInstructionFlags MIF_ZERO      = 0x00;
constexpr ScbeMicroInstructionFlags MIF_JUMP_DEST = 0x01;
constexpr ScbeMicroInstructionFlags MIF_REACHED   = 0x02;

struct ScbeMicroInstruction
{
    SWAG_FORCE_INLINE bool hasLeftOpFlag(ScbeMicroOpFlags fl) const { return g_MicroOpInfos[static_cast<int>(op)].leftFlags.has(fl); }
    SWAG_FORCE_INLINE bool hasRightOpFlag(ScbeMicroOpFlags fl) const { return g_MicroOpInfos[static_cast<int>(op)].rightFlags.has(fl); }
    SWAG_FORCE_INLINE bool hasOpFlag(ScbeMicroOpFlags fl) const { return hasLeftOpFlag(fl) || hasRightOpFlag(fl); }

    bool hasWriteMemA() const { return hasLeftOpFlag(MOF_WRITE_MEM); }
    bool hasWriteMemB() const { return hasRightOpFlag(MOF_WRITE_MEM); }
    bool hasReadMemA() const { return hasLeftOpFlag(MOF_READ_MEM); }
    bool hasReadMemB() const { return hasRightOpFlag(MOF_READ_MEM); }

    bool hasReadRegA() const
    {
        if (op == ScbeMicroOp::OpBinaryRR)
        {
            if (cpuOp == CpuOp::BSR || cpuOp == CpuOp::BSF || cpuOp == CpuOp::POPCNT)
                return false;
            if (cpuOp == CpuOp::CVTF2F || cpuOp == CpuOp::CVTF2I || cpuOp == CpuOp::CVTI2F || cpuOp == CpuOp::CVTU2F64)
                return false;
        }

        return (hasLeftOpFlag(MOF_REG_A) && hasLeftOpFlag(MOF_READ_REG)) || (hasRightOpFlag(MOF_REG_A) && hasRightOpFlag(MOF_READ_REG));
    }

    bool hasWriteRegB() const
    {
        if (op == ScbeMicroOp::OpBinaryMR || op == ScbeMicroOp::OpBinaryRR)
        {
            if (cpuOp == CpuOp::XCHG)
                return true;
        }

        return (hasLeftOpFlag(MOF_REG_B) && hasLeftOpFlag(MOF_WRITE_REG)) || (hasRightOpFlag(MOF_REG_B) && hasRightOpFlag(MOF_WRITE_REG));
    }

    bool hasReadRegB() const { return (hasLeftOpFlag(MOF_REG_B) && hasLeftOpFlag(MOF_READ_REG)) || (hasRightOpFlag(MOF_REG_B) && hasRightOpFlag(MOF_READ_REG)); }
    bool hasReadRegC() const { return (hasLeftOpFlag(MOF_REG_C) && hasLeftOpFlag(MOF_READ_REG)) || (hasRightOpFlag(MOF_REG_C) && hasRightOpFlag(MOF_READ_REG)); }
    bool hasWriteRegA() const { return (hasLeftOpFlag(MOF_REG_A) && hasLeftOpFlag(MOF_WRITE_REG)) || (hasRightOpFlag(MOF_REG_A) && hasRightOpFlag(MOF_WRITE_REG)); }
    bool hasWriteRegC() const { return (hasLeftOpFlag(MOF_REG_C) && hasLeftOpFlag(MOF_WRITE_REG)) || (hasRightOpFlag(MOF_REG_C) && hasRightOpFlag(MOF_WRITE_REG)); }

    bool hasRegA() const { return (hasReadRegA() || hasWriteRegA()); }
    bool hasRegB() const { return (hasReadRegB() || hasWriteRegB()); }
    bool hasRegC() const { return (hasReadRegC() || hasWriteRegC()); }

    OpBits getOpBitsReadReg() const;

    uint32_t getNumBytes() const;

    bool isJumpDest() const { return flags.has(MIF_JUMP_DEST); }
    bool isJump() const;
    bool isJumpCond() const;
    bool isCall() const;
    bool isCallerParams() const;
    bool isRet() const;
    bool isTest() const;
    bool isEnd() const { return op == ScbeMicroOp::End; }

    uint32_t getStackOffsetRead() const;
    uint32_t getStackOffsetWrite() const;
    uint32_t getStackOffset() const;

    Utf8 name;

    union
    {
        uint64_t        valueA;
        const CallConv* cc;
    };

    uint64_t valueB;
    uint64_t valueC;

    ScbeMicroOp op;
    CpuOp       cpuOp;
    CpuCond     cpuCond;
    CpuCondJump jumpType;

    OpBits                    opBitsA;
    OpBits                    opBitsB;
    CpuEmitFlags              emitFlags;
    ScbeMicroInstructionFlags flags;

    CpuReg regA;
    CpuReg regB;
    CpuReg regC;
};
