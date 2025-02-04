#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/ByteCode_Math.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

void SCBE::emitIMMA(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_A))
        pp.emitLoad(reg, ip->a.u64, numBits);
    else
        pp.emitLoad(reg, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
}

void SCBE::emitIMMB(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_B))
        pp.emitLoad(reg, ip->b.u64, numBits);
    else
        pp.emitLoad(reg, CPUReg::RDI, REG_OFFSET(ip->b.u32), numBits);
}

void SCBE::emitIMMC(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_C))
        pp.emitLoad(reg, ip->c.u64, numBits);
    else
        pp.emitLoad(reg, CPUReg::RDI, REG_OFFSET(ip->c.u32), numBits);
}

void SCBE::emitIMMD(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_D))
        pp.emitLoad(reg, ip->d.u64, numBits);
    else
        pp.emitLoad(reg, CPUReg::RDI, REG_OFFSET(ip->d.u32), numBits);
}

void SCBE::emitIMMB(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUSignedType srcType, CPUSignedType dstType)
{
    if (ip->hasFlag(BCI_IMM_B))
    {
        SWAG_ASSERT(dstType == CPUSignedType::S16 || dstType == CPUSignedType::U16 || dstType == CPUSignedType::S32 || dstType == CPUSignedType::U32);
        pp.emitLoad(reg, ip->b.u64, CPUBits::B32);
    }
    else
    {
        pp.emitLoad(reg, CPUReg::RDI, REG_OFFSET(ip->b.u32), srcType, dstType);
    }
}

void SCBE::emitIMMC(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUSignedType srcType, CPUSignedType dstType)
{
    if (ip->hasFlag(BCI_IMM_C))
    {
        SWAG_ASSERT(dstType == CPUSignedType::S16 || dstType == CPUSignedType::U16 || dstType == CPUSignedType::S32 || dstType == CPUSignedType::U32);
        pp.emitLoad(reg, ip->c.u64, CPUBits::B32);
    }
    else
    {
        pp.emitLoad(reg, CPUReg::RDI, REG_OFFSET(ip->c.u32), srcType, dstType);
    }
}

void SCBE::emitShiftRightArithmetic(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUBits numBits)
{
    if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitOp(CPUReg::RAX, ip->b.u32, CPUOp::SAR, numBits);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoad(CPUReg::RCX, static_cast<uint8_t>(min(ip->b.u32, static_cast<uint32_t>(numBits) - 1)), CPUBits::B8);
        else
        {
            pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
            pp.emitLoad(CPUReg::RAX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
            pp.emitCmp(CPUReg::RCX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
            pp.emitCMov(CPUReg::RCX, CPUReg::RAX, CPUOp::CMOVG, numBits);
        }

        if (ip->hasFlag(BCI_IMM_A))
            pp.emitLoad(CPUReg::RAX, ip->a.u64, numBits);
        else
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::SAR, numBits);
    }

    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
}

void SCBE::emitShiftRightEqArithmetic(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitOpInd(CPUReg::RAX, ip->b.u32, CPUOp::SAR, numBits);
    }
    else
    {
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
        pp.emitLoad(CPUReg::RAX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
        pp.emitCmp(CPUReg::RCX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
        pp.emitCMov(CPUReg::RCX, CPUReg::RAX, CPUOp::CMOVG, numBits);

        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitOp(CPUReg::RAX, 0, CPUReg::RCX, CPUOp::SAR, numBits);
    }
}

void SCBE::emitShiftLogical(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= static_cast<uint32_t>(numBits))
    {
        pp.emitClear(CPUReg::RAX, numBits);
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
    }
    else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitOp(CPUReg::RAX, ip->b.u8, op, numBits);
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_A))
            pp.emitLoad(CPUReg::RAX, ip->a.u64, numBits);
        else
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);

        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoad(CPUReg::RCX, ip->b.u8, CPUBits::B8);
        else
            pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
        pp.emitOp(CPUReg::RCX, CPUReg::RAX, op, numBits);

        pp.emitClear(CPUReg::R8, numBits);
        pp.emitCmp(CPUReg::RCX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
        pp.emitCMov(CPUReg::RAX, CPUReg::R8, CPUOp::CMOVG, numBits);
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
    }
}

void SCBE::emitShiftEqLogical(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= static_cast<uint32_t>(numBits))
    {
        pp.emitClear(CPUReg::RCX, numBits);
        pp.emitStore(CPUReg::RAX, 0, CPUReg::RCX, numBits);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitOpInd(CPUReg::RAX, ip->b.u8, op, numBits);
    }
    else
    {
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
        pp.emitCmp(CPUReg::RCX, static_cast<uint32_t>(numBits), CPUBits::B32);
        const auto seekPtr = pp.emitJumpNear(JL);
        const auto seekJmp = pp.concat.totalCount();
        pp.emitClear(CPUReg::RCX, numBits);
        pp.emitStore(CPUReg::RAX, 0, CPUReg::RCX, numBits);
        *seekPtr = static_cast<uint8_t>(pp.concat.totalCount() - seekJmp);
        pp.emitOp(CPUReg::RAX, 0, CPUReg::RCX, op, numBits);
    }
}

void SCBE::emitOverflow(SCBE_X64& pp, const ByteCodeInstruction* ip, const char* msg, bool isSigned)
{
    const bool nw = !ip->node->hasAttribute(ATTRIBUTE_CAN_OVERFLOW_ON) && !ip->hasFlag(BCI_CAN_OVERFLOW);
    if (nw && pp.module->mustEmitSafetyOverflow(ip->node) && !ip->hasFlag(BCI_CANT_OVERFLOW))
    {
        const auto seekPtr = pp.emitJumpNear(isSigned ? JNO : JAE);
        const auto seekJmp = pp.concat.totalCount();
        emitInternalPanic(pp, ip->node, msg);
        *seekPtr = static_cast<uint8_t>(pp.concat.totalCount() - seekJmp);
    }
}

void SCBE::emitBinOp(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    if (numBits == CPUBits::F32 || numBits == CPUBits::F64)
    {
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))
        {
            pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
            pp.emitOp(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), op, numBits);
        }
        else
        {
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoad(CPUReg::XMM0, ip->a.u64, numBits);
            else
                pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
            if (ip->hasFlag(BCI_IMM_B))
                pp.emitLoad(CPUReg::XMM1, ip->b.u64, numBits);
            else
                pp.emitLoad(CPUReg::XMM1, CPUReg::RDI, REG_OFFSET(ip->b.u32), numBits);
            pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, op, numBits);
        }
    }
    else
    {
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))
        {
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
            pp.emitOp(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), op, numBits);
        }
        // Mul by power of 2 => shift by log2
        else if (op == CPUOp::MUL &&
                 !ip->hasFlag(BCI_IMM_A) &&
                 ip->hasFlag(BCI_IMM_B) &&
                 Math::isPowerOfTwo(ip->b.u64) &&
                 ip->b.u64 < static_cast<uint64_t>(numBits))
        {
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
            pp.emitLoad(CPUReg::RCX, static_cast<uint8_t>(log2(ip->b.u64)), CPUBits::B8);
            pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::SHL, numBits);
        }
        else if ((op == CPUOp::AND || op == CPUOp::OR || op == CPUOp::XOR || op == CPUOp::ADD || op == CPUOp::SUB) &&
                 !ip->hasFlag(BCI_IMM_A) &&
                 ip->hasFlag(BCI_IMM_B) &&
                 ip->b.u64 <= 0x7FFFFFFF)
        {
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
            pp.emitOpInd(CPUReg::RAX, ip->b.u64, op, numBits);
        }
        else
        {
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoad(CPUReg::RAX, ip->a.u64, numBits);
            else
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
            if (ip->hasFlag(BCI_IMM_B))
                pp.emitLoad(CPUReg::RCX, ip->b.u64, numBits);
            else
                pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), numBits);
            pp.emitOp(CPUReg::RCX, CPUReg::RAX, op, numBits);
        }
    }
}

void SCBE::emitBinOpAtReg(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    emitBinOp(pp, ip, op, numBits);
    if (numBits == CPUBits::F32 || numBits == CPUBits::F64)
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::XMM0, numBits);
    else
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
}

void SCBE::emitBinOpAtRegOverflow(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits, SafetyMsg safetyMsg, TypeInfo* safetyType)
{
    const char* msg      = ByteCodeGen::safetyMsg(safetyMsg, safetyType);
    const bool  isSigned = safetyType->isNativeIntegerSigned();

    emitBinOp(pp, ip, op, numBits);
    if (numBits == CPUBits::F32 || numBits == CPUBits::F64)
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::XMM0, numBits);
    else
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);

    emitOverflow(pp, ip, msg, isSigned);
}

void SCBE::emitBinOpDivAtReg(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    SWAG_ASSERT(op == CPUOp::DIV || op == CPUOp::MOD || op == CPUOp::IDIV || op == CPUOp::IMOD);
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));

    switch (numBits)
    {
        case CPUBits::B8:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoad(CPUReg::RAX, ip->a.u8, CPUBits::B32);
            else if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUSignedType::S8, CPUSignedType::S32);
            else
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUSignedType::U8, CPUSignedType::U32);
            break;

        case CPUBits::B16:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoad(CPUReg::RAX, ip->a.u16, CPUBits::B16);
            else
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B16);
            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B16);
            else
                pp.emitClear(CPUReg::RDX, CPUBits::B16);
            break;

        case CPUBits::B32:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoad(CPUReg::RAX, ip->a.u32, CPUBits::B32);
            else
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
            else
                pp.emitClear(CPUReg::RDX, CPUBits::B32);
            break;

        case CPUBits::B64:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoad(CPUReg::RAX, ip->a.u64, CPUBits::B64);
            else
                pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B64);
            else
                pp.emitClear(CPUReg::RDX, CPUBits::B64);
            break;

        default:
            SWAG_ASSERT(false);
            break;
    }

    if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad(CPUReg::RCX, ip->b.u64, numBits);
        pp.emitOp(CPUReg::RAX, CPUReg::RCX, op, numBits);
    }
    else
    {
        pp.emitOp(CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUReg::RAX, op, numBits);
    }

    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
}

void SCBE::emitBinOp(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUBits numBits)
{
    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_B))
    {
        const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::XMM0;
        pp.emitLoad(r0, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->b.u32), r0, numBits);
    }
    else if (ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))
    {
        const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::XMM0;
        pp.emitLoad(r0, ip->a.u64, numBits);
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->b.u32), r0, numBits);
    }
    else if (SCBE_CPU::isInt(numBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B) && ip->b.u64 <= 0x7FFFFFFF)
    {
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->b.u32, numBits);
    }
    else
    {
        const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::XMM0;
        const auto r1 = SCBE_CPU::isInt(numBits) ? CPUReg::RCX : CPUReg::XMM1;
        emitIMMA(pp, ip, r0, numBits);
        emitIMMB(pp, ip, r1, numBits);
        pp.emitCmp(r0, r1, numBits);
    }
}

void SCBE::emitBinOpEq(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offset, CPUOp op, CPUBits numBits)
{
    if (op == CPUOp::IDIV || op == CPUOp::IMOD)
    {
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);

        if (numBits == CPUBits::B8)
            pp.emitLoad(CPUReg::RAX, CPUReg::RCX, offset, CPUSignedType::S8, CPUSignedType::S32);
        else
        {
            pp.emitLoad(CPUReg::RAX, CPUReg::RCX, offset, numBits);
            pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, numBits);
        }

        emitIMMB(pp, ip, CPUReg::RCX, numBits);
        pp.emitOp(CPUReg::RAX, CPUReg::RCX, op, numBits);
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitStore(CPUReg::RCX, offset, CPUReg::RAX, numBits);
    }
    else if (op == CPUOp::DIV || op == CPUOp::MOD)
    {
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);

        if (numBits == CPUBits::B8)
            pp.emitLoad(CPUReg::RAX, CPUReg::RCX, offset, CPUSignedType::U8, CPUSignedType::U32);
        else
        {
            pp.emitLoad(CPUReg::RAX, CPUReg::RCX, offset, numBits);
            pp.emitClear(CPUReg::RDX, numBits);
        }

        emitIMMB(pp, ip, CPUReg::RCX, numBits);
        pp.emitOp(CPUReg::RAX, CPUReg::RCX, op, numBits);
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitStore(CPUReg::RCX, offset, CPUReg::RAX, numBits);
    }
    else if (op == CPUOp::FDIV)
    {
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitLoad(CPUReg::XMM0, CPUReg::RCX, 0, numBits);
        emitIMMB(pp, ip, CPUReg::XMM1, numBits);
        pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, op, numBits);
        pp.emitStore(CPUReg::RCX, 0, CPUReg::XMM0, numBits);
    }
    else if (op == CPUOp::IMUL || op == CPUOp::MUL || op == CPUOp::FMUL)
    {
        const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::XMM0;
        const auto r1 = SCBE_CPU::isInt(numBits) ? CPUReg::RCX : CPUReg::XMM1;
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitLoad(r0, CPUReg::RCX, offset, numBits);
        emitIMMB(pp, ip, r1, numBits);
        pp.emitOp(r0, r1, op, numBits);
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitStore(CPUReg::RCX, offset, r0, numBits);
    }
    else if (SCBE_CPU::isInt(numBits) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitOp(CPUReg::RAX, offset, ip->b.u64, op, numBits);
    }
    else
    {
        const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::RCX;
        const auto r1 = SCBE_CPU::isInt(numBits) ? CPUReg::RCX : CPUReg::XMM1;
        pp.emitLoad(r0, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        emitIMMB(pp, ip, r1, numBits);
        pp.emitOp(r0, offset, r1, op, numBits);
    }
}

void SCBE::emitBinOpEqOverflow(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offset, CPUOp op, CPUBits numBits, SafetyMsg safetyMsg, TypeInfo* safetyType)
{
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));
    const char* msg      = ByteCodeGen::safetyMsg(safetyMsg, safetyType);
    const bool  isSigned = safetyType->isNativeIntegerSigned();

    if (op == CPUOp::IMUL || op == CPUOp::MUL)
    {
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitLoad(CPUReg::RAX, CPUReg::RCX, offset, numBits);
        emitIMMB(pp, ip, CPUReg::RCX, numBits);
        pp.emitOp(CPUReg::RAX, CPUReg::RCX, op, numBits);
        emitOverflow(pp, ip, msg, isSigned);
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitStore(CPUReg::RCX, offset, CPUReg::RAX, numBits);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitOp(CPUReg::RAX, offset, ip->b.u64, op, numBits);
        emitOverflow(pp, ip, msg, isSigned);
    }
    else
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        emitIMMB(pp, ip, CPUReg::RCX, numBits);
        pp.emitOp(CPUReg::RAX, offset, CPUReg::RCX, op, numBits);
        emitOverflow(pp, ip, msg, isSigned);
    }
}

void SCBE::emitBinOpEqS(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offsetStack, CPUOp op, CPUBits numBits)
{
    if (op == CPUOp::IDIV)
    {
        if (numBits == CPUBits::B8)
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUSignedType::S8, CPUSignedType::S32);
        else
        {
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, numBits);
            pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, numBits);
        }

        emitIMMB(pp, ip, CPUReg::RCX, numBits);
        pp.emitOp(CPUReg::RAX, CPUReg::RCX, op, numBits);
        pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
        pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, numBits);
    }
    else if (op == CPUOp::DIV)
    {
        if (numBits == CPUBits::B8)
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUSignedType::U8, CPUSignedType::U32);
        else
        {
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, numBits);
            pp.emitClear(CPUReg::RDX, numBits);
        }

        emitIMMB(pp, ip, CPUReg::RCX, numBits);
        pp.emitOp(CPUReg::RAX, CPUReg::RCX, op, numBits);
        pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
        pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, numBits);
    }
    else if (op == CPUOp::FDIV)
    {
        pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, offsetStack + ip->a.u32, numBits);
        emitIMMB(pp, ip, CPUReg::XMM1, numBits);
        pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, op, numBits);
        pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32);
        pp.emitStore(CPUReg::RAX, 0, CPUReg::XMM0, numBits);
    }
    else if (op == CPUOp::IMUL || op == CPUOp::MUL)
    {
        const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::XMM0;
        const auto r1 = SCBE_CPU::isInt(numBits) ? CPUReg::RCX : CPUReg::XMM1;
        pp.emitLoad(r0, CPUReg::RDI, offsetStack + ip->a.u32, numBits);
        emitIMMB(pp, ip, r1, numBits);
        pp.emitOp(r0, r1, op, numBits);
        pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
        pp.emitStore(CPUReg::RCX, 0, r0, numBits);
    }
    else if (SCBE_CPU::isInt(numBits) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitOp(CPUReg::RDI, offsetStack + ip->a.u32, ip->b.u64, op, numBits);
    }
    else
    {
        const auto r1 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::XMM1;
        emitIMMB(pp, ip, r1, numBits);
        pp.emitOp(CPUReg::RDI, offsetStack + ip->a.u32, r1, op, numBits);
    }
}

void SCBE::emitBinOpEqSS(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offsetStack, CPUOp op, CPUBits numBits)
{
    if (op == CPUOp::IDIV)
    {
        if (numBits == CPUBits::B8)
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUSignedType::S8, CPUSignedType::S32);
        else
        {
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, numBits);
            pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, numBits);
        }
        
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, numBits);
        pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, numBits);
        pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
        pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, numBits);        
    }
    else if (op == CPUOp::DIV)
    {
        if (numBits == CPUBits::B8)
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, CPUSignedType::U8, CPUSignedType::U32);
        else
        {
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, numBits);
            pp.emitClear(CPUReg::RDX, numBits);
        }
        
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, numBits);
        pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::IDIV, numBits);
        pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
        pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, numBits);        
    }
    else if (op == CPUOp::FDIV)
    {
        pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, offsetStack + ip->a.u32, numBits);
        pp.emitLoad(CPUReg::XMM1, CPUReg::RDI, offsetStack + ip->b.u32, numBits);
        pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FDIV, numBits);
        pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32);
        pp.emitStore(CPUReg::RAX, 0, CPUReg::XMM0, numBits);        
    }
    else if (op == CPUOp::IMUL || op == CPUOp::MUL)
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32, numBits);
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->b.u32, numBits);
        pp.emitOp(CPUReg::RAX, CPUReg::RCX, op, numBits);
        pp.emitSetAddress(CPUReg::RCX, CPUReg::RDI, offsetStack + ip->a.u32);
        pp.emitStore(CPUReg::RCX, 0, CPUReg::RAX, numBits);
    }
    else
    {
        const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::XMM1;
        pp.emitLoad(r0, CPUReg::RDI, offsetStack + ip->b.u32, numBits);
        pp.emitOp(CPUReg::RDI, offsetStack + ip->a.u32, r0, op, numBits);
    }
}

void SCBE::emitBinOpEqLock(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
    emitIMMB(pp, ip, CPUReg::RAX, numBits);
    pp.emitOp(CPUReg::RCX, 0, CPUReg::RAX, op, numBits, true);
}

void SCBE::emitAddSubMul64(SCBE_X64& pp, const ByteCodeInstruction* ip, uint64_t mul, CPUOp op)
{
    SWAG_ASSERT(op == CPUOp::ADD || op == CPUOp::SUB);

    const auto val = ip->b.u64 * mul;
    if (ip->hasFlag(BCI_IMM_B) && val <= 0x7FFFFFFF && ip->a.u32 == ip->c.u32)
    {
        pp.emitOp(CPUReg::RDI, REG_OFFSET(ip->a.u32), static_cast<uint32_t>(val), op, CPUBits::B64);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoad(CPUReg::RAX, val, CPUBits::B64);
        else
        {
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
            pp.emitOp(CPUReg::RAX, mul, CPUOp::IMUL, CPUBits::B64);
        }

        if (ip->a.u32 == ip->c.u32)
            pp.emitOp(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, op, CPUBits::B64);
        else
        {
            pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
            pp.emitOp(CPUReg::RAX, CPUReg::RCX, op, CPUBits::B64);
            pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RCX, CPUBits::B64);
        }
    }
}

void SCBE::emitJumpCmp(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUJumpType op, CPUBits numBits)
{
    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::XMM0;
        pp.emitLoad(r0, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->c.u32), r0, numBits);
    }
    else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_C) && SCBE_CPU::isInt(numBits) && ip->c.u64 <= 0x7fffffff)
    {
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->c.u32, numBits);
    }
    else
    {
        const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::XMM0;
        const auto r1 = SCBE_CPU::isInt(numBits) ? CPUReg::RCX : CPUReg::XMM1;
        emitIMMA(pp, ip, r0, numBits);
        emitIMMC(pp, ip, r1, numBits);
        pp.emitCmp(r0, r1, numBits);
    }

    pp.emitJump(op, instructionCount, ip->b.s32);
}

void SCBE::emitJumpCmpAddr(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUJumpType op, CPUReg memReg, uint32_t memOffset, CPUBits numBits)
{
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));

    if (!ip->hasFlag(BCI_IMM_C))
    {
        pp.emitLoad(CPUReg::RAX, memReg, memOffset, numBits);
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
    }
    else if (ip->c.u64 <= 0x7fffffff)
    {
        pp.emitCmp(memReg, memOffset, ip->c.u32, numBits);
    }
    else
    {
        pp.emitLoad(CPUReg::RAX, memReg, memOffset, numBits);
        emitIMMC(pp, ip, CPUReg::RCX, numBits);
        pp.emitCmp(CPUReg::RAX, CPUReg::RCX, numBits);
    }

    pp.emitJump(op, instructionCount, ip->b.s32);
}

void SCBE::emitJumpCmp2(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUJumpType op1, CPUJumpType op2, CPUBits numBits)
{
    SWAG_ASSERT(SCBE_CPU::isFloat(numBits));

    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::XMM0, numBits);
    }
    else
    {
        emitIMMA(pp, ip, CPUReg::XMM0, numBits);
        emitIMMC(pp, ip, CPUReg::XMM1, numBits);
        pp.emitCmp(CPUReg::XMM0, CPUReg::XMM1, numBits);
    }

    pp.emitJump(op1, instructionCount, ip->b.s32);
    pp.emitJump(op2, instructionCount, ip->b.s32);
}

void SCBE::emitJumpCmp3(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUJumpType op1, CPUJumpType op2, CPUBits numBits)
{
    SWAG_ASSERT(SCBE_CPU::isFloat(numBits));

    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        pp.emitLoad(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::XMM0, numBits);
    }
    else
    {
        emitIMMA(pp, ip, CPUReg::XMM0, numBits);
        emitIMMC(pp, ip, CPUReg::XMM1, numBits);
        pp.emitCmp(CPUReg::XMM0, CPUReg::XMM1, numBits);
    }

    pp.emitJump(op1, instructionCount, 0);
    pp.emitJump(op2, instructionCount, ip->b.s32);
}

void SCBE::emitInternalPanic(SCBE_X64& pp, const AstNode* node, const char* msg)
{
    const auto np = node->token.sourceFile->path;
    pp.pushParams.clear();
    pp.pushParams.push_back({CPUPushParamType::GlobalString, reinterpret_cast<uint64_t>(np.cstr())});
    pp.pushParams.push_back({CPUPushParamType::Imm, node->token.startLocation.line});
    pp.pushParams.push_back({CPUPushParamType::Imm, node->token.startLocation.column});
    if (msg)
        pp.pushParams.push_back({CPUPushParamType::GlobalString, reinterpret_cast<uint64_t>(msg)});
    else
        pp.pushParams.push_back({CPUPushParamType::Imm, 0});
    emitInternalCallExt(pp, g_LangSpec->name_priv_panic, pp.pushParams);
}
