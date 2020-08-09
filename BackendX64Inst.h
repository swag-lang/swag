#pragma once

namespace BackendX64Inst
{
    enum Instruction : uint8_t
    {
        Xor = 0x33,
        Ret = 0xC3,
    };

    enum ModRM : uint8_t
    {
        EAX_EAX = 0xC0,
    };

    inline void emit(Concat& concat, Instruction inst)
    {
        concat.addU8(inst);
    }

    inline void emit(Concat& concat, Instruction inst, ModRM modRM)
    {
        concat.addU16(modRM << 8 | inst);
    }
};