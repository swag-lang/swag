#include "pch.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"

OpBits ScbeMicroInstruction::getOpBitsReadReg() const
{
    if (hasLeftOpFlag(MOF_READ_REG) && hasLeftOpFlag(MOF_OPBITS_A))
        return opBitsA;
    if (hasRightOpFlag(MOF_READ_REG) && hasRightOpFlag(MOF_OPBITS_A))
        return opBitsA;    
    if (hasRightOpFlag(MOF_READ_REG) && hasRightOpFlag(MOF_OPBITS_B))
        return opBitsB;
    return OpBits::Zero;
}

OpBits ScbeMicroInstruction::getOpBitsWriteReg() const
{
    if (hasLeftOpFlag(MOF_WRITE_REG) && hasLeftOpFlag(MOF_OPBITS_A))
        return opBitsA;
    if (hasRightOpFlag(MOF_WRITE_REG) && hasRightOpFlag(MOF_OPBITS_A))
        return opBitsA;    
    if (hasRightOpFlag(MOF_WRITE_REG) && hasRightOpFlag(MOF_OPBITS_B))
        return opBitsB;
    return OpBits::Zero;    
}

bool ScbeMicroInstruction::hasReadRegA() const
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
           op == ScbeMicroOp::JumpTable ||
           op == ScbeMicroOp::JumpCondI ||
           op == ScbeMicroOp::JumpCond;
}

bool ScbeMicroInstruction::isJumpCond() const
{
    return (op == ScbeMicroOp::JumpCondI || op == ScbeMicroOp::JumpCond) && jumpType != CpuCondJump::JUMP;
}

bool ScbeMicroInstruction::isCall() const
{
    return op == ScbeMicroOp::CallExtern ||
           op == ScbeMicroOp::CallIndirect ||
           op == ScbeMicroOp::CallLocal;
}

bool ScbeMicroInstruction::isCallerParams() const
{
    return op == ScbeMicroOp::LoadCallParam ||
           op == ScbeMicroOp::LoadCallAddrParam ||
           op == ScbeMicroOp::StoreCallParam ||
           op == ScbeMicroOp::LoadCallZeroExtParam;
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
    switch (op)
    {
        case ScbeMicroOp::LoadRM:
        case ScbeMicroOp::LoadZeroExtRM:
        case ScbeMicroOp::LoadSignedExtRM:
        case ScbeMicroOp::OpBinaryRM:
            if (regB == CpuReg::Rsp)
                return static_cast<uint32_t>(valueA);
            break;
        case ScbeMicroOp::OpBinaryMR:
        case ScbeMicroOp::OpBinaryMI:
        case ScbeMicroOp::OpUnaryM:
            if (regA == CpuReg::Rsp)
                return static_cast<uint32_t>(valueA);
            break;
        case ScbeMicroOp::LoadAddrRM:
            if (regB == CpuReg::Rsp)
                return static_cast<uint32_t>(valueA);
            break;
        case ScbeMicroOp::CmpMI:
        case ScbeMicroOp::CmpMR:
            if (regA == CpuReg::Rsp)
                return static_cast<uint32_t>(valueA);
            break;
        case ScbeMicroOp::LoadAmcRM:
        case ScbeMicroOp::LoadAddrAmcRM:
            if (regB == CpuReg::Rsp || regC == CpuReg::Rsp)
                return static_cast<uint32_t>(valueB);
            break;
    }

    return UINT32_MAX;
}

uint32_t ScbeMicroInstruction::getStackOffsetWrite() const
{
    switch (op)
    {
        case ScbeMicroOp::LoadAddrRM:
            if (regB == CpuReg::Rsp)
                return static_cast<uint32_t>(valueA);
            break;
        case ScbeMicroOp::LoadMR:
        case ScbeMicroOp::LoadMI:
        case ScbeMicroOp::OpBinaryMR:
        case ScbeMicroOp::OpBinaryMI:
        case ScbeMicroOp::OpUnaryM:
            if (regA == CpuReg::Rsp)
                return static_cast<uint32_t>(valueA);
            break;
        case ScbeMicroOp::LoadAmcMR:
        case ScbeMicroOp::LoadAmcMI:
            if (regA == CpuReg::Rsp || regB == CpuReg::Rsp)
                return static_cast<uint32_t>(valueB);
            break;
        case ScbeMicroOp::LoadAddrAmcRM:
            if (regB == CpuReg::Rsp || regC == CpuReg::Rsp)
                return static_cast<uint32_t>(valueB);
            break;
    }

    return UINT32_MAX;
}

uint32_t ScbeMicroInstruction::getStackOffset() const
{
    auto result = getStackOffsetRead();
    if (result == UINT32_MAX)
        result = getStackOffsetWrite();
    return result;
}
