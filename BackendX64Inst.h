#pragma once
#include "Register.h"

namespace BackendX64Inst
{
    inline void emitSymbol2RAX(X64PerThread& pp, uint32_t symbolIndex)
    {
        auto& concat = pp.concat;

        concat.addString("\x48\x8D\x05", 3);

        CoffRelocation reloc;
        reloc.virtualAddress = concat.totalCount - pp.textSectionOffset;
        reloc.symbolIndex    = symbolIndex;
        reloc.type           = IMAGE_REL_AMD64_REL32;
        pp.relocTableTextSection.table.push_back(reloc);
        concat.addU32(0);
    }

    inline void emitAdd2RAX(X64PerThread& pp, uint32_t value)
    {
        // add rax, ?
        pp.concat.addString("\x48\x05", 2);
        pp.concat.addU32(value);
    }

    inline void emitMoveRAX2Stack(X64PerThread& pp, uint32_t stackOffset)
    {
        // mov [rdi + ?] = rax
        pp.concat.addString("\x48\x89\x87", 3);
        pp.concat.addU32(stackOffset);
    }

    inline void emitMoveRAX2Reg(X64PerThread& pp, uint32_t r)
    {
        emitMoveRAX2Stack(pp, r * sizeof(Register));
    }

    inline void emitMoveStack2RAX(X64PerThread& pp, uint32_t stackOffset)
    {
        // mov rax, [rdi + ?]
        pp.concat.addString("\x48\x8B\x87", 3);
        pp.concat.addU32(stackOffset);
    }

    inline void emitMoveReg2RAX(X64PerThread& pp, uint32_t r)
    {
        emitMoveStack2RAX(pp, r * sizeof(Register));
    }

    inline void emitMoveStack2RCX(X64PerThread& pp, uint32_t stackOffset)
    {
        // mov rcx, [rdi + ?]
        pp.concat.addString("\x48\x8B\x8F", 3);
        pp.concat.addU32(stackOffset);
    }

    inline void emitMoveReg2RCX(X64PerThread& pp, uint32_t r)
    {
        emitMoveStack2RCX(pp, r * sizeof(Register));
    }
}; // namespace BackendX64Inst