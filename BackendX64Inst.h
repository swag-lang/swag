#pragma once
#include "Register.h"
#include "ByteCode.h"

#define regOffset(__r) __r * sizeof(Register)

enum Reg
{
    RAX  = 0b0000,
    XMM0 = 0b0000,
    RCX  = 0b0001,
    XMM1 = 0b0001,
    RDX  = 0b0010,
    XMM2 = 0b0010,
    RBX  = 0b0011,
    XMM3 = 0b0011,
    RSP  = 0b0100,
    RBP  = 0b0101,
    RSI  = 0b0110,
    RDI  = 0b0111,
    R8   = 0b1000,
    R9   = 0b1001,
    R10  = 0b1010,
    R11  = 0b1011,
    R12  = 0b1100,
    R13  = 0b1101,
    R14  = 0b1110,
    R15  = 0b1111,
};

namespace BackendX64Inst
{
    enum Disp
    {
        DISP8  = 0b01,
        DISP32 = 0b10,
    };

    inline uint8_t modRM(uint8_t mod, uint8_t r, uint8_t m)
    {
        return mod << 6 | r << 3 | m;
    }

    enum JumpType
    {
        JNZ,
        JZ,
        JUMP,
    };

    inline void emit_ModRM(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        if (stackOffset == 0)
        {
            // mov al, byte ptr [rdi]
            pp.concat.addU8(modRM(0, reg, memReg));
        }
        else if (stackOffset <= 0x7F)
        {
            // mov al, byte ptr [rdi + ??]
            pp.concat.addU8(modRM(DISP8, reg, memReg));
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            // mov al, byte ptr [rdi + ????????]
            pp.concat.addU8(modRM(DISP32, reg, memReg));
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Load8_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x8A);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_Load16_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x8B);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_Load32_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(memReg < R8);
        if (reg >= R8)
            pp.concat.addU8(0x44);
        pp.concat.addU8(0x8B);
        emit_ModRM(pp, stackOffset, (reg & 0b111), memReg);
    }

    inline void emit_Load64_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(memReg < R8);
        pp.concat.addU8(0x48 | ((reg & 0b1000) >> 1));
        pp.concat.addU8(0x8B);
        emit_ModRM(pp, stackOffset, (reg & 0b111), memReg);
    }

    inline void emit_LoadF32_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0xF3);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x10);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_LoadF64_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0xF2);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x10);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_Store8_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x88);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_Store16_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x89);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_Store32_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x89);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_Store64_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        pp.concat.addU8(0x48 | ((reg & 0b1000) >> 1) | ((memReg & 0b1000) >> 3));
        pp.concat.addU8(0x89);
        emit_ModRM(pp, stackOffset, (reg & 0b111), (memReg & 0b111));
    }

    inline void emit_StoreF32_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0xF3);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x11);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_StoreF64_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0xF2);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x11);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_Store8_Immediate(X64PerThread& pp, uint32_t offset, uint8_t val, uint8_t reg)
    {
        pp.concat.addU8(0xC6);
        emit_ModRM(pp, offset, 0, reg);
        pp.concat.addU8(val);
    }

    inline void emit_Store16_Immediate(X64PerThread& pp, uint32_t offset, uint16_t val, uint8_t reg)
    {
        pp.concat.addU8(0x66);
        pp.concat.addU8(0xC7);
        emit_ModRM(pp, offset, 0, reg);
        pp.concat.addU32((uint32_t) val);
    }

    inline void emit_Store32_Immediate(X64PerThread& pp, uint32_t offset, uint32_t val, uint8_t reg)
    {
        pp.concat.addU8(0xC7);
        emit_ModRM(pp, offset, 0, reg);
        pp.concat.addU32((uint32_t) val);
    }

    inline void emit_Store64_Immediate(X64PerThread& pp, uint32_t offset, uint64_t val, uint8_t reg)
    {
        SWAG_ASSERT(val <= 0x7FFFFFFF);
        pp.concat.addU8(0x48);
        pp.concat.addU8(0xC7);
        emit_ModRM(pp, offset, 0, reg);
        pp.concat.addU32((uint32_t) val);
    }

    inline void emit_Clear16(X64PerThread& pp, uint8_t reg)
    {
        SWAG_ASSERT(reg < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x31);
        pp.concat.addU8(modRM(3, reg, reg));
    }

    inline void emit_Clear32(X64PerThread& pp, uint8_t reg)
    {
        SWAG_ASSERT(reg < R8);
        pp.concat.addU8(0x31);
        pp.concat.addU8(modRM(3, reg, reg));
    }

    inline void emit_Clear64(X64PerThread& pp, uint8_t reg)
    {
        pp.concat.addU8(0x48 | ((reg & 0b1000) >> 1) | ((reg & 0b1000) >> 3));
        pp.concat.addU8(0x31);
        pp.concat.addU8(modRM(3, (reg & 0b111), (reg & 0b111)));
    }

    inline void emit_Load64_Immediate(X64PerThread& pp, uint64_t val, uint8_t reg, bool force64bits = false)
    {
        if (force64bits)
        {
            pp.concat.addU8(0x48 | ((reg & 0b1000) >> 3));
            pp.concat.addU8(0xB8 | reg);
            pp.concat.addU64(val);
            return;
        }

        if (val == 0)
        {
            emit_Clear64(pp, reg);
            return;
        }

        pp.concat.addU8(0x48 | ((reg & 0b1000) >> 3));
        if (val <= 0x7FFFFFFF)
        {
            pp.concat.addU8(0xC7);
            pp.concat.addU8(0xC0 | (reg & 0b111));
            pp.concat.addU32((uint32_t) val);
        }
        else
        {
            pp.concat.addU8(0xB8 | reg);
            pp.concat.addU64(val);
        }
    }

    inline void emit_Copy64(X64PerThread& pp, uint32_t regDst, uint32_t regSrc)
    {
        pp.concat.addU8(0x48 | ((regDst & 0b1000) >> 3) | ((regSrc & 0b1000) >> 1));
        pp.concat.addU8(0x89);
        pp.concat.addU8(modRM(0b11, regSrc & 0b111, regDst & 0b111));
    }

    inline void emit_SetNE(X64PerThread& pp)
    {
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x95);
        pp.concat.addU8(0xC0);
    }

    inline void emit_SetA(X64PerThread& pp)
    {
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x97);
        pp.concat.addU8(0xC0);
    }

    inline void emit_SetB(X64PerThread& pp)
    {
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x92);
        pp.concat.addU8(0xC0);
    }

    inline void emit_SetE(X64PerThread& pp)
    {
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x94);
        pp.concat.addU8(0xC0);
    }

    inline void emit_SetG(X64PerThread& pp)
    {
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x9F);
        pp.concat.addU8(0xC0);
    }

    inline void emit_SetL(X64PerThread& pp)
    {
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x9C);
        pp.concat.addU8(0xC0);
    }

    inline void emit_Test64(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        pp.concat.addU8(0x48 | ((reg1 & 0b1000) >> 3) | ((reg2 & 0b1000) >> 1));
        pp.concat.addU8(0x85);
        pp.concat.addU8(modRM(3, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_Test32(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x85);
        pp.concat.addU8(modRM(3, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_Test16(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x85);
        pp.concat.addU8(modRM(3, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_Test8(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x84);
        pp.concat.addU8(modRM(3, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_Cmp64(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        pp.concat.addU8(0x48 | ((reg1 & 0b1000) >> 3) | ((reg2 & 0b1000) >> 1));
        pp.concat.addU8(0x39);
        pp.concat.addU8(modRM(3, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_Cmp32(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x39);
        pp.concat.addU8(modRM(3, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_Cmp16(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x39);
        pp.concat.addU8(modRM(3, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_Cmp8(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x38);
        pp.concat.addU8(modRM(3, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_CmpF32(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x2F);
        pp.concat.addU8(modRM(3, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_CmpF64(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x2F);
        pp.concat.addU8(modRM(3, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_LoadAddress(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        if (stackOffset == 0)
        {
            emit_Copy64(pp, reg, memReg);
            return;
        }

        pp.concat.addU8(0x48);
        pp.concat.addU8(0x8D);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    ///////////////////////////////////////////////////////

    inline void emit_Symbol_Relocation(X64PerThread& pp, uint32_t symbolIndex)
    {
        auto&          concat = pp.concat;
        CoffRelocation reloc;
        reloc.virtualAddress = concat.totalCount() - pp.textSectionOffset;
        reloc.symbolIndex    = symbolIndex;
        reloc.type           = IMAGE_REL_AMD64_REL32;
        pp.relocTableTextSection.table.push_back(reloc);
        concat.addU32(0);
    }

    inline void emit_SymbolAddr_In_RAX(X64PerThread& pp, uint32_t symbolIndex)
    {
        auto& concat = pp.concat;
        concat.addString3("\x48\x8D\x05"); // mov rax, qword ptr ????????[rip]
        emit_Symbol_Relocation(pp, symbolIndex);
    }

    inline void emit_Sub_Cst32_To_RSP(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            if (value <= 0x7F)
            {
                pp.concat.addString3("\x48\x83\xEC"); // sub rsp, ??
                pp.concat.addU8((uint8_t) value);
            }
            else
            {
                pp.concat.addString3("\x48\x81\xEC"); // sub rsp, ????????
                pp.concat.addU32(value);
            }
        }
    }

    inline void emit_Add_Cst32_To_RSP(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            if (value <= 0x7F)
            {
                pp.concat.addString3("\x48\x83\xC4"); // add rsp, ??
                pp.concat.addU8((uint8_t) value);
            }
            else
            {
                pp.concat.addString3("\x48\x81\xC4"); // add rsp, ????????
                pp.concat.addU32(value);
            }
        }
    }

    inline void emit_Add_Cst32_At_RAX(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            if (value <= 0x7F)
            {
                pp.concat.addString2("\x83\x00"); // add dword ptr [rax], ??
                pp.concat.addU8((uint8_t) value);
            }
            else
            {
                pp.concat.addString2("\x81\x00"); // add dword ptr [rax], ????????
                pp.concat.addU32(value);
            }
        }
    }

    inline void emit_Sub_Cst32_At_RAX(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            if (value <= 0x7F)
            {
                pp.concat.addString2("\x83\x28"); // sub dword ptr [rax], ??
                pp.concat.addU8((uint8_t) value);
            }
            else
            {
                pp.concat.addString2("\x81\x28"); // sub dword ptr [rax], ????????
                pp.concat.addU32(value);
            }
        }
    }

    inline void emit_Add_Cst32_To_RAX(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            if (value <= 0x7F)
            {
                pp.concat.addString3("\x48\x83\xc0"); // add rax, ??
                pp.concat.addU8((uint8_t) value);
            }
            else
            {
                pp.concat.addString2("\x48\x05"); // add rax, ????????
                pp.concat.addU32(value);
            }
        }
    }

    //////////////////////////////////////////////////////

    // clang-format off
    inline void emit_SignedExtend_AL_To_AX(X64PerThread& pp) { pp.concat.addString2("\x66\x98"); } // cbw
    inline void emit_SignedExtend_AX_To_EAX(X64PerThread& pp) { pp.concat.addU8(0x98); } // cwde
    inline void emit_SignedExtend_BX_To_EBX(X64PerThread& pp) { pp.concat.addString3("\x0f\xbf\xdb"); } // movsx ebx, bx
    inline void emit_SignedExtend_EAX_To_RAX(X64PerThread& pp) { pp.concat.addString2("\x48\x98"); } // cdqe
    // clang-format on

    //////////////////////////////////////////////////

    inline void emit_BinOpFloat_At_Reg(X64PerThread& pp, ByteCodeInstruction* ip, uint8_t op, uint32_t bits)
    {
        switch (bits)
        {
        case 32:
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            pp.concat.addU8(0xF3);
            break;
        case 64:
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            pp.concat.addU8(0xF2);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }

        pp.concat.addU8(0x0F);
        pp.concat.addU8(op);

        uint32_t offsetStack = ip->b.u32 * sizeof(Register);
        if (offsetStack == 0)
        {
            pp.concat.addU8(0x07);
        }
        else if (offsetStack <= 0x7F)
        {
            pp.concat.addU8(0x47);
            pp.concat.addU8((uint8_t) offsetStack);
        }
        else
        {
            pp.concat.addU8(0x87);
            pp.concat.addU32(offsetStack);
        }

        switch (bits)
        {
        case 32:
            BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->c.u32), XMM0, RDI);
            break;
        case 64:
            BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->c.u32), XMM0, RDI);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
    }

    inline void emit_BinOpInt_At_Reg(X64PerThread& pp, ByteCodeInstruction* ip, uint16_t op, uint32_t bits)
    {
        switch (bits)
        {
        case 8:
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case 16:
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            break;
        case 32:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (op == 0xF7)                   // idiv
                pp.concat.addString1("\x99"); // cdq
            break;
        case 64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (op == 0xF7)                       // idiv
                pp.concat.addString2("\x48\x99"); // cqo
            pp.concat.addU8(0x48);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }

        if (op & 0xFF00)
            pp.concat.addU16(op);
        else
            pp.concat.addU8((uint8_t) op);

        uint32_t offsetStack = ip->b.u32 * sizeof(Register);
        if (offsetStack == 0)
        {
            if (op == 0xF7) // idiv
                pp.concat.addU8(0x3F);
            else
                pp.concat.addU8(0x07);
        }
        else if (offsetStack <= 0x7F)
        {
            if (op == 0xF7) // idiv
                pp.concat.addU8(0x7F);
            else
                pp.concat.addU8(0x47);
            pp.concat.addU8((uint8_t) offsetStack);
        }
        else
        {
            if (op == 0xF7) // idiv
                pp.concat.addU8(0xBF);
            else
                pp.concat.addU8(0x87);
            pp.concat.addU32(offsetStack);
        }

        switch (bits)
        {
        case 8:
            BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case 16:
            BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case 32:
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        case 64:
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
    }

    inline void emit_BinOpInt_Div_At_Reg(X64PerThread& pp, ByteCodeInstruction* ip, bool isSigned, uint32_t bits, bool modulo = false)
    {
        switch (bits)
        {
        case 32:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (isSigned)
                pp.concat.addString1("\x99"); // cdq
            else
                emit_Clear32(pp, RDX);
            break;
        case 64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (isSigned)
                pp.concat.addString2("\x48\x99"); // cqo
            else
                emit_Clear64(pp, RDX);
            pp.concat.addU8(0x48);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }

        pp.concat.addU8(0xF7);

        uint32_t offsetStack = ip->b.u32 * sizeof(Register);
        if (offsetStack == 0)
        {
            if (isSigned)
                pp.concat.addU8(0x3F);
            else
                pp.concat.addU8(0x37);
        }
        else if (offsetStack <= 0x7F)
        {
            if (isSigned)
                pp.concat.addU8(0x7F);
            else
                pp.concat.addU8(0x77);
            pp.concat.addU8((uint8_t) offsetStack);
        }
        else
        {
            if (isSigned)
                pp.concat.addU8(0xBF);
            else
                pp.concat.addU8(0xB7);
            pp.concat.addU32(offsetStack);
        }

        if (modulo)
        {
            switch (bits)
            {
            case 32:
                BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RDX, RDI);
                break;
            case 64:
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RDX, RDI);
                break;
            default:
                SWAG_ASSERT(false);
                break;
            }
        }
        else
        {
            switch (bits)
            {
            case 32:
                BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
                break;
            case 64:
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
                break;
            default:
                SWAG_ASSERT(false);
                break;
            }
        }
    }

    inline void emitJump(X64PerThread& pp, JumpType jumpType, int32_t instructionCount, int32_t jumpOffset)
    {
        LabelToSolve label;
        label.ipDest = jumpOffset + instructionCount + 1;

        // Can we solve the lavel now ?
        auto it = pp.labels.find(label.ipDest);
        if (it != pp.labels.end())
        {
            // Estimate jump
            label.currentOffset = (int32_t) pp.concat.totalCount() + 1;
            int relOffset       = it->second - (label.currentOffset + 1);
            if (relOffset >= -127 && relOffset <= 128)
            {
                switch (jumpType)
                {
                case JNZ:
                    pp.concat.addString1("\x75"); // jnz ??
                    break;
                case JZ:
                    pp.concat.addString1("\x74"); // jz ??
                    break;
                case JUMP:
                    pp.concat.addString1("\xeb"); // jmp ??
                    break;
                }

                int8_t offset8 = (int8_t) relOffset;
                SWAG_ASSERT(offset8 >= -127 && offset8 <= 128);
                pp.concat.addU8(*(uint8_t*) &offset8);
            }
            else
            {
                switch (jumpType)
                {
                case JNZ:
                    pp.concat.addString2("\x0F\x85"); // jnz ????????
                    break;
                case JZ:
                    pp.concat.addString2("\x0F\x84"); // jz ????????
                    break;
                case JUMP:
                    pp.concat.addU8(0xE9); // jmp ????????
                    break;
                }

                label.currentOffset = (int32_t) pp.concat.totalCount();
                relOffset           = it->second - (label.currentOffset + 4);
                pp.concat.addU32(*(uint32_t*) &relOffset);
            }

            return;
        }

        // Here we do not know the destination label, so we assume 32 bits of offset
        switch (jumpType)
        {
        case JNZ:
            pp.concat.addString2("\x0F\x85"); // jnz ????????
            break;
        case JZ:
            pp.concat.addString2("\x0F\x84"); // jz ????????
            break;
        case JUMP:
            pp.concat.addU8(0xE9); // jmp ????????
            break;
        }

        label.currentOffset = (int32_t) pp.concat.totalCount();
        pp.concat.addU32(0);
        label.patch = pp.concat.getSeekPtr() - 4;
        pp.labelsToSolve.push_back(label);
    }
}; // namespace BackendX64Inst