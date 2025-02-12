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

void SCBE::emitIMMB(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBitsSrc, CPUBits numBitsDst, bool isSigned)
{
    if (ip->hasFlag(BCI_IMM_B))
    {
        SWAG_ASSERT(numBitsDst == CPUBits::B16 || numBitsDst == CPUBits::B32);
        pp.emitLoad(reg, ip->b.u64, CPUBits::B32);
    }
    else
    {
        pp.emitLoad(reg, CPUReg::RDI, REG_OFFSET(ip->b.u32), numBitsDst, numBitsSrc, isSigned);
    }
}

void SCBE::emitIMMC(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUReg reg, CPUBits numBitsSrc, CPUBits numBitsDst, bool isSigned)
{
    if (ip->hasFlag(BCI_IMM_C))
    {
        SWAG_ASSERT(numBitsDst == CPUBits::B16 || numBitsDst == CPUBits::B32);
        pp.emitLoad(reg, ip->c.u64, CPUBits::B32);
    }
    else
    {
        pp.emitLoad(reg, CPUReg::RDI, REG_OFFSET(ip->c.u32), numBitsDst, numBitsSrc, isSigned);
    }
}

void SCBE::emitShiftRightArithmetic(SCBE_X64& pp, const ByteCodeInstruction* ip)
{
    const auto numBits = SCBE_CPU::getCPUBits(ip->op);
    if (!ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitOp(CPUReg::RAX, ip->b.u32, CPUOp::SAR, numBits);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoad(CPUReg::RCX, min(ip->b.u8, SCBE_CPU::getBitsCount(numBits) - 1), CPUBits::B8);
        else
        {
            pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
            pp.emitLoad(CPUReg::RAX, SCBE_CPU::getBitsCount(numBits) - 1, CPUBits::B32);
            pp.emitCmp(CPUReg::RCX, SCBE_CPU::getBitsCount(numBits) - 1, CPUBits::B32);
            pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::CMOVG, numBits);
        }

        emitIMMA(pp, ip, CPUReg::RAX, numBits);
        pp.emitOp(CPUReg::RAX, CPUReg::RCX, CPUOp::SAR, numBits);
    }

    pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
}

void SCBE::emitShiftRightEqArithmetic(SCBE_X64& pp, const ByteCodeInstruction* ip)
{
    const auto numBits = SCBE_CPU::getCPUBits(ip->op);
    if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitOp(CPUReg::RAX, 0, ip->b.u32, CPUOp::SAR, numBits);
    }
    else
    {
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
        pp.emitLoad(CPUReg::RAX, SCBE_CPU::getBitsCount(numBits) - 1, CPUBits::B32);
        pp.emitCmp(CPUReg::RCX, SCBE_CPU::getBitsCount(numBits) - 1, CPUBits::B32);
        pp.emitOp(CPUReg::RCX, CPUReg::RAX, CPUOp::CMOVG, numBits);
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitOp(CPUReg::RAX, 0, CPUReg::RCX, CPUOp::SAR, numBits);
    }
}

void SCBE::emitShiftLogical(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op)
{
    const auto numBits = SCBE_CPU::getCPUBits(ip->op);
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= SCBE_CPU::getBitsCount(numBits))
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
        emitIMMA(pp, ip, CPUReg::RAX, numBits);
        emitIMMB(pp, ip, CPUReg::RCX, CPUBits::B32);
        pp.emitOp(CPUReg::RAX, CPUReg::RCX, op, numBits);
        pp.emitClear(CPUReg::R8, numBits);
        pp.emitCmp(CPUReg::RCX, SCBE_CPU::getBitsCount(numBits) - 1, CPUBits::B32);
        pp.emitOp(CPUReg::RAX, CPUReg::R8, CPUOp::CMOVG, numBits);
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
    }
}

void SCBE::emitShiftEqLogical(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op)
{
    const auto numBits = SCBE_CPU::getCPUBits(ip->op);
    pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
    if (ip->hasFlag(BCI_IMM_B) && ip->b.u32 >= SCBE_CPU::getBitsCount(numBits))
    {
        pp.emitClear(CPUReg::RCX, numBits);
        pp.emitStore(CPUReg::RAX, 0, CPUReg::RCX, numBits);
    }
    else if (ip->hasFlag(BCI_IMM_B))
    {
        pp.emitOp(CPUReg::RAX, 0, ip->b.u8, op, numBits);
    }
    else
    {
        pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B32);
        pp.emitCmp(CPUReg::RCX, SCBE_CPU::getBitsCount(numBits), CPUBits::B32);
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

void SCBE::emitBinOp(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, CPUEmitFlags emitFlags)
{
    const auto numBits = SCBE_CPU::getCPUBits(ip->op);
    if (SCBE_CPU::isInt(numBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), 0, false, op, numBits);
        pp.emitOp(CPUReg::RAX, ip->b.u64, op, numBits, emitFlags);
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
    }
    else
    {
        const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::XMM0;
        const auto r1 = SCBE_CPU::isInt(numBits) ? CPUReg::RCX : CPUReg::XMM1;
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->a.u64, ip->hasFlag(BCI_IMM_A), op, numBits);
        emitIMMB(pp, ip, r1, numBits);
        pp.emitOp(r0, r1, op, numBits, emitFlags);
        pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), r0, numBits);
    }
}

void SCBE::emitBinOpOverflow(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType)
{
    const auto  numBits  = SCBE_CPU::getCPUBits(ip->op);
    const char* msg      = ByteCodeGen::safetyMsg(safetyMsg, safetyType);
    const bool  isSigned = safetyType->isNativeIntegerSigned();
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));

    emitBinOp(pp, ip, op, EMITF_Overflow);
    emitOverflow(pp, ip, msg, isSigned);
}

void SCBE::emitBinOpEq(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offset, CPUOp op, CPUEmitFlags emitFlags)
{
    const auto numBits = SCBE_CPU::getCPUBits(ip->op);
    if (SCBE_CPU::isInt(numBits) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        pp.emitOp(CPUReg::RAX, offset, ip->b.u64, op, numBits, emitFlags);
    }
    else
    {
        const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::RCX;
        const auto r1 = SCBE_CPU::isInt(numBits) ? CPUReg::RCX : CPUReg::XMM1;
        pp.emitLoad(r0, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
        emitIMMB(pp, ip, r1, numBits);
        pp.emitOp(r0, offset, r1, op, numBits, emitFlags);
    }
}

void SCBE::emitBinOpEqOverflow(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offset, CPUOp op, SafetyMsg safetyMsg, TypeInfo* safetyType)
{
    const char* msg      = ByteCodeGen::safetyMsg(safetyMsg, safetyType);
    const bool  isSigned = safetyType->isNativeIntegerSigned();
    SWAG_ASSERT(SCBE_CPU::isInt(SCBE_CPU::getCPUBits(ip->op)));

    emitBinOpEq(pp, ip, 0, op, EMITF_Overflow);
    emitOverflow(pp, ip, msg, isSigned);
}

void SCBE::emitBinOpEqLock(SCBE_X64& pp, const ByteCodeInstruction* ip, CPUOp op)
{
    const auto numBits = SCBE_CPU::getCPUBits(ip->op);
    pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
    emitIMMB(pp, ip, CPUReg::RAX, numBits);
    pp.emitOp(CPUReg::RCX, 0, CPUReg::RAX, op, numBits, EMITF_Lock);
}

void SCBE::emitBinOpEqS(SCBE_X64& pp, const ByteCodeInstruction* ip, uint32_t offsetStack, CPUOp op)
{
    const auto numBits = SCBE_CPU::getCPUBits(ip->op);
    if (SCBE_CPU::isInt(numBits) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitSetAddress(CPUReg::RAX, CPUReg::RDI, offsetStack + ip->a.u32);
        pp.emitOp(CPUReg::RAX, 0, ip->b.u64, op, numBits);
    }
    else
    {
        const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::RCX;
        const auto r1 = SCBE_CPU::isInt(numBits) ? CPUReg::RCX : CPUReg::XMM1;
        pp.emitSetAddress(r0, CPUReg::RDI, offsetStack + ip->a.u32);
        emitIMMB(pp, ip, r1, numBits);
        pp.emitOp(r0, 0, r1, op, numBits);
    }
}

void SCBE::emitCompareOp(SCBE_X64& pp, const ByteCodeInstruction* ip)
{
    const auto numBits = SCBE_CPU::getCPUBits(ip->op);
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
    else if (SCBE_CPU::isInt(numBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_B))
    {
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->b.u64, numBits);
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

void SCBE::emitAddSubMul64(SCBE_X64& pp, const ByteCodeInstruction* ip, uint64_t mulValue, CPUOp op)
{
    SWAG_ASSERT(op == CPUOp::ADD || op == CPUOp::SUB);

    const auto value = ip->b.u64 * mulValue;
    if (ip->hasFlag(BCI_IMM_B) && value == 0)
    {
        if (ip->a.u32 != ip->c.u32)
        {
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
            pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, CPUBits::B64);
        }
    }
    else if (ip->hasFlag(BCI_IMM_B) && ip->a.u32 == ip->c.u32)
    {
        pp.emitOp(CPUReg::RDI, REG_OFFSET(ip->a.u32), value, op, CPUBits::B64);
    }
    else
    {
        if (ip->hasFlag(BCI_IMM_B))
            pp.emitLoad(CPUReg::RAX, value, CPUBits::B64);
        else
        {
            pp.emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(ip->b.u32), CPUBits::B64);
            pp.emitOp(CPUReg::RAX, mulValue, CPUOp::IMUL, CPUBits::B64);
        }

        if (ip->a.u32 == ip->c.u32)
            pp.emitOp(CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUReg::RAX, op, CPUBits::B64);
        else
        {
            pp.emitLoad(CPUReg::RCX, CPUReg::RDI, REG_OFFSET(ip->a.u32), CPUBits::B64);
            pp.emitOp(CPUReg::RCX, CPUReg::RAX, op, CPUBits::B64);
            pp.emitStore(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RCX, CPUBits::B64);
        }
    }
}

void SCBE::emitJumpCmp(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUCondJump op, CPUBits numBits)
{
    if (!ip->hasFlag(BCI_IMM_A | BCI_IMM_C))
    {
        const auto r0 = SCBE_CPU::isInt(numBits) ? CPUReg::RAX : CPUReg::XMM0;
        pp.emitLoad(r0, CPUReg::RDI, REG_OFFSET(ip->a.u32), numBits);
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->c.u32), r0, numBits);
    }
    else if (SCBE_CPU::isInt(numBits) && !ip->hasFlag(BCI_IMM_A) && ip->hasFlag(BCI_IMM_C))
    {
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->a.u32), ip->c.u64, numBits);
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

void SCBE::emitJumpCmpAddr(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUCondJump op, CPUReg memReg, uint64_t memOffset, CPUBits numBits)
{
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));

    if (ip->hasFlag(BCI_IMM_C))
    {
        pp.emitCmp(memReg, memOffset, ip->c.u64, numBits);
    }
    else
    {
        pp.emitLoad(CPUReg::RAX, memReg, memOffset, numBits);
        pp.emitCmp(CPUReg::RDI, REG_OFFSET(ip->c.u32), CPUReg::RAX, numBits);
    }

    pp.emitJump(op, instructionCount, ip->b.s32);
}

void SCBE::emitJumpCmp2(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUCondJump op1, CPUCondJump op2, CPUBits numBits)
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

void SCBE::emitJumpCmp3(SCBE_X64& pp, const ByteCodeInstruction* ip, int32_t instructionCount, CPUCondJump op1, CPUCondJump op2, CPUBits numBits)
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
