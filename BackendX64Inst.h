#pragma once

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
}; // namespace BackendX64Inst