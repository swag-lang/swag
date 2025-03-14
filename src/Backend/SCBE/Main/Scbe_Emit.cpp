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
        pp.emitLoadRI(reg, ip->a.u64, opBits);
    else
        pp.emitLoadRM(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
}

void Scbe::emitIMMB(ScbeCpu& pp, CpuReg reg, OpBits opBits)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_B))
        pp.emitLoadRI(reg, ip->b.u64, opBits);
    else
        pp.emitLoadRM(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), opBits);
}

void Scbe::emitIMMC(ScbeCpu& pp, CpuReg reg, OpBits opBits)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_C))
        pp.emitLoadRI(reg, ip->c.u64, opBits);
    else
        pp.emitLoadRM(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), opBits);
}

void Scbe::emitIMMD(ScbeCpu& pp, CpuReg reg, OpBits opBits)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_D))
        pp.emitLoadRI(reg, ip->d.u64, opBits);
    else
        pp.emitLoadRM(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->d.u32), opBits);
}

void Scbe::emitIMMB(ScbeCpu& pp, CpuReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_B))
    {
        SWAG_ASSERT(numBitsDst == OpBits::B16 || numBitsDst == OpBits::B32);
        pp.emitLoadRI(reg, ip->b.u64, OpBits::B32);
    }
    else if (isSigned)
    {
        pp.emitLoadSignedExtendRM(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), numBitsDst, numBitsSrc);
    }
    else
    {
        pp.emitLoadZeroExtendRM(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), numBitsDst, numBitsSrc);
    }
}

void Scbe::emitIMMC(ScbeCpu& pp, CpuReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_C))
    {
        SWAG_ASSERT(numBitsDst == OpBits::B16 || numBitsDst == OpBits::B32);
        pp.emitLoadRI(reg, ip->c.u64, OpBits::B32);
    }
    else if (isSigned)
    {
        pp.emitLoadSignedExtendRM(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), numBitsDst, numBitsSrc);
    }
    else
    {
        pp.emitLoadZeroExtendRM(reg, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), numBitsDst, numBitsSrc);
    }
}

void Scbe::emitShiftRightArithmetic(ScbeCpu& pp)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitOpBinaryRI(cc->computeRegI0, ip->b.u32, CpuOp::SAR, opBits);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRI(cc->computeRegI1, std::min(static_cast<uint32_t>(ip->b.u8), ScbeCpu::getNumBits(opBits) - 1), OpBits::B8);
        emitIMMA(pp, cc->computeRegI0, opBits);
        pp.emitOpBinaryRR(cc->computeRegI0, cc->computeRegI1, CpuOp::SAR, opBits);
    }
    else
    {
        pp.emitLoadRM(cc->computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32);
        pp.emitLoadRI(cc->computeRegI0, ScbeCpu::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitCmpRI(cc->computeRegI1, ScbeCpu::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitOpBinaryRR(cc->computeRegI1, cc->computeRegI0, CpuOp::CMOVG, opBits);
        emitIMMA(pp, cc->computeRegI0, opBits);
        pp.emitOpBinaryRR(cc->computeRegI0, cc->computeRegI1, CpuOp::SAR, opBits);
    }

    pp.emitStoreMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI0, opBits);
}

void Scbe::emitShiftRightEqArithmetic(ScbeCpu& pp)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryMI(cc->computeRegI0, 0, ip->b.u32, CpuOp::SAR, opBits);
    }
    else
    {
        pp.emitLoadRM(cc->computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32);
        pp.emitLoadRI(cc->computeRegI0, ScbeCpu::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitCmpRI(cc->computeRegI1, ScbeCpu::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitOpBinaryRR(cc->computeRegI1, cc->computeRegI0, CpuOp::CMOVG, opBits);
        pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryMR(cc->computeRegI0, 0, cc->computeRegI1, CpuOp::SAR, opBits);
    }
}

void Scbe::emitShiftLogical(ScbeCpu& pp, CpuOp op)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= ScbeCpu::getNumBits(opBits))
    {
        pp.emitStoreMI(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), 0, opBits);
    }
    else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitOpBinaryRI(cc->computeRegI0, ip->b.u8, op, opBits);
        pp.emitStoreMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI0, opBits);
    }
    else
    {
        emitIMMA(pp, cc->computeRegI0, opBits);
        emitIMMB(pp, cc->computeRegI1, OpBits::B32);
        pp.emitOpBinaryRR(cc->computeRegI0, cc->computeRegI1, op, opBits);
        pp.emitClearR(CpuReg::R8, opBits);
        pp.emitCmpRI(cc->computeRegI1, ScbeCpu::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitOpBinaryRR(cc->computeRegI0, CpuReg::R8, CpuOp::CMOVG, opBits);
        pp.emitStoreMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI0, opBits);
    }
}

void Scbe::emitShiftEqLogical(ScbeCpu& pp, CpuOp op)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= ScbeCpu::getNumBits(opBits))
    {
        pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitStoreMI(cc->computeRegI0, 0, 0, opBits);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryMI(cc->computeRegI0, 0, ip->b.u8, op, opBits);
    }
    else
    {
        pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitLoadRM(cc->computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32);
        pp.emitCmpRI(cc->computeRegI1, ScbeCpu::getNumBits(opBits), OpBits::B32);
        const auto jump = pp.emitJump(CpuCondJump::JL, OpBits::B8);
        pp.emitClearR(cc->computeRegI1, opBits);
        pp.emitStoreMR(cc->computeRegI0, 0, cc->computeRegI1, opBits);
        pp.emitPatchJump(jump);
        pp.emitOpBinaryMR(cc->computeRegI0, 0, cc->computeRegI1, op, opBits);
    }
}

void Scbe::emitOverflow(ScbeCpu& pp, const char* msg, bool isSigned)
{
    const auto ip = pp.ip;
    if (BackendEncoder::mustCheckOverflow(pp.buildParams.module, ip))
    {
        const auto jump = pp.emitJump(isSigned ? CpuCondJump::JNO : CpuCondJump::JAE, OpBits::B8);
        emitInternalPanic(pp, msg);
        pp.emitPatchJump(jump);
    }
}

void Scbe::emitBinOp(ScbeCpu& pp, CpuOp op, CpuEmitFlags emitFlags)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (ScbeCpu::isInt(opBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitOpBinaryRI(cc->computeRegI0, ip->b.u64, op, opBits, emitFlags);
        pp.emitStoreMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI0, opBits);
    }
    else
    {
        const auto r0 = ScbeCpu::isInt(opBits) ? cc->computeRegI0 : cc->computeRegF0;
        const auto r1 = ScbeCpu::isInt(opBits) ? cc->computeRegI1 : cc->computeRegF1;
        if (ip->hasFlag(BCI_IMM_A))
            pp.emitLoadRI(r0, ip->a.u64, opBits);
        else
            pp.emitLoadRM(r0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        emitIMMB(pp, r1, opBits);
        pp.emitOpBinaryRR(r0, r1, op, opBits, emitFlags);
        pp.emitStoreMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), r0, opBits);
    }
}

void Scbe::emitBinOpOverflow(ScbeCpu& pp, CpuOp op, SafetyMsg safetyMsg, TypeInfo* safetyType)
{
    const char* msg      = ByteCodeGen::safetyMsg(safetyMsg, safetyType);
    const bool  isSigned = safetyType->isNativeIntegerSigned();
    SWAG_ASSERT(ScbeCpu::isInt(ScbeCpu::getOpBits(pp.ip->op)));

    emitBinOp(pp, op, EMITF_Overflow);
    emitOverflow(pp, msg, isSigned);
}

void Scbe::emitBinOpEq(ScbeCpu& pp, uint32_t offset, CpuOp op, CpuEmitFlags emitFlags)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (ScbeCpu::isInt(opBits) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryMI(cc->computeRegI0, offset, ip->b.u64, op, opBits, emitFlags);
    }
    else
    {
        const auto r0 = ScbeCpu::isInt(opBits) ? cc->computeRegI0 : cc->computeRegI1;
        const auto r1 = ScbeCpu::isInt(opBits) ? cc->computeRegI1 : cc->computeRegF1;
        pp.emitLoadRM(r0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        emitIMMB(pp, r1, opBits);
        pp.emitOpBinaryMR(r0, offset, r1, op, opBits, emitFlags);
    }
}

void Scbe::emitBinOpEqOverflow(ScbeCpu& pp, CpuOp op, SafetyMsg safetyMsg, TypeInfo* safetyType)
{
    const char* msg      = ByteCodeGen::safetyMsg(safetyMsg, safetyType);
    const bool  isSigned = safetyType->isNativeIntegerSigned();
    SWAG_ASSERT(ScbeCpu::isInt(ScbeCpu::getOpBits(pp.ip->op)));

    emitBinOpEq(pp, 0, op, EMITF_Overflow);
    emitOverflow(pp, msg, isSigned);
}

void Scbe::emitBinOpEqLock(ScbeCpu& pp, CpuOp op)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    pp.emitLoadRM(cc->computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
    emitIMMB(pp, cc->computeRegI0, opBits);
    pp.emitOpBinaryMR(cc->computeRegI1, 0, cc->computeRegI0, op, opBits, EMITF_Lock);
}

void Scbe::emitBinOpEqS(ScbeCpu& pp, CpuOp op)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (ScbeCpu::isInt(opBits) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitOpBinaryMI(CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, ip->b.u64, op, opBits);
    }
    else
    {
        const auto r1 = ScbeCpu::isInt(opBits) ? cc->computeRegI1 : cc->computeRegF1;
        emitIMMB(pp, r1, opBits);
        pp.emitOpBinaryMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, r1, op, opBits);
    }
}

void Scbe::emitCompareOp(ScbeCpu& pp, CpuReg reg, CpuCondFlag cond)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);
    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_B))
    {
        const auto r0 = ScbeCpu::isInt(opBits) ? cc->computeRegI0 : cc->computeRegF0;
        pp.emitLoadRM(r0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitCmpMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), r0, opBits);
    }
    else if (ScbeCpu::isInt(opBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitCmpMI(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), ip->b.u64, opBits);
    }
    else
    {
        const auto r0 = ScbeCpu::isInt(opBits) ? cc->computeRegI0 : cc->computeRegF0;
        const auto r1 = ScbeCpu::isInt(opBits) ? cc->computeRegI1 : cc->computeRegF1;
        emitIMMA(pp, r0, opBits);
        emitIMMB(pp, r1, opBits);
        pp.emitCmpRR(r0, r1, opBits);
    }

    pp.emitSetCC(reg, cond);
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
        pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitStoreMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI0, OpBits::B64);
    }
    else if (ip->hasFlag(BCI_IMM_B) && ip->a.u32 == ip->c.u32)
    {
        pp.emitOpBinaryMI(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), value, op, OpBits::B64);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryRI(cc->computeRegI0, value, op, OpBits::B64);
        pp.emitStoreMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI0, OpBits::B64);
    }
    else if (ip->a.u32 == ip->c.u32)
    {
        pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
        pp.emitOpBinaryRI(cc->computeRegI0, mulValue, CpuOp::IMUL, OpBits::B64);
        pp.emitOpBinaryMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc->computeRegI0, op, OpBits::B64);
    }
    else if (op == CpuOp::ADD)
    {
        pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
        pp.emitOpBinaryRI(cc->computeRegI0, mulValue, CpuOp::IMUL, OpBits::B64);
        pp.emitOpBinaryRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), op, OpBits::B64);
        pp.emitStoreMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI0, OpBits::B64);
    }
    else
    {
        pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
        pp.emitOpBinaryRI(cc->computeRegI0, mulValue, CpuOp::IMUL, OpBits::B64);
        pp.emitLoadRM(cc->computeRegI1, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryRR(cc->computeRegI1, cc->computeRegI0, op, OpBits::B64);
        pp.emitStoreMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI1, OpBits::B64);
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
    const auto cc = pp.cc;
    const auto ip = pp.ip;
    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        const auto r0 = ScbeCpu::isInt(opBits) ? cc->computeRegI0 : cc->computeRegF0;
        pp.emitLoadRM(r0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitCmpMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), r0, opBits);
    }
    else if (ScbeCpu::isInt(opBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_C))
    {
        pp.emitCmpMI(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), ip->c.u64, opBits);
    }
    else
    {
        const auto r0 = ScbeCpu::isInt(opBits) ? cc->computeRegI0 : cc->computeRegF0;
        const auto r1 = ScbeCpu::isInt(opBits) ? cc->computeRegI1 : cc->computeRegF1;
        emitIMMA(pp, r0, opBits);
        emitIMMC(pp, r1, opBits);
        pp.emitCmpRR(r0, r1, opBits);
    }

    pp.emitJumpCI(op, pp.ipIndex + ip->b.s32 + 1);
}

void Scbe::emitJumpCmpAddr(ScbeCpu& pp, CpuCondJump op, CpuReg memReg, uint64_t memOffset, OpBits opBits)
{
    const auto cc = pp.cc;
    const auto ip = pp.ip;
    SWAG_ASSERT(ScbeCpu::isInt(opBits));

    if (ip->hasFlag(BCI_IMM_C))
    {
        pp.emitCmpMI(memReg, memOffset, ip->c.u64, opBits);
    }
    else
    {
        pp.emitLoadRM(cc->computeRegI0, memReg, memOffset, opBits);
        pp.emitCmpMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegI0, opBits);
    }

    pp.emitJumpCI(op, pp.ipIndex + ip->b.s32 + 1);
}

void Scbe::emitJumpCmp2(ScbeCpu& pp, CpuCondJump op1, CpuCondJump op2, OpBits opBits)
{
    const auto cc = pp.cc;
    const auto ip = pp.ip;
    SWAG_ASSERT(ScbeCpu::isFloat(opBits));

    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        pp.emitLoadRM(cc->computeRegF0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitCmpMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegF0, opBits);
    }
    else
    {
        emitIMMA(pp, cc->computeRegF0, opBits);
        emitIMMC(pp, cc->computeRegF1, opBits);
        pp.emitCmpRR(cc->computeRegF0, cc->computeRegF1, opBits);
    }

    pp.emitJumpCI(op1, pp.ipIndex + ip->b.s32 + 1);
    pp.emitJumpCI(op2, pp.ipIndex + ip->b.s32 + 1);
}

void Scbe::emitJumpCmp3(ScbeCpu& pp, CpuCondJump op1, CpuCondJump op2, OpBits opBits)
{
    const auto cc = pp.cc;
    const auto ip = pp.ip;
    SWAG_ASSERT(ScbeCpu::isFloat(opBits));

    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        pp.emitLoadRM(cc->computeRegF0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitCmpMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->c.u32), cc->computeRegF0, opBits);
    }
    else
    {
        emitIMMA(pp, cc->computeRegF0, opBits);
        emitIMMC(pp, cc->computeRegF1, opBits);
        pp.emitCmpRR(cc->computeRegF0, cc->computeRegF1, opBits);
    }

    pp.emitJumpCI(op1, pp.ipIndex + 1);
    pp.emitJumpCI(op2, pp.ipIndex + ip->b.s32 + 1);
}

void Scbe::emitJumpDyn(ScbeCpu& pp)
{
    const auto cc     = pp.cc;
    const auto ip     = pp.ip;
    const auto opBits = ScbeCpu::getOpBits(ip->op);

    pp.emitLoadSignedExtendRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64, opBits);

    // Note:
    //
    // This is not optimal yet.
    // The sub could be removed by baking it in the 'cmp', and by changing the jump table address by subtracting the min value
    // Also, if the jump table was encoded in the text segment, then there will be no need to have two relocations
    //
    // We could in the end remove two instructions and be as the llvm generation

    pp.emitOpBinaryRI(cc->computeRegI0, ip->b.u64 - 1, CpuOp::SUB, OpBits::B64);
    pp.emitCmpRI(cc->computeRegI0, ip->c.u64, OpBits::B64);
    const auto tableCompiler = reinterpret_cast<int32_t*>(pp.buildParams.module->compilerSegment.address(ip->d.u32));
    pp.emitJumpCI(CpuCondJump::JAE, pp.ipIndex + tableCompiler[0] + 1);

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
                    pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(reg), OpBits::B8);
                    pp.emitStoreMR(CpuReg::Rsp, offset, cc->computeRegI0, OpBits::B8);
                    break;
                case 2:
                    pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(reg), OpBits::B16);
                    pp.emitStoreMR(CpuReg::Rsp, offset, cc->computeRegI0, OpBits::B16);
                    break;
                case 4:
                    pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(reg), OpBits::B32);
                    pp.emitStoreMR(CpuReg::Rsp, offset, cc->computeRegI0, OpBits::B32);
                    break;
                case 8:
                    pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(reg), OpBits::B64);
                    pp.emitStoreMR(CpuReg::Rsp, offset, cc->computeRegI0, OpBits::B64);
                    break;
                default:
                    break;
            }

            idxParam--;
            offset += sizeOf;
        }

        pp.emitLoadAddressM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->sizeStackCallParams - variadicStackSize);
        pp.emitStoreMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc->computeRegI0, OpBits::B64);
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
            pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(pp.pushRAParams[idxParam]), OpBits::B64);
            pp.emitStoreMR(CpuReg::Rsp, offset, cc->computeRegI0, OpBits::B64);
            idxParam--;
            offset += 8;
        }

        pp.emitLoadAddressM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->sizeStackCallParams - variadicStackSize);
        pp.emitStoreMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(ip->a.u32), cc->computeRegI0, OpBits::B64);
    }
}

void Scbe::emitMakeLambda(ScbeCpu& pp)
{
    const auto cc       = pp.cc;
    const auto funcNode = castAst<AstFuncDecl>(reinterpret_cast<AstNode*>(pp.ip->b.pointer), AstNodeKind::FuncDecl);
    SWAG_ASSERT(!pp.ip->c.pointer || (funcNode && funcNode->hasExtByteCode() && funcNode->extByteCode()->bc == reinterpret_cast<ByteCode*>(pp.ip->c.pointer)));
    pp.emitSymbolRelocationPtr(cc->computeRegI0, funcNode->getCallName());
    pp.emitStoreMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(pp.ip->a.u32), cc->computeRegI0, OpBits::B64);
}

void Scbe::emitMakeCallback(ScbeCpu& pp)
{
    const auto cc = pp.cc;

    // Test if it's a bytecode lambda
    pp.emitLoadRM(cc->computeRegI0, CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(pp.ip->a.u32), OpBits::B64);
    pp.emitLoadRI(cc->computeRegI1, SWAG_LAMBDA_BC_MARKER, OpBits::B64);
    pp.emitOpBinaryRR(cc->computeRegI1, cc->computeRegI0, CpuOp::AND, OpBits::B64);

    const auto jump = pp.emitJump(CpuCondJump::JZ, OpBits::B32);

    // ByteCode lambda
    //////////////////

    pp.emitLoadRR(cc->computeRegI1, cc->computeRegI0, OpBits::B64);
    pp.emitSymbolRelocationAddress(cc->computeRegI0, pp.symPI_makeCallback, 0);
    pp.emitLoadRM(cc->computeRegI0, cc->computeRegI0, 0, OpBits::B64);
    pp.emitCallIndirect(cc->computeRegI0);

    // End
    //////////////////
    pp.emitPatchJump(jump);
    pp.emitStoreMR(CpuReg::Rsp, pp.cpuFct->getStackOffsetReg(pp.ip->a.u32), cc->computeRegI0, OpBits::B64);
}
