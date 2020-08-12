#pragma once
#include "Register.h"

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
            pp.concat.addString3("\x48\x81\xEC"); // sub rsp, ?
            pp.concat.addU32(value);
        }
    }

    inline void emit_Add_Cst32_To_RSP(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            pp.concat.addString3("\x48\x81\xC4"); // add rsp, ?
            pp.concat.addU32(value);
        }
    }

    inline void emit_Add_Cst32_To_RAX(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            pp.concat.addString2("\x48\x05"); // add rax, ?
            pp.concat.addU32(value);
        }
    }

    inline void emit_Sub_Cst32_To_RAX(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            pp.concat.addString2("\x48\x2D"); // sub rax, ?
            pp.concat.addU32(value);
        }
    }

    inline void emit_Move_RAX_At_Stack(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x89\x07"); // mov qword ptr [rdi], rax
        }
        else
        {
            pp.concat.addString3("\x48\x89\x87"); // mov qword ptr [rdi + ?], rax
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_AL_At_Stack(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString2("\x88\x07"); // mov byte ptr [rdi], al
        }
        else
        {
            pp.concat.addString2("\x88\x87"); // mov byte ptr [rdi + ?], al
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_AX_At_Stack(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x66\x89\x07"); // mov word ptr [rdi], ax
        }
        else
        {
            pp.concat.addString3("\x66\x89\x87"); // mov word ptr [rdi + ?], ax
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_EAX_At_Stack(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString2("\x89\x07"); // mov dword ptr [rdi], eax
        }
        else
        {
            pp.concat.addString2("\x89\x87"); // mov dword ptr [rdi + ?], eax
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_RAX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x8B\x07"); // mov rax, qword ptr [rdi]
        }
        else
        {
            pp.concat.addString3("\x48\x8B\x87"); // mov rax, qword ptr [rdi + ?]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_RBX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x8B\x1F"); // mov rbx, qword ptr [rdi]
        }
        else
        {
            pp.concat.addString3("\x48\x8B\x9F"); // mov rbx, qword ptr [rdi + ?]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_RCX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x8B\x0F"); // mov rcx, qword ptr [rdi]
        }
        else
        {
            pp.concat.addString3("\x48\x8B\x8F"); // mov rcx, qword ptr [rdi + ?]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Stack_In_RDX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x8B\x17"); // mov rdx, qword ptr [rdi]
        }
        else
        {
            pp.concat.addString3("\x48\x8B\x97"); // mov rdx, qword ptr [rdi + ?]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_DeRef8_RAX(X64PerThread& pp)
    {
        pp.concat.addString2("\x8a\x00"); // mov al, byte ptr [rax]
    }

    inline void emit_DeRef16_RAX(X64PerThread& pp)
    {
        pp.concat.addString3("\x66\x8b\x00"); // mov ax, word ptr [rax]
    }

    inline void emit_DeRef32_RAX(X64PerThread& pp)
    {
        pp.concat.addString2("\x8b\x00"); // mov eax, dword ptr [rax]
    }

    inline void emit_DeRef64_RAX(X64PerThread& pp)
    {
        pp.concat.addString3("\x48\x8B\x00"); // mov rax, [rax]
    }

    inline void emit_DeRef64_RBX(X64PerThread& pp)
    {
        pp.concat.addString3("\x48\x8B\x1B"); // mov rbx, [rbx]
    }

    inline void emit_Lea_Stack_In_RAX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x89\xF8"); // mov rax, rdi
        }
        else
        {
            pp.concat.addString3("\x48\x8D\x87"); // lea rax, [rdi + ?]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Move_Cst8_At_RAX(X64PerThread& pp, uint8_t val)
    {
        pp.concat.addString2("\xC6\x00"); // mov byte ptr [rax], ?
        pp.concat.addU8(val);
    }

    inline void emit_Move_Cst16_At_RAX(X64PerThread& pp, uint16_t val)
    {
        pp.concat.addString3("\x66\xC7\x00"); // mov word ptr [rax], ?
        pp.concat.addU16(val);
    }

    inline void emit_Move_Cst32_At_RAX(X64PerThread& pp, uint32_t val)
    {
        pp.concat.addString3("\x48\xc7\x00"); // mov qword ptr [rax], ?
        pp.concat.addU32(val);
    }

    inline void emit_Clear_RAX(X64PerThread& pp)
    {
        pp.concat.addString3("\x48\x31\xc0"); // xor rax, rax
    }

    inline void emit_Clear_RBX(X64PerThread& pp)
    {
        pp.concat.addString3("\x48\x31\xdb"); // xor rbx, rbx
    }

    inline void emit_Clear_RDX(X64PerThread& pp)
    {
        pp.concat.addString2("\x31\xd2"); // xor rdx, rdx
    }

    inline void emit_Move_Cst64_In_RAX(X64PerThread& pp, uint64_t val)
    {
        if (val == 0)
            emit_Clear_RAX(pp);
        else if (val <= 0xFFFFFFFF)
        {
            pp.concat.addString3("\x48\xc7\xc0"); // mov rax, ?
            pp.concat.addU32((uint32_t) val);
        }
        {
            pp.concat.addString2("\x48\xb8"); // mov rax, ?
            pp.concat.addU64(val);
        }
    }

    inline void emit_Move_Cst64_In_RBX(X64PerThread& pp, uint64_t val)
    {
        if (val == 0)
            emit_Clear_RBX(pp);
        else if (val <= 0xFFFFFFFF)
        {
            pp.concat.addString3("\x48\xc7\xc3"); // mov rbx, ?
            pp.concat.addU32((uint32_t) val);
        }
        {
            pp.concat.addString2("\x48\xbb"); // mov rbx, ?
            pp.concat.addU64(val);
        }
    }

    inline void emit_Move_BL_At_RAX(X64PerThread& pp)
    {
        pp.concat.addString2("\x88\x18"); // mov [rax], bl
    }

    inline void emit_Move_BX_At_RAX(X64PerThread& pp)
    {
        pp.concat.addString3("\x66\x89\x18"); // mov [rax], bx
    }

    inline void emit_Move_EBX_At_RAX(X64PerThread& pp)
    {
        pp.concat.addString2("\x89\x18"); // mov [rax], ebx
    }

    inline void emit_Move_RBX_At_RAX(X64PerThread& pp)
    {
        pp.concat.addString3("\x48\x89\x18"); // mov qword ptr [rax], rbx
    }

    inline void emit_Move_AL_At_RBX(X64PerThread& pp)
    {
        pp.concat.addString2("\x88\x03"); // mov [rbx], al
    }

    inline void emit_Move_AX_At_RBX(X64PerThread& pp)
    {
        pp.concat.addString3("\x66\x89\x03"); // mov [rbx], ax
    }

    inline void emit_Move_EAX_At_RBX(X64PerThread& pp)
    {
        pp.concat.addString2("\x89\x03"); // mov [rbx], eax
    }

    inline void emit_Move_RAX_At_RBX(X64PerThread& pp)
    {
        pp.concat.addString3("\x48\x89\x03"); // mov [rbx], rax
    }

    inline void emit_Cmp_AL_With_BL(X64PerThread& pp)
    {
        pp.concat.addString2("\x38\xd8"); // cmp al, bl
    }

    inline void emit_Cmp_AX_With_BX(X64PerThread& pp)
    {
        pp.concat.addString3("\x66\x39\xd8"); // cmp ax, bx
    }

    inline void emit_Cmp_EAX_With_EBX(X64PerThread& pp)
    {
        pp.concat.addString2("\x39\xD8"); // cmp eax, ebx
    }

    inline void emit_Cmp_RAX_With_RBX(X64PerThread& pp)
    {
        pp.concat.addString3("\x48\x39\xD8"); // cmp rax, rbx
    }

    inline void emit_Test_AL_With_AL(X64PerThread& pp)
    {
        pp.concat.addString2("\x84\xC0"); // test al, al
    }

    inline void emit_Test_EAX_With_EAX(X64PerThread& pp)
    {
        pp.concat.addString2("\x85\xC0"); // test eax, eax
    }

    inline void emit_Test_RAX_With_RAX(X64PerThread& pp)
    {
        pp.concat.addString3("\x48\x85\xc0"); // test rax, rax
    }

    //////////////////////////////////////////////////
    inline void emit_Move_AL_At_Reg(X64PerThread& pp, uint32_t r)
    {
        emit_Move_AL_At_Stack(pp, r * sizeof(Register));
    }

    inline void emit_Move_AX_At_Reg(X64PerThread& pp, uint32_t r)
    {
        emit_Move_AX_At_Stack(pp, r * sizeof(Register));
    }

    inline void emit_Move_EAX_At_Reg(X64PerThread& pp, uint32_t r)
    {
        emit_Move_EAX_At_Stack(pp, r * sizeof(Register));
    }

    inline void emit_Move_RAX_At_Reg(X64PerThread& pp, uint32_t r)
    {
        emit_Move_RAX_At_Stack(pp, r * sizeof(Register));
    }

    inline void emit_Move_Reg_In_RAX(X64PerThread& pp, uint32_t r)
    {
        emit_Move_Stack_In_RAX(pp, r * sizeof(Register));
    }

    inline void emit_Move_Reg_In_RBX(X64PerThread& pp, uint32_t r)
    {
        emit_Move_Stack_In_RBX(pp, r * sizeof(Register));
    }

    inline void emit_Move_Reg_In_RCX(X64PerThread& pp, uint32_t r)
    {
        emit_Move_Stack_In_RCX(pp, r * sizeof(Register));
    }

    inline void emit_Move_Reg_In_RDX(X64PerThread& pp, uint32_t r)
    {
        emit_Move_Stack_In_RDX(pp, r * sizeof(Register));
    }

    inline void emit_Lea_Reg_In_RAX(X64PerThread& pp, uint32_t r)
    {
        emit_Lea_Stack_In_RAX(pp, r * sizeof(Register));
    }
}; // namespace BackendX64Inst