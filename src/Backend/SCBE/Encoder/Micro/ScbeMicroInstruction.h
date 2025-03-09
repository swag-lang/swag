#pragma once
#include "Backend/SCBE/Encoder/ScbeCpu.h"

enum class OpBits : uint8_t;
enum class CpuCondJump : uint8_t;
enum class CpuCondFlag : uint8_t;
enum class CpuOp : uint8_t;
enum class ScbeMicroOp : uint8_t;

using ScbeMicroInstructionFlag                   = Flags<uint8_t>;
constexpr ScbeMicroInstructionFlag MIF_ZERO      = 0x00;
constexpr ScbeMicroInstructionFlag MIF_JUMP_DEST = 0x01;

struct ScbeMicroInstruction
{
    Utf8     name;
    uint64_t valueA;
    uint64_t valueB;

    ScbeMicroOp op;
    CpuOp       cpuOp;
    CpuCondFlag cpuCond;
    CpuCondJump jumpType;

    OpBits                   opBitsA;
    OpBits                   opBitsB;
    CpuEmitFlags             emitFlags;
    ScbeMicroInstructionFlag flags;

    uint32_t valueC;
    CpuReg   regA;
    CpuReg   regB;
    CpuReg   regC;

    bool isJump() const;
    bool isCall() const;
};
