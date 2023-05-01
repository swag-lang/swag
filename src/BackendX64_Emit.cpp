#include "pch.h"
#include "BackendX64.h"
#include "ByteCode.h"
#include "LanguageSpec.h"
#include "Math.h"

void BackendX64::emitShiftRightArithmetic(X64Gen& pp, ByteCodeInstruction* ip, X64Bits numBits)
{
    if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B))
    {
        pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, numBits);
        pp.emit_ShiftN(RAX, min(ip->b.u32, (uint32_t) numBits - 1), numBits, X64Op::SAR);
    }
    else
    {
        if (ip->flags & BCI_IMM_B)
            pp.emit_Load8_Immediate(RCX, (uint8_t) min(ip->b.u32, (uint32_t) numBits - 1));
        else
        {
            pp.emit_Load32_Indirect(regOffset(ip->b.u32), RCX);
            pp.emit_Load32_Immediate(RAX, (uint32_t) numBits - 1);
            pp.emit_CmpN_Immediate(RCX, {(uint32_t) numBits - 1}, X64Bits::B32);
            pp.emit_CMovN(RCX, RAX, numBits, X64Op::CMOVG);
        }

        if (ip->flags & BCI_IMM_A)
            pp.emit_LoadN_Immediate(RAX, ip->a, numBits);
        else
            pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, numBits);

        switch (numBits)
        {
        case X64Bits::B8:
            pp.concat.addString2("\xD2\xF8"); // sar al, cl
            break;
        case X64Bits::B16:
            pp.concat.addString3("\x66\xD3\xF8"); // sar ax, cl
            break;
        case X64Bits::B32:
            pp.concat.addString2("\xD3\xF8"); // sar eax, cl
            break;
        case X64Bits::B64:
            pp.concat.addString3("\x48\xD3\xF8"); // sar rax, cl
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
    }

    pp.emit_StoreN_Indirect(regOffset(ip->c.u32), RAX, RDI, numBits);
}

void BackendX64::emitShiftRightEqArithmetic(X64Gen& pp, ByteCodeInstruction* ip, X64Bits numBits)
{
    if (ip->flags & BCI_IMM_B)
    {
        pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
        switch (numBits)
        {
        case X64Bits::B8:
            pp.concat.addString2("\xC0\x38");
            break;
        case X64Bits::B16:
            pp.concat.addString3("\x66\xC1\x38");
            break;
        case X64Bits::B32:
            pp.concat.addString2("\xC1\x38");
            break;
        case X64Bits::B64:
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
        pp.emit_Load32_Indirect(regOffset(ip->b.u32), RCX);
        pp.emit_Load32_Immediate(RAX, (uint32_t) numBits - 1);
        pp.emit_CmpN_Immediate(RCX, {(uint32_t) numBits - 1}, X64Bits::B32);
        pp.emit_CMovN(RCX, RAX, numBits, X64Op::CMOVG);

        pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
        switch (numBits)
        {
        case X64Bits::B8:
            pp.concat.addString2("\xd2\x38");
            break;
        case X64Bits::B16:
            pp.concat.addString3("\x66\xd3\x38");
            break;
        case X64Bits::B32:
            pp.concat.addString2("\xd3\x38");
            break;
        case X64Bits::B64:
            pp.concat.addString3("\x48\xd3\x38");
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
    }
}

void BackendX64::emitShiftLogical(X64Gen& pp, ByteCodeInstruction* ip, X64Bits numBits, X64Op op)
{
    if ((ip->flags & BCI_IMM_B) && ip->b.u32 >= (uint32_t) numBits)
    {
        pp.emit_ClearN(RAX, numBits);
        pp.emit_StoreN_Indirect(regOffset(ip->c.u32), RAX, RDI, numBits);
    }
    else if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B))
    {
        pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, numBits);
        pp.emit_ShiftN(RAX, ip->b.u8, numBits, op);
        pp.emit_StoreN_Indirect(regOffset(ip->c.u32), RAX, RDI, numBits);
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
            pp.emit_LoadN_Immediate(RAX, ip->a, numBits);
        else
            pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, numBits);

        if (ip->flags & BCI_IMM_B)
            pp.emit_Load8_Immediate(RCX, ip->b.u8);
        else
            pp.emit_Load32_Indirect(regOffset(ip->b.u32), RCX);

        switch (numBits)
        {
        case X64Bits::B8:
            pp.concat.addString1("\xD2");
            break;
        case X64Bits::B16:
            pp.concat.addString2("\x66\xD3");
            break;
        case X64Bits::B32:
            pp.concat.addString1("\xD3");
            break;
        case X64Bits::B64:
            pp.concat.addString2("\x48\xD3");
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }

        pp.concat.addU8((uint8_t) op);

        pp.emit_ClearN(R8, numBits);
        pp.emit_CmpN_Immediate(RCX, {(uint32_t) numBits - 1}, X64Bits::B32);
        pp.emit_CMovN(RAX, R8, numBits, X64Op::CMOVG);
        pp.emit_StoreN_Indirect(regOffset(ip->c.u32), RAX, RDI, numBits);
    }
}

void BackendX64::emitShiftEqLogical(X64Gen& pp, ByteCodeInstruction* ip, X64Bits numBits, X64Op op)
{
    pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
    if ((ip->flags & BCI_IMM_B) && ip->b.u32 >= (uint32_t) numBits)
    {
        pp.emit_ClearN(RCX, numBits);
        pp.emit_StoreN_Indirect(0, RCX, RAX, numBits);
    }
    else if (ip->flags & BCI_IMM_B)
    {
        pp.emit_ShiftN(RAX, ip->b.u8, numBits, op);
    }
    else
    {
        pp.emit_Load32_Indirect(regOffset(ip->b.u32), RCX);
        pp.emit_CmpN_Immediate(RCX, {(uint32_t) numBits}, X64Bits::B32);
        pp.emit_NearJumpOp(JL);
        pp.concat.addU8(0); // clear + store below
        auto seekPtr = pp.concat.getSeekPtr() - 1;
        auto seekJmp = pp.concat.totalCount();
        pp.emit_ClearN(RCX, numBits);
        pp.emit_StoreN_Indirect(0, RCX, RAX, numBits);
        *seekPtr = (uint8_t) (pp.concat.totalCount() - seekJmp);

        switch (numBits)
        {
        case X64Bits::B8:
            pp.concat.addString1("\xd2");
            break;
        case X64Bits::B16:
            pp.concat.addString2("\x66\xd3");
            break;
        case X64Bits::B32:
            pp.concat.addString1("\xd3");
            break;
        case X64Bits::B64:
            pp.concat.addString2("\x48\xd3");
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }

        pp.concat.addU8((uint8_t) op);
    }
}

void BackendX64::emitOverflowSigned(X64Gen& pp, AstNode* node, const char* msg)
{
    if (!module->mustEmitSafetyOverflow(node))
        return;
    pp.emit_LongJumpOp(JNO);
    pp.concat.addU32(0);
    auto addr      = (uint32_t*) pp.concat.getSeekPtr() - 1;
    auto prevCount = pp.concat.totalCount();
    emitInternalPanic(pp, node, msg);
    *addr = pp.concat.totalCount() - prevCount;
}

void BackendX64::emitOverflowUnsigned(X64Gen& pp, AstNode* node, const char* msg)
{
    if (!module->mustEmitSafetyOverflow(node))
        return;
    pp.emit_LongJumpOp(JAE);
    pp.concat.addU32(0);
    auto addr      = (uint32_t*) pp.concat.getSeekPtr() - 1;
    auto prevCount = pp.concat.totalCount();
    emitInternalPanic(pp, node, msg);
    *addr = pp.concat.totalCount() - prevCount;
}

void BackendX64::emitInternalPanic(X64Gen& pp, AstNode* node, const char* msg)
{
    auto np = node->sourceFile->path.string();
    pp.pushParams.clear();
    pp.pushParams.push_back({X64PushParamType::GlobalString, (uint64_t) np.c_str()});
    pp.pushParams.push_back({X64PushParamType::Imm, node->token.startLocation.line});
    pp.pushParams.push_back({X64PushParamType::Imm, node->token.startLocation.column});
    if (msg)
        pp.pushParams.push_back({X64PushParamType::GlobalString, (uint64_t) msg});
    else
        pp.pushParams.push_back({X64PushParamType::Imm, 0});
    emitInternalCallExt(pp, module, g_LangSpec->name__panic, pp.pushParams);
}

void BackendX64::emitSymbolRelocation(X64Gen& pp, const Utf8& name)
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

void BackendX64::emitBinOpFloat32(X64Gen& pp, ByteCodeInstruction* ip, X64Op op)
{
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
    {
        pp.emit_LoadF32_Indirect(regOffset(ip->a.u32), XMM0);
        pp.concat.addU8(0xF3);
        pp.concat.addU8(0x0F);
        pp.concat.addU8((uint8_t) op);
        pp.emit_ModRM(regOffset(ip->b.u32), XMM0, RDI);
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
        {
            pp.emit_Load32_Immediate(RAX, ip->a.u32);
            pp.emit_CopyF32(XMM0, RAX);
        }
        else
            pp.emit_LoadF32_Indirect(regOffset(ip->a.u32), XMM0);
        if (ip->flags & BCI_IMM_B)
        {
            pp.emit_Load32_Immediate(RAX, ip->b.u32);
            pp.emit_CopyF32(XMM1, RAX);
        }
        else
            pp.emit_LoadF32_Indirect(regOffset(ip->b.u32), XMM1);
        pp.concat.addU8(0xF3);
        pp.concat.addU8(0x0F);
        pp.concat.addU8((uint8_t) op);
        pp.concat.addU8(0xC1);
    }
}

void BackendX64::emitBinOpFloat64(X64Gen& pp, ByteCodeInstruction* ip, X64Op op)
{
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
    {
        pp.emit_LoadF64_Indirect(regOffset(ip->a.u32), XMM0);
        pp.concat.addU8(0xF2);
        pp.concat.addU8(0x0F);
        pp.concat.addU8((uint8_t) op);
        pp.emit_ModRM(regOffset(ip->b.u32), XMM0, RDI);
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
        {
            pp.emit_Load64_Immediate(RAX, ip->a.u64);
            pp.emit_CopyF64(XMM0, RAX);
        }
        else
            pp.emit_LoadF64_Indirect(regOffset(ip->a.u32), XMM0);
        if (ip->flags & BCI_IMM_B)
        {
            pp.emit_Load64_Immediate(RAX, ip->b.u64);
            pp.emit_CopyF64(XMM1, RAX);
        }
        else
            pp.emit_LoadF64_Indirect(regOffset(ip->b.u32), XMM1);
        pp.concat.addU8(0xF2);
        pp.concat.addU8(0x0F);
        pp.concat.addU8((uint8_t) op);
        pp.concat.addU8(0xC1);
    }
}

void BackendX64::emitBinOpFloat32AtReg(X64Gen& pp, ByteCodeInstruction* ip, X64Op op)
{
    emitBinOpFloat32(pp, ip, op);
    pp.emit_StoreF32_Indirect(regOffset(ip->c.u32), XMM0);
}

void BackendX64::emitBinOpFloat64AtReg(X64Gen& pp, ByteCodeInstruction* ip, X64Op op)
{
    emitBinOpFloat64(pp, ip, op);
    pp.emit_StoreF64_Indirect(regOffset(ip->c.u32), XMM0);
}

void BackendX64::emitBinOpIntN(X64Gen& pp, ByteCodeInstruction* ip, X64Op op, X64Bits numBits)
{
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
    {
        pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, numBits);
        pp.emit_REX(numBits);
        if (op == X64Op::MUL)
        {
            if (numBits == X64Bits::B8)
                pp.concat.addString1("\xF6");
            else
                pp.concat.addString1("\xF7");
            pp.emit_ModRM(regOffset(ip->b.u32), 4, RDI);
        }
        else if (op == X64Op::IMUL)
        {
            if (numBits == X64Bits::B8)
            {
                pp.concat.addString1("\xF6");
                pp.emit_ModRM(regOffset(ip->b.u32), 5, RDI);
            }
            else if (numBits == X64Bits::B16)
            {
                pp.concat.addString1("\xF7");
                pp.emit_ModRM(regOffset(ip->b.u32), 5, RDI);
            }
            else
            {
                pp.concat.addString2("\x0F\xAF");
                pp.emit_ModRM(regOffset(ip->b.u32), RAX, RDI);
            }
        }
        else
        {
            if (numBits == X64Bits::B8)
                pp.concat.addU8((uint8_t) op + 1);
            else
                pp.concat.addU8((uint8_t) op | 2);
            pp.emit_ModRM(regOffset(ip->b.u32), RAX, RDI);
        }
    }
    // Mul by power of 2 => shift by log2
    else if (op == X64Op::MUL && !(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B) && isPowerOfTwo(ip->b.u64) && (ip->b.u64 < (uint64_t) numBits))
    {
        pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, numBits);
        pp.emit_Load8_Immediate(RCX, (uint8_t) log2(ip->b.u64));
        pp.emit_REX(numBits);
        pp.concat.addString2("\xD3\xE0"); // shl rax, cl
    }
    else if ((op == X64Op::AND || op == X64Op::OR || op == X64Op::XOR || op == X64Op::ADD || op == X64Op::SUB) && !(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B) && (ip->b.u64 <= 0x7FFFFFFF))
    {
        pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, numBits);
        pp.emit_REX(numBits);
        if (ip->b.u32 <= 0x7F)
        {
            if (numBits == X64Bits::B8)
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
            case X64Bits::B8:
                pp.concat.addU8((uint8_t) op + 3);
                pp.concat.addU8(ip->b.u8);
                break;
            case X64Bits::B16:
                pp.concat.addU8(0x81);
                pp.concat.addU8(0xBF + (uint8_t) op);
                pp.concat.addU16(ip->b.u16);
                break;
            case X64Bits::B32:
            case X64Bits::B64:
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
            pp.emit_LoadN_Immediate(RAX, ip->a, numBits);
        else
            pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, numBits);
        if (ip->flags & BCI_IMM_B)
            pp.emit_LoadN_Immediate(RCX, ip->b, numBits);
        else
            pp.emit_LoadN_Indirect(regOffset(ip->b.u32), RCX, RDI, numBits);

        if (op == X64Op::MUL)
        {
            pp.emit_REX(numBits);
            if (numBits == X64Bits::B8 || numBits == X64Bits::B16)
                pp.concat.addString2("\xF6\xE1");
            else
                pp.concat.addString2("\xF7\xE1");
        }
        else if (op == X64Op::IMUL)
        {
            pp.emit_REX(numBits);
            if (numBits == X64Bits::B8 || numBits == X64Bits::B16)
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

void BackendX64::emitBinOpIntNAtReg(X64Gen& pp, ByteCodeInstruction* ip, X64Op op, X64Bits numBits)
{
    emitBinOpIntN(pp, ip, op, numBits);
    pp.emit_StoreN_Indirect(regOffset(ip->c.u32), RAX, RDI, numBits);
}

void BackendX64::emitBinOpDivIntNAtReg(X64Gen& pp, ByteCodeInstruction* ip, bool isSigned, X64Bits numBits, bool modulo)
{
    switch (numBits)
    {
    case X64Bits::B8:
        if (ip->flags & BCI_IMM_A)
            pp.emit_Load32_Immediate(RAX, (uint32_t) ip->a.u8);
        else if (isSigned)
            pp.emit_LoadS8S32_Indirect(regOffset(ip->a.u32), RAX, RDI);
        else
            pp.emit_LoadU8U32_Indirect(regOffset(ip->a.u32), RAX, RDI);
        break;
    case X64Bits::B16:
        if (ip->flags & BCI_IMM_A)
            pp.emit_Load16_Immediate(RAX, ip->a.u16);
        else
            pp.emit_Load16_Indirect(regOffset(ip->a.u32), RAX);
        if (isSigned)
            pp.emit_Cwd();
        else
            pp.emit_ClearN(RDX, X64Bits::B16);
        break;
    case X64Bits::B32:
        if (ip->flags & BCI_IMM_A)
            pp.emit_Load32_Immediate(RAX, ip->a.u32);
        else
            pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX);
        if (isSigned)
            pp.emit_Cdq();
        else
            pp.emit_ClearN(RDX, X64Bits::B32);
        break;
    case X64Bits::B64:
        if (ip->flags & BCI_IMM_A)
            pp.emit_Load64_Immediate(RAX, ip->a.u64);
        else
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
        if (isSigned)
            pp.emit_Cqo();
        else
            pp.emit_ClearN(RDX, X64Bits::B64);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    if (ip->flags & BCI_IMM_B)
    {
        pp.emit_LoadN_Immediate(RCX, ip->b, numBits);
        pp.emit_OpN(RAX, RCX, isSigned ? X64Op::IDIV : X64Op::DIV, numBits);
    }
    else
    {
        // div [rdi+?]
        pp.emit_REX(numBits);
        if (numBits == X64Bits::B8)
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
        if (numBits == X64Bits::B8)
            pp.concat.addString2("\x88\xE2"); // mov dl, ah
        pp.emit_StoreN_Indirect(regOffset(ip->c.u32), RDX, RDI, numBits);
    }
    else
    {
        pp.emit_StoreN_Indirect(regOffset(ip->c.u32), RAX, RDI, numBits);
    }
}

void BackendX64::emitAddSubMul64(X64Gen& pp, ByteCodeInstruction* ip, uint64_t mul, X64Op op)
{
    auto val = ip->b.u64 * mul;
    if (ip->flags & BCI_IMM_B && val <= 0x7FFFFFFF && ip->a.u32 == ip->c.u32)
    {
        pp.emit_REX(X64Bits::B64);
        if (val <= 0x7F)
            pp.concat.addU8(0x83);
        else
            pp.concat.addU8(0x81);
        if (regOffset(ip->a.u32) <= 0x7F)
        {
            pp.concat.addU8(0x46 + (uint8_t) op);
            pp.concat.addU8((uint8_t) regOffset(ip->a.u32));
        }
        else
        {
            pp.concat.addU8(0x86 + (uint8_t) op);
            pp.concat.addU32(regOffset(ip->a.u32));
        }
        if (val <= 0x7F)
            pp.concat.addU8((uint8_t) val);
        else
            pp.concat.addU32((uint32_t) val);
    }
    else
    {
        if (ip->flags & BCI_IMM_B)
            pp.emit_Load64_Immediate(RAX, val);
        else
        {
            pp.emit_Load64_Indirect(regOffset(ip->b.u32), RAX);
            if (mul != 1 && isPowerOfTwo(mul))
            {
                pp.concat.addString3("\x48\xC1\xE0"); // shl rax, ?
                pp.concat.addU8((uint8_t) log2(mul));
            }
            else if (mul != 1)
            {
                pp.emit_Load64_Immediate(RCX, mul);
                pp.concat.addString3("\x48\xF7\xE1"); // mul rcx
            }
        }

        if (ip->a.u32 == ip->c.u32)
            pp.emit_OpN_Indirect(regOffset(ip->a.u32), RAX, RDI, op, X64Bits::B64);
        else
        {
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_OpN(RAX, RCX, op, X64Bits::B64);
            pp.emit_Store64_Indirect(regOffset(ip->c.u32), RCX);
        }
    }
}
