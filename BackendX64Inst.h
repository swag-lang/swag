#pragma once
#include "Register.h"
#include "ByteCode.h"

#define regOffset(__r) __r * sizeof(Register)

enum Reg
{
    RAX = 0b0000,
    RCX = 0b0001,
    RDX = 0b0010,
    RBX = 0b0011,
    RSP = 0b0100,
    RBP = 0b0101,
    RSI = 0b0110,
    RDI = 0b0111,
    R8  = 0b1000,
    R9  = 0b1001,
    R10 = 0b1010,
    R11 = 0b1011,
    R12 = 0b1100,
    R13 = 0b1101,
    R14 = 0b1110,
    R15 = 0b1111,
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

    inline void emit_Move8_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        pp.concat.addU8(0x8A);
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

    inline void emit_Move16_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x8B);
        if (stackOffset == 0)
        {
            // mov ax, word ptr [rdi]
            pp.concat.addU8(modRM(0, reg, memReg));
        }
        else if (stackOffset <= 0x7F)
        {
            // mov ax, word ptr [rdi + ??]
            pp.concat.addU8(modRM(DISP8, reg, memReg));
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            // mov ax, word ptr [rdi + ????????]
            pp.concat.addU8(modRM(DISP32, reg, memReg));
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move32_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        pp.concat.addU8(0x8B);
        if (stackOffset == 0)
        {
            // mov eax, dword ptr [rdi]
            pp.concat.addU8(modRM(0, reg, memReg));
        }
        else if (stackOffset <= 0x7F)
        {
            // mov eax, dword ptr [rdi + ??]
            pp.concat.addU8(modRM(DISP8, reg, memReg));
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            // mov eax, dword ptr [rdi + ????????]
            pp.concat.addU8(modRM(DISP32, reg, memReg));
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move64_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        pp.concat.addU8(0x48);
        pp.concat.addU8(0x8B);
        if (stackOffset == 0)
        {
            // mov rax, qword ptr [rdi]
            pp.concat.addU8(modRM(0, reg, memReg));
        }
        else if (stackOffset <= 0x7F)
        {
            // mov rax, qword ptr [rdi + ??]
            pp.concat.addU8(modRM(DISP8, reg, memReg));
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            // mov rax, qword ptr [rdi + ????????]
            pp.concat.addU8(modRM(DISP32, reg, memReg));
            pp.concat.addU32(stackOffset);
        }
    }

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

    inline void emit_Move_AL_At_Stack(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString2("\x88\x07"); // mov byte ptr [rdi], al
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString2("\x88\x47"); // mov byte ptr [rdi + ??], al
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString2("\x88\x87"); // mov byte ptr [rdi + ????????], al
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_AX_At_Stack(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x66\x89\x07"); // mov word ptr [rdi], ax
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString3("\x66\x89\x47"); // mov word ptr [rdi + ??], ax
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString3("\x66\x89\x87"); // mov word ptr [rdi + ????????], ax
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_EDX_At_Stack(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString2("\x89\x17"); // mov dword ptr [rdi], edx
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString2("\x89\x57"); // mov dword ptr [rdi + ??], edx
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString2("\x89\x97"); // mov dword ptr [rdi + ????????], edx
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_RDX_At_Stack(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x89\x17"); // mov qword ptr [rdi], rdx
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString3("\x48\x89\x57"); // mov qword ptr [rdi + ??], rdx
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString3("\x48\x89\x97"); // mov qword ptr [rdi + ????????], rdx
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_EAX_At_Stack(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString2("\x89\x07"); // mov dword ptr [rdi], eax
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString2("\x89\x47"); // mov dword ptr [rdi + ??], eax
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString2("\x89\x87"); // mov dword ptr [rdi + ????????], eax
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_RAX_At_Stack(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x89\x07"); // mov qword ptr [rdi], rax
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString3("\x48\x89\x47"); // mov qword ptr [rdi + ??], rax
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString3("\x48\x89\x87"); // mov qword ptr [rdi + ????????], rax
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_RCX_At_Stack(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x89\x0f"); // mov qword ptr [rdi], rcx
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString3("\x48\x89\x4f"); // mov qword ptr [rdi + ??], rcx
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString3("\x48\x89\x8f"); // mov qword ptr [rdi + ????????], rcx
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_R8D(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x44\x8b\x07"); // mov r8d, dword ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString3("\x44\x8b\x47"); // mov r8d, dword ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString3("\x44\x8b\x87"); // mov r8d, dword ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_R8(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x4c\x8b\x07"); // mov r8, qword ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString3("\x4c\x8b\x47"); // mov r8, qword ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString3("\x4c\x8b\x87"); // mov r8, qword ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_R9D(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x44\x8b\x0f"); // mov r9d, dword ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString3("\x44\x8b\x4f"); // mov r9d, dword ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString3("\x44\x8b\x8f"); // mov r9d, dword ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_R9(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x4c\x8b\x0f"); // mov r9, qword ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString3("\x4c\x8b\x4f"); // mov r9, qword ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString3("\x4c\x8b\x8f"); // mov r9, qword ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_XMM0_F32(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString4("\xf3\x0f\x10\x07"); // movss xmm0, dword ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString4("\xf3\x0f\x10\x47"); // movss xmm0, dword ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString4("\xf3\x0f\x10\x87"); // movss xmm0, dword ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_XMM1_F32(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString4("\xf3\x0f\x10\x0f"); // movss xmm1, dword ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString4("\xf3\x0f\x10\x4f"); // movss xmm1, dword ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString4("\xf3\x0f\x10\x8f"); // movss xmm1, dword ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_XMM0_F64(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString4("\xf2\x0f\x10\x07"); // movsd xmm0, qword ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString4("\xf2\x0f\x10\x47"); // movsd xmm0, qword ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString4("\xf2\x0f\x10\x87"); // movsd xmm0, qword ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_XMM1_F64(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString4("\xf2\x0f\x10\x0f"); // movsd xmm1, qword ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString4("\xf2\x0f\x10\x4f"); // movsd xmm1, qword ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString4("\xf2\x0f\x10\x8f"); // movsd xmm1, qword ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_XMM2_F64(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString4("\xf2\x0f\x10\x17"); // movsd xmm2, qword ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString4("\xf2\x0f\x10\x57"); // movsd xmm2, qword ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString4("\xf2\x0f\x10\x97"); // movsd xmm2, qword ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_XMM3_F64(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString4("\xf2\x0f\x10\x1f"); // movsd xmm3, qword ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString4("\xf2\x0f\x10\x5f"); // movsd xmm3, qword ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString4("\xf2\x0f\x10\x9f"); // movsd xmm3, qword ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_XMM0_At_Stack_F32(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString4("\xf3\x0f\x11\x07"); // movss dword ptr [rdi], xmm0
        }
        else
        {
            pp.concat.addString4("\xf3\x0f\x11\x87"); // movss dword ptr [rdi + ?], xmm0
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_XMM0_At_Stack_F64(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString4("\xf2\x0f\x11\x07"); // movsd qword ptr [rdi], xmm0
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString4("\xf2\x0f\x11\x47"); // movsd qword ptr [rdi + ??], xmm0
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString4("\xf2\x0f\x11\x87"); // movsd qword ptr [rdi + ????????], xmm0
            pp.concat.addU32(stackOffset);
        }
    }

    // clang-format off
    inline void emit_DeRef8_RAX(X64PerThread& pp) { pp.concat.addString2("\x8a\x00"); } // mov al, [rax]
    inline void emit_DeRef16_RAX(X64PerThread& pp) { pp.concat.addString3("\x66\x8b\x00"); } // mov ax, [rax]
    inline void emit_DeRef32_RAX(X64PerThread& pp) { pp.concat.addString2("\x8b\x00"); } // mov eax, [rax]
    inline void emit_DeRef64_RAX(X64PerThread& pp) { pp.concat.addString3("\x48\x8B\x00"); } // mov rax, [rax]
    inline void emit_DeRef8_RBX(X64PerThread& pp) { pp.concat.addString2("\x8a\x1b"); } // mov bl, [rbx]
    inline void emit_DeRef16_RBX(X64PerThread& pp) { pp.concat.addString3("\x66\x8b\x1b"); } // mov bx, [rbx]
    inline void emit_DeRef32_RBX(X64PerThread& pp) { pp.concat.addString2("\x8b\x1b"); } // mov ebx, [rbx]
    inline void emit_DeRef64_RBX(X64PerThread& pp) { pp.concat.addString3("\x48\x8B\x1B"); } // mov rbx, [rbx]
    inline void emit_DeRef_RAX_In_XMM0_F32(X64PerThread& pp) { pp.concat.addString4("\xf3\x0f\x10\x00"); } // movss xmm0, dword ptr [rax]
    inline void emit_DeRef_RAX_In_XMM0_F64(X64PerThread& pp) { pp.concat.addString4("\xf2\x0f\x10\x00"); } // movsd xmm0, qword ptr [rax]
    // clang-format on

    inline void emit_Lea_Stack_In_RAX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x89\xF8"); // mov rax, rdi
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString3("\x48\x8D\x47"); // lea rax, [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString3("\x48\x8D\x87"); // lea rax, [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Lea_Stack_In_RCX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x89\xf9"); // mov rcx, rdi
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString3("\x48\x8d\x4f"); // lea rcx, [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString3("\x48\x8d\x8f"); // lea rcx, [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    // clang-format off
    inline void emit_Move_XMM0_At_RAX_F32(X64PerThread& pp) { pp.concat.addString4("\xf3\x0f\x11\x00"); } // movss dword ptr [rax], xmm0
    inline void emit_Move_XMM0_At_RAX_F64(X64PerThread& pp) { pp.concat.addString4("\xf2\x0f\x11\x00"); } // movsd qword ptr [rax], xmm0
    // clang-format on

    inline void emit_Move_Cst8_At_RAX(X64PerThread& pp, uint32_t offset, uint8_t val)
    {
        if (offset == 0)
        {
            pp.concat.addString2("\xc6\x00"); // mov byte ptr [rax], ??
            pp.concat.addU8(val);
        }
        else if (offset <= 0x7F)
        {
            pp.concat.addString2("\xc6\x40"); // mov byte ptr [rax + ??], ??
            pp.concat.addU8((uint8_t) offset);
            pp.concat.addU8(val);
        }
        else
        {
            pp.concat.addString2("\xc6\x80"); // mov byte ptr [rax + ????????], ??
            pp.concat.addU32(offset);
            pp.concat.addU8(val);
        }
    }

    inline void emit_Move_Cst16_At_RAX(X64PerThread& pp, uint32_t offset, uint16_t val)
    {
        if (offset == 0)
        {
            pp.concat.addString3("\x66\xc7\x00"); // mov word ptr [rax], ????
            pp.concat.addU16(val);
        }
        else if (offset <= 0x7F)
        {
            pp.concat.addString3("\x66\xc7\x40"); // mov word ptr [rax + ??], ????
            pp.concat.addU8((uint8_t) offset);
            pp.concat.addU16(val);
        }
        else
        {
            pp.concat.addString3("\x66\xc7\x80"); // mov word ptr [rax + ????????], ????
            pp.concat.addU32(offset);
            pp.concat.addU16(val);
        }

    } // namespace BackendX64Inst

    inline void emit_Move_Cst32_At_RAX(X64PerThread& pp, uint32_t offset, uint32_t val)
    {
        if (offset == 0)
        {
            pp.concat.addString2("\xc7\x00"); // mov dword ptr [rax], ????????
            pp.concat.addU32(val);
        }
        else if (offset <= 0x7F)
        {
            pp.concat.addString2("\xc7\x40"); // mov dword ptr [rax + ??], ????????
            pp.concat.addU8((uint8_t) offset);
            pp.concat.addU32(val);
        }
        else
        {
            pp.concat.addString2("\xc7\x80"); // mov dword ptr [rax + ????????], ????????
            pp.concat.addU32(offset);
            pp.concat.addU32(val);
        }
    }

    inline void emit_Move_Cst64_At_RAX(X64PerThread& pp, uint32_t offset, uint64_t val)
    {
        if (val <= 0x7FFFFFFF)
        {
            if (offset == 0)
            {
                pp.concat.addString3("\x48\xc7\x00"); // mov qword ptr [rax], ????????
                pp.concat.addU32((uint32_t) val);
            }
            else if (offset <= 0x7F)
            {
                pp.concat.addString3("\x48\xc7\x40"); // mov qword ptr [rax + ??], ????????
                pp.concat.addU8((uint8_t) offset);
                pp.concat.addU32((uint32_t) val);
            }
            else
            {
                pp.concat.addString3("\x48\xc7\x80"); // mov qword ptr [rax + ????????], ????????
                pp.concat.addU32(offset);
                pp.concat.addU32((uint32_t) val);
            }
        }
        else
        {
            SWAG_ASSERT(false);
        }
    }

    // clang-format off
    inline void emit_Clear_RAX(X64PerThread& pp) { pp.concat.addString3("\x48\x31\xc0"); } // xor rax, rax
    inline void emit_Clear_RBX(X64PerThread& pp) { pp.concat.addString3("\x48\x31\xdb"); } // xor rbx, rbx
    inline void emit_Clear_ECX(X64PerThread& pp) { pp.concat.addString2("\x31\xc9"); } // xor ecx, ecx
    inline void emit_Clear_RCX(X64PerThread& pp) { pp.concat.addString3("\x48\x31\xc9"); } // xor rcx, rcx
    inline void emit_Clear_R8(X64PerThread& pp) { pp.concat.addString3("\x4d\x31\xc0"); } // xor r8, r8
    inline void emit_Clear_DX(X64PerThread& pp) { pp.concat.addString3("\x66\x31\xd2"); } // xor dx, dx
    inline void emit_Clear_EDX(X64PerThread& pp) { pp.concat.addString2("\x31\xd2"); } // xor edx, edx
    inline void emit_Clear_RDX(X64PerThread& pp) { pp.concat.addString3("\x48\x31\xd2"); } // xor rdx, rdx
    // clang-format on

    inline void emit_Move_Cst64_In_R8(X64PerThread& pp, uint64_t val)
    {
        if (val == 0)
        {
            emit_Clear_R8(pp);
        }
        else if (val <= 0x7FFFFFFF)
        {
            pp.concat.addString3("\x49\xc7\xc0"); // mov r8, ????????
            pp.concat.addU32((uint32_t) val);
        }
        else
        {
            pp.concat.addString2("\x49\xb8"); // mov r8, ????????_????????
            pp.concat.addU64(val);
        }
    }

    inline void emit_Move_Cst64_In_RAX(X64PerThread& pp, uint64_t val)
    {
        if (val == 0)
        {
            emit_Clear_RAX(pp);
        }
        else if (val <= 0x7FFFFFFF)
        {
            pp.concat.addString3("\x48\xc7\xc0"); // mov rax, ????????
            pp.concat.addU32((uint32_t) val);
        }
        else
        {
            pp.concat.addString2("\x48\xb8"); // mov rax, ????????_????????
            pp.concat.addU64(val);
        }
    }

    inline void emit_Move_Cst64_In_RBX(X64PerThread& pp, uint64_t val)
    {
        if (val == 0)
        {
            emit_Clear_RBX(pp);
        }
        else if (val <= 0x7FFFFFFF)
        {
            pp.concat.addString3("\x48\xc7\xc3"); // mov rbx, ????????
            pp.concat.addU32((uint32_t) val);
        }
        else
        {
            pp.concat.addString2("\x48\xbb"); // mov rbx, ????????_????????
            pp.concat.addU64(val);
        }
    }

    inline void emit_Move_Cst64_In_RCX(X64PerThread& pp, uint64_t val)
    {
        if (val == 0)
        {
            emit_Clear_RCX(pp);
        }
        else if (val <= 0x7FFFFFFF)
        {
            pp.concat.addString3("\x48\xc7\xc1"); // mov rcx, ????????
            pp.concat.addU32((uint32_t) val);
        }
        else
        {
            pp.concat.addString2("\x48\xb9"); // mov rcx, ????????_????????
            pp.concat.addU64(val);
        }
    }

    // clang-format off
    inline void emit_Move_BL_At_RAX(X64PerThread& pp) { pp.concat.addString2("\x88\x18"); } // mov [rax], bl
    inline void emit_Move_BX_At_RAX(X64PerThread& pp) { pp.concat.addString3("\x66\x89\x18"); } // mov [rax], bx
    inline void emit_Move_EBX_At_RAX(X64PerThread& pp) { pp.concat.addString2("\x89\x18"); } // mov [rax], ebx
    inline void emit_Move_RBX_At_RAX(X64PerThread& pp) { pp.concat.addString3("\x48\x89\x18"); } // mov [rax], rbx
    inline void emit_Move_AL_At_RBX(X64PerThread& pp) { pp.concat.addString2("\x88\x03"); } // mov [rbx], al
    inline void emit_Move_AX_At_RBX(X64PerThread& pp) { pp.concat.addString3("\x66\x89\x03"); } // mov [rbx], ax
    inline void emit_Move_EAX_At_RBX(X64PerThread& pp) { pp.concat.addString2("\x89\x03"); } // mov [rbx], eax
    inline void emit_Move_RAX_At_RBX(X64PerThread& pp) { pp.concat.addString3("\x48\x89\x03"); } // mov [rbx], rax
    inline void emit_Move_DL_At_RBX(X64PerThread& pp) { pp.concat.addString2("\x88\x13"); } // mov [rbx], dl
    inline void emit_Move_DX_At_RBX(X64PerThread& pp) { pp.concat.addString3("\x66\x89\x13"); } // mov [rbx], dx
    inline void emit_Move_EDX_At_RBX(X64PerThread& pp) { pp.concat.addString2("\x89\x13"); } // mov [rbx], edx
    inline void emit_Move_RDX_At_RBX(X64PerThread& pp) { pp.concat.addString3("\x48\x89\x13"); } // mov [rbx], rdx

    inline void emit_Cmp_AL_With_BL(X64PerThread& pp) { pp.concat.addString2("\x38\xd8"); } // cmp al, bl
    inline void emit_Cmp_AX_With_BX(X64PerThread& pp) { pp.concat.addString3("\x66\x39\xd8"); } // cmp ax, bx
    inline void emit_Cmp_EAX_With_EBX(X64PerThread& pp) { pp.concat.addString2("\x39\xD8"); } // cmp eax, ebx
    inline void emit_Cmp_RAX_With_RBX(X64PerThread& pp) { pp.concat.addString3("\x48\x39\xD8"); } // cmp rax, rbx
    inline void emit_Cmp_XMM0_With_XMM1_F32(X64PerThread& pp) { pp.concat.addString3("\x0f\x2f\xc8"); } // comiss xmm1, xmm0 (inverted !)
    inline void emit_Cmp_XMM0_With_XMM1_F64(X64PerThread& pp) { pp.concat.addString4("\x66\x0f\x2f\xc8"); } // comisd xmm1, xmm0 (inverted !)

    inline void emit_Test_AL_With_AL(X64PerThread& pp) { pp.concat.addString2("\x84\xC0"); } // test al, al
    inline void emit_Test_AX_With_AX(X64PerThread& pp) { pp.concat.addString3("\x66\x85\xc0"); } // test ax, ax
    inline void emit_Test_EAX_With_EAX(X64PerThread& pp) { pp.concat.addString2("\x85\xC0"); } // test eax, eax
    inline void emit_Test_RAX_With_RAX(X64PerThread& pp) { pp.concat.addString3("\x48\x85\xc0"); } // test rax, rax
    inline void emit_Test_RBX_With_RBX(X64PerThread& pp) { pp.concat.addString3("\x48\x85\xdb"); } // test rbx, rbx

    inline void emit_SignedExtend_AL_To_AX(X64PerThread& pp) { pp.concat.addString2("\x66\x98"); } // cbw
    inline void emit_SignedExtend_AX_To_EAX(X64PerThread& pp) { pp.concat.addU8(0x98); } // cwde
    inline void emit_SignedExtend_BX_To_EBX(X64PerThread& pp) { pp.concat.addString3("\x0f\xbf\xdb"); } // movsx ebx, bx
    inline void emit_SignedExtend_EAX_To_RAX(X64PerThread& pp) { pp.concat.addString2("\x48\x98"); } // cdqe
    // clang-format on

    //////////////////////////////////////////////////

    // clang-format off
    inline void emit_Move_AL_At_Reg(X64PerThread& pp, uint32_t r) {emit_Move_AL_At_Stack(pp, r * sizeof(Register)); }
    inline void emit_Move_AX_At_Reg(X64PerThread& pp, uint32_t r) { emit_Move_AX_At_Stack(pp, r * sizeof(Register)); }
    inline void emit_Move_EAX_At_Reg(X64PerThread& pp, uint32_t r) { emit_Move_EAX_At_Stack(pp, r * sizeof(Register));}
    inline void emit_Move_RAX_At_Reg(X64PerThread& pp, uint32_t r) { emit_Move_RAX_At_Stack(pp, r * sizeof(Register)); }
    inline void emit_Move_RCX_At_Reg(X64PerThread& pp, uint32_t r) { emit_Move_RCX_At_Stack(pp, r * sizeof(Register)); }
    inline void emit_Move_EDX_At_Reg(X64PerThread& pp, uint32_t r) { emit_Move_EDX_At_Stack(pp, r * sizeof(Register)); }
    inline void emit_Move_RDX_At_Reg(X64PerThread& pp, uint32_t r) { emit_Move_RDX_At_Stack(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_R8D(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_R8D(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_R8(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_R8(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_R9(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_R9(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_R9D(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_R9D(pp, r * sizeof(Register)); }
    inline void emit_Lea_Reg_In_RAX(X64PerThread& pp, uint32_t r) { emit_Lea_Stack_In_RAX(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_XMM0_F32(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_XMM0_F32(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_XMM1_F32(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_XMM1_F32(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_XMM0_F64(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_XMM0_F64(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_XMM1_F64(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_XMM1_F64(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_XMM2_F64(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_XMM2_F64(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_XMM3_F64(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_XMM3_F64(pp, r * sizeof(Register)); }
    inline void emit_Move_XMM0_At_Reg_F32(X64PerThread& pp, uint32_t r) { emit_Move_XMM0_At_Stack_F32(pp, r * sizeof(Register)); }
    inline void emit_Move_XMM0_At_Reg_F64(X64PerThread& pp, uint32_t r) { emit_Move_XMM0_At_Stack_F64(pp, r * sizeof(Register)); }
    // clang-format on

    //////////////////////////////////////////////////

    inline void emit_BinOpFloat_At_Reg(X64PerThread& pp, ByteCodeInstruction* ip, uint8_t op, uint32_t bits)
    {
        switch (bits)
        {
        case 32:
            BackendX64Inst::emit_Move_Reg_In_XMM0_F32(pp, ip->a.u32);
            pp.concat.addU8(0xF3);
            break;
        case 64:
            BackendX64Inst::emit_Move_Reg_In_XMM0_F64(pp, ip->a.u32);
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
            BackendX64Inst::emit_Move_XMM0_At_Reg_F32(pp, ip->c.u32);
            break;
        case 64:
            BackendX64Inst::emit_Move_XMM0_At_Reg_F64(pp, ip->c.u32);
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
            BackendX64Inst::emit_Move8_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            break;
        case 16:
            BackendX64Inst::emit_Move16_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            break;
        case 32:
            BackendX64Inst::emit_Move32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (op == 0xF7)                   // idiv
                pp.concat.addString1("\x99"); // cdq
            break;
        case 64:
            BackendX64Inst::emit_Move64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
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
            BackendX64Inst::emit_Move_AL_At_Reg(pp, ip->c.u32);
            break;
        case 16:
            BackendX64Inst::emit_Move_AX_At_Reg(pp, ip->c.u32);
            break;
        case 32:
            BackendX64Inst::emit_Move_EAX_At_Reg(pp, ip->c.u32);
            break;
        case 64:
            BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->c.u32);
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
            BackendX64Inst::emit_Move32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (isSigned)
                pp.concat.addString1("\x99"); // cdq
            else
                emit_Clear_EDX(pp);
            break;
        case 64:
            BackendX64Inst::emit_Move64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (isSigned)
                pp.concat.addString2("\x48\x99"); // cqo
            else
                emit_Clear_RDX(pp);
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
                BackendX64Inst::emit_Move_EDX_At_Reg(pp, ip->c.u32);
                break;
            case 64:
                BackendX64Inst::emit_Move_RDX_At_Reg(pp, ip->c.u32);
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
                BackendX64Inst::emit_Move_EAX_At_Reg(pp, ip->c.u32);
                break;
            case 64:
                BackendX64Inst::emit_Move_RAX_At_Reg(pp, ip->c.u32);
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