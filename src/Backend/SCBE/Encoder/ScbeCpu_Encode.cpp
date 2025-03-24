#include "pch.h"
#include "Backend/SCBE/Encoder/ScbeCpu.h"
#include "Backend/SCBE/Main/Scbe.h"
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

void ScbeCpu::emitSymbolRelocationRef(const Utf8& name)
{
    encodeSymbolRelocationRef(name, EMIT_Zero);
}

void ScbeCpu::emitSymbolRelocationAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset)
{
    encodeSymbolRelocationAddress(reg, symbolIndex, offset, EMIT_Zero);
}

void ScbeCpu::emitSymbolRelocationValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset)
{
    encodeSymbolRelocationValue(reg, symbolIndex, offset, EMIT_Zero);
}

void ScbeCpu::emitSymbolGlobalString(CpuReg reg, const Utf8& str)
{
    encodeSymbolGlobalString(reg, str, EMIT_Zero);
}

void ScbeCpu::emitPush(CpuReg reg)
{
    encodePush(reg, EMIT_Zero);
}

void ScbeCpu::emitPop(CpuReg reg)
{
    encodePop(reg, EMIT_Zero);
}

void ScbeCpu::emitNop()
{
    encodeNop(EMIT_Zero);
}

void ScbeCpu::emitRet()
{
    encodeRet(EMIT_Zero);
}

void ScbeCpu::emitCallLocal(const Utf8& symbolName)
{
    encodeCallLocal(symbolName, EMIT_Zero);
}

void ScbeCpu::emitCallExtern(const Utf8& symbolName)
{
    encodeCallExtern(symbolName, EMIT_Zero);
}

void ScbeCpu::emitCallReg(CpuReg reg)
{
    encodeCallReg(reg, EMIT_Zero);
}

void ScbeCpu::emitJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries)
{
    encodeJumpTable(table, offset, currentIp, offsetTable, numEntries, EMIT_Zero);
}

void ScbeCpu::emitJump(CpuJump& jump, CpuCondJump jumpType, OpBits opBits)
{
    encodeJump(jump, jumpType, opBits, EMIT_Zero);
}

void ScbeCpu::emitPatchJump(const CpuJump& jump)
{
    encodePatchJump(jump, EMIT_Zero);
}

void ScbeCpu::emitPatchJump(const CpuJump& jump, uint64_t offsetDestination)
{
    encodePatchJump(jump, offsetDestination, EMIT_Zero);
}

void ScbeCpu::emitJumpReg(CpuReg reg)
{
    encodeJumpReg(reg, EMIT_Zero);
}

void ScbeCpu::emitLoadRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits)
{
    encodeLoadRegMem(reg, memReg, memOffset, opBits, EMIT_Zero);
}

void ScbeCpu::emitLoadRegImm(CpuReg reg, uint64_t value, OpBits opBits)
{
    maskValue(value, opBits);
    if (value == 0)
    {
        emitClearReg(reg, opBits);
        return;
    }

    const auto result = cpu->encodeLoadRegImm(reg, value, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeLoadRegImm(reg, value, opBits, EMIT_Zero);
        return;
    }

    if (result == CpuEncodeResult::Right2Reg)
    {
        SWAG_ASSERT(reg != cc->computeRegI2);
        emitLoadRegImm(cc->computeRegI2, value, opBits);
        emitLoadRegReg(reg, cc->computeRegI2, opBits);
        return;
    }

    Report::internalError(module, "emitLoadRegImm, cannot encode");
}

void ScbeCpu::emitLoadRegReg(CpuReg regDst, CpuReg regSrc, OpBits opBits)
{
    encodeLoadRegReg(regDst, regSrc, opBits, EMIT_Zero);
}

void ScbeCpu::emitLoadRegImm64(CpuReg reg, uint64_t value)
{
    encodeLoadRegImm64(reg, value, EMIT_Zero);
}

void ScbeCpu::emitLoadSignedExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    if (numBitsSrc == numBitsDst)
        emitLoadRegReg(regDst, regSrc, numBitsSrc);
    else
        encodeLoadSignedExtendRegReg(regDst, regSrc, numBitsDst, numBitsSrc, EMIT_Zero);
}

void ScbeCpu::emitLoadSignedExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    if (numBitsSrc == numBitsDst)
        emitLoadRegMem(reg, memReg, memOffset, numBitsSrc);
    else
        encodeLoadSignedExtendRegMem(reg, memReg, memOffset, numBitsDst, numBitsSrc, EMIT_Zero);
}

void ScbeCpu::emitLoadZeroExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    if (numBitsSrc == numBitsDst)
        emitLoadRegReg(regDst, regSrc, numBitsSrc);
    else
        encodeLoadZeroExtendRegReg(regDst, regSrc, numBitsDst, numBitsSrc, EMIT_Zero);
}

void ScbeCpu::emitLoadZeroExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    if (numBitsSrc == numBitsDst)
        emitLoadRegMem(reg, memReg, memOffset, numBitsSrc);
    else
        encodeLoadZeroExtendRegMem(reg, memReg, memOffset, numBitsDst, numBitsSrc, EMIT_Zero);
}

void ScbeCpu::emitLoadAddressAddMul(CpuReg regDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, OpBits opBits)
{
    encodeLoadAddressAddMul(regDst, regSrc1, regSrc2, mulValue, opBits, EMIT_Zero);
}

void ScbeCpu::emitLoadAddressMem(CpuReg reg, CpuReg memReg, uint64_t memOffset)
{
    encodeLoadAddressMem(reg, memReg, memOffset, EMIT_Zero);
}

void ScbeCpu::emitLoadMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits)
{
    encodeLoadMemReg(memReg, memOffset, reg, opBits, EMIT_Zero);
}

void ScbeCpu::emitLoadMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    const auto result = cpu->encodeLoadMemImm(memReg, memOffset, value, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeLoadMemImm(memReg, memOffset, value, opBits, EMIT_Zero);
        return;
    }

    if (result == CpuEncodeResult::Right2Reg)
    {
        SWAG_ASSERT(memReg != cc->computeRegI1);
        emitLoadRegImm(cc->computeRegI1, value, OpBits::B64);
        emitLoadMemReg(memReg, memOffset, cc->computeRegI1, OpBits::B64);
        return;
    }

    Report::internalError(module, "emitLoadMemImm, cannot encode");
}

void ScbeCpu::emitCmpRegReg(CpuReg reg0, CpuReg reg1, OpBits opBits)
{
    if (isFloat(reg0) && isInt(reg1))
    {
        SWAG_ASSERT(reg0 != cc->computeRegF1);
        emitLoadRegReg(cc->computeRegF1, reg1, opBits);
        emitCmpRegReg(reg0, cc->computeRegF1, opBits);
    }
    else
    {
        encodeCmpRegReg(reg0, reg1, opBits, EMIT_Zero);
    }
}

void ScbeCpu::emitCmpMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits)
{
    encodeCmpMemReg(memReg, memOffset, reg, opBits, EMIT_Zero);
}

void ScbeCpu::emitCmpMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    maskValue(value, opBits);

    const auto result = cpu->encodeCmpMemImm(memReg, memOffset, value, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeCmpMemImm(memReg, memOffset, value, opBits, EMIT_Zero);
        return;
    }

    if (result == CpuEncodeResult::Right2Reg)
    {
        SWAG_ASSERT(memReg != cc->computeRegI2);
        emitLoadRegMem(cc->computeRegI2, memReg, memOffset, opBits);
        emitCmpRegImm(cc->computeRegI2, value, opBits);
        return;
    }

    Report::internalError(module, "emitCmpMemImm, cannot encode");
}

void ScbeCpu::emitCmpRegImm(CpuReg reg, uint64_t value, OpBits opBits)
{
    maskValue(value, opBits);

    const auto result = cpu->encodeCmpRegImm(reg, value, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeCmpRegImm(reg, value, opBits, EMIT_Zero);
        return;
    }

    if (result == CpuEncodeResult::Right2Reg)
    {
        SWAG_ASSERT(reg != cc->computeRegI1);
        emitLoadRegImm(cc->computeRegI1, value, opBits);
        emitCmpRegReg(reg, cc->computeRegI1, opBits);
        return;
    }

    Report::internalError(module, "emitCmpRegImm, cannot encode");
}

void ScbeCpu::emitSetCond(CpuReg reg, CpuCondFlag setType)
{
    if (setType == CpuCondFlag::EP)
    {
        SWAG_ASSERT(reg != cc->computeRegI2);
        emitSetCond(reg, CpuCondFlag::E);
        emitSetCond(cc->computeRegI2, CpuCondFlag::NP);
        emitOpBinaryRegReg(reg, cc->computeRegI2, CpuOp::AND, OpBits::B8, EMIT_Zero);
        return;
    }

    if (setType == CpuCondFlag::NEP)
    {
        SWAG_ASSERT(reg != cc->computeRegI2);
        emitSetCond(reg, CpuCondFlag::NE);
        emitSetCond(cc->computeRegI2, CpuCondFlag::P);
        emitOpBinaryRegReg(reg, cc->computeRegI2, CpuOp::OR, OpBits::B8, EMIT_Zero);
        return;
    }

    encodeSetCond(reg, setType, EMIT_Zero);
}

void ScbeCpu::emitClearReg(CpuReg reg, OpBits opBits)
{
    encodeClearReg(reg, opBits, EMIT_Zero);
}

void ScbeCpu::emitClearMem(CpuReg memReg, uint64_t memOffset, uint32_t count)
{
    encodeClearMem(memReg, memOffset, count, EMIT_Zero);
}

void ScbeCpu::emitCopy(CpuReg regDst, CpuReg regSrc, uint32_t count)
{
    encodeCopy(regDst, regSrc, count, EMIT_Zero);
}

void ScbeCpu::emitOpUnaryMem(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits)
{
    encodeOpUnaryMem(memReg, memOffset, op, opBits, EMIT_Zero);
}

void ScbeCpu::emitOpUnaryReg(CpuReg reg, CpuOp op, OpBits opBits)
{
    const auto result = cpu->encodeOpUnaryReg(reg, op, opBits, EMIT_CanEncode);
    if (result == CpuEncodeResult::Zero)
    {
        encodeOpUnaryReg(reg, op, opBits, EMIT_Zero);
        return;
    }

    if (result == CpuEncodeResult::NotSupported)
    {
        SWAG_ASSERT(reg == cc->computeRegF0);
        emitLoadMemImm(CpuReg::Rsp, cpuFct->getStackOffsetFLT(), opBits == OpBits::B32 ? 0x80000000 : 0x80000000'00000000, OpBits::B64);
        emitLoadRegMem(cc->computeRegF1, CpuReg::Rsp, cpuFct->getStackOffsetFLT(), opBits);
        emitOpBinaryRegReg(cc->computeRegF0, cc->computeRegF1, CpuOp::FXOR, opBits, EMIT_Zero);
        return;
    }

    Report::internalError(module, "emitOpUnaryReg, cannot encode");
}

void ScbeCpu::emitOpBinaryRegReg(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    encodeOpBinaryRegReg(regDst, regSrc, op, opBits, emitFlags);
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
    encodeOpBinaryMemReg(memReg, memOffset, reg, op, opBits, emitFlags);
}

void ScbeCpu::emitOpBinaryRegImm(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    maskValue(value, opBits);
    if (optLevel > BuildCfgBackendOptim::O1 &&
        isInt(reg) &&
        isNoOp(value, op, opBits, emitFlags))
    {
        return;
    }

    encodeOpBinaryRegImm(reg, value, op, opBits, emitFlags);
}

void ScbeCpu::emitOpBinaryMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    maskValue(value, opBits);
    if (optLevel > BuildCfgBackendOptim::O1 &&
        isNoOp(value, op, opBits, emitFlags))
    {
        return;
    }

    encodeOpBinaryMemImm(memReg, memOffset, value, op, opBits, emitFlags);
}

void ScbeCpu::emitOpMulAdd(CpuReg regDst, CpuReg regMul, CpuReg regAdd, OpBits opBits)
{
    encodeOpMulAdd(regDst, regMul, regAdd, opBits, EMIT_Zero);
}
