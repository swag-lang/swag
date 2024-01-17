#include "pch.h"
#include "BackendSCBE.h"
#include "ByteCode.h"
#include "LanguageSpec.h"
#include "Math.h"

void BackendSCBE::emitShiftRightArithmetic(EncoderX64& pp, ByteCodeInstruction* ip, CPUBits numBits)
{
    if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B))
    {
        pp.emit_LoadN_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);
        pp.emit_ShiftN(RAX, min(ip->b.u32, (uint32_t) numBits - 1), numBits, CPUOp::SAR);
    }
    else
    {
        if (ip->flags & BCI_IMM_B)
            pp.emit_Load8_Immediate(RCX, (uint8_t) min(ip->b.u32, (uint32_t) numBits - 1));
        else
        {
            pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RCX);
            pp.emit_Load32_Immediate(RAX, (uint32_t) numBits - 1);
            pp.emit_CmpN_Immediate(RCX, (uint32_t) numBits - 1, CPUBits::B32);
            pp.emit_CMovN(RCX, RAX, numBits, CPUOp::CMOVG);
        }

        if (ip->flags & BCI_IMM_A)
            pp.emit_LoadN_Immediate(RAX, ip->a.u64, numBits);
        else
            pp.emit_LoadN_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);

        pp.emit_OpN(RCX, RAX, CPUOp::SAR, numBits);
    }

    pp.emit_StoreN_Indirect(REG_OFFSET(ip->c.u32), RAX, RDI, numBits);
}

void BackendSCBE::emitShiftRightEqArithmetic(EncoderX64& pp, ByteCodeInstruction* ip, CPUBits numBits)
{
    if (ip->flags & BCI_IMM_B)
    {
        pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
        switch (numBits)
        {
        case CPUBits::B8:
            pp.concat.addString2("\xC0\x38"); // sar byte ptr [rax], ??
            break;
        case CPUBits::B16:
            pp.concat.addString3("\x66\xC1\x38");
            break;
        case CPUBits::B32:
            pp.concat.addString2("\xC1\x38");
            break;
        case CPUBits::B64:
            pp.concat.addString3("\x48\xC1\x38");
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }

        pp.concat.addU8((uint8_t) min(ip->b.u32, (uint32_t) numBits - 1));
    }
    else
    {
        pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RCX);
        pp.emit_Load32_Immediate(RAX, (uint32_t) numBits - 1);
        pp.emit_CmpN_Immediate(RCX, (uint32_t) numBits - 1, CPUBits::B32);
        pp.emit_CMovN(RCX, RAX, numBits, CPUOp::CMOVG);

        pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
        pp.emit_OpN_IndirectDst(RCX, RAX, CPUOp::SAR, numBits);
    }
}

void BackendSCBE::emitShiftLogical(EncoderX64& pp, ByteCodeInstruction* ip, CPUBits numBits, CPUOp op)
{
    if ((ip->flags & BCI_IMM_B) && ip->b.u32 >= (uint32_t) numBits)
    {
        pp.emit_ClearN(RAX, numBits);
        pp.emit_StoreN_Indirect(REG_OFFSET(ip->c.u32), RAX, RDI, numBits);
    }
    else if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B))
    {
        pp.emit_LoadN_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);
        pp.emit_ShiftN(RAX, ip->b.u8, numBits, op);
        pp.emit_StoreN_Indirect(REG_OFFSET(ip->c.u32), RAX, RDI, numBits);
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
            pp.emit_LoadN_Immediate(RAX, ip->a.u64, numBits);
        else
            pp.emit_LoadN_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);

        if (ip->flags & BCI_IMM_B)
            pp.emit_Load8_Immediate(RCX, ip->b.u8);
        else
            pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RCX);
        pp.emit_OpN(RCX, RAX, op, numBits);

        pp.emit_ClearN(R8, numBits);
        pp.emit_CmpN_Immediate(RCX, (uint32_t) numBits - 1, CPUBits::B32);
        pp.emit_CMovN(RAX, R8, numBits, CPUOp::CMOVG);
        pp.emit_StoreN_Indirect(REG_OFFSET(ip->c.u32), RAX, RDI, numBits);
    }
}

void BackendSCBE::emitShiftEqLogical(EncoderX64& pp, ByteCodeInstruction* ip, CPUBits numBits, CPUOp op)
{
    pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
    if ((ip->flags & BCI_IMM_B) && ip->b.u32 >= (uint32_t) numBits)
    {
        pp.emit_ClearN(RCX, numBits);
        pp.emit_StoreN_Indirect(0, RCX, RAX, numBits);
    }
    else if (ip->flags & BCI_IMM_B)
    {
        pp.emit_OpN_IndirectDst(RAX, ip->b.u8, op, numBits);
    }
    else
    {
        pp.emit_Load32_Indirect(REG_OFFSET(ip->b.u32), RCX);
        pp.emit_CmpN_Immediate(RCX, (uint32_t) numBits, CPUBits::B32);
        pp.emit_NearJumpOp(JL);
        pp.concat.addU8(0); // clear + store below
        auto seekPtr = pp.concat.getSeekPtr() - 1;
        auto seekJmp = pp.concat.totalCount();
        pp.emit_ClearN(RCX, numBits);
        pp.emit_StoreN_Indirect(0, RCX, RAX, numBits);
        *seekPtr = (uint8_t) (pp.concat.totalCount() - seekJmp);
        pp.emit_OpN_IndirectDst(RCX, RAX, op, numBits);
    }
}

void BackendSCBE::emitOverflowSigned(EncoderX64& pp, ByteCodeInstruction* ip, const char* msg)
{
    bool nw = (ip->node->attributeFlags & ATTRIBUTE_CAN_OVERFLOW_ON) || (ip->flags & BCI_CAN_OVERFLOW) ? false : true;
    if (nw && module->mustEmitSafetyOverflow(ip->node) && !(ip->flags & BCI_CANT_OVERFLOW))
    {
        pp.emit_LongJumpOp(JNO);
        pp.concat.addU32(0);
        auto addr      = (uint32_t*) pp.concat.getSeekPtr() - 1;
        auto prevCount = pp.concat.totalCount();
        emitInternalPanic(pp, ip->node, msg);
        *addr = pp.concat.totalCount() - prevCount;
    }
}

void BackendSCBE::emitOverflowUnsigned(EncoderX64& pp, ByteCodeInstruction* ip, const char* msg)
{
    bool nw = (ip->node->attributeFlags & ATTRIBUTE_CAN_OVERFLOW_ON) || (ip->flags & BCI_CAN_OVERFLOW) ? false : true;
    if (nw && module->mustEmitSafetyOverflow(ip->node) && !(ip->flags & BCI_CANT_OVERFLOW))
    {
        pp.emit_LongJumpOp(JAE);
        pp.concat.addU32(0);
        auto addr      = (uint32_t*) pp.concat.getSeekPtr() - 1;
        auto prevCount = pp.concat.totalCount();
        emitInternalPanic(pp, ip->node, msg);
        *addr = pp.concat.totalCount() - prevCount;
    }
}

void BackendSCBE::emitInternalPanic(EncoderX64& pp, AstNode* node, const char* msg)
{
    auto np = node->sourceFile->path.string();
    pp.pushParams.clear();
    pp.pushParams.push_back({CPUPushParamType::GlobalString, (uint64_t) np.c_str()});
    pp.pushParams.push_back({CPUPushParamType::Imm, node->token.startLocation.line});
    pp.pushParams.push_back({CPUPushParamType::Imm, node->token.startLocation.column});
    if (msg)
        pp.pushParams.push_back({CPUPushParamType::GlobalString, (uint64_t) msg});
    else
        pp.pushParams.push_back({CPUPushParamType::Imm, 0});
    emitInternalCallExt(pp, module, g_LangSpec->name__panic, pp.pushParams);
}

void BackendSCBE::emitSymbolRelocation(EncoderX64& pp, const Utf8& name)
{
    auto& concat  = pp.concat;
    auto  callSym = pp.getOrAddSymbol(name, CoffSymbolKind::Extern);
    if (callSym->kind == CoffSymbolKind::Function)
    {
        concat.addS32((callSym->value + pp.textSectionOffset) - (concat.totalCount() + 4));
    }
    else
    {
        CoffRelocation reloc;
        reloc.virtualAddress = concat.totalCount() - pp.textSectionOffset;
        reloc.symbolIndex    = callSym->index;
        reloc.type           = IMAGE_REL_AMD64_REL32;
        pp.relocTableTextSection.table.push_back(reloc);
        concat.addU32(0);
    }
}

void BackendSCBE::emitBinOpFloat32(EncoderX64& pp, ByteCodeInstruction* ip, CPUOp op)
{
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
    {
        pp.emit_LoadF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
        pp.concat.addU8(0xF3);
        pp.concat.addU8(0x0F);
        pp.concat.addU8((uint8_t) op);
        pp.emit_ModRM(REG_OFFSET(ip->b.u32), XMM0, RDI);
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
        {
            pp.emit_Load32_Immediate(RAX, ip->a.u32);
            pp.emit_CopyF32(XMM0, RAX);
        }
        else
            pp.emit_LoadF32_Indirect(REG_OFFSET(ip->a.u32), XMM0);
        if (ip->flags & BCI_IMM_B)
        {
            pp.emit_Load32_Immediate(RAX, ip->b.u32);
            pp.emit_CopyF32(XMM1, RAX);
        }
        else
            pp.emit_LoadF32_Indirect(REG_OFFSET(ip->b.u32), XMM1);
        pp.concat.addU8(0xF3);
        pp.concat.addU8(0x0F);
        pp.concat.addU8((uint8_t) op);
        pp.concat.addU8(0xC1);
    }
}

void BackendSCBE::emitBinOpFloat64(EncoderX64& pp, ByteCodeInstruction* ip, CPUOp op)
{
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
    {
        pp.emit_LoadF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
        pp.concat.addU8(0xF2);
        pp.concat.addU8(0x0F);
        pp.concat.addU8((uint8_t) op);
        pp.emit_ModRM(REG_OFFSET(ip->b.u32), XMM0, RDI);
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
        {
            pp.emit_Load64_Immediate(RAX, ip->a.u64);
            pp.emit_CopyF64(XMM0, RAX);
        }
        else
            pp.emit_LoadF64_Indirect(REG_OFFSET(ip->a.u32), XMM0);
        if (ip->flags & BCI_IMM_B)
        {
            pp.emit_Load64_Immediate(RAX, ip->b.u64);
            pp.emit_CopyF64(XMM1, RAX);
        }
        else
            pp.emit_LoadF64_Indirect(REG_OFFSET(ip->b.u32), XMM1);
        pp.concat.addU8(0xF2);
        pp.concat.addU8(0x0F);
        pp.concat.addU8((uint8_t) op);
        pp.concat.addU8(0xC1);
    }
}

void BackendSCBE::emitBinOpFloat32AtReg(EncoderX64& pp, ByteCodeInstruction* ip, CPUOp op)
{
    emitBinOpFloat32(pp, ip, op);
    pp.emit_StoreF32_Indirect(REG_OFFSET(ip->c.u32), XMM0);
}

void BackendSCBE::emitBinOpFloat64AtReg(EncoderX64& pp, ByteCodeInstruction* ip, CPUOp op)
{
    emitBinOpFloat64(pp, ip, op);
    pp.emit_StoreF64_Indirect(REG_OFFSET(ip->c.u32), XMM0);
}

void BackendSCBE::emitBinOpIntN(EncoderX64& pp, ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
    {
        pp.emit_LoadN_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);
        pp.emit_REX(numBits);
        if (op == CPUOp::MUL)
        {
            if (numBits == CPUBits::B8)
                pp.concat.addString1("\xF6");
            else
                pp.concat.addString1("\xF7");
            pp.emit_ModRM(REG_OFFSET(ip->b.u32), 4, RDI);
        }
        else if (op == CPUOp::IMUL)
        {
            if (numBits == CPUBits::B8)
            {
                pp.concat.addString1("\xF6");
                pp.emit_ModRM(REG_OFFSET(ip->b.u32), 5, RDI);
            }
            else if (numBits == CPUBits::B16)
            {
                pp.concat.addString1("\xF7");
                pp.emit_ModRM(REG_OFFSET(ip->b.u32), 5, RDI);
            }
            else
            {
                pp.concat.addString2("\x0F\xAF");
                pp.emit_ModRM(REG_OFFSET(ip->b.u32), RAX, RDI);
            }
        }
        else
        {
            if (numBits == CPUBits::B8)
                pp.concat.addU8((uint8_t) op + 1);
            else
                pp.concat.addU8((uint8_t) op | 2);
            pp.emit_ModRM(REG_OFFSET(ip->b.u32), RAX, RDI);
        }
    }
    // Mul by power of 2 => shift by log2
    else if (op == CPUOp::MUL && !(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B) && isPowerOfTwo(ip->b.u64) && (ip->b.u64 < (uint64_t) numBits))
    {
        pp.emit_LoadN_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);
        pp.emit_Load8_Immediate(RCX, (uint8_t) log2(ip->b.u64));
        pp.emit_REX(numBits);
        pp.concat.addString2("\xD3\xE0"); // shl rax, cl
    }
    else if ((op == CPUOp::AND || op == CPUOp::OR || op == CPUOp::XOR || op == CPUOp::ADD || op == CPUOp::SUB) && !(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B) && (ip->b.u64 <= 0x7FFFFFFF))
    {
        pp.emit_LoadN_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);
        pp.emit_REX(numBits);
        if (ip->b.u32 <= 0x7F)
        {
            if (numBits == CPUBits::B8)
            {
                pp.concat.addU8((uint8_t) op + 3);
            }
            else
            {
                pp.concat.addU8(0x83);
                pp.concat.addU8(0xBF + (uint8_t) op);
            }
            pp.concat.addU8(ip->b.u8);
        }
        else
        {
            switch (numBits)
            {
            case CPUBits::B8:
                pp.concat.addU8((uint8_t) op + 3);
                pp.concat.addU8(ip->b.u8);
                break;
            case CPUBits::B16:
                pp.concat.addU8(0x81);
                pp.concat.addU8(0xBF + (uint8_t) op);
                pp.concat.addU16(ip->b.u16);
                break;
            case CPUBits::B32:
            case CPUBits::B64:
                pp.concat.addU8(0x81);
                pp.concat.addU8(0xBF + (uint8_t) op);
                pp.concat.addU32(ip->b.u32);
                break;
            }
        }
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
            pp.emit_LoadN_Immediate(RAX, ip->a.u64, numBits);
        else
            pp.emit_LoadN_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI, numBits);
        if (ip->flags & BCI_IMM_B)
            pp.emit_LoadN_Immediate(RCX, ip->b.u64, numBits);
        else
            pp.emit_LoadN_Indirect(REG_OFFSET(ip->b.u32), RCX, RDI, numBits);

        if (op == CPUOp::MUL)
        {
            pp.emit_REX(numBits);
            if (numBits == CPUBits::B8 || numBits == CPUBits::B16)
                pp.concat.addString2("\xF6\xE1");
            else
                pp.concat.addString2("\xF7\xE1");
        }
        else if (op == CPUOp::IMUL)
        {
            pp.emit_REX(numBits);
            if (numBits == CPUBits::B8 || numBits == CPUBits::B16)
                pp.concat.addString2("\xF6\xE9");
            else
                pp.concat.addString3("\x0F\xAF\xC1");
        }
        else
        {
            pp.emit_OpN(RCX, RAX, op, numBits);
        }
    }
}

void BackendSCBE::emitBinOpIntNAtReg(EncoderX64& pp, ByteCodeInstruction* ip, CPUOp op, CPUBits numBits)
{
    emitBinOpIntN(pp, ip, op, numBits);
    pp.emit_StoreN_Indirect(REG_OFFSET(ip->c.u32), RAX, RDI, numBits);
}

void BackendSCBE::emitBinOpDivIntNAtReg(EncoderX64& pp, ByteCodeInstruction* ip, bool isSigned, CPUBits numBits, bool modulo)
{
    switch (numBits)
    {
    case CPUBits::B8:
        if (ip->flags & BCI_IMM_A)
            pp.emit_Load32_Immediate(RAX, (uint32_t) ip->a.u8);
        else if (isSigned)
            pp.emit_LoadS8S32_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
        else
            pp.emit_LoadU8U32_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI);
        break;
    case CPUBits::B16:
        if (ip->flags & BCI_IMM_A)
            pp.emit_Load16_Immediate(RAX, ip->a.u16);
        else
            pp.emit_Load16_Indirect(REG_OFFSET(ip->a.u32), RAX);
        if (isSigned)
            pp.emit_Cwd();
        else
            pp.emit_ClearN(RDX, CPUBits::B16);
        break;
    case CPUBits::B32:
        if (ip->flags & BCI_IMM_A)
            pp.emit_Load32_Immediate(RAX, ip->a.u32);
        else
            pp.emit_Load32_Indirect(REG_OFFSET(ip->a.u32), RAX);
        if (isSigned)
            pp.emit_Cdq();
        else
            pp.emit_ClearN(RDX, CPUBits::B32);
        break;
    case CPUBits::B64:
        if (ip->flags & BCI_IMM_A)
            pp.emit_Load64_Immediate(RAX, ip->a.u64);
        else
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RAX);
        if (isSigned)
            pp.emit_Cqo();
        else
            pp.emit_ClearN(RDX, CPUBits::B64);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    if (ip->flags & BCI_IMM_B)
    {
        pp.emit_LoadN_Immediate(RCX, ip->b.u64, numBits);
        pp.emit_OpN(RAX, RCX, isSigned ? CPUOp::IDIV : CPUOp::DIV, numBits);
    }
    else
    {
        // div [rdi+?]
        pp.emit_REX(numBits);
        if (numBits == CPUBits::B8)
            pp.concat.addU8(0xF6);
        else
            pp.concat.addU8(0xF7);

        uint32_t offsetStack = ip->b.u32 * sizeof(Register);
        if (offsetStack == 0)
        {
            pp.concat.addU8(0x37 | (isSigned ? 0b1000 : 0));
        }
        else if (offsetStack <= 0x7F)
        {
            pp.concat.addU8(0x77 | (isSigned ? 0b1000 : 0));
            pp.concat.addU8((uint8_t) offsetStack);
        }
        else
        {
            pp.concat.addU8(0xB7 | (isSigned ? 0b1000 : 0));
            pp.concat.addU32(offsetStack);
        }
    }

    // modulo in 8 bits stores the reminder in AH and not RDX
    if (modulo)
    {
        if (numBits == CPUBits::B8)
            pp.concat.addString2("\x88\xE2"); // mov dl, ah
        pp.emit_StoreN_Indirect(REG_OFFSET(ip->c.u32), RDX, RDI, numBits);
    }
    else
    {
        pp.emit_StoreN_Indirect(REG_OFFSET(ip->c.u32), RAX, RDI, numBits);
    }
}

void BackendSCBE::emitAddSubMul64(EncoderX64& pp, ByteCodeInstruction* ip, uint64_t mul, CPUOp op)
{
    SWAG_ASSERT(op == CPUOp::ADD || op == CPUOp::SUB);

    auto val = ip->b.u64 * mul;
    if (ip->flags & BCI_IMM_B && val <= 0x7FFFFFFF && ip->a.u32 == ip->c.u32)
    {
        pp.emit_OpN_IndirectDst(REG_OFFSET(ip->a.u32), (uint32_t) val, RDI, op, CPUBits::B64);
    }
    else
    {
        if (ip->flags & BCI_IMM_B)
            pp.emit_Load64_Immediate(RAX, val);
        else
        {
            pp.emit_Load64_Indirect(REG_OFFSET(ip->b.u32), RAX);
            pp.emit_OpN_Immediate(RAX, mul, CPUOp::IMUL, CPUBits::B64);
        }

        if (ip->a.u32 == ip->c.u32)
            pp.emit_OpN_Indirect(REG_OFFSET(ip->a.u32), RAX, RDI, op, CPUBits::B64);
        else
        {
            pp.emit_Load64_Indirect(REG_OFFSET(ip->a.u32), RCX);
            pp.emit_OpN(RAX, RCX, op, CPUBits::B64);
            pp.emit_Store64_Indirect(REG_OFFSET(ip->c.u32), RCX);
        }
    }
}
