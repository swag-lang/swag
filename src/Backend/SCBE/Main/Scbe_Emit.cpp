#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/ByteCode_Math.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

void Scbe::emitIMMA(ScbeCpu& pp, CpuReg reg, OpBits opBits)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_A))
        pp.emitLoadRegImm(reg, ip->a.u64, opBits);
    else
        pp.emitLoadRegMem(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
}

void Scbe::emitIMMB(ScbeCpu& pp, CpuReg reg, OpBits opBits)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_B))
        pp.emitLoadRegImm(reg, ip->b.u64, opBits);
    else
        pp.emitLoadRegMem(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), opBits);
}

void Scbe::emitIMMC(ScbeCpu& pp, CpuReg reg, OpBits opBits)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_C))
        pp.emitLoadRegImm(reg, ip->c.u64, opBits);
    else
        pp.emitLoadRegMem(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), opBits);
}

void Scbe::emitIMMD(ScbeCpu& pp, CpuReg reg, OpBits opBits)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_D))
        pp.emitLoadRegImm(reg, ip->d.u64, opBits);
    else
        pp.emitLoadRegMem(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->d.u32), opBits);
}

void Scbe::emitIMMB(ScbeCpu& pp, CpuReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_B))
    {
        SWAG_ASSERT(numBitsDst == OpBits::B16 || numBitsDst == OpBits::B32);
        pp.emitLoadRegImm(reg, ip->b.u64, OpBits::B32);
    }
    else if (isSigned)
    {
        pp.emitLoadSignedExtendRegMem(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), numBitsDst, numBitsSrc);
    }
    else
    {
        pp.emitLoadZeroExtendRegMem(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), numBitsDst, numBitsSrc);
    }
}

void Scbe::emitIMMC(ScbeCpu& pp, CpuReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_C))
    {
        SWAG_ASSERT(numBitsDst == OpBits::B16 || numBitsDst == OpBits::B32);
        pp.emitLoadRegImm(reg, ip->c.u64, OpBits::B32);
    }
    else if (isSigned)
    {
        pp.emitLoadSignedExtendRegMem(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), numBitsDst, numBitsSrc);
    }
    else
    {
        pp.emitLoadZeroExtendRegMem(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), numBitsDst, numBitsSrc);
    }
}

void Scbe::emitShiftRightArithmetic(ScbeCpu& pp)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitOpBinaryRegImm(cc->computeRegI0, ip->b.u32, CpuOp::SAR, opBits);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRegImm(cc->computeRegI1, std::min(static_cast<uint32_t>(ip->b.u8), ScbeCpu::getNumBits(opBits) - 1), OpBits::B8);
        emitIMMA(pp, cc->computeRegI0, opBits);
        pp.emitOpBinaryRegReg(cc->computeRegI0, cc->computeRegI1, CpuOp::SAR, opBits);
    }
    else
    {
        pp.emitLoadRegMem(cc->computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32);
        pp.emitLoadRegImm(cc->computeRegI0, ScbeCpu::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitCmpRegImm(cc->computeRegI1, ScbeCpu::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitLoadCondRegReg(cc->computeRegI1, cc->computeRegI0, CpuCond::G, opBits);
        emitIMMA(pp, cc->computeRegI0, opBits);
        pp.emitOpBinaryRegReg(cc->computeRegI0, cc->computeRegI1, CpuOp::SAR, opBits);
    }

    pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI0, opBits);
}

void Scbe::emitShiftRightEqArithmetic(ScbeCpu& pp)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryMemImm(cc->computeRegI0, 0, ip->b.u32, CpuOp::SAR, opBits);
    }
    else
    {
        pp.emitLoadRegMem(cc->computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32);
        pp.emitLoadRegImm(cc->computeRegI0, ScbeCpu::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitCmpRegImm(cc->computeRegI1, ScbeCpu::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitLoadCondRegReg(cc->computeRegI1, cc->computeRegI0, CpuCond::G, opBits);
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryMemReg(cc->computeRegI0, 0, cc->computeRegI1, CpuOp::SAR, opBits);
    }
}

void Scbe::emitShiftLogical(ScbeCpu& pp, CpuOp op)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= ScbeCpu::getNumBits(opBits))
    {
        pp.emitLoadMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), 0, opBits);
    }
    else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitOpBinaryRegImm(cc->computeRegI0, ip->b.u8, op, opBits);
        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI0, opBits);
    }
    else
    {
        emitIMMA(pp, cc->computeRegI0, opBits);
        emitIMMB(pp, cc->computeRegI1, OpBits::B32);
        pp.emitOpBinaryRegReg(cc->computeRegI0, cc->computeRegI1, op, opBits);
        pp.emitClearReg(cc->computeRegI2, opBits);
        pp.emitCmpRegImm(cc->computeRegI1, ScbeCpu::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitLoadCondRegReg(cc->computeRegI0, cc->computeRegI2, CpuCond::G, opBits);
        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI0, opBits);
    }
}

void Scbe::emitShiftEqLogical(ScbeCpu& pp, CpuOp op)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= ScbeCpu::getNumBits(opBits))
    {
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitLoadMemImm(cc->computeRegI0, 0, 0, opBits);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryMemImm(cc->computeRegI0, 0, ip->b.u8, op, opBits);
    }
    else
    {
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitLoadRegMem(cc->computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32);
        pp.emitCmpRegImm(cc->computeRegI1, ScbeCpu::getNumBits(opBits), OpBits::B32);
        CpuJump jump;
        pp.emitJump(jump, CpuCondJump::JL, OpBits::B8);
        pp.emitClearReg(cc->computeRegI1, opBits);
        pp.emitLoadMemReg(cc->computeRegI0, 0, cc->computeRegI1, opBits);
        pp.emitPatchJump(jump);
        pp.emitOpBinaryMemReg(cc->computeRegI0, 0, cc->computeRegI1, op, opBits);
    }
}

void Scbe::emitOverflow(ScbeCpu& pp, const char* msg, bool isSigned)
{
    const auto ip = pp.ip;
    if (BackendEncoder::mustCheckOverflow(pp.buildParams.module, ip))
    {
        CpuJump jump;
        pp.emitJump(jump, isSigned ? CpuCondJump::JNO : CpuCondJump::JAE, OpBits::B8);
        emitInternalPanic(pp, msg);
        pp.emitPatchJump(jump);
    }
}

void Scbe::emitBinOp(ScbeCpu& pp, CpuOp op, CpuEmitFlags emitFlags)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto isInt  = ScbeCpu::isInt(ip->op);
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (isInt && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitOpBinaryRegImm(cc->computeRegI0, ip->b.u64, op, opBits, emitFlags);
        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI0, opBits);
    }
    else if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_B))
    {
        const auto r0 = isInt ? cc->computeRegI0 : cc->computeRegF0;
        pp.emitLoadRegMem(r0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitOpBinaryRegMem(r0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), op, opBits, emitFlags);
        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), r0, opBits);
    }
    else
    {
        const auto r0 = isInt ? cc->computeRegI0 : cc->computeRegF0;
        const auto r1 = isInt ? cc->computeRegI1 : cc->computeRegF1;
        emitIMMA(pp, r0, opBits);
        emitIMMB(pp, r1, opBits);
        pp.emitOpBinaryRegReg(r0, r1, op, opBits, emitFlags);
        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), r0, opBits);
    }
}

void Scbe::emitBinOpOverflow(ScbeCpu& pp, CpuOp op, SafetyMsg safetyMsg, TypeInfo* safetyType)
{
    const char* msg      = ByteCodeGen::safetyMsg(safetyMsg, safetyType);
    const bool  isSigned = safetyType->isNativeIntegerSigned();
    emitBinOp(pp, op, EMIT_Overflow);
    emitOverflow(pp, msg, isSigned);
}

void Scbe::emitBinOpEq(ScbeCpu& pp, uint32_t offset, CpuOp op, CpuEmitFlags emitFlags)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto isInt  = ScbeCpu::isInt(ip->op);
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (isInt && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryMemImm(cc->computeRegI0, offset, ip->b.u64, op, opBits, emitFlags);
    }
    else
    {
        const auto r0 = cc->computeRegI0;
        const auto r1 = isInt ? cc->computeRegI1 : cc->computeRegF0;
        pp.emitLoadRegMem(r0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        emitIMMB(pp, r1, opBits);
        pp.emitOpBinaryMemReg(r0, offset, r1, op, opBits, emitFlags);
    }
}

void Scbe::emitBinOpEqOverflow(ScbeCpu& pp, CpuOp op, SafetyMsg safetyMsg, TypeInfo* safetyType)
{
    const char* msg      = ByteCodeGen::safetyMsg(safetyMsg, safetyType);
    const bool  isSigned = safetyType->isNativeIntegerSigned();

    emitBinOpEq(pp, 0, op, EMIT_Overflow);
    emitOverflow(pp, msg, isSigned);
}

void Scbe::emitBinOpEqLock(ScbeCpu& pp, CpuOp op)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    pp.emitLoadRegMem(cc->computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
    emitIMMB(pp, cc->computeRegI0, opBits);
    pp.emitOpBinaryMemReg(cc->computeRegI1, 0, cc->computeRegI0, op, opBits, EMIT_Lock);
}

void Scbe::emitBinOpEqS(ScbeCpu& pp, CpuOp op)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto isInt  = ScbeCpu::isInt(ip->op);
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (isInt && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitOpBinaryMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetBC() + ip->a.u32, ip->b.u64, op, opBits);
    }
    else
    {
        const auto r1 = isInt ? cc->computeRegI1 : cc->computeRegF1;
        emitIMMB(pp, r1, opBits);
        pp.emitOpBinaryMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetBC() + ip->a.u32, r1, op, opBits);
    }
}

void Scbe::emitCompareOp(ScbeCpu& pp, CpuReg reg, CpuCond cond)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto isInt  = ScbeCpu::isInt(ip->op);
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_B))
    {
        const auto r0 = isInt ? cc->computeRegI0 : cc->computeRegF0;
        pp.emitLoadRegMem(r0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), opBits);
        pp.emitCmpMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), r0, opBits);
    }
    else if (isInt && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitCmpMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), ip->b.u64, opBits);
    }
    else
    {
        const auto r0 = isInt ? cc->computeRegI0 : cc->computeRegF0;
        const auto r1 = isInt ? cc->computeRegI1 : cc->computeRegF1;
        emitIMMA(pp, r0, opBits);
        emitIMMB(pp, r1, opBits);
        pp.emitCmpRegReg(r0, r1, opBits);
    }

    pp.emitSetCondReg(reg, cond);
}

void Scbe::emitAddSubMul64(ScbeCpu& pp, uint64_t mulValue, CpuOp op)
{
    SWAG_ASSERT(op == CpuOp::ADD || op == CpuOp::SUB);

    const auto cc    = pp.cc;
    const auto ip    = pp.ip;
    const auto value = ip->b.u64 * mulValue;
    if (ip->hasFlag(BCI_IMM_B) && value == 0 && ip->a.u32 == ip->c.u32)
        return;

    if (ip->hasFlag(BCI_IMM_B) && value == 0)
    {
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI0, OpBits::B64);
    }
    else if (ip->hasFlag(BCI_IMM_B) && ip->a.u32 == ip->c.u32)
    {
        pp.emitOpBinaryMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), value, op, OpBits::B64);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryRegImm(cc->computeRegI0, value, op, OpBits::B64);
        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI0, OpBits::B64);
    }
    else if (ip->a.u32 == ip->c.u32)
    {
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
        pp.emitOpBinaryRegImm(cc->computeRegI0, mulValue, CpuOp::IMUL, OpBits::B64);
        pp.emitOpBinaryMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc->computeRegI0, op, OpBits::B64);
    }
    else if (op == CpuOp::ADD)
    {
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
        pp.emitOpBinaryRegImm(cc->computeRegI0, mulValue, CpuOp::IMUL, OpBits::B64);
        pp.emitOpBinaryRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), op, OpBits::B64);
        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI0, OpBits::B64);
    }
    else
    {
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
        pp.emitOpBinaryRegImm(cc->computeRegI0, mulValue, CpuOp::IMUL, OpBits::B64);
        pp.emitLoadRegMem(cc->computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryRegReg(cc->computeRegI1, cc->computeRegI0, op, OpBits::B64);
        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI1, OpBits::B64);
    }
}

void Scbe::emitInternalPanic(ScbeCpu& pp, const char* msg)
{
    const auto node = pp.ip->node;
    const auto np   = node->token.sourceFile->path;

    pp.pushParams.clear();
    pp.pushParams.push_back({.type = CpuPushParamType::GlobalString, .value = reinterpret_cast<uint64_t>(np.cstr())});
    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = node->token.startLocation.line});
    pp.pushParams.push_back({.type = CpuPushParamType::Constant, .value = node->token.startLocation.column});
    pp.pushParams.push_back({.type = msg ? CpuPushParamType::GlobalString : CpuPushParamType::Constant, .value = reinterpret_cast<uint64_t>(msg)});
    emitInternalCallCPUParams(pp, g_LangSpec->name_priv_panic, pp.pushParams);
}

void Scbe::emitJumpCmp(ScbeCpu& pp, CpuCondJump op, OpBits opBits)
{
    const auto cc    = pp.cc;
    const auto ip    = pp.ip;
    const auto isInt = ScbeCpu::isInt(ip->op);
    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        const auto r0 = isInt ? cc->computeRegI0 : cc->computeRegF0;
        pp.emitLoadRegMem(r0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), opBits);
        pp.emitCmpMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), r0, opBits);
    }
    else if (isInt && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_C))
    {
        pp.emitCmpMemImm(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), ip->c.u64, opBits);
    }
    else
    {
        const auto r0 = isInt ? cc->computeRegI0 : cc->computeRegF0;
        const auto r1 = isInt ? cc->computeRegI1 : cc->computeRegF1;
        emitIMMA(pp, r0, opBits);
        emitIMMC(pp, r1, opBits);
        pp.emitCmpRegReg(r0, r1, opBits);
    }

    pp.emitJumpCondImm(op, pp.ipIndex + ip->b.s32 + 1);
}

void Scbe::emitJumpCmpAddr(ScbeCpu& pp, CpuCondJump op, CpuReg memReg, uint64_t memOffset, OpBits opBits)
{
    const auto cc = pp.cc;
    const auto ip = pp.ip;

    if (ip->hasFlag(BCI_IMM_C))
    {
        pp.emitCmpMemImm(memReg, memOffset, ip->c.u64, opBits);
    }
    else
    {
        pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), opBits);
        pp.emitCmpMemReg(memReg, memOffset, cc->computeRegI0, opBits);
    }

    pp.emitJumpCondImm(op, pp.ipIndex + ip->b.s32 + 1);
}

void Scbe::emitJumpCmp2(ScbeCpu& pp, CpuCondJump op1, CpuCondJump op2, OpBits opBits)
{
    const auto cc = pp.cc;
    const auto ip = pp.ip;

    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        pp.emitLoadRegMem(cc->computeRegF0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), opBits);
        pp.emitCmpMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc->computeRegF0, opBits);
    }
    else
    {
        emitIMMA(pp, cc->computeRegF0, opBits);
        emitIMMC(pp, cc->computeRegF1, opBits);
        pp.emitCmpRegReg(cc->computeRegF0, cc->computeRegF1, opBits);
    }

    pp.emitJumpCondImm(op1, pp.ipIndex + ip->b.s32 + 1);
    pp.emitJumpCondImm(op2, pp.ipIndex + ip->b.s32 + 1);
}

void Scbe::emitJumpCmp3(ScbeCpu& pp, CpuCondJump op1, CpuCondJump op2, OpBits opBits)
{
    const auto cc = pp.cc;
    const auto ip = pp.ip;

    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        pp.emitLoadRegMem(cc->computeRegF0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), opBits);
        pp.emitCmpMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc->computeRegF0, opBits);
    }
    else
    {
        emitIMMA(pp, cc->computeRegF0, opBits);
        emitIMMC(pp, cc->computeRegF1, opBits);
        pp.emitCmpRegReg(cc->computeRegF0, cc->computeRegF1, opBits);
    }

    pp.emitJumpCondImm(op1, pp.ipIndex + 1);
    pp.emitJumpCondImm(op2, pp.ipIndex + ip->b.s32 + 1);
}

void Scbe::emitJumpTable(ScbeCpu& pp)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);

    pp.emitLoadSignedExtendRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64, opBits);

    // Note:
    //
    // This is not optimal yet.
    // The sub could be removed by baking it in the 'cmp', and by changing the jump table address by subtracting the min value
    // Also, if the jump table was encoded in the text segment, then there will be no need to have two relocations
    //
    // We could in the end remove two instructions and be as the llvm generation

    pp.emitOpBinaryRegImm(cc->computeRegI0, ip->b.u64 - 1, CpuOp::SUB, OpBits::B64);
    pp.emitCmpRegImm(cc->computeRegI0, ip->c.u64, OpBits::B64);
    const auto tableCompiler = reinterpret_cast<int32_t*>(pp.buildParams.module->compilerSegment.address(ip->d.u32));
    pp.emitJumpCondImm(CpuCondJump::JAE, pp.ipIndex + tableCompiler[0] + 1);

    pp.emitJumpTable(cc->computeRegI1, cc->computeRegI0, pp.ipIndex, ip->d.u32, ip->c.u32);
}

void Scbe::emitCopyVaargs(ScbeCpu& pp)
{
    const auto cc = pp.cc;
    const auto ip = pp.ip;
    if (!pp.pushRVParams.empty())
    {
        const auto     sizeOf            = pp.pushRVParams[0].second;
        uint32_t       idxParam          = pp.pushRVParams.size() - 1;
        const uint32_t variadicStackSize = Math::align(idxParam * sizeOf, 16);
        uint32_t       offset            = pp.cpuFct->sizeStackCallParams - variadicStackSize;

        while (idxParam != UINT32_MAX)
        {
            const auto reg = pp.pushRVParams[idxParam].first;
            switch (sizeOf)
            {
                case 1:
                    pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(reg), OpBits::B8);
                    pp.emitLoadMemReg(CpuReg::Rsp, offset, cc->computeRegI0, OpBits::B8);
                    break;
                case 2:
                    pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(reg), OpBits::B16);
                    pp.emitLoadMemReg(CpuReg::Rsp, offset, cc->computeRegI0, OpBits::B16);
                    break;
                case 4:
                    pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(reg), OpBits::B32);
                    pp.emitLoadMemReg(CpuReg::Rsp, offset, cc->computeRegI0, OpBits::B32);
                    break;
                case 8:
                    pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(reg), OpBits::B64);
                    pp.emitLoadMemReg(CpuReg::Rsp, offset, cc->computeRegI0, OpBits::B64);
                    break;
                default:
                    break;
            }

            idxParam--;
            offset += sizeOf;
        }

        pp.emitLoadAddressMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->sizeStackCallParams - variadicStackSize);
        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc->computeRegI0, OpBits::B64);
    }
    else
    {
        // All of this is complicated. But ip->b.u32 has been reduced by one register in case of a closure, and
        // we have a dynamic test for bytecode execution. But for runtime, we put it back.
        auto sizeB = ip->b.u32;

        const auto typeFuncCall = castTypeInfo<TypeInfoFuncAttr>(reinterpret_cast<TypeInfo*>(ip->d.pointer), TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
        if (typeFuncCall->isClosure())
            sizeB += sizeof(Register);

        // We need to flatten all variadic registers, in order, in the stack, and emit the address of that array
        // We compute the number of variadic registers by removing registers of normal parameters (ip->b.u32)
        uint32_t       idxParam          = pp.pushRAParams.size() - sizeB / sizeof(Register) - 1;
        const uint32_t variadicStackSize = Math::align((idxParam + 1) * sizeof(Register), 16);
        uint32_t       offset            = pp.cpuFct->sizeStackCallParams - variadicStackSize;
        while (idxParam != UINT32_MAX)
        {
            pp.emitLoadRegMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(pp.pushRAParams[idxParam]), OpBits::B64);
            pp.emitLoadMemReg(CpuReg::Rsp, offset, cc->computeRegI0, OpBits::B64);
            idxParam--;
            offset += 8;
        }

        pp.emitLoadAddressMem(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->sizeStackCallParams - variadicStackSize);
        pp.emitLoadMemReg(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc->computeRegI0, OpBits::B64);
    }
}

void Scbe::emitClearMem(ScbeCpu& pp, CpuReg memReg, uint64_t memOffset, uint32_t count)
{
    const auto cc = pp.cc;
    if (count >= 16)
    {
        pp.emitClearReg(cc->computeRegF0, OpBits::B32);
        while (count >= 16)
        {
            pp.emitLoadMemReg(memReg, memOffset, pp.cc->computeRegF0, OpBits::B128);
            count -= 16;
            memOffset += 16;
        }
    }

    while (count >= 8)
    {
        pp.emitLoadMemImm(memReg, memOffset, 0, OpBits::B64);
        count -= 8;
        memOffset += 8;
    }

    while (count >= 4)
    {
        pp.emitLoadMemImm(memReg, memOffset, 0, OpBits::B32);
        count -= 4;
        memOffset += 4;
    }

    while (count >= 2)
    {
        pp.emitLoadMemImm(memReg, memOffset, 0, OpBits::B16);
        count -= 2;
        memOffset += 2;
    }

    while (count >= 1)
    {
        pp.emitLoadMemImm(memReg, memOffset, 0, OpBits::B8);
        count -= 1;
        memOffset += 1;
    }
}

void Scbe::emitCopyMem(ScbeCpu& pp, CpuReg memRegDst, CpuReg memRegSrc, uint32_t count)
{
    const auto cc     = pp.cc;
    uint32_t   offset = 0;

    while (count >= 16)
    {
        pp.emitLoadRegMem(cc->computeRegF0, memRegSrc, offset, OpBits::B128);
        pp.emitLoadMemReg(memRegDst, offset, cc->computeRegF0, OpBits::B128);
        count -= 16;
        offset += 16;
    }

    while (count >= 8)
    {
        pp.emitLoadRegMem(cc->computeRegI2, memRegSrc, offset, OpBits::B64);
        pp.emitLoadMemReg(memRegDst, offset, cc->computeRegI2, OpBits::B64);
        count -= 8;
        offset += 8;
    }

    while (count >= 4)
    {
        pp.emitLoadRegMem(cc->computeRegI2, memRegSrc, offset, OpBits::B32);
        pp.emitLoadMemReg(memRegDst, offset, cc->computeRegI2, OpBits::B32);
        count -= 4;
        offset += 4;
    }

    while (count >= 2)
    {
        pp.emitLoadRegMem(cc->computeRegI2, memRegSrc, offset, OpBits::B16);
        pp.emitLoadMemReg(memRegDst, offset, cc->computeRegI2, OpBits::B16);
        count -= 2;
        offset += 2;
    }

    while (count >= 1)
    {
        pp.emitLoadRegMem(cc->computeRegI2, memRegSrc, offset, OpBits::B8);
        pp.emitLoadMemReg(memRegDst, offset, cc->computeRegI2, OpBits::B8);
        count -= 1;
        offset += 1;
    }
}
