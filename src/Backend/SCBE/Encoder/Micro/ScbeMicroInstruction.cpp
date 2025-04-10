#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"

bool ScbeMicroInstruction::hasReadRegA() const
{
    if (op == ScbeMicroOp::OpBinaryRR)
    {
        if (cpuOp == CpuOp::BSR || cpuOp == CpuOp::BSF)
            return false;
    }

    return (hasLeftOpFlag(MOF_REG_A) && hasLeftOpFlag(MOF_READ_REG)) || (hasRightOpFlag(MOF_REG_A) && hasRightOpFlag(MOF_READ_REG));
}

bool ScbeMicroInstruction::hasReadRegB() const
{
    return (hasLeftOpFlag(MOF_REG_B) && hasLeftOpFlag(MOF_READ_REG)) || (hasRightOpFlag(MOF_REG_B) && hasRightOpFlag(MOF_READ_REG));
}

bool ScbeMicroInstruction::hasReadRegC() const
{
    return (hasLeftOpFlag(MOF_REG_C) && hasLeftOpFlag(MOF_READ_REG)) || (hasRightOpFlag(MOF_REG_C) && hasRightOpFlag(MOF_READ_REG));
}

bool ScbeMicroInstruction::hasWriteRegA() const
{
    return (hasLeftOpFlag(MOF_REG_A) && hasLeftOpFlag(MOF_WRITE_REG)) || (hasRightOpFlag(MOF_REG_A) && hasRightOpFlag(MOF_WRITE_REG));
}

bool ScbeMicroInstruction::hasWriteRegB() const
{
    if (op == ScbeMicroOp::OpBinaryMR || op == ScbeMicroOp::OpBinaryRR)
    {
        if (cpuOp == CpuOp::XCHG)
            return true;
    }

    return (hasLeftOpFlag(MOF_REG_B) && hasLeftOpFlag(MOF_WRITE_REG)) || (hasRightOpFlag(MOF_REG_B) && hasRightOpFlag(MOF_WRITE_REG));
}

bool ScbeMicroInstruction::hasWriteRegC() const
{
    return (hasLeftOpFlag(MOF_REG_C) && hasLeftOpFlag(MOF_WRITE_REG)) || (hasRightOpFlag(MOF_REG_C) && hasRightOpFlag(MOF_WRITE_REG));
}

uint32_t ScbeMicroInstruction::getNumBytes() const
{
    uint32_t size = 0;
    if (hasLeftOpFlag(MOF_OPBITS_B) || hasRightOpFlag(MOF_OPBITS_B))
        size = ScbeCpu::getNumBits(opBitsB) / 8;
    else if (hasLeftOpFlag(MOF_OPBITS_A) || hasRightOpFlag(MOF_OPBITS_A))
        size = ScbeCpu::getNumBits(opBitsA) / 8;
    return size;
}

bool ScbeMicroInstruction::isJump() const
{
    return op == ScbeMicroOp::JumpM ||
           op == ScbeMicroOp::JumpCI ||
           op == ScbeMicroOp::JumpTable ||
           op == ScbeMicroOp::JumpCond;
}

bool ScbeMicroInstruction::isJumpCond() const
{
    return op == ScbeMicroOp::JumpCI ||
           op == ScbeMicroOp::JumpCond;
}

bool ScbeMicroInstruction::isCall() const
{
    return op == ScbeMicroOp::CallExtern || op == ScbeMicroOp::CallIndirect || op == ScbeMicroOp::CallLocal;
}

bool ScbeMicroInstruction::isRet() const
{
    return op == ScbeMicroOp::Ret || op == ScbeMicroOp::Leave;
}

bool ScbeMicroInstruction::isTest() const
{
    return op == ScbeMicroOp::CmpMR ||
           op == ScbeMicroOp::CmpRI ||
           op == ScbeMicroOp::CmpRR ||
           op == ScbeMicroOp::CmpMI ||
           op == ScbeMicroOp::SetCondR ||
           op == ScbeMicroOp::LoadCondRR;
}

uint32_t ScbeMicroInstruction::getStackOffsetRead() const
{
    if (hasLeftOpFlag(MOF_READ_MEM) && hasLeftOpFlag(MOF_VALUE_A) && hasLeftOpFlag(MOF_REG_A) && regA == CpuReg::Rsp)
        return static_cast<uint32_t>(valueA);
    if (hasRightOpFlag(MOF_READ_MEM) && hasRightOpFlag(MOF_VALUE_A) && hasRightOpFlag(MOF_REG_B) && regB == CpuReg::Rsp)
        return static_cast<uint32_t>(valueA);
    return UINT32_MAX;
}

uint32_t ScbeMicroInstruction::getStackOffsetWrite() const
{
    if (hasLeftOpFlag(MOF_WRITE_MEM) && hasLeftOpFlag(MOF_VALUE_A) && hasLeftOpFlag(MOF_REG_A) && regA == CpuReg::Rsp)
        return static_cast<uint32_t>(valueA);
    if (hasRightOpFlag(MOF_WRITE_MEM) && hasRightOpFlag(MOF_VALUE_A) && hasRightOpFlag(MOF_REG_B) && regB == CpuReg::Rsp)
        return static_cast<uint32_t>(valueA);
    return UINT32_MAX;
}

uint32_t ScbeMicroInstruction::getStackOffset() const
{
    auto result = getStackOffsetRead();
    if (result == UINT32_MAX)
        result = getStackOffsetWrite();
    return result;
}
