#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/ByteCode_Math.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

void SCBE::emitIMMA(SCBE_CPU& pp, CPUReg reg, OpBits opBits)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_A))
        pp.emitLoad(reg, ip->a.u64, opBits);
    else
        pp.emitLoad(reg, CPUReg::RDI, REG_OFFSET(ip->a.u32), opBits);
}

void SCBE::emitIMMB(SCBE_CPU& pp, CPUReg reg, OpBits opBits)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_B))
        pp.emitLoad(reg, ip->b.u64, opBits);
    else
        pp.emitLoad(reg, CPUReg::RDI, REG_OFFSET(ip->b.u32), opBits);
}

void SCBE::emitIMMC(SCBE_CPU& pp, CPUReg reg, OpBits opBits)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_C))
        pp.emitLoad(reg, ip->c.u64, opBits);
    else
        pp.emitLoad(reg, CPUReg::RDI, REG_OFFSET(ip->c.u32), opBits);
}

void SCBE::emitIMMD(SCBE_CPU& pp, CPUReg reg, OpBits opBits)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_D))
        pp.emitLoad(reg, ip->d.u64, opBits);
    else
        pp.emitLoad(reg, CPUReg::RDI, REG_OFFSET(ip->d.u32), opBits);
}

void SCBE::emitIMMB(SCBE_CPU& pp, CPUReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_B))
    {
        SWAG_ASSERT(numBitsDst == OpBits::B16 || numBitsDst == OpBits::B32);
        pp.emitLoad(reg, ip->b.u64, OpBits::B32);
    }
    else
    {
        pp.emitLoadExtend(reg, CPUReg::RDI, REG_OFFSET(ip->b.u32), numBitsDst, numBitsSrc, isSigned);
    }
}

void SCBE::emitIMMC(SCBE_CPU& pp, CPUReg reg, OpBits numBitsSrc, OpBits numBitsDst, bool isSigned)
{
    const auto ip = pp.ip;
    if (ip->hasFlag(BCI_IMM_C))
    {
        SWAG_ASSERT(numBitsDst == OpBits::B16 || numBitsDst == OpBits::B32);
        pp.emitLoad(reg, ip->c.u64, OpBits::B32);
    }
    else
    {
        pp.emitLoadExtend(reg, CPUReg::RDI, REG_OFFSET(ip->c.u32), numBitsDst, numBitsSrc, isSigned);
    }
}

void SCBE::emitShiftRightArithmetic(SCBE_CPU& pp)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBE_CPU::getOpBits(ip->op);
    if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), opBits);
        pp.emitOpBinary(CPUReg::RAX, ip->b.u32, CPUOp::SAR, opBits);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoad(CPUReg::RCX, std::min(static_cast<uint32_t>(ip->b.u8), SCBE_CPU::getNumBits(opBits) - 1), OpBits::B8);
        else
        {
            pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32);
            pp.emitLoad(CPUReg::RAX, SCBE_CPU::getNumBits(opBits) - 1, OpBits::B32);
            pp.emitCmp(CPUReg::RCX, SCBE_CPU::getNumBits(opBits) - 1, OpBits::B32);
            pp.emitOpBinary(CPUReg::RCX, CPUReg::RAX, CPUOp::CMOVG, opBits);
        }

        emitIMMA(pp, CPUReg::RAX, opBits);
        pp.emitOpBinary(CPUReg::RAX, CPUReg::RCX, CPUOp::SAR, opBits);
    }

    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, opBits);
}

void SCBE::emitShiftRightEqArithmetic(SCBE_CPU& pp)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBE_CPU::getOpBits(ip->op);
    if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
        pp.emitOpBinary(CPUReg::RAX, 0, ip->b.u32, CPUOp::SAR, opBits);
    }
    else
    {
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32);
        pp.emitLoad(CPUReg::RAX, SCBE_CPU::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitCmp(CPUReg::RCX, SCBE_CPU::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitOpBinary(CPUReg::RCX, CPUReg::RAX, CPUOp::CMOVG, opBits);
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
        pp.emitOpBinary(CPUReg::RAX, 0, CPUReg::RCX, CPUOp::SAR, opBits);
    }
}

void SCBE::emitShiftLogical(SCBE_CPU& pp, CPUOp op)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBE_CPU::getOpBits(ip->op);
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= SCBE_CPU::getNumBits(opBits))
    {
        pp.emitClear(CPUReg::RAX, opBits);
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, opBits);
    }
    else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), opBits);
        pp.emitOpBinary(CPUReg::RAX, ip->b.u8, op, opBits);
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, opBits);
    }
    else
    {
        emitIMMA(pp, CPUReg::RAX, opBits);
        emitIMMB(pp, CPUReg::RCX, OpBits::B32);
        pp.emitOpBinary(CPUReg::RAX, CPUReg::RCX, op, opBits);
        pp.emitClear(CPUReg::R8, opBits);
        pp.emitCmp(CPUReg::RCX, SCBE_CPU::getNumBits(opBits) - 1, OpBits::B32);
        pp.emitOpBinary(CPUReg::RAX, CPUReg::R8, CPUOp::CMOVG, opBits);
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, opBits);
    }
}

void SCBE::emitShiftEqLogical(SCBE_CPU& pp, CPUOp op)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBE_CPU::getOpBits(ip->op);
    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= SCBE_CPU::getNumBits(opBits))
    {
        pp.emitClear(CPUReg::RCX, opBits);
        pp.emitStore(CPUReg::RAX, 0, CPUReg::RCX, opBits);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitOpBinary(CPUReg::RAX, 0, ip->b.u8, op, opBits);
    }
    else
    {
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B32);
        pp.emitCmp(CPUReg::RCX, SCBE_CPU::getNumBits(opBits), OpBits::B32);
        const auto jump = pp.emitJump(JL, OpBits::B8);
        pp.emitClear(CPUReg::RCX, opBits);
        pp.emitStore(CPUReg::RAX, 0, CPUReg::RCX, opBits);
        pp.emitPatchJump(jump, pp.concat.totalCount());
        pp.emitOpBinary(CPUReg::RAX, 0, CPUReg::RCX, op, opBits);
    }
}

void SCBE::emitOverflow(SCBE_CPU& pp, const char* msg, bool isSigned)
{
    const auto ip = pp.ip;
    if (BackendEncoder::mustCheckOverflow(pp.buildParams.module, ip))
    {
        const auto jump = pp.emitJump(isSigned ? JNO : JAE, OpBits::B8);
        emitInternalPanic(pp, msg);
        pp.emitPatchJump(jump, pp.concat.totalCount());
    }
}

void SCBE::emitBinOp(SCBE_CPU& pp, CPUOp op, CPUEmitFlags emitFlags)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBE_CPU::getOpBits(ip->op);
    if (SCBE_CPU::isInt(opBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, false, op, opBits);
        pp.emitOpBinary(CPUReg::RAX, ip->b.u64, op, opBits, emitFlags);
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, opBits);
    }
    else
    {
        const auto r0 = SCBE_CPU::isInt(opBits) ? CPUReg::RAX : CPUReg::XMM0;
        const auto r1 = SCBE_CPU::isInt(opBits) ? CPUReg::RCX : CPUReg::XMM1;
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->a.u64, ip->hasFlag(BCI_IMM_A), op, opBits);
        emitIMMB(pp, r1, opBits);
        pp.emitOpBinary(r0, r1, op, opBits, emitFlags);
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), r0, opBits);
    }
}

void SCBE::emitBinOpOverflow(SCBE_CPU& pp, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType)
{
    const auto  ip       = pp.ip;
    const auto  opBits   = SCBE_CPU::getOpBits(ip->op);
    const char* msg      = ByteCodeGen::safetyMsg(safetyMsg, safetyType);
    const bool  isSigned = safetyType->isNativeIntegerSigned();
    SWAG_ASSERT(SCBE_CPU::isInt(opBits));

    emitBinOp(pp, op, EMITF_Overflow);
    emitOverflow(pp, msg, isSigned);
}

void SCBE::emitBinOpEq(SCBE_CPU& pp, uint32_t offset, CPUOp op, CPUEmitFlags emitFlags)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBE_CPU::getOpBits(ip->op);
    if (SCBE_CPU::isInt(opBits) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
        pp.emitOpBinary(CPUReg::RAX, offset, ip->b.u64, op, opBits, emitFlags);
    }
    else
    {
        const auto r0 = SCBE_CPU::isInt(opBits) ? CPUReg::RAX : CPUReg::RCX;
        const auto r1 = SCBE_CPU::isInt(opBits) ? CPUReg::RCX : CPUReg::XMM1;
        pp.emitLoad(r0, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
        emitIMMB(pp, r1, opBits);
        pp.emitOpBinary(r0, offset, r1, op, opBits, emitFlags);
    }
}

void SCBE::emitBinOpEqOverflow(SCBE_CPU& pp, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType)
{
    const auto  ip       = pp.ip;
    const char* msg      = ByteCodeGen::safetyMsg(safetyMsg, safetyType);
    const bool  isSigned = safetyType->isNativeIntegerSigned();
    SWAG_ASSERT(SCBE_CPU::isInt(SCBE_CPU::getOpBits(ip->op)));

    emitBinOpEq(pp, 0, op, EMITF_Overflow);
    emitOverflow(pp, msg, isSigned);
}

void SCBE::emitBinOpEqLock(SCBE_CPU& pp, CPUOp op)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBE_CPU::getOpBits(ip->op);
    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
    emitIMMB(pp, CPUReg::RAX, opBits);
    pp.emitOpBinary(CPUReg::RCX, 0, CPUReg::RAX, op, opBits, EMITF_Lock);
}

void SCBE::emitBinOpEqS(SCBE_CPU& pp, CPUOp op)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBE_CPU::getOpBits(ip->op);
    if (SCBE_CPU::isInt(opBits) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadAddress(CPUReg::RAX, CPUReg::RDI, pp.cpuFct->offsetByteCodeStack + ip->a.u32);
        pp.emitOpBinary(CPUReg::RAX, 0, ip->b.u64, op, opBits);
    }
    else
    {
        const auto r0 = SCBE_CPU::isInt(opBits) ? CPUReg::RAX : CPUReg::RCX;
        const auto r1 = SCBE_CPU::isInt(opBits) ? CPUReg::RCX : CPUReg::XMM1;
        pp.emitLoadAddress(r0, CPUReg::RDI, pp.cpuFct->offsetByteCodeStack + ip->a.u32);
        emitIMMB(pp, r1, opBits);
        pp.emitOpBinary(r0, 0, r1, op, opBits);
    }
}

void SCBE::emitCompareOp(SCBE_CPU& pp)
{
    const auto ip     = pp.ip;
    const auto opBits = SCBE_CPU::getOpBits(ip->op);
    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_B))
    {
        const auto r0 = SCBE_CPU::isInt(opBits) ? CPUReg::RAX : CPUReg::XMM0;
        pp.emitLoad(r0, CPUReg::RDI, REG_OFFSET(ip->a.u32), opBits);
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->b.u32), r0, opBits);
    }
    else if (ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))
    {
        const auto r0 = SCBE_CPU::isInt(opBits) ? CPUReg::RAX : CPUReg::XMM0;
        pp.emitLoad(r0, ip->a.u64, opBits);
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->b.u32), r0, opBits);
    }
    else if (SCBE_CPU::isInt(opBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->b.u64, opBits);
    }
    else
    {
        const auto r0 = SCBE_CPU::isInt(opBits) ? CPUReg::RAX : CPUReg::XMM0;
        const auto r1 = SCBE_CPU::isInt(opBits) ? CPUReg::RCX : CPUReg::XMM1;
        emitIMMA(pp, r0, opBits);
        emitIMMB(pp, r1, opBits);
        pp.emitCmp(r0, r1, opBits);
    }
}

void SCBE::emitAddSubMul64(SCBE_CPU& pp, uint64_t mulValue, CPUOp op)
{
    SWAG_ASSERT(op == CPUOp::ADD || op == CPUOp::SUB);

    const auto ip    = pp.ip;
    const auto value = ip->b.u64 * mulValue;
    if (ip->hasFlag(BCI_IMM_B) && value == 0)
    {
        if (ip->a.u32 != ip->c.u32)
        {
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
            pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, OpBits::B64);
        }
    }
    else if (ip->hasFlag(BCI_IMM_B) && ip->a.u32 == ip->c.u32)
    {
        pp.emitOpBinary(CPUReg::RDI, REG_OFFSET(ip->a.u32), value, op, OpBits::B64);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoad(CPUReg::RAX, value, OpBits::B64);
        else
        {
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), OpBits::B64);
            pp.emitOpBinary(CPUReg::RAX, mulValue, CPUOp::IMUL, OpBits::B64);
        }

        if (ip->a.u32 == ip->c.u32)
            pp.emitOpBinary(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, op, OpBits::B64);
        else
        {
            pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), OpBits::B64);
            pp.emitOpBinary(CPUReg::RCX, CPUReg::RAX, op, OpBits::B64);
            pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RCX, OpBits::B64);
        }
    }
}

void SCBE::emitJumpCmp(SCBE_CPU& pp, CPUCondJump op, OpBits opBits)
{
    const auto ip = pp.ip;
    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        const auto r0 = SCBE_CPU::isInt(opBits) ? CPUReg::RAX : CPUReg::XMM0;
        pp.emitLoad(r0, CPUReg::RDI, REG_OFFSET(ip->a.u32), opBits);
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->c.u32), r0, opBits);
    }
    else if (SCBE_CPU::isInt(opBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_C))
    {
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->c.u64, opBits);
    }
    else
    {
        const auto r0 = SCBE_CPU::isInt(opBits) ? CPUReg::RAX : CPUReg::XMM0;
        const auto r1 = SCBE_CPU::isInt(opBits) ? CPUReg::RCX : CPUReg::XMM1;
        emitIMMA(pp, r0, opBits);
        emitIMMC(pp, r1, opBits);
        pp.emitCmp(r0, r1, opBits);
    }

    emitJump(pp, op, pp.ipIndex, ip->b.s32);
}

void SCBE::emitJumpCmpAddr(SCBE_CPU& pp, CPUCondJump op, CPUReg memReg, uint64_t memOffset, OpBits opBits)
{
    const auto ip = pp.ip;
    SWAG_ASSERT(SCBE_CPU::isInt(opBits));

    if (ip->hasFlag(BCI_IMM_C))
    {
        pp.emitCmp(memReg, memOffset, ip->c.u64, opBits);
    }
    else
    {
        pp.emitLoad(CPUReg::RAX, memReg, memOffset, opBits);
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, opBits);
    }

    emitJump(pp, op, pp.ipIndex, ip->b.s32);
}

void SCBE::emitJumpCmp2(SCBE_CPU& pp, CPUCondJump op1, CPUCondJump op2, OpBits opBits)
{
    const auto ip = pp.ip;
    SWAG_ASSERT(SCBE_CPU::isFloat(opBits));

    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->a.u32), opBits);
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::XMM0, opBits);
    }
    else
    {
        emitIMMA(pp, CPUReg::XMM0, opBits);
        emitIMMC(pp, CPUReg::XMM1, opBits);
        pp.emitCmp(CPUReg::XMM0, CPUReg::XMM1, opBits);
    }

    emitJump(pp, op1, pp.ipIndex, ip->b.s32);
    emitJump(pp, op2, pp.ipIndex, ip->b.s32);
}

void SCBE::emitJumpCmp3(SCBE_CPU& pp, CPUCondJump op1, CPUCondJump op2, OpBits opBits)
{
    const auto ip = pp.ip;
    SWAG_ASSERT(SCBE_CPU::isFloat(opBits));

    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->a.u32), opBits);
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::XMM0, opBits);
    }
    else
    {
        emitIMMA(pp, CPUReg::XMM0, opBits);
        emitIMMC(pp, CPUReg::XMM1, opBits);
        pp.emitCmp(CPUReg::XMM0, CPUReg::XMM1, opBits);
    }

    emitJump(pp, op1, pp.ipIndex, 0);
    emitJump(pp, op2, pp.ipIndex, ip->b.s32);
}

void SCBE::emitInternalPanic(SCBE_CPU& pp, const char* msg)
{
    const auto node = pp.ip->node;
    const auto np   = node->token.sourceFile->path;

    pp.pushParams.clear();
    pp.pushParams.push_back({.type = CPUPushParamType::GlobalString, .value = reinterpret_cast<uint64_t>(np.cstr())});
    pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = node->token.startLocation.line});
    pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = node->token.startLocation.column});
    if (msg)
        pp.pushParams.push_back({.type = CPUPushParamType::GlobalString, .value = reinterpret_cast<uint64_t>(msg)});
    else
        pp.pushParams.push_back({.type = CPUPushParamType::Constant, .value = 0});
    emitInternalCallCPUParams(pp, g_LangSpec->name_priv_panic, pp.pushParams);
}

void SCBE::emitJump(SCBE_CPU& pp, CPUCondJump jumpType, int32_t instructionCount, int32_t jumpOffset)
{
    CPULabelToSolve label;
    label.ipDest = jumpOffset + instructionCount + 1;

    // Can we solve the label now ?
    const auto it = pp.labels.find(label.ipDest);
    if (it != pp.labels.end())
    {
        const auto currentOffset = static_cast<int32_t>(pp.concat.totalCount()) + 1;
        const int  relOffset     = it->second - (currentOffset + 1);
        if (relOffset >= -127 && relOffset <= 128)
        {
            const auto jump = pp.emitJump(jumpType, OpBits::B8);
            pp.emitPatchJump(jump, it->second);
        }
        else
        {
            const auto jump = pp.emitJump(jumpType, OpBits::B32);
            pp.emitPatchJump(jump, it->second);
        }

        return;
    }

    // Here we do not know the destination label, so we assume 32 bits of offset
    label.jump = pp.emitJump(jumpType, OpBits::B32);
    pp.labelsToSolve.push_back(label);
}

void SCBE::emitJumps(SCBE_CPU& pp)
{
    for (auto& toSolve : pp.labelsToSolve)
    {
        auto it = pp.labels.find(toSolve.ipDest);
        SWAG_ASSERT(it != pp.labels.end());
        pp.emitPatchJump(toSolve.jump, it->second);
    }

    pp.labels.clear();
    pp.labelsToSolve.clear();
}