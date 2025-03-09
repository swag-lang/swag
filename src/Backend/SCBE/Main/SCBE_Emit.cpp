#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/ByteCode_Math.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

void SCBE::emitIMMA(SCBECPU& pp, CPUReg reg, OpBits opBits)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_A))
        pp.emitLoadRI(reg, ip->a.u64, opBits);
    else
        pp.emitLoadRM(reg, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
}

void SCBE::emitIMMB(SCBECPU& pp, CPUReg reg, OpBits opBits)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_B))
        pp.emitLoadRI(reg, ip->b.u64, opBits);
    else
        pp.emitLoadRM(reg, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->b.u32), opBits);
}

void SCBE::emitIMMC(SCBECPU& pp, CPUReg reg, OpBits opBits)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_C))
        pp.emitLoadRI(reg, ip->c.u64, opBits);
    else
        pp.emitLoadRM(reg, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), opBits);
}

void SCBE::emitIMMD(SCBECPU& pp, CPUReg reg, OpBits opBits)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_D))
        pp.emitLoadRI(reg, ip->d.u64, opBits);
    else
        pp.emitLoadRM(reg, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->d.u32), opBits);
}

void SCBE::emitIMMB(SCBECPU& pp, CPUReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_B))
    {
        SWAG_ASSERT(numBitsDst == OpBits::B16 || numBitsDst == OpBits::B32);
        pp.emitLoadRI(reg, ip->b.u64, OpBits::B32);
    }
    else if (isSigned)
    {
        pp.emitLoadSignedExtendRM(reg, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->b.u32), numBitsDst, numBitsSrc);
    }
    else
    {
        pp.emitLoadZeroExtendRM(reg, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->b.u32), numBitsDst, numBitsSrc);
    }
}

void SCBE::emitIMMC(SCBECPU& pp, CPUReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_C))
    {
        SWAG_ASSERT(numBitsDst == OpBits::B16 || numBitsDst == OpBits::B32);
        pp.emitLoadRI(reg, ip->c.u64, OpBits::B32);
    }
    else if (isSigned)
    {
        pp.emitLoadSignedExtendRM(reg, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), numBitsDst, numBitsSrc);
    }
    else
    {
        pp.emitLoadZeroExtendRM(reg, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), numBitsDst, numBitsSrc);
    }
}

void SCBE::emitShiftRightArithmetic(SCBECPU& pp)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBECPU::getOpBits(ip->op);
    if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitOpBinaryRI(CPUReg::RAX, ip->b.u32, CPUOp::SAR, opBits);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRI(CPUReg::RCX, std::min(static_cast<uint32_t>(ip->b.u8), SCBECPU::getNumBits(opBits) - 1), OpBits::B8);
        emitIMMA(pp, CPUReg::RAX, opBits);
        pp.emitOpBinaryRR(CPUReg::RAX, CPUReg::RCX, CPUOp::SAR, opBits);
    }
    else
    {
        pp.emitLoadRM(CPUReg::RCX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32);
        pp.emitLoadRI(CPUReg::RAX, SCBECPU::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitCmpRI(CPUReg::RCX, SCBECPU::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitOpBinaryRR(CPUReg::RCX, CPUReg::RAX, CPUOp::CMOVG, opBits);
        emitIMMA(pp, CPUReg::RAX, opBits);
        pp.emitOpBinaryRR(CPUReg::RAX, CPUReg::RCX, CPUOp::SAR, opBits);
    }

    pp.emitStoreMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), CPUReg::RAX, opBits);
}

void SCBE::emitShiftRightEqArithmetic(SCBECPU& pp)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBECPU::getOpBits(ip->op);
    if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryMI(CPUReg::RAX, 0, ip->b.u32, CPUOp::SAR, opBits);
    }
    else
    {
        pp.emitLoadRM(CPUReg::RCX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32);
        pp.emitLoadRI(CPUReg::RAX, SCBECPU::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitCmpRI(CPUReg::RCX, SCBECPU::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitOpBinaryRR(CPUReg::RCX, CPUReg::RAX, CPUOp::CMOVG, opBits);
        pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryMR(CPUReg::RAX, 0, CPUReg::RCX, CPUOp::SAR, opBits);
    }
}

void SCBE::emitShiftLogical(SCBECPU& pp, CPUOp op)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBECPU::getOpBits(ip->op);
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= SCBECPU::getNumBits(opBits))
    {
        pp.emitStoreMI(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), 0, opBits);
    }
    else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitOpBinaryRI(CPUReg::RAX, ip->b.u8, op, opBits);
        pp.emitStoreMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), CPUReg::RAX, opBits);
    }
    else
    {
        emitIMMA(pp, CPUReg::RAX, opBits);
        emitIMMB(pp, CPUReg::RCX, OpBits::B32);
        pp.emitOpBinaryRR(CPUReg::RAX, CPUReg::RCX, op, opBits);
        pp.emitClearR(CPUReg::R8, opBits);
        pp.emitCmpRI(CPUReg::RCX, SCBECPU::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitOpBinaryRR(CPUReg::RAX, CPUReg::R8, CPUOp::CMOVG, opBits);
        pp.emitStoreMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), CPUReg::RAX, opBits);
    }
}

void SCBE::emitShiftEqLogical(SCBECPU& pp, CPUOp op)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBECPU::getOpBits(ip->op);
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= SCBECPU::getNumBits(opBits))
    {
        pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitStoreMI(CPUReg::RAX, 0, 0, opBits);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryMI(CPUReg::RAX, 0, ip->b.u8, op, opBits);
    }
    else
    {
        pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitLoadRM(CPUReg::RCX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B32);
        pp.emitCmpRI(CPUReg::RCX, SCBECPU::getNumBits(opBits), OpBits::B32);
        const auto jump = pp.emitJump(JL, OpBits::B8);
        pp.emitClearR(CPUReg::RCX, opBits);
        pp.emitStoreMR(CPUReg::RAX, 0, CPUReg::RCX, opBits);
        pp.emitPatchJump(jump);
        pp.emitOpBinaryMR(CPUReg::RAX, 0, CPUReg::RCX, op, opBits);
    }
}

void SCBE::emitOverflow(SCBECPU& pp, const char* msg, bool isSigned)
{
    const auto ip = pp.ip;
    if (BackendEncoder::mustCheckOverflow(pp.buildParams.module, ip))
    {
        const auto jump = pp.emitJump(isSigned ? JNO : JAE, OpBits::B8);
        emitInternalPanic(pp, msg);
        pp.emitPatchJump(jump);
    }
}

void SCBE::emitBinOp(SCBECPU& pp, CPUOp op, CPUEmitFlags emitFlags)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBECPU::getOpBits(ip->op);
    if (SCBECPU::isInt(opBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitOpBinaryRI(CPUReg::RAX, ip->b.u64, op, opBits, emitFlags);
        pp.emitStoreMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), CPUReg::RAX, opBits);
    }
    else
    {
        const auto r0 = SCBECPU::isInt(opBits) ? CPUReg::RAX : CPUReg::XMM0;
        const auto r1 = SCBECPU::isInt(opBits) ? CPUReg::RCX : CPUReg::XMM1;
        if (ip->hasFlag(BCI_IMM_A))
            pp.emitLoadRI(r0, ip->a.u64, opBits);
        else
            pp.emitLoadRM(r0, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        emitIMMB(pp, r1, opBits);
        pp.emitOpBinaryRR(r0, r1, op, opBits, emitFlags);
        pp.emitStoreMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), r0, opBits);
    }
}

void SCBE::emitBinOpOverflow(SCBECPU& pp, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType)
{
    const char* msg      = ByteCodeGen::safetyMsg(safetyMsg, safetyType);
    const bool  isSigned = safetyType->isNativeIntegerSigned();
    SWAG_ASSERT(SCBECPU::isInt(SCBECPU::getOpBits(pp.ip->op)));

    emitBinOp(pp, op, EMITF_Overflow);
    emitOverflow(pp, msg, isSigned);
}

void SCBE::emitBinOpEq(SCBECPU& pp, uint32_t offset, CPUOp op, CPUEmitFlags emitFlags)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBECPU::getOpBits(ip->op);
    if (SCBECPU::isInt(opBits) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryMI(CPUReg::RAX, offset, ip->b.u64, op, opBits, emitFlags);
    }
    else
    {
        const auto r0 = SCBECPU::isInt(opBits) ? CPUReg::RAX : CPUReg::RCX;
        const auto r1 = SCBECPU::isInt(opBits) ? CPUReg::RCX : CPUReg::XMM1;
        pp.emitLoadRM(r0, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        emitIMMB(pp, r1, opBits);
        pp.emitOpBinaryMR(r0, offset, r1, op, opBits, emitFlags);
    }
}

void SCBE::emitBinOpEqOverflow(SCBECPU& pp, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType)
{
    const char* msg      = ByteCodeGen::safetyMsg(safetyMsg, safetyType);
    const bool  isSigned = safetyType->isNativeIntegerSigned();
    SWAG_ASSERT(SCBECPU::isInt(SCBECPU::getOpBits(pp.ip->op)));

    emitBinOpEq(pp, 0, op, EMITF_Overflow);
    emitOverflow(pp, msg, isSigned);
}

void SCBE::emitBinOpEqLock(SCBECPU& pp, CPUOp op)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBECPU::getOpBits(ip->op);
    pp.emitLoadRM(CPUReg::RCX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
    emitIMMB(pp, CPUReg::RAX, opBits);
    pp.emitOpBinaryMR(CPUReg::RCX, 0, CPUReg::RAX, op, opBits, EMITF_Lock);
}

void SCBE::emitBinOpEqS(SCBECPU& pp, CPUOp op)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBECPU::getOpBits(ip->op);
    if (SCBECPU::isInt(opBits) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitOpBinaryMI(CPUReg::RSP, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, ip->b.u64, op, opBits);
    }
    else
    {
        const auto r1 = SCBECPU::isInt(opBits) ? CPUReg::RCX : CPUReg::XMM1;
        emitIMMB(pp, r1, opBits);
        pp.emitOpBinaryMR(CPUReg::RSP, pp.cpuFct->getStackOffsetBCStack() + ip->a.u32, r1, op, opBits);
    }
}

void SCBE::emitCompareOp(SCBECPU& pp, CPUReg reg, CPUCondFlag cond)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBECPU::getOpBits(ip->op);
    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_B))
    {
        const auto r0 = SCBECPU::isInt(opBits) ? CPUReg::RAX : CPUReg::XMM0;
        pp.emitLoadRM(r0, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitCmpMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->b.u32), r0, opBits);
    }
    else if (SCBECPU::isInt(opBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitCmpMI(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), ip->b.u64, opBits);
    }
    else
    {
        const auto r0 = SCBECPU::isInt(opBits) ? CPUReg::RAX : CPUReg::XMM0;
        const auto r1 = SCBECPU::isInt(opBits) ? CPUReg::RCX : CPUReg::XMM1;
        emitIMMA(pp, r0, opBits);
        emitIMMB(pp, r1, opBits);
        pp.emitCmpRR(r0, r1, opBits);
    }

    pp.emitSetCC(reg, cond);
}

void SCBE::emitAddSubMul64(SCBECPU& pp, uint64_t mulValue, CPUOp op)
{
    SWAG_ASSERT(op == CPUOp::ADD || op == CPUOp::SUB);

    const auto ip    = pp.ip;
    const auto value = ip->b.u64 * mulValue;
    if (ip->hasFlag(BCI_IMM_B) && value == 0 && ip->a.u32 == ip->c.u32)
        return;

    if (ip->hasFlag(BCI_IMM_B) && value == 0)
    {
        pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitStoreMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), CPUReg::RAX, OpBits::B64);
    }
    else if (ip->hasFlag(BCI_IMM_B) && ip->a.u32 == ip->c.u32)
    {
        pp.emitOpBinaryMI(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), value, op, OpBits::B64);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryRI(CPUReg::RAX, value, op, OpBits::B64);
        pp.emitStoreMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), CPUReg::RAX, OpBits::B64);
    }
    else if (ip->a.u32 == ip->c.u32)
    {
        pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
        pp.emitOpBinaryRI(CPUReg::RAX, mulValue, CPUOp::IMUL, OpBits::B64);
        pp.emitOpBinaryMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), CPUReg::RAX, op, OpBits::B64);
    }
    else if (op == CPUOp::ADD)
    {
        pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
        pp.emitOpBinaryRI(CPUReg::RAX, mulValue, CPUOp::IMUL, OpBits::B64);
        pp.emitOpBinaryRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), op, OpBits::B64);
        pp.emitStoreMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), CPUReg::RAX, OpBits::B64);
    }
    else
    {
        pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->b.u32), OpBits::B64);
        pp.emitOpBinaryRI(CPUReg::RAX, mulValue, CPUOp::IMUL, OpBits::B64);
        pp.emitLoadRM(CPUReg::RCX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64);
        pp.emitOpBinaryRR(CPUReg::RCX, CPUReg::RAX, op, OpBits::B64);
        pp.emitStoreMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), CPUReg::RCX, OpBits::B64);
    }
}

void SCBE::emitInternalPanic(SCBECPU& pp, const char* msg)
{
    const auto node = pp.ip->node;
    const auto np   = node->token.sourceFile->path;

    pp.pushParams.clear();
    pp.pushParams.push_back({.type = CPUPushParamType::GlobalString, .value = reinterpret_cast<uint64_t>(np.cstr())});
    pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = node->token.startLocation.line});
    pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = node->token.startLocation.column});
    pp.pushParams.push_back({.type = msg ? CPUPushParamType::GlobalString : CPUPushParamType::Constant, .value = reinterpret_cast<uint64_t>(msg)});
    emitInternalCallCPUParams(pp, g_LangSpec->name_priv_panic, pp.pushParams);
}

void SCBE::emitJumpCmp(SCBECPU& pp, CPUCondJump op, OpBits opBits)
{
    const auto ip = pp.ip;
    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        const auto r0 = SCBECPU::isInt(opBits) ? CPUReg::RAX : CPUReg::XMM0;
        pp.emitLoadRM(r0, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitCmpMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), r0, opBits);
    }
    else if (SCBECPU::isInt(opBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_C))
    {
        pp.emitCmpMI(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), ip->c.u64, opBits);
    }
    else
    {
        const auto r0 = SCBECPU::isInt(opBits) ? CPUReg::RAX : CPUReg::XMM0;
        const auto r1 = SCBECPU::isInt(opBits) ? CPUReg::RCX : CPUReg::XMM1;
        emitIMMA(pp, r0, opBits);
        emitIMMC(pp, r1, opBits);
        pp.emitCmpRR(r0, r1, opBits);
    }

    pp.emitJumpCI(op, pp.ipIndex + ip->b.s32 + 1);
}

void SCBE::emitJumpCmpAddr(SCBECPU& pp, CPUCondJump op, CPUReg memReg, uint64_t memOffset, OpBits opBits)
{
    const auto ip = pp.ip;
    SWAG_ASSERT(SCBECPU::isInt(opBits));

    if (ip->hasFlag(BCI_IMM_C))
    {
        pp.emitCmpMI(memReg, memOffset, ip->c.u64, opBits);
    }
    else
    {
        pp.emitLoadRM(CPUReg::RAX, memReg, memOffset, opBits);
        pp.emitCmpMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), CPUReg::RAX, opBits);
    }

    pp.emitJumpCI(op, pp.ipIndex + ip->b.s32 + 1);
}

void SCBE::emitJumpCmp2(SCBECPU& pp, CPUCondJump op1, CPUCondJump op2, OpBits opBits)
{
    const auto ip = pp.ip;
    SWAG_ASSERT(SCBECPU::isFloat(opBits));

    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        pp.emitLoadRM(CPUReg::XMM0, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitCmpMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), CPUReg::XMM0, opBits);
    }
    else
    {
        emitIMMA(pp, CPUReg::XMM0, opBits);
        emitIMMC(pp, CPUReg::XMM1, opBits);
        pp.emitCmpRR(CPUReg::XMM0, CPUReg::XMM1, opBits);
    }

    pp.emitJumpCI(op1, pp.ipIndex + ip->b.s32 + 1);
    pp.emitJumpCI(op2, pp.ipIndex + ip->b.s32 + 1);
}

void SCBE::emitJumpCmp3(SCBECPU& pp, CPUCondJump op1, CPUCondJump op2, OpBits opBits)
{
    const auto ip = pp.ip;
    SWAG_ASSERT(SCBECPU::isFloat(opBits));

    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        pp.emitLoadRM(CPUReg::XMM0, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), opBits);
        pp.emitCmpMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->c.u32), CPUReg::XMM0, opBits);
    }
    else
    {
        emitIMMA(pp, CPUReg::XMM0, opBits);
        emitIMMC(pp, CPUReg::XMM1, opBits);
        pp.emitCmpRR(CPUReg::XMM0, CPUReg::XMM1, opBits);
    }

    pp.emitJumpCI(op1, pp.ipIndex + 1);
    pp.emitJumpCI(op2, pp.ipIndex + ip->b.s32 + 1);
}

void SCBE::emitJumpDyn(SCBECPU& pp)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBECPU::getOpBits(ip->op);

    pp.emitLoadSignedExtendRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), OpBits::B64, opBits);

    // Note:
    //
    // This is not optimal yet.
    // The sub could be removed by baking it in the 'cmp', and by changing the jump table address by subtracting the min value
    // Also, if the jump table was encoded in the text segment, then there will be no need to have two relocations
    //
    // We could in the end remove two instructions and be as the llvm generation

    pp.emitOpBinaryRI(CPUReg::RAX, ip->b.u64 - 1, CPUOp::SUB, OpBits::B64);
    pp.emitCmpRI(CPUReg::RAX, ip->c.u64, OpBits::B64);
    const auto tableCompiler = reinterpret_cast<int32_t*>(pp.buildParams.module->compilerSegment.address(ip->d.u32));
    pp.emitJumpCI(JAE, pp.ipIndex + tableCompiler[0] + 1);

    pp.emitJumpTable(CPUReg::RCX, CPUReg::RAX, pp.ipIndex, ip->d.u32, ip->c.u32);
}

void SCBE::emitCopyVaargs(SCBECPU& pp)
{
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
                    pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(reg), OpBits::B8);
                    pp.emitStoreMR(CPUReg::RSP, offset, CPUReg::RAX, OpBits::B8);
                    break;
                case 2:
                    pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(reg), OpBits::B16);
                    pp.emitStoreMR(CPUReg::RSP, offset, CPUReg::RAX, OpBits::B16);
                    break;
                case 4:
                    pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(reg), OpBits::B32);
                    pp.emitStoreMR(CPUReg::RSP, offset, CPUReg::RAX, OpBits::B32);
                    break;
                case 8:
                    pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(reg), OpBits::B64);
                    pp.emitStoreMR(CPUReg::RSP, offset, CPUReg::RAX, OpBits::B64);
                    break;
                default:
                    break;
            }

            idxParam--;
            offset += sizeOf;
        }

        pp.emitLoadAddressM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->sizeStackCallParams - variadicStackSize);
        pp.emitStoreMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), CPUReg::RAX, OpBits::B64);
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
            pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(pp.pushRAParams[idxParam]), OpBits::B64);
            pp.emitStoreMR(CPUReg::RSP, offset, CPUReg::RAX, OpBits::B64);
            idxParam--;
            offset += 8;
        }

        pp.emitLoadAddressM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->sizeStackCallParams - variadicStackSize);
        pp.emitStoreMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(ip->a.u32), CPUReg::RAX, OpBits::B64);
    }
}

void SCBE::emitMakeLambda(SCBECPU& pp)
{
    const auto funcNode = castAst<AstFuncDecl>(reinterpret_cast<AstNode*>(pp.ip->b.pointer), AstNodeKind::FuncDecl);
    SWAG_ASSERT(!pp.ip->c.pointer || (funcNode && funcNode->hasExtByteCode() && funcNode->extByteCode()->bc == reinterpret_cast<ByteCode*>(pp.ip->c.pointer)));
    pp.emitSymbolRelocationPtr(CPUReg::RAX, funcNode->getCallName());
    pp.emitStoreMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(pp.ip->a.u32), CPUReg::RAX, OpBits::B64);
}

void SCBE::emitMakeCallback(SCBECPU& pp)
{
    // Test if it's a bytecode lambda
    pp.emitLoadRM(CPUReg::RAX, CPUReg::RSP, pp.cpuFct->getStackOffsetReg(pp.ip->a.u32), OpBits::B64);
    pp.emitLoadRI(CPUReg::RCX, SWAG_LAMBDA_BC_MARKER, OpBits::B64);
    pp.emitOpBinaryRR(CPUReg::RCX, CPUReg::RAX, CPUOp::AND, OpBits::B64);

    const auto jump = pp.emitJump(JZ, OpBits::B32);

    // ByteCode lambda
    //////////////////

    pp.emitLoadRR(CPUReg::RCX, CPUReg::RAX, OpBits::B64);
    pp.emitSymbolRelocationAddress(CPUReg::RAX, pp.symPI_makeCallback, 0);
    pp.emitLoadRM(CPUReg::RAX, CPUReg::RAX, 0, OpBits::B64);
    pp.emitCallIndirect(CPUReg::RAX);

    // End
    //////////////////
    pp.emitPatchJump(jump);
    pp.emitStoreMR(CPUReg::RSP, pp.cpuFct->getStackOffsetReg(pp.ip->a.u32), CPUReg::RAX, OpBits::B64);
}
