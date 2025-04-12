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

struct ScbeMicroInstruction
{
    SWAG_FORCE_INLINE bool hasLeftOpFlag(ScbeMicroOpFlags fl) const { return g_MicroOpInfos[static_cast<int>(op)].leftFlags.has(fl); }
    SWAG_FORCE_INLINE bool hasRightOpFlag(ScbeMicroOpFlags fl) const { return g_MicroOpInfos[static_cast<int>(op)].rightFlags.has(fl); }
    SWAG_FORCE_INLINE bool hasOpFlag(ScbeMicroOpFlags fl) const { return hasLeftOpFlag(fl) || hasRightOpFlag(fl); }

    bool hasWriteMemA() const { return (hasLeftOpFlag(MOF_VALUE_A) && hasLeftOpFlag(MOF_WRITE_MEM)) || (hasRightOpFlag(MOF_VALUE_A) && hasRightOpFlag(MOF_WRITE_MEM)); }
    bool hasWriteMemB() const { return (hasLeftOpFlag(MOF_VALUE_B) && hasLeftOpFlag(MOF_WRITE_MEM)) || (hasRightOpFlag(MOF_VALUE_B) && hasRightOpFlag(MOF_WRITE_MEM)); }
    bool hasReadMemA() const { return (hasLeftOpFlag(MOF_VALUE_A) && hasLeftOpFlag(MOF_READ_MEM)) || (hasRightOpFlag(MOF_VALUE_A) && hasRightOpFlag(MOF_READ_MEM)); }
    bool hasReadMemB() const { return (hasLeftOpFlag(MOF_VALUE_B) && hasLeftOpFlag(MOF_READ_MEM)) || (hasRightOpFlag(MOF_VALUE_B) && hasRightOpFlag(MOF_READ_MEM)); }

    bool hasReadRegA() const;
    bool hasReadRegB() const;
    bool hasReadRegC() const;
    bool hasWriteRegA() const;
    bool hasWriteRegB() const;
    bool hasWriteRegC() const;
    bool hasRegA() const { return (hasReadRegA() || hasWriteRegA()); }
    bool hasRegB() const { return (hasReadRegB() || hasWriteRegB()); }
    bool hasRegC() const { return (hasReadRegC() || hasWriteRegC()); }

    uint32_t getNumBytes() const;

    bool isJump() const;
    bool isJumpCond() const;
    bool isCall() const;
    bool isCallerParams() const;
    bool isRet() const;
    bool isTest() const;

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

    ScbeMicroOp op;
    CpuOp       cpuOp;
    CpuCond     cpuCond;
    CpuCondJump jumpType;

    OpBits                    opBitsA;
    OpBits                    opBitsB;
    CpuEmitFlags              emitFlags;
    ScbeMicroInstructionFlags flags;

    uint32_t valueC;
    CpuReg   regA;
    CpuReg   regB;
    CpuReg   regC;
};
