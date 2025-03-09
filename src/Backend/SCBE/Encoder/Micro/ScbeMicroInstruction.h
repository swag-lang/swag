#pragma once
#include "Backend/SCBE/Encoder/Micro/ScbeMicroOp.h"
#include "Backend/SCBE/Encoder/ScbeCpu.h"

enum class OpBits : uint8_t;
enum class CpuCondJump : uint8_t;
enum class CpuCondFlag : uint8_t;
enum class CpuOp : uint8_t;
enum class ScbeMicroOp : uint8_t;

using ScbeMicroInstructionFlags                   = Flags<uint8_t>;
constexpr ScbeMicroInstructionFlags MIF_ZERO      = 0x00;
constexpr ScbeMicroInstructionFlags MIF_JUMP_DEST = 0x01;

struct ScbeMicroInstruction
{
    SWAG_FORCE_INLINE bool hasLeftOpFlag(ScbeMicroOpFlags fl) const { return g_MicroOpInfos[static_cast<int>(op)].leftFlags.has(fl); }
    SWAG_FORCE_INLINE bool hasRightOpFlag(ScbeMicroOpFlags fl) const { return g_MicroOpInfos[static_cast<int>(op)].rightFlags.has(fl); }

    bool isWriteMemA() const { return hasLeftOpFlag(MOF_REG_A) && hasLeftOpFlag(MOF_VALUE_A) && hasLeftOpFlag(MOF_WRITE_MEM); }
    bool hasReadRegA() const { return (hasLeftOpFlag(MOF_REG_A) && hasLeftOpFlag(MOF_READ_REG)) || (hasRightOpFlag(MOF_REG_A) && hasRightOpFlag(MOF_READ_REG)); }
    bool hasReadRegB() const { return (hasLeftOpFlag(MOF_REG_B) && hasLeftOpFlag(MOF_READ_REG)) || (hasRightOpFlag(MOF_REG_B) && hasRightOpFlag(MOF_READ_REG)); }
    bool hasReadRegC() const { return (hasLeftOpFlag(MOF_REG_C) && hasLeftOpFlag(MOF_READ_REG)) || (hasRightOpFlag(MOF_REG_C) && hasRightOpFlag(MOF_READ_REG)); }

    bool isJump() const;
    bool isCall() const;

    Utf8     name;
    uint64_t valueA;
    uint64_t valueB;

    ScbeMicroOp op;
    CpuOp       cpuOp;
    CpuCondFlag cpuCond;
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
