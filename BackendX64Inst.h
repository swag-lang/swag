#pragma once
#include "Register.h"
#include "ByteCode.h"

namespace BackendX64Inst
{
    inline void emit_Symbol_In_RAX(X64PerThread& pp, uint32_t symbolIndex)
    {
        auto& concat = pp.concat;

        concat.addString3("\x48\x8D\x05");

        CoffRelocation reloc;
        reloc.virtualAddress = concat.totalCount - pp.textSectionOffset;
        reloc.symbolIndex    = symbolIndex;
        reloc.type           = IMAGE_REL_AMD64_REL32;
        pp.relocTableTextSection.table.push_back(reloc);
        concat.addU32(0);
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

    inline void emit_Move_Stack_In_AL(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString2("\x8a\x07"); // mov al, byte ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString2("\x8a\x47"); // mov al, byte ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString2("\x8a\x87"); // mov al, byte ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_AX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x66\x8b\x07"); // mov ax, word ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString3("\x66\x8b\x47"); // mov ax, word ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString3("\x66\x8b\x87"); // mov ax, word ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_EAX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString2("\x8b\x07"); // mov eax, dword ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString2("\x8b\x47"); // mov eax, dword ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString2("\x8b\x87"); // mov eax, dword ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_RAX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x8B\x07"); // mov rax, qword ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString3("\x48\x8b\x47"); // mov rax, qword ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString3("\x48\x8B\x87"); // mov rax, qword ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_RBX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x8B\x1F"); // mov rbx, qword ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString3("\x48\x8B\x5F"); // mov rbx, qword ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString3("\x48\x8B\x9F"); // mov rbx, qword ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_RCX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x8B\x0F"); // mov rcx, qword ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString3("\x48\x8B\x4F"); // mov rcx, qword ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString3("\x48\x8B\x8F"); // mov rcx, qword ptr [rdi + ????????]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_RDX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x8B\x17"); // mov rdx, qword ptr [rdi]
        }
        else if (stackOffset <= 0x7F)
        {
            pp.concat.addString3("\x48\x8B\x57"); // mov rdx, qword ptr [rdi + ??]
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            pp.concat.addString3("\x48\x8B\x97"); // mov rdx, qword ptr [rdi + ????????]
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

    // clang-format off
    inline void emit_Move_XMM0_At_RAX_F32(X64PerThread& pp) { pp.concat.addString4("\xf3\x0f\x11\x00"); } // movss dword ptr [rax], xmm0
    inline void emit_Move_XMM0_At_RAX_F64(X64PerThread& pp) { pp.concat.addString4("\xf2\x0f\x11\x00"); } // movsd qword ptr [rax], xmm0
    // clang-format on

    inline void emit_Move_Cst8_At_RAX(X64PerThread& pp, uint8_t val)
    {
        pp.concat.addString2("\xC6\x00"); // mov byte ptr [rax], ??
        pp.concat.addU8(val);
    }

    inline void emit_Move_Cst16_At_RAX(X64PerThread& pp, uint16_t val)
    {
        pp.concat.addString3("\x66\xC7\x00"); // mov word ptr [rax], ????
        pp.concat.addU16(val);
    }

    inline void emit_Move_Cst32_At_RAX(X64PerThread& pp, uint32_t val)
    {
        pp.concat.addString3("\x48\xc7\x00"); // mov qword ptr [rax], ????????
        pp.concat.addU32(val);
    }

    // clang-format off
    inline void emit_Clear_RAX(X64PerThread& pp) { pp.concat.addString3("\x48\x31\xc0"); } // xor rax, rax
    inline void emit_Clear_RBX(X64PerThread& pp) { pp.concat.addString3("\x48\x31\xdb"); } // xor rbx, rbx
    inline void emit_Clear_RDX(X64PerThread& pp) { pp.concat.addString2("\x31\xd2"); } // xor rdx, rdx
    // clang-format on

    inline void emit_Move_Cst64_In_RAX(X64PerThread& pp, uint64_t val)
    {
        if (val == 0)
            emit_Clear_RAX(pp);
        else if (val <= 0x7FFFFFFF)
        {
            pp.concat.addString3("\x48\xc7\xc0"); // mov rax, ????????
            pp.concat.addU32((uint32_t) val);
        }
        {
            pp.concat.addString2("\x48\xb8"); // mov rax, ????????_????????
            pp.concat.addU64(val);
        }
    }

    inline void emit_Move_Cst64_In_RBX(X64PerThread& pp, uint64_t val)
    {
        if (val == 0)
            emit_Clear_RBX(pp);
        else if (val <= 0x7FFFFFFF)
        {
            pp.concat.addString3("\x48\xc7\xc3"); // mov rbx, ????????
            pp.concat.addU32((uint32_t) val);
        }
        {
            pp.concat.addString2("\x48\xbb"); // mov rbx, ????????_????????
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
    inline void emit_Move_Reg_In_AL(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_AL(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_AX(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_AX(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_EAX(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_EAX(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_RAX(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_RAX(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_RBX(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_RBX(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_RCX(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_RCX(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_RDX(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_RDX(pp, r * sizeof(Register)); }
    inline void emit_Lea_Reg_In_RAX(X64PerThread& pp, uint32_t r) { emit_Lea_Stack_In_RAX(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_XMM0_F32(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_XMM0_F32(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_XMM1_F32(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_XMM1_F32(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_XMM0_F64(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_XMM0_F64(pp, r * sizeof(Register)); }
    inline void emit_Move_Reg_In_XMM1_F64(X64PerThread& pp, uint32_t r) { emit_Move_Stack_In_XMM1_F64(pp, r * sizeof(Register)); }
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
            BackendX64Inst::emit_Move_Reg_In_AL(pp, ip->a.u32);
            break;
        case 16:
            BackendX64Inst::emit_Move_Reg_In_AX(pp, ip->a.u32);
            break;
        case 32:
            BackendX64Inst::emit_Move_Reg_In_EAX(pp, ip->a.u32);
            break;
        case 64:
            BackendX64Inst::emit_Move_Reg_In_RAX(pp, ip->a.u32);
            pp.concat.addU8(0x48);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }

        if(op & 0xFF00)
            pp.concat.addU16(op);
        else
            pp.concat.addU8((uint8_t) op);

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

}; // namespace BackendX64Inst