#include "pch.h"
#include "Backend/SCBE/Encoder/ScbeCpu.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Core/Math.h"
#include "Report/Report.h"

namespace
{
    void maskValue(uint64_t& value, OpBits opBits)
    {
        switch (opBits)
        {
            case OpBits::B8:
                value &= 0xFF;
                break;
            case OpBits::B16:
                value &= 0xFFFF;
                break;
            case OpBits::B32:
                value &= 0xFFFFFFFF;
                break;
        }
    }

    bool isNoOp(uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
    {
        if (emitFlags.has(EMIT_Overflow))
            return false;
        switch (value)
        {
            case 0:
                return op == CpuOp::ADD || op == CpuOp::SUB || op == CpuOp::SAR || op == CpuOp::SHL || op == CpuOp::SHR || op == CpuOp::ROL || op == CpuOp::ROR || op == CpuOp::BT;
            case 1:
                return op == CpuOp::MUL || op == CpuOp::IMUL || op == CpuOp::DIV || op == CpuOp::IDIV;
            default:
                break;
        }

        return false;
    }
}

void ScbeCpu::emitSymbolRelocationRef(const Utf8& name, CpuEmitFlags emitFlags)
{
    encodeSymbolRelocationRef(name, emitFlags);
}

void ScbeCpu::emitSymbolRelocationAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags)
{
    encodeSymbolRelocationAddress(reg, symbolIndex, offset, emitFlags);
}

void ScbeCpu::emitSymbolRelocationValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags)
{
    encodeSymbolRelocationValue(reg, symbolIndex, offset, emitFlags);
}

void ScbeCpu::emitSymbolGlobalString(CpuReg reg, const Utf8& str, CpuEmitFlags emitFlags)
{
    encodeSymbolGlobalString(reg, str, emitFlags);
}

void ScbeCpu::emitPush(CpuReg reg, CpuEmitFlags emitFlags)
{
    encodePush(reg, emitFlags);
}

void ScbeCpu::emitPop(CpuReg reg, CpuEmitFlags emitFlags)
{
    encodePop(reg, emitFlags);
}

void ScbeCpu::emitNop(CpuEmitFlags emitFlags)
{
    encodeNop(emitFlags);
}

void ScbeCpu::emitRet(CpuEmitFlags emitFlags)
{
    encodeRet(emitFlags);
}

void ScbeCpu::emitCallLocal(const Utf8& symbolName, const CallConv* callConv, CpuEmitFlags emitFlags)
{
    encodeCallLocal(symbolName, callConv, emitFlags);
}

void ScbeCpu::emitCallExtern(const Utf8& symbolName, const CallConv* callConv, CpuEmitFlags emitFlags)
{
    encodeCallExtern(symbolName, callConv, emitFlags);
}

void ScbeCpu::emitCallReg(CpuReg reg, const CallConv* callConv, CpuEmitFlags emitFlags)
{
    encodeCallReg(reg, callConv, emitFlags);
}

void ScbeCpu::emitJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries, CpuEmitFlags emitFlags)
{
    encodeJumpTable(table, offset, currentIp, offsetTable, numEntries, emitFlags);
}

void ScbeCpu::emitJump(CpuJump& jump, CpuCondJump jumpType, OpBits opBits, CpuEmitFlags emitFlags)
{
    encodeJump(jump, jumpType, opBits, emitFlags);
}

void ScbeCpu::emitPatchJump(const CpuJump& jump, CpuEmitFlags emitFlags)
{
    encodePatchJump(jump, emitFlags);
}

void ScbeCpu::emitPatchJump(const CpuJump& jump, uint64_t offsetDestination, CpuEmitFlags emitFlags)
{
    encodePatchJump(jump, offsetDestination, emitFlags);
}

void ScbeCpu::emitJumpReg(CpuReg reg, CpuEmitFlags emitFlags)
{
    encodeJumpReg(reg, emitFlags);
}

void ScbeCpu::emitLoadRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto result = cpu->encodeLoadRegMem(reg, memReg, memOffset, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeLoadRegMem(reg, memReg, memOffset, opBits, emitFlags);
        return;
    }

    Report::internalError(module, "emitLoadRegMem, cannot encode");
}

void ScbeCpu::emitLoadRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    maskValue(value, opBits);
    if (value == 0)
    {
        emitClearReg(reg, opBits, emitFlags);
        return;
    }

    const auto result = cpu->encodeLoadRegImm(reg, value, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeLoadRegImm(reg, value, opBits, emitFlags);
        return;
    }

    if (result == CpuEncodeResult::Right2Reg)
    {
        SWAG_ASSERT(reg != cc->computeRegI2);
        emitLoadRegImm(cc->computeRegI2, value, opBits, emitFlags);
        emitLoadRegReg(reg, cc->computeRegI2, opBits, emitFlags);
        return;
    }

    Report::internalError(module, "emitLoadRegImm, cannot encode");
}

void ScbeCpu::emitLoadRegReg(CpuReg regDst, CpuReg regSrc, OpBits opBits, CpuEmitFlags emitFlags)
{
    encodeLoadRegReg(regDst, regSrc, opBits, emitFlags);
}

void ScbeCpu::emitLoadRegImm64(CpuReg reg, uint64_t value, CpuEmitFlags emitFlags)
{
    encodeLoadRegImm64(reg, value, emitFlags);
}

void ScbeCpu::emitLoadSignedExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    if (numBitsSrc == numBitsDst)
        emitLoadRegReg(regDst, regSrc, numBitsSrc, emitFlags);
    else
        encodeLoadSignedExtendRegReg(regDst, regSrc, numBitsDst, numBitsSrc, emitFlags);
}

void ScbeCpu::emitLoadSignedExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    if (numBitsSrc == numBitsDst)
        emitLoadRegMem(reg, memReg, memOffset, numBitsSrc, emitFlags);
    else
        encodeLoadSignedExtendRegMem(reg, memReg, memOffset, numBitsDst, numBitsSrc, emitFlags);
}

void ScbeCpu::emitLoadZeroExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    if (numBitsSrc == numBitsDst)
        emitLoadRegReg(regDst, regSrc, numBitsSrc, emitFlags);
    else
        encodeLoadZeroExtendRegReg(regDst, regSrc, numBitsDst, numBitsSrc, emitFlags);
}

void ScbeCpu::emitLoadZeroExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    if (numBitsSrc == numBitsDst)
        emitLoadRegMem(reg, memReg, memOffset, numBitsSrc, emitFlags);
    else
        encodeLoadZeroExtendRegMem(reg, memReg, memOffset, numBitsDst, numBitsSrc, emitFlags);
}

void ScbeCpu::emitLoadAddressAddMul(CpuReg regDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, OpBits opBits, CpuEmitFlags emitFlags)
{
    encodeLoadAddressAddMul(regDst, regSrc1, regSrc2, mulValue, opBits, emitFlags);
}

void ScbeCpu::emitLoadAddressMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, CpuEmitFlags emitFlags)
{
    const auto result = cpu->encodeLoadAddressMem(reg, memReg, memOffset, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeLoadAddressMem(reg, memReg, memOffset, emitFlags);
        return;
    }

    Report::internalError(module, "emitLoadAddressMem, cannot encode");
}

void ScbeCpu::emitLoadMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto result = cpu->encodeLoadMemReg(memReg, memOffset, reg, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeLoadMemReg(memReg, memOffset, reg, opBits, emitFlags);
        return;
    }

    Report::internalError(module, "emitLoadMemReg, cannot encode");
}

void ScbeCpu::emitLoadMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto result = cpu->encodeLoadMemImm(memReg, memOffset, value, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeLoadMemImm(memReg, memOffset, value, opBits, emitFlags);
        return;
    }

    if (result == CpuEncodeResult::Right2Reg)
    {
        SWAG_ASSERT(memReg != cc->computeRegI1);
        emitLoadRegImm(cc->computeRegI1, value, OpBits::B64, emitFlags);
        emitLoadMemReg(memReg, memOffset, cc->computeRegI1, OpBits::B64, emitFlags);
        return;
    }

    Report::internalError(module, "emitLoadMemImm, cannot encode");
}

void ScbeCpu::emitCmpRegReg(CpuReg reg0, CpuReg reg1, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (isFloat(reg0) && isInt(reg1))
    {
        SWAG_ASSERT(reg0 != cc->computeRegF1);
        emitLoadRegReg(cc->computeRegF1, reg1, opBits, emitFlags);
        emitCmpRegReg(reg0, cc->computeRegF1, opBits, emitFlags);
    }
    else
    {
        encodeCmpRegReg(reg0, reg1, opBits, emitFlags);
    }
}

void ScbeCpu::emitCmpMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto result = cpu->encodeCmpMemReg(memReg, memOffset, reg, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeCmpMemReg(memReg, memOffset, reg, opBits, emitFlags);
        return;
    }

    if (result == CpuEncodeResult::Left2Reg)
    {
        if (isFloat(reg))
        {
            SWAG_ASSERT(reg != cc->computeRegF1);
            emitLoadRegMem(cc->computeRegF1, memReg, memOffset, opBits, emitFlags);
            emitCmpRegReg(cc->computeRegF1, reg, opBits, emitFlags);
            return;
        }
    }

    Report::internalError(module, "emitCmpMemReg, cannot encode");
}

void ScbeCpu::emitCmpMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    maskValue(value, opBits);

    const auto result = cpu->encodeCmpMemImm(memReg, memOffset, value, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeCmpMemImm(memReg, memOffset, value, opBits, emitFlags);
        return;
    }

    if (result == CpuEncodeResult::Right2Reg)
    {
        SWAG_ASSERT(memReg != cc->computeRegI2);
        emitLoadRegImm(cc->computeRegI2, value, opBits, emitFlags);
        emitCmpMemReg(memReg, memOffset, cc->computeRegI2, opBits, emitFlags);
        return;
    }

    Report::internalError(module, "emitCmpMemImm, cannot encode");
}

void ScbeCpu::emitCmpRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    maskValue(value, opBits);

    const auto result = cpu->encodeCmpRegImm(reg, value, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeCmpRegImm(reg, value, opBits, emitFlags);
        return;
    }

    if (result == CpuEncodeResult::Right2Reg)
    {
        SWAG_ASSERT(reg != cc->computeRegI1);
        emitLoadRegImm(cc->computeRegI1, value, opBits, emitFlags);
        emitCmpRegReg(reg, cc->computeRegI1, opBits, emitFlags);
        return;
    }

    Report::internalError(module, "emitCmpRegImm, cannot encode");
}

void ScbeCpu::emitSetCond(CpuReg reg, CpuCondFlag setType, CpuEmitFlags emitFlags)
{
    if (setType == CpuCondFlag::EP)
    {
        SWAG_ASSERT(reg != cc->computeRegI2);
        emitSetCond(reg, CpuCondFlag::E, emitFlags);
        emitSetCond(cc->computeRegI2, CpuCondFlag::NP, emitFlags);
        emitOpBinaryRegReg(reg, cc->computeRegI2, CpuOp::AND, OpBits::B8, emitFlags);
        return;
    }

    if (setType == CpuCondFlag::NEP)
    {
        SWAG_ASSERT(reg != cc->computeRegI2);
        emitSetCond(reg, CpuCondFlag::NE, emitFlags);
        emitSetCond(cc->computeRegI2, CpuCondFlag::P, emitFlags);
        emitOpBinaryRegReg(reg, cc->computeRegI2, CpuOp::OR, OpBits::B8, emitFlags);
        return;
    }

    encodeSetCond(reg, setType, EMIT_Zero);
}

void ScbeCpu::emitClearReg(CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags)
{
    encodeClearReg(reg, opBits, emitFlags);
}

void ScbeCpu::emitOpUnaryMem(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    encodeOpUnaryMem(memReg, memOffset, op, opBits, emitFlags);
}

void ScbeCpu::emitOpUnaryReg(CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto result = cpu->encodeOpUnaryReg(reg, op, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeOpUnaryReg(reg, op, opBits, emitFlags);
        return;
    }

    if (result == CpuEncodeResult::NotSupported)
    {
        SWAG_ASSERT(reg == cc->computeRegF0);
        emitLoadMemImm(CpuReg::Rsp, cpuFct->getStackOffsetFLT(), opBits == OpBits::B32 ? 0x80000000 : 0x80000000'00000000, OpBits::B64, emitFlags);
        emitLoadRegMem(cc->computeRegF1, CpuReg::Rsp, cpuFct->getStackOffsetFLT(), opBits, emitFlags);
        emitOpBinaryRegReg(cc->computeRegF0, cc->computeRegF1, CpuOp::FXOR, opBits, emitFlags);
        return;
    }

    Report::internalError(module, "emitOpUnaryReg, cannot encode");
}

void ScbeCpu::emitOpBinaryRegReg(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto result = cpu->encodeOpBinaryRegReg(regDst, regSrc, op, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeOpBinaryRegReg(regDst, regSrc, op, opBits, emitFlags);
        return;
    }

    if (result == CpuEncodeResult::NotSupported && op == CpuOp::CVTU2F64)
    {
        SWAG_ASSERT(regSrc != cc->computeRegI1);
        CpuJump jump0, jump1;
        emitClearReg(regDst, OpBits::B32);
        emitCmpRegImm(regSrc, 0, OpBits::B64);
        emitJump(jump0, CpuCondJump::JL, OpBits::B8);
        emitOpBinaryRegReg(regDst, regSrc, CpuOp::CVTI2F, OpBits::B64, EMIT_B64);
        emitJump(jump1, CpuCondJump::JUMP, OpBits::B8);
        emitPatchJump(jump0);
        emitLoadRegReg(cc->computeRegI1, regSrc, OpBits::B64);
        emitOpBinaryRegImm(regSrc, 1, CpuOp::AND, OpBits::B32, EMIT_Zero);
        emitOpBinaryRegImm(cc->computeRegI1, 1, CpuOp::SHR, OpBits::B64, EMIT_Zero);
        emitOpBinaryRegReg(cc->computeRegI1, regSrc, CpuOp::OR, OpBits::B64, EMIT_Zero);
        emitOpBinaryRegReg(regDst, cc->computeRegI1, CpuOp::CVTI2F, OpBits::B64, EMIT_B64);
        emitOpBinaryRegReg(regDst, regDst, CpuOp::FADD, OpBits::B64, EMIT_Zero);
        emitPatchJump(jump1);
        return;
    }

    if (result == CpuEncodeResult::Left2Rax)
    {
        SWAG_ASSERT(regSrc != CpuReg::Rax);
        emitLoadRegReg(CpuReg::Rax, regDst, opBits);
        emitOpBinaryRegReg(CpuReg::Rax, regSrc, op, opBits, emitFlags);
        emitLoadRegReg(regDst, CpuReg::Rax, opBits);
        return;
    }

    if (result == CpuEncodeResult::Right2Rcx)
    {
        SWAG_ASSERT(regDst != CpuReg::Rcx);
        emitLoadRegReg(CpuReg::Rcx, regSrc, opBits);
        emitOpBinaryRegReg(regDst, CpuReg::Rcx, op, opBits, emitFlags);
        return;
    }

    Report::internalError(module, "emitOpBinaryRegReg, cannot encode");
}

void ScbeCpu::emitOpBinaryRegMem(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto result = cpu->encodeOpBinaryRegMem(regDst, memReg, memOffset, op, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeOpBinaryRegMem(regDst, memReg, memOffset, op, opBits, emitFlags);
        return;
    }

    if (result == CpuEncodeResult::Right2Reg)
    {
        const auto r1 = isFloat(regDst) ? cc->computeRegF1 : cc->computeRegI1;
        SWAG_ASSERT(regDst != r1);
        SWAG_ASSERT(memReg != r1);
        emitLoadRegMem(r1, memReg, memOffset, opBits);
        emitOpBinaryRegReg(regDst, r1, op, opBits, emitFlags);
        return;
    }

    Report::internalError(module, "emitOpBinaryRegMem, cannot encode");
}

void ScbeCpu::emitOpBinaryMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto result = cpu->encodeOpBinaryMemReg(memReg, memOffset, reg, op, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeOpBinaryMemReg(memReg, memOffset, reg, op, opBits, emitFlags);
        return;
    }

    if (result == CpuEncodeResult::Left2Reg)
    {
        if (isFloat(reg))
        {
            SWAG_ASSERT(reg != cc->computeRegF2);
            emitLoadRegMem(cc->computeRegF2, memReg, memOffset, opBits);
            emitOpBinaryRegReg(cc->computeRegF2, reg, op, opBits, emitFlags);
            emitLoadMemReg(memReg, memOffset, cc->computeRegF2, opBits);
        }
        else
        {
            if (memReg == cc->computeRegI0)
            {
                SWAG_ASSERT(memReg != cc->computeRegI2);
                emitLoadRegReg(cc->computeRegI2, memReg, OpBits::B64);
                emitOpBinaryMemReg(cc->computeRegI2, memOffset, reg, op, opBits, emitFlags);
            }
            else
            {
                SWAG_ASSERT(reg != cc->computeRegI0);
                emitLoadRegMem(cc->computeRegI0, memReg, memOffset, opBits);
                emitOpBinaryRegReg(cc->computeRegI0, reg, op, opBits, emitFlags);
                emitLoadMemReg(memReg, memOffset, cc->computeRegI0, opBits);
            }
        }

        return;
    }

    if (result == CpuEncodeResult::Right2Rcx)
    {
        SWAG_ASSERT(memReg != CpuReg::Rcx);
        emitLoadRegReg(CpuReg::Rcx, reg, opBits);
        encodeOpBinaryMemReg(memReg, memOffset, CpuReg::Rcx, op, opBits, emitFlags);
        return;
    }

    Report::internalError(module, "emitOpBinaryMemReg, cannot encode");
}

namespace
{
    bool decomposeMul(uint32_t value, uint32_t& factor1, uint32_t& factor2)
    {
        if (value > 1024)
            return false;

        // [3, 5, 9] * [3, 5, 9]
        for (uint32_t i = 3; i <= value; i += 2)
        {
            if ((i == 3 || i == 5 || i == 9) && value % i == 0)
            {
                const uint32_t otherFactor = value / i;
                if ((otherFactor == 3 || otherFactor == 5 || otherFactor == 9))
                {
                    factor1 = i;
                    factor2 = otherFactor;
                    if (factor1 * factor2 == value)
                        return true;
                }
            }
        }

        // powerOf2 * [3, 5, 9]
        const uint32_t pow2        = 1 << std::countr_zero(value);
        const uint32_t otherFactor = value / pow2;
        if ((otherFactor == 3) || (otherFactor == 5) || (otherFactor == 9))
        {
            factor1 = pow2;
            factor2 = otherFactor;
            if (factor1 * factor2 == value)
                return true;
        }

        return false;
    }
}

void ScbeCpu::emitOpBinaryRegImm(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    maskValue(value, opBits);

    if (optLevel > BuildCfgBackendOptim::O1)
    {
        if (isInt(reg) && isNoOp(value, op, opBits, emitFlags))
        {
            return;
        }

        if ((op == CpuOp::MOD || op == CpuOp::IMOD) && Math::isPowerOfTwo(value))
        {
            emitOpBinaryRegImm(reg, value - 1, CpuOp::AND, opBits, emitFlags);
            return;
        }

        if (op == CpuOp::DIV && Math::isPowerOfTwo(value))
        {
            emitOpBinaryRegImm(reg, static_cast<uint32_t>(log2(value)), CpuOp::SHR, opBits, emitFlags);
            return;
        }

        if (op == CpuOp::SHL && value == 1)
        {
            emitOpBinaryRegReg(reg, reg, CpuOp::ADD, opBits, emitFlags);
            return;
        }

        if ((op == CpuOp::MUL || op == CpuOp::IMUL) && value == 0)
        {
            emitClearReg(reg, opBits);
            return;
        }

        if ((op == CpuOp::MUL || op == CpuOp::IMUL) && value == 3)
        {
            emitLoadAddressAddMul(reg, reg, reg, 2, std::max(opBits, OpBits::B32));
            return;
        }

        if ((op == CpuOp::MUL || op == CpuOp::IMUL) && value == 5)
        {
            emitLoadAddressAddMul(reg, reg, reg, 4, std::max(opBits, OpBits::B32));
            return;
        }

        if ((op == CpuOp::MUL || op == CpuOp::IMUL) && value == 9)
        {
            emitLoadAddressAddMul(reg, reg, reg, 8, std::max(opBits, OpBits::B32));
            return;
        }

        if ((op == CpuOp::MUL || op == CpuOp::IMUL) && Math::isPowerOfTwo(value))
        {
            emitOpBinaryRegImm(reg, static_cast<uint32_t>(log2(value)), CpuOp::SHL, opBits, emitFlags);
            return;
        }

        if (op == CpuOp::MUL || op == CpuOp::IMUL)
        {
            uint32_t factor1, factor2;
            if (decomposeMul(static_cast<uint32_t>(value), factor1, factor2))
            {
                if (factor1 != 1)
                    emitOpBinaryRegImm(reg, factor1, CpuOp::MUL, opBits, emitFlags);
                if (factor2 != 1)
                    emitOpBinaryRegImm(reg, factor2, CpuOp::MUL, opBits, emitFlags);
                return;
            }
        }
    }

    const auto result = cpu->encodeOpBinaryRegImm(reg, value, op, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeOpBinaryRegImm(reg, value, op, opBits, emitFlags);
        return;
    }

    if (result == CpuEncodeResult::Right2Reg)
    {
        if (reg != cc->computeRegI1)
        {
            emitLoadRegImm(cc->computeRegI1, value, opBits);
            emitOpBinaryRegReg(reg, cc->computeRegI1, op, opBits, emitFlags);
        }
        else
        {
            emitLoadRegImm(cc->computeRegI2, value, opBits);
            emitOpBinaryRegReg(reg, cc->computeRegI2, op, opBits, emitFlags);
        }

        return;
    }

    Report::internalError(module, "emitOpBinaryRegImm, cannot encode");
}

void ScbeCpu::emitOpBinaryMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    maskValue(value, opBits);

    if (optLevel >= BuildCfgBackendOptim::O1)
    {
        if (isNoOp(value, op, opBits, emitFlags))
        {
            return;
        }

        if ((op == CpuOp::IMOD || op == CpuOp::MOD) && Math::isPowerOfTwo(value))
        {
            emitOpBinaryMemImm(memReg, memOffset, value - 1, CpuOp::AND, opBits, emitFlags);
            return;
        }

        if (op == CpuOp::DIV && Math::isPowerOfTwo(value))
        {
            emitOpBinaryMemImm(memReg, memOffset, static_cast<uint32_t>(log2(value)), op == CpuOp::IDIV ? CpuOp::SAR : CpuOp::SHR, opBits, emitFlags);
            return;
        }

        if ((op == CpuOp::IMUL || op == CpuOp::MUL) && Math::isPowerOfTwo(value))
        {
            emitOpBinaryMemImm(memReg, memOffset, static_cast<uint32_t>(log2(value)), CpuOp::SHL, opBits, emitFlags);
            return;
        }
    }

    const auto result = cpu->encodeOpBinaryMemImm(memReg, memOffset, value, op, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeOpBinaryMemImm(memReg, memOffset, value, op, opBits, emitFlags);
        return;
    }

    if (result == CpuEncodeResult::Right2Reg)
    {
        SWAG_ASSERT(memReg != cc->computeRegI1);
        emitLoadRegImm(cc->computeRegI1, value, opBits);
        emitOpBinaryMemReg(memReg, memOffset, cc->computeRegI1, op, opBits, emitFlags);
        return;
    }

    Report::internalError(module, "emitOpBinaryMemImm, cannot encode");
}

void ScbeCpu::emitOpTernaryRegRegReg(CpuReg reg0, CpuReg reg1, CpuReg reg2, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    const auto result = cpu->encodeOpTernaryRegRegReg(reg0, reg1, reg2, op, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeOpTernaryRegRegReg(reg0, reg1, reg2, op, opBits, emitFlags);
        return;
    }

    if (result == CpuEncodeResult::Left2Rax)
    {
        SWAG_ASSERT(reg1 != CpuReg::Rax);
        SWAG_ASSERT(reg2 != CpuReg::Rax);
        emitLoadRegReg(CpuReg::Rax, reg0, opBits);
        emitOpTernaryRegRegReg(CpuReg::Rax, reg1, reg2, op, opBits, emitFlags);
        return;
    }

    Report::internalError(module, "emitOpTernaryRegRegReg, cannot encode");
}
