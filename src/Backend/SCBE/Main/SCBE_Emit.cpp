#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/ByteCode_Math.h"
#include "Backend/SCBE/Main/SCBE.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

void SCBE::emitShiftRightArithmetic(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUBits numBits)
{
    if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitOpImmediate(CPUReg::RAX, ip->b.u32, CPUOp::SAR, numBits);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoadImmediate(CPUReg::RCX, static_cast<uint8_t>(min(ip->b.u32, static_cast<uint32_t>(numBits) - 1)), CPUBits::B8);
        else
        {
            pp.emitLoadIndirect(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
            pp.emitLoadImmediate(CPUReg::RAX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
            pp.emitCmpImmediate(CPUReg::RCX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
            pp.emitCMov(CPUReg::RCX, CPUReg::RAX, CPUOp::CMOVG, numBits);
        }

        if (ip->hasFlag(BCI_IMM_A))
            pp.emitLoadImmediate(CPUReg::RAX, ip->a.u64, numBits);
        else
            pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::SAR, numBits);
    }

    pp.emitStoreIndirect(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
}

void SCBE::emitShiftRightEqArithmetic(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitOpIndirectDst(CPUReg::RAX, ip->b.u32, CPUOp::SAR, numBits);
    }
    else
    {
        pp.emitLoadIndirect(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
        pp.emitLoadImmediate(CPUReg::RAX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
        pp.emitCmpImmediate(CPUReg::RCX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
        pp.emitCMov(CPUReg::RCX, CPUReg::RAX, CPUOp::CMOVG, numBits);

        pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitOpIndirectDst(CPUReg::RAX, CPUReg::RCX, CPUOp::SAR, numBits);
    }
}

void SCBE::emitShiftLogical(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= static_cast<uint32_t>(numBits))
    {
        pp.emitClear(CPUReg::RAX, numBits);
        pp.emitStoreIndirect(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
    }
    else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitOpImmediate(CPUReg::RAX, ip->b.u8, op, numBits);
        pp.emitStoreIndirect(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_A))
            pp.emitLoadImmediate(CPUReg::RAX, ip->a.u64, numBits);
        else
            pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);

        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoadImmediate(CPUReg::RCX, ip->b.u8, CPUBits::B8);
        else
            pp.emitLoadIndirect(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
        pp.emitOp(CPUReg::RAX, CPUReg::RCX, op, numBits);

        pp.emitClear(CPUReg::R8, numBits);
        pp.emitCmpImmediate(CPUReg::RCX, static_cast<uint32_t>(numBits) - 1, CPUBits::B32);
        pp.emitCMov(CPUReg::RAX, CPUReg::R8, CPUOp::CMOVG, numBits);
        pp.emitStoreIndirect(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
    }
}

void SCBE::emitShiftEqLogical(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= static_cast<uint32_t>(numBits))
    {
        pp.emitClear(CPUReg::RCX, numBits);
        pp.emitStoreIndirect(CPUReg::RAX, 0, CPUReg::RCX, numBits);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitOpIndirectDst(CPUReg::RAX, ip->b.u8, op, numBits);
    }
    else
    {
        pp.emitLoadIndirect(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
        pp.emitCmpImmediate(CPUReg::RCX, static_cast<uint32_t>(numBits), CPUBits::B32);
        const auto seekPtr = pp.emitJumpNear(JL);
        const auto seekJmp = pp.concat.totalCount();
        pp.emitClear(CPUReg::RCX, numBits);
        pp.emitStoreIndirect(CPUReg::RAX, 0, CPUReg::RCX, numBits);
        *seekPtr = static_cast<uint8_t>(pp.concat.totalCount() - seekJmp);
        pp.emitOpIndirectDst(CPUReg::RAX, CPUReg::RCX, op, numBits);
    }
}

void SCBE::emitOverflowSigned(SCBE_X64& pp, const ByteCodeInstruction* ip, const char* msg)
{
    const bool nw = !ip->node->hasAttribute(ATTRIBUTE_CAN_OVERFLOW_ON) && !ip->hasFlag(BCI_CAN_OVERFLOW);
    if (nw && pp.module->mustEmitSafetyOverflow(ip->node) && !ip->hasFlag(BCI_CANT_OVERFLOW))
    {
        const auto seekPtr = pp.emitJumpNear(JNO);
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
        const auto seekPtr = pp.emitJumpNear(JAE);
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
            pp.emitLoadIndirect(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
            pp.emitOpIndirectSrc(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->b.u32), op, numBits);
        }
        else
        {
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoadImmediate(CPUReg::XMM0, ip->a.u64, numBits);
            else
                pp.emitLoadIndirect(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
            if (ip->hasFlag(BCI_IMM_B))
                pp.emitLoadImmediate(CPUReg::XMM1, ip->b.u64, numBits);
            else
                pp.emitLoadIndirect(CPUReg::XMM1, CPUReg::RDI, REG_OFFSET(ip->b.u32), numBits);
            pp.emitOp(CPUReg::XMM0, CPUReg::XMM1, op, numBits);
        }
    }
    else
    {
        if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))
        {
            pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
            pp.emitOpIndirectSrc(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), op, numBits);
        }
        // Mul by power of 2 => shift by log2
        else if (op == CPUOp::MUL &&
                 !ip->hasFlag(BCI_IMM_A) &&
                 ip->hasFlag(BCI_IMM_B) &&
                 Math::isPowerOfTwo(ip->b.u64) &&
                 ip->b.u64 < static_cast<uint64_t>(numBits))
        {
            pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
            pp.emitLoadImmediate(CPUReg::RCX, static_cast<uint8_t>(log2(ip->b.u64)), CPUBits::B8);
            pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::SHL, numBits);
        }
        else if ((op == CPUOp::AND || op == CPUOp::OR || op == CPUOp::XOR || op == CPUOp::ADD || op == CPUOp::SUB) &&
                 !ip->hasFlag(BCI_IMM_A) &&
                 ip->hasFlag(BCI_IMM_B) &&
                 ip->b.u64 <= 0x7FFFFFFF)
        {
            pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
            pp.emitOpIndirectDst(CPUReg::RAX, ip->b.u64, op, numBits);
        }
        else
        {
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoadImmediate(CPUReg::RAX, ip->a.u64, numBits);
            else
                pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
            if (ip->hasFlag(BCI_IMM_B))
                pp.emitLoadImmediate(CPUReg::RCX, ip->b.u64, numBits);
            else
                pp.emitLoadIndirect(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), numBits);
            pp.emitOp(CPUReg::RAX, CPUReg::RCX, op, numBits);
        }
    }
}

void SCBE::emitBinOpAtReg(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    emitBinOp(pp, ip, op, numBits);
    if (numBits == CPUBits::F32 || numBits == CPUBits::F64)
        pp.emitStoreIndirect(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::XMM0, numBits);
    else
        pp.emitStoreIndirect(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
}

void SCBE::emitBinOpDivAtReg(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    SWAG_ASSERT(op == CPUOp::DIV || op == CPUOp::MOD || op == CPUOp::IDIV || op == CPUOp::IMOD);
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));

    switch (numBits)
    {
        case CPUBits::B8:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoadImmediate(CPUReg::RAX, ip->a.u8, CPUBits::B32);
            else if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUSignedType::S8, CPUSignedType::S32);
            else
                pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUSignedType::U8, CPUSignedType::U32);
            break;

        case CPUBits::B16:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoadImmediate(CPUReg::RAX, ip->a.u16, CPUBits::B16);
            else
                pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B16);
            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B8);
            else
                pp.emitClear(CPUReg::RDX, CPUBits::B16);
            break;

        case CPUBits::B32:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoadImmediate(CPUReg::RAX, ip->a.u32, CPUBits::B32);
            else
                pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B32);
            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B16);
            else
                pp.emitClear(CPUReg::RDX, CPUBits::B32);
            break;

        case CPUBits::B64:
            if (ip->hasFlag(BCI_IMM_A))
                pp.emitLoadImmediate(CPUReg::RAX, ip->a.u64, CPUBits::B64);
            else
                pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                pp.emitConvert(CPUReg::RDX, CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
            else
                pp.emitClear(CPUReg::RDX, CPUBits::B64);
            break;

        default:
            SWAG_ASSERT(false);
            break;
    }

    if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadImmediate(CPUReg::RCX, ip->b.u64, numBits);
        pp.emitOp(CPUReg::RCX, CPUReg::RAX, op, numBits);
    }
    else
    {
        pp.emitOpIndirect(CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUReg::RAX, op, numBits);
    }

    pp.emitStoreIndirect(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
}

void SCBE::emitAddSubMul64(SCBE_X64& pp, const ByteCodeInstruction* ip, uint64_t mul, CPUOp op)
{
    SWAG_ASSERT(op == CPUOp::ADD || op == CPUOp::SUB);

    const auto val = ip->b.u64 * mul;
    if (ip->hasFlag(BCI_IMM_B) && val <= 0x7FFFFFFF && ip->a.u32 == ip->c.u32)
    {
        pp.emitOpIndirectDst(CPUReg::RDI, REG_OFFSET(ip->a.u32), static_cast<uint32_t>(val), op, CPUBits::B64);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoadImmediate(CPUReg::RAX, val, CPUBits::B64);
        else
        {
            pp.emitLoadIndirect(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
            pp.emitOpImmediate(CPUReg::RAX, mul, CPUOp::IMUL, CPUBits::B64);
        }

        if (ip->a.u32 == ip->c.u32)
            pp.emitOpIndirect(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, op, CPUBits::B64);
        else
        {
            pp.emitLoadIndirect(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
            pp.emitOp(CPUReg::RCX, CPUReg::RAX, op, CPUBits::B64);
            pp.emitStoreIndirect(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RCX, CPUBits::B64);
        }
    }
}

void SCBE::emitBinOp(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUBits numBits)
{
    const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::XMM0;
    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_B))
    {
        pp.emitLoadIndirect(r0, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitCmpIndirect(CPUReg::RDI, REG_OFFSET(ip->b.u32), r0, numBits);
    }
    else if (ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoadImmediate(r0, ip->a.u64, numBits);
        pp.emitCmpIndirect(CPUReg::RDI, REG_OFFSET(ip->b.u32), r0, numBits);
    }
    else if (SCBE_CPU::isInt(numBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B) && ip->b.u64 <= 0x7FFFFFFF)
    {
        pp.emitCmpIndirectDst(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->b.u32, numBits);
    }
    else
    {
        const auto r1 = SCBE_CPU::isInt(numBits) ? CPUReg::RCX : CPUReg::XMM1;
        emitIMMA(pp, ip, r0, numBits);
        emitIMMB(pp, ip, r1, numBits);
        pp.emitCmp(r0, r1, numBits);
    }
}

void SCBE::emitJumpCmp(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUJumpType op, CPUBits numBits)
{
    const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::XMM0;
    const auto r1 = SCBE_CPU::isInt(numBits) ? CPUReg::RCX : CPUReg::XMM1;
    if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))
    {
        pp.emitLoadIndirect(r0, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitCmpIndirect(CPUReg::RDI, REG_OFFSET(ip->c.u32), r0, numBits);
    }
    else if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_C) && SCBE_CPU::isInt(numBits) && ip->c.u64 <= 0x7fffffff)
    {
        pp.emitCmpIndirectDst(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->c.u32, numBits);
    }
    else
    {
        emitIMMA(pp, ip, r0, numBits);
        emitIMMC(pp, ip, r1, numBits);
        pp.emitCmp(r0, r1, numBits);
    }

    pp.emitJump(op, instructionCount, ip->b.s32);
}

void SCBE::emitJumpCmpAddr(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUJumpType op, uint32_t memOffset, CPUReg memReg, CPUBits numBits)
{
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));
    if (!ip->hasFlag(BCI_IMM_C))
    {
        pp.emitLoadIndirect(CPUReg::RAX, memReg, memOffset, numBits);
        pp.emitCmpIndirect(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
    }
    else if (ip->c.u64 <= 0x7fffffff)
    {
        pp.emitCmpIndirectDst(memReg, memOffset, ip->c.u32, numBits);
    }
    else
    {
        pp.emitLoadIndirect(CPUReg::RAX, memReg, memOffset, numBits);
        emitIMMC(pp, ip, CPUReg::RCX, numBits);
        pp.emitCmp(CPUReg::RAX, CPUReg::RCX, numBits);
    }

    pp.emitJump(op, instructionCount, ip->b.s32);
}

void SCBE::emitJumpCmp2(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUJumpType op1, CPUJumpType op2, CPUBits numBits)
{
    if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))
    {
        pp.emitLoadIndirect(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitCmpIndirect(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::XMM0, numBits);
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
    if (!ip->hasFlag(BCI_IMM_A) && !ip->hasFlag(BCI_IMM_C))
    {
        pp.emitLoadIndirect(CPUReg::XMM0, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitCmpIndirect(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::XMM0, numBits);
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

void SCBE::emitBinOpEq(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offset, CPUOp op, CPUBits numBits)
{
    const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::RCX;
    const auto r1 = SCBE_CPU::isInt(numBits) ? CPUReg::XMM1 : CPUReg::RCX;
    pp.emitLoadIndirect(r0, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
    emitIMMB(pp, ip, r1, numBits);
    pp.emitOpIndirect(r0, offset, r1, op, numBits);
}

void SCBE::emitBinOpEqS(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offset, CPUOp op, CPUBits numBits)
{
    if (SCBE_CPU::isInt(numBits) && ip->hasFlag(BCI_IMM_B))
        pp.emitOpIndirectDst(CPUReg::RDI, offset + ip->a.u32, ip->b.u64, op, numBits);
    else
    {
        const auto r1 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::XMM1;
        emitIMMB(pp, ip, r1, numBits);
        pp.emitOpIndirect(CPUReg::RDI, offset + ip->a.u32, r1, op, numBits);
    }
}

void SCBE::emitBinOpEqSS(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offset, CPUOp op, CPUBits numBits)
{
    const auto r1 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::XMM1;
    pp.emitLoadIndirect(r1, CPUReg::RDI, offset + ip->b.u32, numBits);
    pp.emitOpIndirect(CPUReg::RDI, offset + ip->a.u32, r1, op, numBits);
}

void SCBE::emitBinOpEqLock(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    pp.emitLoadIndirect(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
    emitIMMB(pp, ip, CPUReg::RAX, numBits);
    pp.emitOpIndirect(CPUReg::RCX, 0, CPUReg::RAX, op, numBits, true);
}

void SCBE::emitIMMA(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_A))
        pp.emitLoadImmediate(reg, ip->a.u64, numBits);
    else
        pp.emitLoadIndirect(reg, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
}

void SCBE::emitIMMB(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_B))
        pp.emitLoadImmediate(reg, ip->b.u64, numBits);
    else
        pp.emitLoadIndirect(reg, CPUReg::RDI, REG_OFFSET(ip->b.u32), numBits);
}

void SCBE::emitIMMC(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_C))
        pp.emitLoadImmediate(reg, ip->c.u64, numBits);
    else
        pp.emitLoadIndirect(reg, CPUReg::RDI, REG_OFFSET(ip->c.u32), numBits);
}

void SCBE::emitIMMD(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBits)
{
    if (ip->hasFlag(BCI_IMM_D))
        pp.emitLoadImmediate(reg, ip->d.u64, numBits);
    else
        pp.emitLoadIndirect(reg, CPUReg::RDI, REG_OFFSET(ip->d.u32), numBits);
}

void SCBE::emitIMMB(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUSignedType srcType, CPUSignedType dstType)
{
    if (ip->hasFlag(BCI_IMM_B))
    {
        SWAG_ASSERT(dstType == CPUSignedType::S16 || dstType == CPUSignedType::U16 || dstType == CPUSignedType::S32 || dstType == CPUSignedType::U32);
        pp.emitLoadImmediate(reg, ip->b.u64, CPUBits::B32);
    }
    else
    {
        pp.emitLoadIndirect(reg, CPUReg::RDI, REG_OFFSET(ip->b.u32), srcType, dstType);
    }
}

void SCBE::emitIMMC(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUSignedType srcType, CPUSignedType dstType)
{
    if (ip->hasFlag(BCI_IMM_C))
    {
        SWAG_ASSERT(dstType == CPUSignedType::S16 || dstType == CPUSignedType::U16 || dstType == CPUSignedType::S32 || dstType == CPUSignedType::U32);
        pp.emitLoadImmediate(reg, ip->c.u64, CPUBits::B32);
    }
    else
    {
        pp.emitLoadIndirect(reg, CPUReg::RDI, REG_OFFSET(ip->c.u32), srcType, dstType);
    }
}
