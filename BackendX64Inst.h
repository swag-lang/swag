#pragma once
#include "Register.h"

namespace BackendX64Inst
{
    inline void emitSymbol2RAX(X64PerThread& pp, uint32_t symbolIndex)
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

    inline void emitSubRSP(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            pp.concat.addString3("\x48\x81\xEC"); // sub rsp, ?
            pp.concat.addU32(value);
        }
    }

    inline void emitAddRSP(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            pp.concat.addString3("\x48\x81\xC4"); // add rsp, ?
            pp.concat.addU32(value);
        }
    }

    inline void emitAdd2RAX(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            pp.concat.addString2("\x48\x05"); // add rax, ?
            pp.concat.addU32(value);
        }
    }

    inline void emitSub2RAX(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            pp.concat.addString2("\x48\x2D"); // sub rax, ?
            pp.concat.addU32(value);
        }
    }

    inline void emitMoveRAX2Stack(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x89\x07"); // mov [rdi], rax
        }
        else
        {
            pp.concat.addString3("\x48\x89\x87"); // mov [rdi + ?], rax
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emitMoveEAX2Stack(X64PerThread& pp, uint32_t stackOffset)
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

    inline void emitMoveStack2RAX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x8B\x07"); // mov rax, [rdi]
        }
        else
        {
            pp.concat.addString3("\x48\x8B\x87"); // mov rax, [rdi + ?]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emitMoveStack2RBX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x8B\x1F"); // mov rbx, [rdi]
        }
        else
        {
            pp.concat.addString3("\x48\x8B\x9F"); // mov rbx, [rdi + ?]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emitMoveStack2RCX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x8B\x0F"); // mov rcx, [rdi]
        }
        else
        {
            pp.concat.addString3("\x48\x8B\x8F"); // mov rcx, [rdi + ?]
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emitMoveStack2RDX(X64PerThread& pp, uint32_t stackOffset)
    {
        if (stackOffset == 0)
        {
            pp.concat.addString3("\x48\x8B\x17"); // mov rdx, [rdi]
        }
        else
        {
            pp.concat.addString3("\x48\x8B\x97"); // mov rdx, [rdi + ?]
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

    inline void emitLeaStack2RAX(X64PerThread& pp, uint32_t stackOffset)
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

    //////////////////////////////////////////////////
    inline void emitMoveRAX2Reg(X64PerThread& pp, uint32_t r)
    {
        emitMoveRAX2Stack(pp, r * sizeof(Register));
    }

    inline void emitMoveEAX2Reg(X64PerThread& pp, uint32_t r)
    {
        emitMoveEAX2Stack(pp, r * sizeof(Register));
    }

    inline void emitMoveReg2RAX(X64PerThread& pp, uint32_t r)
    {
        emitMoveStack2RAX(pp, r * sizeof(Register));
    }

    inline void emitMoveReg2RBX(X64PerThread& pp, uint32_t r)
    {
        emitMoveStack2RBX(pp, r * sizeof(Register));
    }

    inline void emitMoveReg2RCX(X64PerThread& pp, uint32_t r)
    {
        emitMoveStack2RCX(pp, r * sizeof(Register));
    }

    inline void emitMoveReg2RDX(X64PerThread& pp, uint32_t r)
    {
        emitMoveStack2RDX(pp, r * sizeof(Register));
    }

    inline void emitLeaReg2RAX(X64PerThread& pp, uint32_t r)
    {
        emitLeaStack2RAX(pp, r * sizeof(Register));
    }
}; // namespace BackendX64Inst