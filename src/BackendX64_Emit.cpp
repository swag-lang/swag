#include "pch.h"
#include "BackendX64.h"
#include "ByteCode.h"
#include "LanguageSpec.h"
#include "Math.h"

void BackendX64::emitAddSubMul64(X64Gen& pp, ByteCodeInstruction* ip, uint64_t mul, X64Op op)
{
    auto val = ip->b.u64 * mul;
    if (ip->flags & BCI_IMM_B && val <= 0x7FFFFFFF && ip->a.u32 == ip->c.u32)
    {
        pp.emit_REX();
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
            pp.emit_Load64_Immediate(val, RAX);
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
                pp.emit_Load64_Immediate(mul, RCX);
                pp.concat.addString3("\x48\xF7\xE1"); // mul rcx
            }
        }

        if (ip->a.u32 == ip->c.u32)
            pp.emit_Op64_Indirect(regOffset(ip->a.u32), RAX, RDI, op);
        else
        {
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RCX);
            pp.emit_Op64(RAX, RCX, op);
            pp.emit_Store64_Indirect(regOffset(ip->c.u32), RCX);
        }
    }
}

void BackendX64::emitShiftArithmetic(X64Gen& pp, ByteCodeInstruction* ip, uint8_t numBits)
{
    if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B) && ip->b.u32 >= numBits)
    {
        pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, numBits);
        switch (numBits)
        {
        case 8:
            pp.concat.addString2("\xC0\xF8"); // sar al
            break;
        case 16:
            pp.concat.addString3("\x66\xC1\xF8"); // sar ax
            break;
        case 32:
            pp.concat.addString2("\xC1\xF8"); // sar eax
            break;
        case 64:
            pp.concat.addString3("\x48\xC1\xF8"); // sar rax
            break;
        }

        pp.concat.addU8(numBits - 1);
    }
    else if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B) && ip->b.u32 == 1)
    {
        pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, numBits);
        switch (numBits)
        {
        case 8:
            pp.concat.addString2("\xD0\xF8"); // sar al, 1
            break;
        case 16:
            pp.concat.addString3("\x66\xD1\xF8"); // sar ax, 1
            break;
        case 32:
            pp.concat.addString2("\xD1\xF8"); // sar eax, 1
            break;
        case 64:
            pp.concat.addString3("\x48\xD1\xF8"); // sar rax, 1
            break;
        }
    }
    else if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B))
    {
        pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, numBits);
        switch (numBits)
        {
        case 8:
            pp.concat.addString2("\xC0\xF8"); // sar al
            break;
        case 16:
            pp.concat.addString3("\x66\xC1\xF8"); // sar ax
            break;
        case 32:
            pp.concat.addString2("\xC1\xF8"); // sar eax
            break;
        case 64:
            pp.concat.addString3("\x48\xC1\xF8"); // sar rax
            break;
        }

        pp.concat.addU8(ip->b.u8 & (numBits - 1));
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
            pp.emit_LoadN_Immediate(ip->a, RAX, numBits);
        else
            pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, numBits);
        if (ip->flags & BCI_IMM_B)
            pp.emit_Load8_Immediate(ip->b.u8 & (numBits - 1), RCX);
        else
        {
            pp.emit_Load32_Indirect(regOffset(ip->b.u32), RCX);
            pp.concat.addString2("\x83\xF9"); // cmp ecx, ??
            pp.concat.addU8(numBits);
            pp.emit_NearJumpOp(JL);
            pp.concat.addU8(0); // mov below
            auto seekPtr = pp.concat.getSeekPtr() - 1;
            auto seekJmp = pp.concat.totalCount();
            pp.emit_Load8_Immediate(numBits - 1, RCX);
            *seekPtr = (uint8_t) (pp.concat.totalCount() - seekJmp);
        }

        switch (numBits)
        {
        case 8:
            pp.concat.addString2("\xD2\xF8"); // sar al, cl
            break;
        case 16:
            pp.concat.addString3("\x66\xD3\xF8"); // sar ax, cl
            break;
        case 32:
            pp.concat.addString2("\xD3\xF8"); // sar eax, cl
            break;
        case 64:
            pp.concat.addString3("\x48\xD3\xF8"); // sar rax, cl
            break;
        }
    }

    pp.emit_StoreN_Indirect(regOffset(ip->c.u32), RAX, RDI, numBits);
}

void BackendX64::emitShiftEqArithmetic(X64Gen& pp, ByteCodeInstruction* ip, uint8_t numBits)
{
    pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
    if (ip->flags & BCI_IMM_B)
    {
        switch (numBits)
        {
        case 8:
            pp.concat.addString2("\xC0\x38");
            break;
        case 16:
            pp.concat.addString3("\x66\xC1\x38");
            break;
        case 32:
            pp.concat.addString2("\xC1\x38");
            break;
        case 64:
            pp.concat.addString3("\x48\xC1\x38");
            break;
        }

        pp.concat.addU8(min(ip->b.u8, numBits - 1));
    }
    else
    {
        pp.emit_Load32_Indirect(regOffset(ip->b.u32), RCX);
        pp.concat.addString2("\x83\xF9"); // cmp ecx, ??
        pp.concat.addU8(numBits);
        pp.emit_NearJumpOp(JL);
        pp.concat.addU8(0); // move below
        auto seekPtr = pp.concat.getSeekPtr() - 1;
        auto seekJmp = pp.concat.totalCount();
        pp.emit_Load8_Immediate(numBits - 1, RCX);
        *seekPtr = (uint8_t) (pp.concat.totalCount() - seekJmp);

        switch (numBits)
        {
        case 8:
            pp.concat.addString2("\xd2\x38");
            break;
        case 16:
            pp.concat.addString3("\x66\xd3\x38");
            break;
        case 32:
            pp.concat.addString2("\xd3\x38");
            break;
        case 64:
            pp.concat.addString3("\x48\xd3\x38");
            break;
        }
    }
}

void BackendX64::emitShiftLogical(X64Gen& pp, ByteCodeInstruction* ip, uint8_t numBits, X64Op op)
{
    if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B) && ip->b.u32 >= numBits)
    {
        pp.emit_ClearN(RAX, numBits);
    }
    else if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B))
    {
        pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, numBits);
        pp.emit_ShiftN_Immediate(RAX, ip->b.u8, numBits, op);
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
            pp.emit_LoadN_Immediate(ip->a, RAX, numBits);
        else
            pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, numBits);

        if (ip->flags & BCI_IMM_B)
            pp.emit_Load8_Immediate(ip->b.u8 & (numBits - 1), RCX);
        else
        {
            pp.emit_Load32_Indirect(regOffset(ip->b.u32), RCX);
            pp.concat.addString2("\x83\xF9"); // cmp ecx, ?
            pp.concat.addU8(numBits);
            pp.emit_NearJumpOp(JL);
            pp.concat.addU8(0); // clear below
            auto seekPtr = pp.concat.getSeekPtr() - 1;
            auto seekJmp = pp.concat.totalCount();
            pp.emit_ClearN(RAX, numBits);
            *seekPtr = (uint8_t) (pp.concat.totalCount() - seekJmp);
        }

        switch (numBits)
        {
        case 8:
            pp.concat.addString1("\xD2");
            break;
        case 16:
            pp.concat.addString2("\x66\xD3");
            break;
        case 32:
            pp.concat.addString1("\xD3");
            break;
        case 64:
            pp.concat.addString2("\x48\xD3");
            break;
        }

        pp.concat.addU8((uint8_t) op);
    }

    pp.emit_StoreN_Indirect(regOffset(ip->c.u32), RAX, RDI, numBits);
}

void BackendX64::emitShiftEqLogical(X64Gen& pp, ByteCodeInstruction* ip, uint8_t numBits, X64Op op)
{
    pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
    if (ip->flags & BCI_IMM_B && ip->b.u32 >= numBits)
    {
        pp.emit_ClearN(RCX, numBits);
        pp.emit_StoreN_Indirect(0, RCX, RAX, numBits);
    }
    else if (ip->flags & BCI_IMM_B)
    {
        switch (numBits)
        {
        case 8:
            pp.concat.addString1("\xc0");
            break;
        case 16:
            pp.concat.addString2("\x66\xc1");
            break;
        case 32:
            pp.concat.addString1("\xc1");
            break;
        case 64:
            pp.concat.addString2("\x48\xc1");
            break;
        }

        pp.concat.addU8((uint8_t) op);
        pp.concat.addU8(ip->b.u8);
    }
    else
    {
        pp.emit_Load32_Indirect(regOffset(ip->b.u32), RCX);
        pp.concat.addString2("\x83\xF9"); // cmp ecx, ??
        pp.concat.addU8(numBits);
        pp.emit_NearJumpOp(JL);
        pp.concat.addU8(0); // clear + store below
        auto seekPtr = pp.concat.getSeekPtr() - 1;
        auto seekJmp = pp.concat.totalCount();
        pp.emit_ClearN(RCX, numBits);
        pp.emit_StoreN_Indirect(0, RCX, RAX, numBits);
        *seekPtr = (uint8_t) (pp.concat.totalCount() - seekJmp);

        switch (numBits)
        {
        case 8:
            pp.concat.addString1("\xd2");
            break;
        case 16:
            pp.concat.addString2("\x66\xd3");
            break;
        case 32:
            pp.concat.addString1("\xd3");
            break;
        case 64:
            pp.concat.addString2("\x48\xd3");
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
            pp.emit_Load32_Immediate(ip->a.u32, RAX);
            pp.emit_CopyF32(RAX, XMM0);
        }
        else
            pp.emit_LoadF32_Indirect(regOffset(ip->a.u32), XMM0);
        if (ip->flags & BCI_IMM_B)
        {
            pp.emit_Load32_Immediate(ip->b.u32, RAX);
            pp.emit_CopyF32(RAX, XMM1);
        }
        else
            pp.emit_LoadF32_Indirect(regOffset(ip->b.u32), XMM1);
        pp.concat.addU8(0xF3);
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
            pp.emit_Load64_Immediate(ip->a.u64, RAX);
            pp.emit_CopyF64(RAX, XMM0);
        }
        else
            pp.emit_LoadF64_Indirect(regOffset(ip->a.u32), XMM0);
        if (ip->flags & BCI_IMM_B)
        {
            pp.emit_Load64_Immediate(ip->b.u64, RAX);
            pp.emit_CopyF64(RAX, XMM1);
        }
        else
            pp.emit_LoadF64_Indirect(regOffset(ip->b.u32), XMM1);
        pp.concat.addU8(0xF2);
        pp.concat.addU8(0x0F);
        pp.concat.addU8((uint8_t) op);
        pp.concat.addU8(0xC1);
    }
}

void BackendX64::emitBinOpFloat64AtReg(X64Gen& pp, ByteCodeInstruction* ip, X64Op op)
{
    emitBinOpFloat64(pp, ip, op);
    pp.emit_StoreF64_Indirect(regOffset(ip->c.u32), XMM0);
}

void BackendX64::emitBinOpInt8(X64Gen& pp, ByteCodeInstruction* ip, X64Op op)
{
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
    {
        pp.emit_Load8_Indirect(regOffset(ip->a.u32), RAX);
        if (op == X64Op::MUL)
        {
            pp.concat.addString1("\xF6"); // mul
            pp.emit_ModRM(regOffset(ip->b.u32), 4, RDI);
        }
        else if (op == X64Op::IMUL)
        {
            pp.concat.addString1("\xF6"); // imul
            pp.emit_ModRM(regOffset(ip->b.u32), 5, RDI);
        }
        else
        {
            pp.concat.addU8((uint8_t) op + 1);
            pp.emit_ModRM(regOffset(ip->b.u32), RAX, RDI); // al, [rdi+?]
        }
    }
    else if ((op == X64Op::AND || op == X64Op::OR || op == X64Op::XOR || op == X64Op::ADD || op == X64Op::SUB) && !(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B))
    {
        pp.emit_Load8_Indirect(regOffset(ip->a.u32), RAX);
        pp.concat.addU8((uint8_t) op + 3);
        pp.concat.addU8(ip->b.u8);
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
            pp.emit_Load8_Immediate(ip->a.u8, RAX);
        else
            pp.emit_Load8_Indirect(regOffset(ip->a.u32), RAX);
        if (ip->flags & BCI_IMM_B)
            pp.emit_Load8_Immediate(ip->b.u8, RCX);
        else
            pp.emit_Load8_Indirect(regOffset(ip->b.u32), RCX);

        if (op == X64Op::MUL)
            pp.concat.addString2("\xF6\xE1"); // mul cl
        else if (op == X64Op::IMUL)
            pp.concat.addString2("\xF6\xE9"); // imul cl
        else
            pp.emit_Op8(RCX, RAX, op);
    }
}

void BackendX64::emitBinOpInt16(X64Gen& pp, ByteCodeInstruction* ip, X64Op op)
{
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
    {
        pp.emit_Load16_Indirect(regOffset(ip->a.u32), RAX);
        if (op == X64Op::MUL)
        {
            pp.concat.addString2("\x66\xF7"); // mul word ptr [rdi]
            pp.emit_ModRM(regOffset(ip->b.u32), 4, RDI);
        }
        else if (op == X64Op::IMUL)
        {
            pp.concat.addString2("\x66\xF7"); // imul word ptr [rdi]
            pp.emit_ModRM(regOffset(ip->b.u32), 5, RDI);
        }
        else
        {
            pp.concat.addU8(0x66);
            pp.concat.addU8((uint8_t) op | 2);
            pp.emit_ModRM(regOffset(ip->b.u32), RAX, RDI); // ax, [rdi+?]
        }
    }
    else if ((op == X64Op::AND || op == X64Op::OR || op == X64Op::XOR || op == X64Op::ADD || op == X64Op::SUB) && !(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B))
    {
        pp.emit_Load16_Indirect(regOffset(ip->a.u32), RAX);
        pp.concat.addU8((uint8_t) 0x66);
        if (ip->b.u16 <= 0x7F)
        {
            pp.concat.addU8(0x83); // op eax, ??
            pp.concat.addU8(0xBF + (uint8_t) op);
            pp.concat.addU8(ip->b.u8);
        }
        else
        {
            pp.concat.addU8(0x81); // op eax, ????????
            pp.concat.addU8(0xBF + (uint8_t) op);
            pp.concat.addU16(ip->b.u16);
        }
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
            pp.emit_Load16_Immediate(ip->a.u16, RAX);
        else
            pp.emit_Load16_Indirect(regOffset(ip->a.u32), RAX);
        if (ip->flags & BCI_IMM_B)
            pp.emit_Load16_Immediate(ip->b.u16, RCX);
        else
            pp.emit_Load16_Indirect(regOffset(ip->b.u32), RCX);

        if (op == X64Op::MUL)
            pp.concat.addString3("\x66\xF6\xE1"); // mul cx
        else if (op == X64Op::IMUL)
            pp.concat.addString3("\x66\xF6\xE9"); // imul cx
        else
            pp.emit_Op16(RCX, RAX, op);
    }
}

void BackendX64::emitBinOpIntN(X64Gen& pp, ByteCodeInstruction* ip, X64Op op, uint8_t bits)
{
    SWAG_ASSERT(bits == 32 || bits == 64);
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
    {
        pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, bits);
        pp.emit_REX(bits);
        if (op == X64Op::MUL)
        {
            pp.concat.addString1("\xF7"); // mul
            pp.emit_ModRM(regOffset(ip->b.u32), 4, RDI);
        }
        else if (op == X64Op::IMUL)
        {
            pp.concat.addString2("\x0F\xAF"); // imul
            pp.emit_ModRM(regOffset(ip->b.u32), RAX, RDI);
        }
        else
        {
            pp.concat.addU8((uint8_t) op | 2);
            pp.emit_ModRM(regOffset(ip->b.u32), RAX, RDI);
        }
    }
    // Mul by power of 2 => shift by log2
    else if (op == X64Op::MUL && !(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B) && isPowerOfTwo(ip->b.u64) && (ip->b.u64 < 64))
    {
        pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, bits);
        pp.emit_Load8_Immediate((uint8_t) log2(ip->b.u64), RCX);
        pp.emit_REX(bits);
        pp.concat.addString2("\xD3\xE0"); // shl rax, cl
    }
    else if ((op == X64Op::AND || op == X64Op::OR || op == X64Op::XOR || op == X64Op::ADD || op == X64Op::SUB) && !(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B) && (ip->b.u64 <= 0x7FFFFFFF))
    {
        pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, bits);
        pp.emit_REX(bits);
        if (ip->b.u32 <= 0x7F)
        {
            pp.concat.addU8(0x83); // op rax, ??
            pp.concat.addU8(0xBF + (uint8_t) op);
            pp.concat.addU8(ip->b.u8);
        }
        else
        {
            pp.concat.addU8(0x81); // op rax, ????????
            pp.concat.addU8(0xBF + (uint8_t) op);
            pp.concat.addU32(ip->b.u32);
        }
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
            pp.emit_LoadN_Immediate(ip->a, RAX, bits);
        else
            pp.emit_LoadN_Indirect(regOffset(ip->a.u32), RAX, RDI, bits);
        if (ip->flags & BCI_IMM_B)
            pp.emit_LoadN_Immediate(ip->b, RCX, bits);
        else
            pp.emit_LoadN_Indirect(regOffset(ip->b.u32), RCX, RDI, bits);

        if (op == X64Op::MUL)
        {
            pp.emit_REX(bits);
            pp.concat.addString2("\xF7\xE1"); // mul rcx
        }
        else if (op == X64Op::IMUL)
        {
            pp.emit_REX(bits);
            pp.concat.addString3("\x0F\xAF\xC1"); // imul rax, rcx
        }
        else
        {
            switch (bits)
            {
            case 32:
                pp.emit_Op32(RCX, RAX, op);
                break;
            case 64:
                pp.emit_Op64(RCX, RAX, op);
                break;
            default:
                SWAG_ASSERT(false);
                break;
            }
        }
    }
}

void BackendX64::emitBinOpInt8AtReg(X64Gen& pp, ByteCodeInstruction* ip, X64Op op)
{
    emitBinOpInt8(pp, ip, op);
    pp.emit_Store8_Indirect(regOffset(ip->c.u32), RAX);
}

void BackendX64::emitBinOpInt16AtReg(X64Gen& pp, ByteCodeInstruction* ip, X64Op op)
{
    emitBinOpInt16(pp, ip, op);
    pp.emit_Store16_Indirect(regOffset(ip->c.u32), RAX);
}

void BackendX64::emitBinOpInt32AtReg(X64Gen& pp, ByteCodeInstruction* ip, X64Op op)
{
    emitBinOpIntN(pp, ip, op, 32);
    pp.emit_Store32_Indirect(regOffset(ip->c.u32), RAX);
}

void BackendX64::emitBinOpInt64AtReg(X64Gen& pp, ByteCodeInstruction* ip, X64Op op)
{
    emitBinOpIntN(pp, ip, op, 64);
    pp.emit_Store64_Indirect(regOffset(ip->c.u32), RAX);
}

void BackendX64::emitBinOpIntDivAtReg(X64Gen& pp, ByteCodeInstruction* ip, bool isSigned, uint8_t bits, bool modulo)
{
    switch (bits)
    {
    case 8:
        if (ip->flags & BCI_IMM_A)
            pp.emit_Load32_Immediate((uint32_t) ip->a.u8, RAX);
        else if (isSigned)
            pp.emit_LoadS8S32_Indirect(regOffset(ip->a.u32), RAX, RDI);
        else
            pp.emit_LoadU8U32_Indirect(regOffset(ip->a.u32), RAX, RDI);
        break;
    case 16:
        if (ip->flags & BCI_IMM_A)
            pp.emit_Load16_Immediate(ip->a.u16, RAX);
        else
            pp.emit_Load16_Indirect(regOffset(ip->a.u32), RAX);
        if (isSigned)
            pp.emit_Cwd();
        else
            pp.emit_Clear16(RDX);
        break;
    case 32:
        if (ip->flags & BCI_IMM_A)
            pp.emit_Load32_Immediate(ip->a.u32, RAX);
        else
            pp.emit_Load32_Indirect(regOffset(ip->a.u32), RAX);
        if (isSigned)
            pp.emit_Cdq();
        else
            pp.emit_Clear32(RDX);
        break;
    case 64:
        if (ip->flags & BCI_IMM_A)
            pp.emit_Load64_Immediate(ip->a.u64, RAX);
        else
            pp.emit_Load64_Indirect(regOffset(ip->a.u32), RAX);
        if (isSigned)
            pp.emit_Cqo();
        else
            pp.emit_Clear64(RDX);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    if (ip->flags & BCI_IMM_B)
    {
        pp.emit_LoadN_Immediate(ip->b, RCX, bits);
        switch (bits)
        {
        case 8:
            pp.emit_Op8(RAX, RCX, isSigned ? X64Op::IDIV : X64Op::DIV);
            break;
        case 16:
            pp.emit_Op16(RAX, RCX, isSigned ? X64Op::IDIV : X64Op::DIV);
            break;
        case 32:
            pp.emit_Op32(RAX, RCX, isSigned ? X64Op::IDIV : X64Op::DIV);
            break;
        case 64:
            pp.emit_Op64(RAX, RCX, isSigned ? X64Op::IDIV : X64Op::DIV);
            break;
        }
    }
    else
    {
        // div [rdi+?]
        pp.emit_REX(bits);
        if (bits == 8)
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
        if (bits == 8)
            pp.concat.addString2("\x88\xE2"); // mov dl, ah
        pp.emit_StoreN_Indirect(regOffset(ip->c.u32), RDX, RDI, bits);
    }
    else
    {
        pp.emit_StoreN_Indirect(regOffset(ip->c.u32), RAX, RDI, bits);
    }
}
