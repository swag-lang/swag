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
    return OpBits::B64;
}

uint32_t ScbeMicroInstruction::getNumBytes() const
{
    uint32_t size = 0;
    if (hasLeftOpFlag(MOF_OPBITS_B) || hasRightOpFlag(MOF_OPBITS_B))
        size = BackendEncoder::getNumBits(opBitsB) / 8;
    else if (hasLeftOpFlag(MOF_OPBITS_A) || hasRightOpFlag(MOF_OPBITS_A))
        size = BackendEncoder::getNumBits(opBitsA) / 8;
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
