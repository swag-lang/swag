#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/ByteCode_Math.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Backend/SCBE/Main/SCBE_Macros.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

void SCBE::emitShiftRightArithmetic(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUBits numBits)
{
    if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadIndirect(RAX, RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitOpImmediate(RAX, ip->b.u32, CPUOp::SAR, numBits);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoadImmediate(RCX, static_cast<uint8_t>(min(ip->b.u32, static_cast<uint32_t>(numBits) - 1)), CPUBits::B8);
        else
        {
            pp.emitLoadIndirect(RCX, RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
            pp.emitLoadImmediate(RAX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
            pp.emitCmpImmediate(RCX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
            pp.emitCMov(RCX, RAX, CPUOp::CMOVG, numBits);
        }

        if (ip->hasFlag(BCI_IMM_A))
            pp.emitLoadImmediate(RAX, ip->a.u64, numBits);
        else
            pp.emitLoadIndirect(RAX, RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitOp(RAX, RCX, CPUOp::SAR, numBits);
    }

    pp.emitStoreIndirect(RDI, REG_OFFSET(ip->c.u32), RAX, numBits);
}

void SCBE::emitShiftRightEqArithmetic(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadIndirect(RAX, RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitOpIndirectDst(RAX, ip->b.u32, CPUOp::SAR, numBits);
    }
    else
    {
        pp.emitLoadIndirect(RCX, RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
        pp.emitLoadImmediate(RAX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
        pp.emitCmpImmediate(RCX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
        pp.emitCMov(RCX, RAX, CPUOp::CMOVG, numBits);

        pp.emitLoadIndirect(RAX, RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitOpIndirectDst(RAX, RCX, CPUOp::SAR, numBits);
    }
}

void SCBE::emitShiftLogical(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= static_cast<uint32_t>(numBits))
    {
        pp.emitClear(RAX, numBits);
        pp.emitStoreIndirect(RDI, REG_OFFSET(ip->c.u32), RAX, numBits);
    }
    else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadIndirect(RAX, RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitOpImmediate(RAX, ip->b.u8, op, numBits);
        pp.emitStoreIndirect(RDI, REG_OFFSET(ip->c.u32), RAX, numBits);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_A))
            pp.emitLoadImmediate(RAX, ip->a.u64, numBits);
        else
            pp.emitLoadIndirect(RAX, RDI, REG_OFFSET(ip->a.u32), numBits);

        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoadImmediate(RCX, ip->b.u8, CPUBits::B8);
        else
            pp.emitLoadIndirect(RCX, RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
        pp.emitOp(RAX, RCX, op, numBits);

        pp.emitClear(R8, numBits);
        pp.emitCmpImmediate(RCX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
        pp.emitCMov(RAX, R8, CPUOp::CMOVG, numBits);
        pp.emitStoreIndirect(RDI, REG_OFFSET(ip->c.u32), RAX, numBits);
    }
}

void SCBE::emitShiftEqLogical(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    pp.emitLoadIndirect(RAX, RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= static_cast<uint32_t>(numBits))
    {
        pp.emitClear(RCX, numBits);
        pp.emitStoreIndirect(RAX, 0, RCX, numBits);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitOpIndirectDst(RAX, ip->b.u8, op, numBits);
    }
    else
    {
        pp.emitLoadIndirect(RCX, RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
        pp.emitCmpImmediate(RCX, static_cast<uint32_t>(numBits), CPUBits::B32);
        const auto seekPtr = pp.emitNearJumpOp(JL);
        const auto seekJmp = pp.concat.totalCount();
        pp.emitClear(RCX, numBits);
        pp.emitStoreIndirect(RAX, 0, RCX, numBits);
        *seekPtr = static_cast<uint8_t>(pp.concat.totalCount() - seekJmp);
        pp.emitOpIndirectDst(RAX, RCX, op, numBits);
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
    pp.pushParams.push_back({CPUPushParamType::GlobalString, reinterpret_cast<uint64_t>(np.cstr())});
    pp.pushParams.push_back({CPUPushParamType::Imm, node->token.startLocation.line});
    pp.pushParams.push_back({CPUPushParamType::Imm, node->token.startLocation.column});
    if (msg)
        pp.pushParams.push_back({CPUPushParamType::GlobalString, reinterpret_cast<uint64_t>(msg)});
    else
        pp.pushParams.push_back({CPUPushParamType::Imm, 0});
    emitInternalCallExt(pp, g_LangSpec->name_priv_panic, pp.pushParams);
}

void SCBE::emitBinOp(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    if (numBits == CPUBits::F32 || numBits == CPUBits::F64)
    {
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))
        {
            pp.emitLoadIndirect(XMM0, RDI, REG_OFFSET(ip->a.u32), numBits);
            pp.emitOp(REG_OFFSET(ip->b.u32), op, numBits);
        }
        else
        {
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoadImmediate(XMM0, ip->a.u64, numBits);
            else
                pp.emitLoadIndirect(XMM0, RDI, REG_OFFSET(ip->a.u32), numBits);
            if (ip->hasFlag(BCI_IMM_B))
                pp.emitLoadImmediate(XMM1, ip->b.u64, numBits);
            else
                pp.emitLoadIndirect(XMM1, RDI, REG_OFFSET(ip->b.u32), numBits);
            pp.emitOp(XMM0, XMM1, op, numBits);
        }
    }
    else
    {
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))
        {
            pp.emitLoadIndirect(RAX, RDI, REG_OFFSET(ip->a.u32), numBits);
            pp.emitOp(REG_OFFSET(ip->b.u32), op, numBits);
        }
        // Mul by power of 2 => shift by log2
        else if (op == CPUOp::MUL &&
                 !ip->hasFlag(BCI_IMM_A) &&
                 ip->hasFlag(BCI_IMM_B) &&
                 Math::isPowerOfTwo(ip->b.u64) &&
                 ip->b.u64 < static_cast<uint64_t>(numBits))
        {
            pp.emitLoadIndirect(RAX, RDI, REG_OFFSET(ip->a.u32), numBits);
            pp.emitLoadImmediate(RCX, static_cast<uint8_t>(log2(ip->b.u64)), CPUBits::B8);
            pp.emitOp(RAX, RCX, CPUOp::SHL, numBits);
        }
        else if ((op == CPUOp::AND || op == CPUOp::OR || op == CPUOp::XOR || op == CPUOp::ADD || op == CPUOp::SUB) &&
                 !ip->hasFlag(BCI_IMM_A) &&
                 ip->hasFlag(BCI_IMM_B) &&
                 ip->b.u64 <= 0x7FFFFFFF)
        {
            pp.emitLoadIndirect(RAX, RDI, REG_OFFSET(ip->a.u32), numBits);
            pp.emitOpIndirectDst(RAX, ip->b.u64, op, numBits);
        }
        else
        {
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoadImmediate(RAX, ip->a.u64, numBits);
            else
                pp.emitLoadIndirect(RAX, RDI, REG_OFFSET(ip->a.u32), numBits);
            if (ip->hasFlag(BCI_IMM_B))
                pp.emitLoadImmediate(RCX, ip->b.u64, numBits);
            else
                pp.emitLoadIndirect(RCX, RDI, REG_OFFSET(ip->b.u32), numBits);
            pp.emitOp(RAX, RCX, op, numBits);
        }
    }
}

void SCBE::emitBinOpAtReg(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    emitBinOp(pp, ip, op, numBits);
    if (numBits == CPUBits::F32 || numBits == CPUBits::F64)
        pp.emitStoreIndirect(RDI, REG_OFFSET(ip->c.u32), XMM0, numBits);
    else
        pp.emitStoreIndirect(RDI, REG_OFFSET(ip->c.u32), RAX, numBits);
}

void SCBE::emitBinOpDivAtReg(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    SWAG_ASSERT(op == CPUOp::DIV || op == CPUOp::MOD || op == CPUOp::IDIV || op == CPUOp::IMOD);
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));

    switch (numBits)
    {
        case CPUBits::B8:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoadImmediate(RAX, ip->a.u8, CPUBits::B32);
            else if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitLoadS8S32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
            else
                pp.emitLoadU8U32Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
            break;

        case CPUBits::B16:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoadImmediate(RAX, ip->a.u16, CPUBits::B16);
            else
                pp.emitLoadIndirect(RAX, RDI, REG_OFFSET(ip->a.u32), CPUBits::B16);
            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitCwd();
            else
                pp.emitClear(RDX, CPUBits::B16);
            break;

        case CPUBits::B32:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoadImmediate(RAX, ip->a.u32, CPUBits::B32);
            else
                pp.emitLoadIndirect(RAX, RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitCdq();
            else
                pp.emitClear(RDX, CPUBits::B32);
            break;

        case CPUBits::B64:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoadImmediate(RAX, ip->a.u64, CPUBits::B64);
            else
                pp.emitLoadIndirect(RAX, RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitCqo();
            else
                pp.emitClear(RDX, CPUBits::B64);
            break;

        default:
            SWAG_ASSERT(false);
            break;
    }

    if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadImmediate(RCX, ip->b.u64, numBits);
        pp.emitOp(RCX, RAX, op, numBits);
    }
    else
    {
        pp.emitOpIndirect(RDI, REG_OFFSET(ip->b.u32), RAX, op, numBits);
    }

    pp.emitStoreIndirect(RDI, REG_OFFSET(ip->c.u32), RAX, numBits);
}

void SCBE::emitAddSubMul64(SCBE_X64& pp, const ByteCodeInstruction* ip, uint64_t mul, CPUOp op)
{
    SWAG_ASSERT(op == CPUOp::ADD || op == CPUOp::SUB);

    const auto val = ip->b.u64 * mul;
    if (ip->hasFlag(BCI_IMM_B) && val <= 0x7FFFFFFF && ip->a.u32 == ip->c.u32)
    {
        pp.emitOpIndirectDst(RDI, REG_OFFSET(ip->a.u32), static_cast<uint32_t>(val), op, CPUBits::B64);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoadImmediate(RAX, val, CPUBits::B64);
        else
        {
            pp.emitLoadIndirect(RAX, RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
            pp.emitOpImmediate(RAX, mul, CPUOp::IMUL, CPUBits::B64);
        }

        if (ip->a.u32 == ip->c.u32)
            pp.emitOpIndirect(RDI, REG_OFFSET(ip->a.u32), RAX, op, CPUBits::B64);
        else
        {
            pp.emitLoadIndirect(RCX, RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
            pp.emitOp(RCX, RAX, op, CPUBits::B64);
            pp.emitStoreIndirect(RDI, REG_OFFSET(ip->c.u32), RCX, CPUBits::B64);
        }
    }
}

void SCBE::emitBinOp(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUBits numBits)
{
    const auto r0 = SCBE_CPU::isInt(numBits) ? RAX : XMM0;
    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_B))
    {
        pp.emitLoadIndirect(r0, RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitCmpIndirect(RDI, REG_OFFSET(ip->b.u32), r0, numBits);
    }
    else if (ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadImmediate(r0, ip->a.u64, numBits);
        pp.emitCmpIndirect(RDI, REG_OFFSET(ip->b.u32), r0, numBits);
    }
    else if (SCBE_CPU::isInt(numBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B) && ip->b.u64 <= 0x7FFFFFFF)
    {
        pp.emitCmpIndirectDst(RDI, REG_OFFSET(ip->a.u32), ip->b.u32, numBits);
    }
    else
    {
        const auto r1 = SCBE_CPU::isInt(numBits) ? RCX : XMM1;
        emitIMMA(pp, ip, r0, numBits);
        emitIMMB(pp, ip, r1, numBits);
        pp.emitCmp(r0, r1, numBits);
    }
}

void SCBE::emitJumpCmp(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUJumpType op, CPUBits numBits)
{
    const auto r0 = SCBE_CPU::isInt(numBits) ? RAX : XMM0;
    const auto r1 = SCBE_CPU::isInt(numBits) ? RCX : XMM1;
    if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))
    {
        pp.emitLoadIndirect(r0, RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitCmpIndirect(RDI, REG_OFFSET(ip->c.u32), r0, numBits);
    }
    else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_C) && SCBE_CPU::isInt(numBits) && ip->c.u64 <= 0x7fffffff)
    {
        pp.emitCmpIndirectDst(RDI, REG_OFFSET(ip->a.u32), ip->c.u32, numBits);
    }
    else
    {
        emitIMMA(pp, ip, r0, numBits);
        emitIMMC(pp, ip, r1, numBits);
        pp.emitCmp(r0, r1, numBits);
    }

    pp.emitJump(op, instructionCount, ip->b.s32);
}

void SCBE::emitJumpCmpAddr(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUJumpType op, uint32_t memOffset, CPURegister memReg, CPUBits numBits)
{
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));
    if (!ip->hasFlag(BCI_IMM_C))
    {
        pp.emitLoadIndirect(RAX, memReg, memOffset, numBits);
        pp.emitCmpIndirect(RDI, REG_OFFSET(ip->c.u32), RAX, numBits);
    }
    else if (ip->c.u64 <= 0x7fffffff)
    {
        pp.emitCmpIndirectDst(memReg, memOffset, ip->c.u32, numBits);
    }
    else
    {
        pp.emitLoadIndirect(RAX, memReg, memOffset, numBits);
        emitIMMC(pp, ip, RCX, numBits);
        pp.emitCmp(RAX, RCX, numBits);
    }

    pp.emitJump(op, instructionCount, ip->b.s32);
}

void SCBE::emitJumpCmp2(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUJumpType op1, CPUJumpType op2, CPUBits numBits)
{
    if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))
    {
        pp.emitLoadIndirect(XMM0, RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitCmpIndirect(RDI, REG_OFFSET(ip->c.u32), XMM0, numBits);
    }
    else
    {
        emitIMMA(pp, ip, XMM0, numBits);
        emitIMMC(pp, ip, XMM1, numBits);
        pp.emitCmp(XMM0, XMM1, numBits);
    }
    pp.emitJump(op1, instructionCount, ip->b.s32);
    pp.emitJump(op2, instructionCount, ip->b.s32);
}

void SCBE::emitJumpCmp3(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUJumpType op1, CPUJumpType op2, CPUBits numBits)
{
    if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))
    {
        pp.emitLoadIndirect(XMM0, RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitCmpIndirect(RDI, REG_OFFSET(ip->c.u32), XMM0, numBits);
    }
    else
    {
        emitIMMA(pp, ip, XMM0, numBits);
        emitIMMC(pp, ip, XMM1, numBits);
        pp.emitCmp(XMM0, XMM1, numBits);
    }
    pp.emitJump(op1, instructionCount, 0);
    pp.emitJump(op2, instructionCount, ip->b.s32);
}

void SCBE::emitBinOpEq(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offset, CPUOp op, CPUBits numBits)
{
    const auto r0 = SCBE_CPU::isInt(numBits) ? RAX : RCX;
    const auto r1 = SCBE_CPU::isInt(numBits) ? XMM1 : RCX;
    pp.emitLoadIndirect(r0, RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
    emitIMMB(pp, ip, r1, numBits);
    pp.emitOpIndirect(r0, offset, r1, op, numBits);
}

void SCBE::emitBinOpEqS(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offset, CPUOp op, CPUBits numBits)
{
    if (SCBE_CPU::isInt(numBits) && ip->hasFlag(BCI_IMM_B))
        pp.emitOpIndirectDst(RDI, offset + ip->a.u32, ip->b.u64, op, numBits);
    else
    {
        const auto r1 = SCBE_CPU::isInt(numBits) ? RAX : XMM1;
        emitIMMB(pp, ip, r1, numBits);
        pp.emitOpIndirect(RDI, offset + ip->a.u32, r1, op, numBits);
    }
}

void SCBE::emitBinOpEqSS(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offset, CPUOp op, CPUBits numBits)
{
    const auto r1 = SCBE_CPU::isInt(numBits) ? RAX : XMM1;
    pp.emitLoadIndirect(r1, RDI, offset + ip->b.u32, numBits);
    pp.emitOpIndirect(RDI, offset + ip->a.u32, r1, op, numBits);
}

void SCBE::emitBinOpEqLock(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    pp.emitLoadIndirect(RCX, RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
    emitIMMB(pp, ip, RAX, numBits);
    pp.emitOpIndirect(RCX, 0, RAX, op, numBits, true);
}

void SCBE::emitIMMA(SCBE_X64& pp, const ByteCodeInstruction* ip, CPURegister reg, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_A))
        pp.emitLoadImmediate(reg, ip->a.u64, numBits);
    else
        pp.emitLoadIndirect(reg, RDI, REG_OFFSET(ip->a.u32), numBits);
}

void SCBE::emitIMMB(SCBE_X64& pp, const ByteCodeInstruction* ip, CPURegister reg, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_B))
        pp.emitLoadImmediate(reg, ip->b.u64, numBits);
    else
        pp.emitLoadIndirect(reg, RDI, REG_OFFSET(ip->b.u32), numBits);
}

void SCBE::emitIMMC(SCBE_X64& pp, const ByteCodeInstruction* ip, CPURegister reg, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_C))
        pp.emitLoadImmediate(reg, ip->c.u64, numBits);
    else
        pp.emitLoadIndirect(reg, RDI, REG_OFFSET(ip->c.u32), numBits);
}

void SCBE::emitIMMD(SCBE_X64& pp, const ByteCodeInstruction* ip, CPURegister reg, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_D))
        pp.emitLoadImmediate(reg, ip->d.u64, numBits);
    else
        pp.emitLoadIndirect(reg, RDI, REG_OFFSET(ip->d.u32), numBits);
}
