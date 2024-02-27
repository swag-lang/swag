#include "pch.h"
#include "ByteCode.h"
#include "ByteCode_Math.h"
#include "LanguageSpec.h"
#include "SCBE.h"

void SCBE::emitShiftRightArithmetic(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUBits numBits)
{
    if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);
        pp.emitOpNImmediate(RAX, ip->b.u32, CPUOp::SAR, numBits);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoad8Immediate(RCX, static_cast<uint8_t>(min(ip->b.u32, static_cast<uint32_t>(numBits) - 1)));
        else
        {
            pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), RCX);
            pp.emitLoad32Immediate(RAX, static_cast<uint32_t>(numBits) - 1);
            pp.emitCmpNImmediate(RCX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
            pp.emitCMovN(RCX, RAX, CPUOp::CMOVG, numBits);
        }

        if (ip->hasFlag(BCI_IMM_A))
            pp.emitLoadNImmediate(RAX, ip->a.u64, numBits);
        else
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);
        pp.emitOpN(RCX, RAX, CPUOp::SAR, numBits);
    }

    pp.emitStoreNIndirect(REG_OFFSET(ip->c.u32), RAX, RDI, numBits);
}

void SCBE::emitShiftRightEqArithmetic(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX);
        pp.emitOpNIndirectDst(RAX, ip->b.u32, CPUOp::SAR, numBits);
    }
    else
    {
        pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), RCX);
        pp.emitLoad32Immediate(RAX, static_cast<uint32_t>(numBits) - 1);
        pp.emitCmpNImmediate(RCX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
        pp.emitCMovN(RCX, RAX, CPUOp::CMOVG, numBits);

        pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX);
        pp.emitOpNIndirectDst(RCX, RAX, CPUOp::SAR, numBits);
    }
}

void SCBE::emitShiftLogical(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= static_cast<uint32_t>(numBits))
    {
        pp.emitClearN(RAX, numBits);
        pp.emitStoreNIndirect(REG_OFFSET(ip->c.u32), RAX, RDI, numBits);
    }
    else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);
        pp.emitOpNImmediate(RAX, ip->b.u8, op, numBits);
        pp.emitStoreNIndirect(REG_OFFSET(ip->c.u32), RAX, RDI, numBits);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_A))
            pp.emitLoadNImmediate(RAX, ip->a.u64, numBits);
        else
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);

        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoad8Immediate(RCX, ip->b.u8);
        else
            pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), RCX);
        pp.emitOpN(RCX, RAX, op, numBits);

        pp.emitClearN(R8, numBits);
        pp.emitCmpNImmediate(RCX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
        pp.emitCMovN(RAX, R8, CPUOp::CMOVG, numBits);
        pp.emitStoreNIndirect(REG_OFFSET(ip->c.u32), RAX, RDI, numBits);
    }
}

void SCBE::emitShiftEqLogical(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX);
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= static_cast<uint32_t>(numBits))
    {
        pp.emitClearN(RCX, numBits);
        pp.emitStoreNIndirect(0, RCX, RAX, numBits);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitOpNIndirectDst(RAX, ip->b.u8, op, numBits);
    }
    else
    {
        pp.emitLoad32Indirect(REG_OFFSET(ip->b.u32), RCX);
        pp.emitCmpNImmediate(RCX, static_cast<uint32_t>(numBits), CPUBits::B32);
        const auto seekPtr = pp.emitNearJumpOp(JL);
        const auto seekJmp = pp.concat.totalCount();
        pp.emitClearN(RCX, numBits);
        pp.emitStoreNIndirect(0, RCX, RAX, numBits);
        *seekPtr = static_cast<uint8_t>(pp.concat.totalCount() - seekJmp);
        pp.emitOpNIndirectDst(RCX, RAX, op, numBits);
    }
}

void SCBE::emitOverflowSigned(SCBE_X64& pp, const ByteCodeInstruction* ip, const char* msg)
{
    const bool nw = !ip->node->hasAttribute(ATTRIBUTE_CAN_OVERFLOW_ON) && !ip->hasFlag(BCI_CAN_OVERFLOW);
    if (nw && pp.module->mustEmitSafetyOverflow(ip->node) && !ip->hasFlag(BCI_CANT_OVERFLOW))
    {
        const auto seekPtr = pp.emitNearJumpOp(JNO);
        const auto seekJmp = pp.concat.totalCount();
        emitInternalPanic(pp, ip->node, msg);
        *seekPtr = static_cast<uint8_t>(pp.concat.totalCount() - seekJmp);
    }
}

void SCBE::emitOverflowUnsigned(SCBE_X64& pp, const ByteCodeInstruction* ip, const char* msg)
{
    const bool nw = !ip->node->hasAttribute(ATTRIBUTE_CAN_OVERFLOW_ON) && !ip->hasFlag(BCI_CAN_OVERFLOW);
    if (nw && pp.module->mustEmitSafetyOverflow(ip->node) && !ip->hasFlag(BCI_CANT_OVERFLOW))
    {
        const auto seekPtr = pp.emitNearJumpOp(JAE);
        const auto seekJmp = pp.concat.totalCount();
        emitInternalPanic(pp, ip->node, msg);
        *seekPtr = static_cast<uint8_t>(pp.concat.totalCount() - seekJmp);
    }
}

void SCBE::emitInternalPanic(SCBE_X64& pp, const AstNode* node, const char* msg)
{
    const auto np = node->token.sourceFile->path;
    pp.pushParams.clear();
    pp.pushParams.push_back({CPUPushParamType::GlobalString, reinterpret_cast<uint64_t>(np.c_str())});
    pp.pushParams.push_back({CPUPushParamType::Imm, node->token.startLocation.line});
    pp.pushParams.push_back({CPUPushParamType::Imm, node->token.startLocation.column});
    if (msg)
        pp.pushParams.push_back({CPUPushParamType::GlobalString, reinterpret_cast<uint64_t>(msg)});
    else
        pp.pushParams.push_back({CPUPushParamType::Imm, 0});
    emitInternalCallExt(pp, g_LangSpec->name_priv_panic, pp.pushParams);
}

void SCBE::emitBinOpFloat32(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op)
{
    if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadF32Indirect(REG_OFFSET(ip->a.u32), XMM0);
        pp.emitOpF32(REG_OFFSET(ip->b.u32), op);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_A))
        {
            pp.emitLoad32Immediate(RAX, ip->a.u32);
            pp.emitCopyF32(XMM0, RAX);
        }
        else
            pp.emitLoadF32Indirect(REG_OFFSET(ip->a.u32), XMM0);
        if (ip->hasFlag(BCI_IMM_B))
        {
            pp.emitLoad32Immediate(RAX, ip->b.u32);
            pp.emitCopyF32(XMM1, RAX);
        }
        else
            pp.emitLoadF32Indirect(REG_OFFSET(ip->b.u32), XMM1);
        pp.emitOpF32(XMM0, XMM1, op);
    }
}

void SCBE::emitBinOpFloat64(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op)
{
    if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadF64Indirect(REG_OFFSET(ip->a.u32), XMM0);
        pp.emitOpF64(REG_OFFSET(ip->b.u32), op);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_A))
        {
            pp.emitLoad64Immediate(RAX, ip->a.u64);
            pp.emitCopyF64(XMM0, RAX);
        }
        else
            pp.emitLoadF64Indirect(REG_OFFSET(ip->a.u32), XMM0);
        if (ip->hasFlag(BCI_IMM_B))
        {
            pp.emitLoad64Immediate(RAX, ip->b.u64);
            pp.emitCopyF64(XMM1, RAX);
        }
        else
            pp.emitLoadF64Indirect(REG_OFFSET(ip->b.u32), XMM1);
        pp.emitOpF64(XMM0, XMM1, op);
    }
}

void SCBE::emitBinOpFloat32AtReg(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op)
{
    emitBinOpFloat32(pp, ip, op);
    pp.emitStoreF32Indirect(REG_OFFSET(ip->c.u32), XMM0);
}

void SCBE::emitBinOpFloat64AtReg(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op)
{
    emitBinOpFloat64(pp, ip, op);
    pp.emitStoreF64Indirect(REG_OFFSET(ip->c.u32), XMM0);
}

void SCBE::emitBinOpIntN(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);
        pp.emitOpN(REG_OFFSET(ip->b.u32), op, numBits);
    }
    // Mul by power of 2 => shift by log2
    else if (op == CPUOp::MUL &&
             !ip->hasFlag(BCI_IMM_A) &&
             ip->hasFlag(BCI_IMM_B) &&
             Math::isPowerOfTwo(ip->b.u64) &&
             ip->b.u64 < static_cast<uint64_t>(numBits))
    {
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);
        pp.emitLoad8Immediate(RCX, static_cast<uint8_t>(log2(ip->b.u64)));
        pp.emitOpN(RCX, RAX, CPUOp::SHL, numBits);
    }
    else if ((op == CPUOp::AND || op == CPUOp::OR || op == CPUOp::XOR || op == CPUOp::ADD || op == CPUOp::SUB) &&
             !ip->hasFlag(BCI_IMM_A) &&
             ip->hasFlag(BCI_IMM_B) &&
             ip->b.u64 <= 0x7FFFFFFF)
    {
        pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);
        pp.emitOpNIndirectDst(RAX, ip->b.u64, op, numBits);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_A))
            pp.emitLoadNImmediate(RAX, ip->a.u64, numBits);
        else
            pp.emitLoadNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);
        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoadNImmediate(RCX, ip->b.u64, numBits);
        else
            pp.emitLoadNIndirect(REG_OFFSET(ip->b.u32), RCX, RDI, numBits);
        pp.emitOpN(RCX, RAX, op, numBits);
    }
}

void SCBE::emitBinOpIntNAtReg(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    emitBinOpIntN(pp, ip, op, numBits);
    pp.emitStoreNIndirect(REG_OFFSET(ip->c.u32), RAX, RDI, numBits);
}

void SCBE::emitBinOpDivIntNAtReg(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    SWAG_ASSERT(op == CPUOp::DIV || op == CPUOp::MOD || op == CPUOp::IDIV || op == CPUOp::IMOD);

    switch (numBits)
    {
        case CPUBits::B8:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoad32Immediate(RAX, ip->a.u8);
            else if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitLoadS8S32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
            else
                pp.emitLoadU8U32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
            break;

        case CPUBits::B16:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoad16Immediate(RAX, ip->a.u16);
            else
                pp.emitLoad16Indirect(REG_OFFSET(ip->a.u32), RAX);
            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitCwd();
            else
                pp.emitClearN(RDX, CPUBits::B16);
            break;

        case CPUBits::B32:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoad32Immediate(RAX, ip->a.u32);
            else
                pp.emitLoad32Indirect(REG_OFFSET(ip->a.u32), RAX);
            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitCdq();
            else
                pp.emitClearN(RDX, CPUBits::B32);
            break;

        case CPUBits::B64:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoad64Immediate(RAX, ip->a.u64);
            else
                pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RAX);
            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitCqo();
            else
                pp.emitClearN(RDX, CPUBits::B64);
            break;

        default:
            SWAG_ASSERT(false);
            break;
    }

    if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadNImmediate(RCX, ip->b.u64, numBits);
        pp.emitOpN(RAX, RCX, op, numBits);
    }
    else
    {
        pp.emitOpNIndirect(REG_OFFSET(ip->b.u32), RAX, RDI, op, numBits);
    }

    pp.emitStoreNIndirect(REG_OFFSET(ip->c.u32), RAX, RDI, numBits);
}

void SCBE::emitAddSubMul64(SCBE_X64& pp, const ByteCodeInstruction* ip, uint64_t mul, CPUOp op)
{
    SWAG_ASSERT(op == CPUOp::ADD || op == CPUOp::SUB);

    const auto val = ip->b.u64 * mul;
    if (ip->hasFlag(BCI_IMM_B) && val <= 0x7FFFFFFF && ip->a.u32 == ip->c.u32)
    {
        pp.emitOpNIndirectDst(REG_OFFSET(ip->a.u32), static_cast<uint32_t>(val), RDI, op, CPUBits::B64);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoad64Immediate(RAX, val);
        else
        {
            pp.emitLoad64Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emitOpNImmediate(RAX, mul, CPUOp::IMUL, CPUBits::B64);
        }

        if (ip->a.u32 == ip->c.u32)
            pp.emitOpNIndirect(REG_OFFSET(ip->a.u32), RAX, RDI, op, CPUBits::B64);
        else
        {
            pp.emitLoad64Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emitOpN(RAX, RCX, op, CPUBits::B64);
            pp.emitStore64Indirect(REG_OFFSET(ip->c.u32), RCX);
        }
    }
}
