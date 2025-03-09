#pragma once
#include "Backend/SCBE/Encoder/ScbeCPU.h"

enum class OpBits : uint8_t;
enum CPUCondJump : uint8_t;
enum class CPUCondFlag : uint8_t;
enum class CPUOp : uint8_t;
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
    CPUOp       cpuOp;
    CPUCondFlag cpuCond;
    CPUCondJump jumpType;

    OpBits                   opBitsA;
    OpBits                   opBitsB;
    CPUEmitFlags             emitFlags;
    ScbeMicroInstructionFlag flags;

    uint32_t valueC;
    CPUReg   regA;
    CPUReg   regB;
    CPUReg   regC;
    
    bool isJump() const;
    bool isCall() const;
};
