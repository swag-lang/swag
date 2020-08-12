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

    inline void emit_SubCst32_To_RSP(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            pp.concat.addString3("\x48\x81\xEC"); // sub rsp, ?
            pp.concat.addU32(value);
        }
    }

    inline void emit_AddCst32_To_RSP(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            pp.concat.addString3("\x48\x81\xC4"); // add rsp, ?
            pp.concat.addU32(value);
        }
    }

    inline void emit_AddCst32_To_RAX(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            pp.concat.addString2("\x48\x05"); // add rax, ?
            pp.concat.addU32(value);
        }
    }

    inline void emit_SubCst32_To_RAX(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            pp.concat.addString2("\x48\x2D"); // sub rax, ?
            pp.concat.addU32(value);
        }
    }

    inline void emit_MoveRAX_At_Stack(X64PerThread& pp, uint32_t stackOffset)
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

    inline void emit_MoveEAX_At_Stack(X64PerThread& pp, uint32_t stackOffset)
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

    inline void emit_MoveStack_In_RAX(X64PerThread& pp, uint32_t stackOffset)
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

    inline void emit_MoveStack_In_RBX(X64PerThread& pp, uint32_t stackOffset)
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

    inline void emit_MoveStack_In_RCX(X64PerThread& pp, uint32_t stackOffset)
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

    inline void emit_MoveStack_In_RDX(X64PerThread& pp, uint32_t stackOffset)
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

    inline void emitDeRefRAX(X64PerThread& pp)
    {
        pp.concat.addString3("\x48\x8B\x00"); // mov rax, [rax]
    }

    inline void emitDeRefRBX(X64PerThread& pp)
    {
        pp.concat.addString3("\x48\x8B\x1B"); // mov rbx, [rbx]
    }

    inline void emit_LeaStack_In_RAX(X64PerThread& pp, uint32_t stackOffset)
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

    inline void emit_MoveCst32_At_RAX(X64PerThread& pp, uint32_t val)
    {
        pp.concat.addString3("\x48\xc7\x00"); // mov qword ptr [rax], ?
        pp.concat.addU32(val);
    }

    inline void emit_MoveCst64_In_RBX(X64PerThread& pp, uint64_t val)
    {
        pp.concat.addString2("\x48\xbb"); // mov rbx, ?
        pp.concat.addU64(val);
    }

    inline void emit_MoveRBX_At_RAX(X64PerThread& pp)
    {
        pp.concat.addString3("\x48\x89\x18"); // mov qword ptr [rax], rbx
    }

    //////////////////////////////////////////////////
    inline void emit_MoveRAX_At_Reg(X64PerThread& pp, uint32_t r)
    {
        emit_MoveRAX_At_Stack(pp, r * sizeof(Register));
    }

    inline void emitMoveEAX2Reg(X64PerThread& pp, uint32_t r)
    {
        emit_MoveEAX_At_Stack(pp, r * sizeof(Register));
    }

    inline void emit_MoveReg_In_RAX(X64PerThread& pp, uint32_t r)
    {
        emit_MoveStack_In_RAX(pp, r * sizeof(Register));
    }

    inline void emit_MoveReg_In_RBX(X64PerThread& pp, uint32_t r)
    {
        emit_MoveStack_In_RBX(pp, r * sizeof(Register));
    }

    inline void emit_MoveReg_In_RCX(X64PerThread& pp, uint32_t r)
    {
        emit_MoveStack_In_RCX(pp, r * sizeof(Register));
    }

    inline void emit_MoveReg_In_RDX(X64PerThread& pp, uint32_t r)
    {
        emit_MoveStack_In_RDX(pp, r * sizeof(Register));
    }

    inline void emit_LeaReg_In_RAX(X64PerThread& pp, uint32_t r)
    {
        emit_LeaStack_In_RAX(pp, r * sizeof(Register));
    }
}; // namespace BackendX64Inst