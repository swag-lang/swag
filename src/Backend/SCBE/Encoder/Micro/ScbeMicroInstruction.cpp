#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"

bool ScbeMicroInstruction::isJump() const
{
    return op == ScbeMicroOp::JumpM ||
           op == ScbeMicroOp::JumpCI ||
           op == ScbeMicroOp::JumpTable ||
           op == ScbeMicroOp::JumpCC;
}

bool ScbeMicroInstruction::isJumpCond() const
{
    return op == ScbeMicroOp::JumpCI ||
           op == ScbeMicroOp::JumpCC;
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
           op == ScbeMicroOp::SetCC;
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
