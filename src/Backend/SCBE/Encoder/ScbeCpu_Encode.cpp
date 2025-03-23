#include "pch.h"
#include "Backend/SCBE/Encoder/ScbeCpu.h"
#include "Backend/SCBE/Main/Scbe.h"

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
    encodeSymbolRelocationRef(name);
}

void ScbeCpu::emitSymbolRelocationAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset)
{
    encodeSymbolRelocationAddress(reg, symbolIndex, offset);
}

void ScbeCpu::emitSymbolRelocationValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset)
{
    encodeSymbolRelocationValue(reg, symbolIndex, offset);
}

void ScbeCpu::emitSymbolGlobalString(CpuReg reg, const Utf8& str)
{
    encodeSymbolGlobalString(reg, str);
}

void ScbeCpu::emitPush(CpuReg reg)
{
    encodePush(reg);
}

void ScbeCpu::emitPop(CpuReg reg)
{
    encodePop(reg);
}

void ScbeCpu::emitNop()
{
    encodeNop();
}

void ScbeCpu::emitRet()
{
    encodeRet();
}

void ScbeCpu::emitCallLocal(const Utf8& symbolName)
{
    encodeCallLocal(symbolName);
}

void ScbeCpu::emitCallExtern(const Utf8& symbolName)
{
    encodeCallExtern(symbolName);
}

void ScbeCpu::emitCallReg(CpuReg reg)
{
    encodeCallReg(reg);
}

void ScbeCpu::emitJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries)
{
    encodeJumpTable(table, offset, currentIp, offsetTable, numEntries);
}

CpuJump ScbeCpu::emitJump(CpuCondJump jumpType, OpBits opBits)
{
    return encodeJump(jumpType, opBits);
}

void ScbeCpu::emitPatchJump(const CpuJump& jump)
{
    encodePatchJump(jump);
}

void ScbeCpu::emitPatchJump(const CpuJump& jump, uint64_t offsetDestination)
{
    encodePatchJump(jump, offsetDestination);
}

void ScbeCpu::emitJumpReg(CpuReg reg)
{
    encodeJumpReg(reg);
}

void ScbeCpu::emitLoadRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits)
{
    encodeLoadRegMem(reg, memReg, memOffset, opBits);
}

void ScbeCpu::emitLoadRegImm(CpuReg reg, uint64_t value, OpBits opBits)
{
    encodeLoadRegImm(reg, value, opBits);
}

void ScbeCpu::emitLoadRegReg(CpuReg regDst, CpuReg regSrc, OpBits opBits)
{
    encodeLoadRegReg(regDst, regSrc, opBits);
}

void ScbeCpu::emitLoadRegImm64(CpuReg reg, uint64_t value)
{
    encodeLoadRegImm64(reg, value);
}

void ScbeCpu::emitLoadSignedExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    encodeLoadSignedExtendRegReg(regDst, regSrc, numBitsDst, numBitsSrc);
}

void ScbeCpu::emitLoadSignedExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    encodeLoadSignedExtendRegMem(reg, memReg, memOffset, numBitsDst, numBitsSrc);
}

void ScbeCpu::emitLoadZeroExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    encodeLoadZeroExtendRegReg(regDst, regSrc, numBitsDst, numBitsSrc);
}

void ScbeCpu::emitLoadZeroExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    encodeLoadZeroExtendRegMem(reg, memReg, memOffset, numBitsDst, numBitsSrc);
}

void ScbeCpu::emitLoadAddressAddMul(CpuReg regDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, OpBits opBits)
{
    encodeLoadAddressAddMul(regDst, regSrc1, regSrc2, mulValue, opBits);
}

void ScbeCpu::emitLoadAddressMem(CpuReg reg, CpuReg memReg, uint64_t memOffset)
{
    encodeLoadAddressMem(reg, memReg, memOffset);
}

void ScbeCpu::emitLoadMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits)
{
    encodeLoadMemReg(memReg, memOffset, reg, opBits);
}

void ScbeCpu::emitLoadMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    encodeLoadMemImm(memReg, memOffset, value, opBits);
}

void ScbeCpu::emitCmpRegReg(CpuReg reg0, CpuReg reg1, OpBits opBits)
{
    encodeCmpRegReg(reg0, reg1, opBits);
}

void ScbeCpu::emitCmpMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits)
{
    encodeCmpMemReg(memReg, memOffset, reg, opBits);
}

void ScbeCpu::emitCmpMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    maskValue(value, opBits);
    encodeCmpMemImm(memReg, memOffset, value, opBits);
}

void ScbeCpu::emitCmpRegImm(CpuReg reg, uint64_t value, OpBits opBits)
{
    maskValue(value, opBits);
    encodeCmpRegImm(reg, value, opBits);
}

void ScbeCpu::emitSetCond(CpuReg reg, CpuCondFlag setType)
{
    encodeSetCond(reg, setType);
}

void ScbeCpu::emitClearReg(CpuReg reg, OpBits opBits)
{
    encodeClearReg(reg, opBits);
}

void ScbeCpu::emitClearMem(CpuReg memReg, uint64_t memOffset, uint32_t count)
{
    encodeClearMem(memReg, memOffset, count);
}

void ScbeCpu::emitCopy(CpuReg regDst, CpuReg regSrc, uint32_t count)
{
    encodeCopy(regDst, regSrc, count);
}

void ScbeCpu::emitOpUnaryMem(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits)
{
    encodeOpUnaryMem(memReg, memOffset, op, opBits);
}

void ScbeCpu::emitOpUnaryReg(CpuReg reg, CpuOp op, OpBits opBits)
{
    encodeOpUnaryReg(reg, op, opBits);
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
    encodeOpMulAdd(regDst, regMul, regAdd, opBits);
}
