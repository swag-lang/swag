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
        if (emitFlags.has(EMITF_Overflow))
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
    encodeSymbolRelocationRef(name, EMITF_Zero);
}

void ScbeCpu::emitSymbolRelocationAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset)
{
    encodeSymbolRelocationAddress(reg, symbolIndex, offset, EMITF_Zero);
}

void ScbeCpu::emitSymbolRelocationValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset)
{
    encodeSymbolRelocationValue(reg, symbolIndex, offset, EMITF_Zero);
}

void ScbeCpu::emitSymbolGlobalString(CpuReg reg, const Utf8& str)
{
    encodeSymbolGlobalString(reg, str, EMITF_Zero);
}

void ScbeCpu::emitPush(CpuReg reg)
{
    encodePush(reg, EMITF_Zero);
}

void ScbeCpu::emitPop(CpuReg reg)
{
    encodePop(reg, EMITF_Zero);
}

void ScbeCpu::emitNop()
{
    encodeNop(EMITF_Zero);
}

void ScbeCpu::emitRet()
{
    encodeRet(EMITF_Zero);
}

void ScbeCpu::emitCallLocal(const Utf8& symbolName)
{
    encodeCallLocal(symbolName, EMITF_Zero);
}

void ScbeCpu::emitCallExtern(const Utf8& symbolName)
{
    encodeCallExtern(symbolName, EMITF_Zero);
}

void ScbeCpu::emitCallReg(CpuReg reg)
{
    encodeCallReg(reg, EMITF_Zero);
}

void ScbeCpu::emitJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries)
{
    encodeJumpTable(table, offset, currentIp, offsetTable, numEntries, EMITF_Zero);
}

void ScbeCpu::emitJump(CpuJump& jump, CpuCondJump jumpType, OpBits opBits)
{
    encodeJump(jump, jumpType, opBits, EMITF_Zero);
}

void ScbeCpu::emitPatchJump(const CpuJump& jump)
{
    encodePatchJump(jump, EMITF_Zero);
}

void ScbeCpu::emitPatchJump(const CpuJump& jump, uint64_t offsetDestination)
{
    encodePatchJump(jump, offsetDestination, EMITF_Zero);
}

void ScbeCpu::emitJumpReg(CpuReg reg)
{
    encodeJumpReg(reg, EMITF_Zero);
}

void ScbeCpu::emitLoadRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits)
{
    encodeLoadRegMem(reg, memReg, memOffset, opBits, EMITF_Zero);
}

void ScbeCpu::emitLoadRegImm(CpuReg reg, uint64_t value, OpBits opBits)
{
    maskValue(value, opBits);
    if (value == 0)
    {
        emitClearReg(reg, opBits);
        return;
    }

    const auto result = cpu->encodeLoadRegImm(reg, value, opBits, EMITF_CanEncode);
    if (result == RESULTF_Zero)
    {
        encodeLoadRegImm(reg, value, opBits, EMITF_Zero);
        return;
    }

    if (result.has(RESULTF_Right2Reg))
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
    encodeLoadRegReg(regDst, regSrc, opBits, EMITF_Zero);
}

void ScbeCpu::emitLoadRegImm64(CpuReg reg, uint64_t value)
{
    encodeLoadRegImm64(reg, value, EMITF_Zero);
}

void ScbeCpu::emitLoadSignedExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    if (numBitsSrc == numBitsDst)
        emitLoadRegReg(regDst, regSrc, numBitsSrc);
    else
        encodeLoadSignedExtendRegReg(regDst, regSrc, numBitsDst, numBitsSrc, EMITF_Zero);
}

void ScbeCpu::emitLoadSignedExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    if (numBitsSrc == numBitsDst)
        emitLoadRegMem(reg, memReg, memOffset, numBitsSrc);
    else
        encodeLoadSignedExtendRegMem(reg, memReg, memOffset, numBitsDst, numBitsSrc, EMITF_Zero);
}

void ScbeCpu::emitLoadZeroExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    if (numBitsSrc == numBitsDst)
        emitLoadRegReg(regDst, regSrc, numBitsSrc);
    else
        encodeLoadZeroExtendRegReg(regDst, regSrc, numBitsDst, numBitsSrc, EMITF_Zero);
}

void ScbeCpu::emitLoadZeroExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    if (numBitsSrc == numBitsDst)
        emitLoadRegMem(reg, memReg, memOffset, numBitsSrc);
    else
        encodeLoadZeroExtendRegMem(reg, memReg, memOffset, numBitsDst, numBitsSrc, EMITF_Zero);
}

void ScbeCpu::emitLoadAddressAddMul(CpuReg regDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, OpBits opBits)
{
    encodeLoadAddressAddMul(regDst, regSrc1, regSrc2, mulValue, opBits, EMITF_Zero);
}

void ScbeCpu::emitLoadAddressMem(CpuReg reg, CpuReg memReg, uint64_t memOffset)
{
    encodeLoadAddressMem(reg, memReg, memOffset, EMITF_Zero);
}

void ScbeCpu::emitLoadMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits)
{
    encodeLoadMemReg(memReg, memOffset, reg, opBits, EMITF_Zero);
}

void ScbeCpu::emitLoadMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    const auto result = cpu->encodeLoadMemImm(memReg, memOffset, value, opBits, EMITF_CanEncode);
    if (result == RESULTF_Zero)
    {
        encodeLoadMemImm(memReg, memOffset, value, opBits, EMITF_Zero);
        return;
    }

    if (result.has(RESULTF_Right2Reg))
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
        encodeCmpRegReg(reg0, reg1, opBits, EMITF_Zero);
    }
}

void ScbeCpu::emitCmpMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits)
{
    encodeCmpMemReg(memReg, memOffset, reg, opBits, EMITF_Zero);
}

void ScbeCpu::emitCmpMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    maskValue(value, opBits);
    encodeCmpMemImm(memReg, memOffset, value, opBits, EMITF_Zero);
}

void ScbeCpu::emitCmpRegImm(CpuReg reg, uint64_t value, OpBits opBits)
{
    maskValue(value, opBits);

    const auto result = cpu->encodeCmpRegImm(reg, value, opBits, EMITF_CanEncode);
    if (result == RESULTF_Zero)
    {
        encodeCmpRegImm(reg, value, opBits, EMITF_Zero);
        return;
    }

    if (result.has(RESULTF_Right2Reg))
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
        emitOpBinaryRegReg(reg, cc->computeRegI2, CpuOp::AND, OpBits::B8, EMITF_Zero);
        return;
    }

    if (setType == CpuCondFlag::NEP)
    {
        SWAG_ASSERT(reg != cc->computeRegI2);
        emitSetCond(reg, CpuCondFlag::NE);
        emitSetCond(cc->computeRegI2, CpuCondFlag::P);
        emitOpBinaryRegReg(reg, cc->computeRegI2, CpuOp::OR, OpBits::B8, EMITF_Zero);
        return;
    }

    encodeSetCond(reg, setType, EMITF_Zero);
}

void ScbeCpu::emitClearReg(CpuReg reg, OpBits opBits)
{
    encodeClearReg(reg, opBits, EMITF_Zero);
}

void ScbeCpu::emitClearMem(CpuReg memReg, uint64_t memOffset, uint32_t count)
{
    encodeClearMem(memReg, memOffset, count, EMITF_Zero);
}

void ScbeCpu::emitCopy(CpuReg regDst, CpuReg regSrc, uint32_t count)
{
    encodeCopy(regDst, regSrc, count, EMITF_Zero);
}

void ScbeCpu::emitOpUnaryMem(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits)
{
    encodeOpUnaryMem(memReg, memOffset, op, opBits, EMITF_Zero);
}

void ScbeCpu::emitOpUnaryReg(CpuReg reg, CpuOp op, OpBits opBits)
{
    encodeOpUnaryReg(reg, op, opBits, EMITF_Zero);
}

void ScbeCpu::emitOpBinaryRegReg(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    encodeOpBinaryRegReg(regDst, regSrc, op, opBits, emitFlags);
}

void ScbeCpu::emitOpBinaryRegMem(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    encodeOpBinaryRegMem(regDst, memReg, memOffset, op, opBits, emitFlags);
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
    encodeOpMulAdd(regDst, regMul, regAdd, opBits, EMITF_Zero);
}
