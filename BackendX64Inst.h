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

    inline void emitAdd2RAX(X64PerThread& pp, uint32_t value)
    {
        if (value)
        {
            pp.concat.addString2("\x48\x05"); // add rax, ?
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

    inline void emitMoveRAX2Reg(X64PerThread& pp, uint32_t r)
    {
        emitMoveRAX2Stack(pp, r * sizeof(Register));
    }

    inline void emitMoveStack2RAX(X64PerThread& pp, uint32_t stackOffset)
    {
        pp.concat.addString3("\x48\x8B\x87"); // mov rax, [rdi + ?]
        pp.concat.addU32(stackOffset);
    }

    inline void emitMoveReg2RAX(X64PerThread& pp, uint32_t r)
    {
        emitMoveStack2RAX(pp, r * sizeof(Register));
    }

    inline void emitMoveStack2RBX(X64PerThread& pp, uint32_t stackOffset)
    {
        pp.concat.addString3("\x48\x8B\x9F"); // mov rbx, [rdi + ?]
        pp.concat.addU32(stackOffset);
    }

    inline void emitMoveReg2RBX(X64PerThread& pp, uint32_t r)
    {
        emitMoveStack2RBX(pp, r * sizeof(Register));
    }

    inline void emitMoveStack2RCX(X64PerThread& pp, uint32_t stackOffset)
    {
        pp.concat.addString3("\x48\x8B\x8F"); // mov rcx, [rdi + ?]
        pp.concat.addU32(stackOffset);
    }

    inline void emitMoveReg2RCX(X64PerThread& pp, uint32_t r)
    {
        emitMoveStack2RCX(pp, r * sizeof(Register));
    }

    inline void emitDeRef64Rax(X64PerThread& pp)
    {
        pp.concat.addString3("\x48\x8B\x00"); // mov rax, [rax]
    }

    inline void emitLeaStack2Rax(X64PerThread& pp, uint32_t stackOffset)
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
}; // namespace BackendX64Inst