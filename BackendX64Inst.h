#pragma once
#include "Register.h"

namespace BackendX64Inst
{
    enum Instruction : uint8_t
    {
        Xor  = 0x33,
        Ret  = 0xC3,
        Call = 0xE8,
        Push = 0x50,
        Pop  = 0x58,
    };

    enum ModRM : uint8_t
    {
        EAX     = 0,
        ECX     = 1,
        EDX     = 2,
        EBX     = 3,
        ESP     = 4,
        EBP     = 5,
        ESI     = 6,
        EDI     = 7,
        EAX_EAX = 0xC0,
    };

    inline void emitR64(Concat& concat, Instruction inst, ModRM op)
    {
        concat.addU8(inst | op);
    }

    inline void emit(Concat& concat, Instruction inst)
    {
        concat.addU8(inst);
    }

    inline void emit(Concat& concat, Instruction inst, ModRM modRM)
    {
        concat.addU16(modRM << 8 | inst);
    }

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