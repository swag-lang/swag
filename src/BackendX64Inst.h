#pragma once
#include "Register.h"
#include "ByteCode.h"

#define regOffset(__r) __r * sizeof(Register)

enum Reg
{
    RAX = 0b0000,
    //RBX  = 0b0011,
    RCX = 0b0001,
    RDX = 0b0010,
    RSP = 0b0100,
    //RBP  = 0b0101,
    //RSI  = 0b0110,
    RDI = 0b0111,
    R8  = 0b1000,
    R9  = 0b1001,
    R10 = 0b1010,
    R11 = 0b1011,
    //R12  = 0b1100,
    //R13  = 0b1101,
    //R14  = 0b1110,
    //R15  = 0b1111,
    XMM0 = 0b0000,
    XMM1 = 0b0001,
    XMM2 = 0b0010,
    XMM3 = 0b0011,
};

enum class X64Op : uint8_t
{
    ADD  = 0x01,
    OR   = 0x09,
    AND  = 0x21,
    SUB  = 0x29,
    XOR  = 0x31,
    IDIV = 0xF7,
    MUL  = 0xC0,
    IMUL = 0xC1,
    FADD = 0x58,
    FSUB = 0x5C,
    FMUL = 0x59,
    FDIV = 0x5E,
    XCHG = 0x87
};

namespace BackendX64Inst
{
    enum Disp
    {
        DISP8  = 0b01,
        DISP32 = 0b10,
        REGREG = 0b11,
    };

    inline uint8_t modRM(uint8_t mod, uint8_t r, uint8_t m)
    {
        return mod << 6 | r << 3 | m;
    }

    enum JumpType
    {
        JNZ,
        JZ,
        JL,
        JLE,
        JB,
        JBE,
        JGE,
        JAE,
        JG,
        JA,
        JUMP,
    };

    inline void emit_ModRM(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg, uint8_t op = 1)
    {
        if (stackOffset == 0)
        {
            // mov al, byte ptr [rdi]
            pp.concat.addU8(modRM(0, reg, memReg) | (op - 1));
            if (memReg == RSP)
                pp.concat.addU8(0x24);
        }
        else if (stackOffset <= 0x7F)
        {
            // mov al, byte ptr [rdi + ??]
            pp.concat.addU8(modRM(DISP8, reg, memReg) | (op - 1));
            if (memReg == RSP)
                pp.concat.addU8(0x24);
            pp.concat.addU8((uint8_t) stackOffset);
        }
        else
        {
            // mov al, byte ptr [rdi + ????????]
            pp.concat.addU8(modRM(DISP32, reg, memReg) | (op - 1));
            if (memReg == RSP)
                pp.concat.addU8(0x24);
            pp.concat.addU32(stackOffset);
        }
    }

    inline void emit_Load8_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x8A);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_LoadS8S32_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0xBE);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_LoadU8U32_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0xB6);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_Load16_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x8B);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_Load32_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(memReg < R8);
        if (reg >= R8)
            pp.concat.addU8(0x44);
        pp.concat.addU8(0x8B);
        emit_ModRM(pp, stackOffset, (reg & 0b111), memReg);
    }

    inline void emit_Load64_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(memReg < R8);
        pp.concat.addU8(0x48 | ((reg & 0b1000) >> 1));
        pp.concat.addU8(0x8B);
        emit_ModRM(pp, stackOffset, (reg & 0b111), memReg);
    }

    inline void emit_LoadF32_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0xF3);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x10);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_LoadF64_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0xF2);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x10);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_Store8_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x88);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_Store16_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x89);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_Store32_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x89);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_Store64_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        pp.concat.addU8(0x48 | ((reg & 0b1000) >> 1) | ((memReg & 0b1000) >> 3));
        pp.concat.addU8(0x89);
        emit_ModRM(pp, stackOffset, (reg & 0b111), (memReg & 0b111));
    }

    inline void emit_StoreF32_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0xF3);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x11);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_StoreF64_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0xF2);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x11);
        emit_ModRM(pp, stackOffset, reg, memReg);
    }

    inline void emit_Store8_Immediate(X64PerThread& pp, uint32_t offset, uint8_t val, uint8_t reg)
    {
        pp.concat.addU8(0xC6);
        emit_ModRM(pp, offset, 0, reg);
        pp.concat.addU8(val);
    }

    inline void emit_Store16_Immediate(X64PerThread& pp, uint32_t offset, uint16_t val, uint8_t reg)
    {
        pp.concat.addU8(0x66);
        pp.concat.addU8(0xC7);
        emit_ModRM(pp, offset, 0, reg);
        pp.concat.addU16(val);
    }

    inline void emit_Store32_Immediate(X64PerThread& pp, uint32_t offset, uint32_t val, uint8_t reg)
    {
        pp.concat.addU8(0xC7);
        emit_ModRM(pp, offset, 0, reg);
        pp.concat.addU32((uint32_t) val);
    }

    inline void emit_Store64_Immediate(X64PerThread& pp, uint32_t offset, uint64_t val, uint8_t reg)
    {
        SWAG_ASSERT(val <= 0x7FFFFFFF);
        pp.concat.addU8(0x48);
        pp.concat.addU8(0xC7);
        emit_ModRM(pp, offset, 0, reg);
        pp.concat.addU32((uint32_t) val);
    }

    inline void emit_Clear8(X64PerThread& pp, uint8_t reg)
    {
        SWAG_ASSERT(reg < R8);
        pp.concat.addU8(0x30);
        pp.concat.addU8(modRM(3, reg, reg));
    }

    inline void emit_Clear16(X64PerThread& pp, uint8_t reg)
    {
        SWAG_ASSERT(reg < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x31);
        pp.concat.addU8(modRM(3, reg, reg));
    }

    inline void emit_Clear32(X64PerThread& pp, uint8_t reg)
    {
        SWAG_ASSERT(reg < R8);
        pp.concat.addU8(0x31);
        pp.concat.addU8(modRM(3, reg, reg));
    }

    inline void emit_Clear64(X64PerThread& pp, uint8_t reg)
    {
        pp.concat.addU8(0x48 | ((reg & 0b1000) >> 1) | ((reg & 0b1000) >> 3));
        pp.concat.addU8(0x31);
        pp.concat.addU8(modRM(3, (reg & 0b111), (reg & 0b111)));
    }

    inline void emit_Load8_Immediate(X64PerThread& pp, uint8_t val, uint8_t reg)
    {
        if (val == 0)
        {
            emit_Clear8(pp, reg);
            return;
        }

        pp.concat.addU8(0xB0 | reg);
        pp.concat.addU8(val);
    }

    inline void emit_Load16_Immediate(X64PerThread& pp, uint16_t val, uint8_t reg)
    {
        if (val == 0)
        {
            emit_Clear16(pp, reg);
            return;
        }

        pp.concat.addU8(0x66);
        pp.concat.addU8(0xB8 | reg);
        pp.concat.addU16(val);
    }

    inline void emit_Load32_Immediate(X64PerThread& pp, uint32_t val, uint8_t reg)
    {
        if (val == 0)
        {
            emit_Clear32(pp, reg);
            return;
        }

        pp.concat.addU8(0xB8 | reg);
        pp.concat.addU32(val);
    }

    inline void emit_Load64_Immediate(X64PerThread& pp, uint64_t val, uint8_t reg, bool force64bits = false)
    {
        if (force64bits)
        {
            pp.concat.addU8(0x48 | ((reg & 0b1000) >> 3));
            pp.concat.addU8(0xB8 | reg);
            pp.concat.addU64(val);
            return;
        }

        if (val == 0)
        {
            emit_Clear64(pp, reg);
            return;
        }

        if (val <= 0x7FFFFFFF && reg < R8)
        {
            emit_Load32_Immediate(pp, (uint32_t) val, reg);
            return;
        }

        pp.concat.addU8(0x48 | ((reg & 0b1000) >> 3));
        if (val <= 0x7FFFFFFF)
        {
            pp.concat.addU8(0xC7);
            pp.concat.addU8(0xC0 | (reg & 0b111));
            pp.concat.addU32((uint32_t) val);
        }
        else
        {
            pp.concat.addU8(0xB8 | reg);
            pp.concat.addU64(val);
        }
    }

    inline void emit_Push(X64PerThread& pp, uint8_t reg)
    {
        SWAG_ASSERT(reg < R8);
        pp.concat.addU8(0x50 | (reg & 0b111));
    }

    inline void emit_Pop(X64PerThread& pp, uint8_t reg)
    {
        SWAG_ASSERT(reg < R8);
        pp.concat.addU8(0x58 | (reg & 0b111));
    }

    inline void emit_Ret(X64PerThread& pp)
    {
        pp.concat.addU8(0xC3);
    }

    inline void emit_Copy64(X64PerThread& pp, uint8_t regSrc, uint8_t regDst)
    {
        pp.concat.addU8(0x48 | ((regDst & 0b1000) >> 3) | ((regSrc & 0b1000) >> 1));
        pp.concat.addU8(0x89);
        pp.concat.addU8(modRM(0b11, regSrc & 0b111, regDst & 0b111));
    }

    inline void emit_Copy8(X64PerThread& pp, uint8_t regSrc, uint8_t regDst)
    {
        SWAG_ASSERT(regDst < R8 && regSrc < R8);
        pp.concat.addU8(0x88);
        pp.concat.addU8(modRM(0b11, regSrc & 0b111, regDst & 0b111));
    }

    inline void emit_Copy16(X64PerThread& pp, uint8_t regSrc, uint8_t regDst)
    {
        SWAG_ASSERT(regDst < R8 && regSrc < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x89);
        pp.concat.addU8(modRM(0b11, regSrc & 0b111, regDst & 0b111));
    }

    inline void emit_Copy32(X64PerThread& pp, uint8_t regSrc, uint8_t regDst)
    {
        SWAG_ASSERT(regDst < R8 && regSrc < R8);
        pp.concat.addU8(0x89);
        pp.concat.addU8(modRM(0b11, regSrc & 0b111, regDst & 0b111));
    }

    inline void emit_CopyF32(X64PerThread& pp, uint8_t regSrc, uint8_t regDst)
    {
        SWAG_ASSERT(regSrc == RAX);
        SWAG_ASSERT(regDst == XMM0 || regDst == XMM1);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x6E);
        pp.concat.addU8(0xC0 | (regDst << 3));
    }

    inline void emit_CopyF64(X64PerThread& pp, uint8_t regSrc, uint8_t regDst)
    {
        SWAG_ASSERT(regSrc == RAX);
        SWAG_ASSERT(regDst == XMM0 || regDst == XMM1);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x48);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x6E);
        pp.concat.addU8(0xC0 | (regDst << 3));
    }

    inline void emit_SetNE(X64PerThread& pp)
    {
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x95);
        pp.concat.addU8(0xC0);
    }

    inline void emit_SetA(X64PerThread& pp, uint8_t regDst = RAX)
    {
        SWAG_ASSERT(regDst < R8);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x97);
        pp.concat.addU8(0xC0 | regDst);
    }

    inline void emit_SetAE(X64PerThread& pp, uint8_t regDst = RAX)
    {
        SWAG_ASSERT(regDst < R8);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x93);
        pp.concat.addU8(0xC0 | regDst);
    }

    inline void emit_SetNA(X64PerThread& pp)
    {
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x96);
        pp.concat.addU8(0xC0);
    }

    inline void emit_SetB(X64PerThread& pp)
    {
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x92);
        pp.concat.addU8(0xC0);
    }

    inline void emit_SetBE(X64PerThread& pp)
    {
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x96);
        pp.concat.addU8(0xC0);
    }

    inline void emit_SetE(X64PerThread& pp)
    {
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x94);
        pp.concat.addU8(0xC0);
    }

    inline void emit_SetG(X64PerThread& pp)
    {
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x9F);
        pp.concat.addU8(0xC0);
    }

    inline void emit_SetGE(X64PerThread& pp)
    {
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x9D);
        pp.concat.addU8(0xC0);
    }

    inline void emit_SetL(X64PerThread& pp)
    {
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x9C);
        pp.concat.addU8(0xC0);
    }

    inline void emit_SetLE(X64PerThread& pp)
    {
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x9E);
        pp.concat.addU8(0xC0);
    }

    inline void emit_Test64(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        pp.concat.addU8(0x48 | ((reg1 & 0b1000) >> 3) | ((reg2 & 0b1000) >> 1));
        pp.concat.addU8(0x85);
        pp.concat.addU8(modRM(0b11, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_Test32(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x85);
        pp.concat.addU8(modRM(0b11, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_Test16(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x85);
        pp.concat.addU8(modRM(0b11, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_Test8(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x84);
        pp.concat.addU8(modRM(0b11, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_Cmp8(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x38);
        pp.concat.addU8(modRM(0b11, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_Cmp16(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x39);
        pp.concat.addU8(modRM(0b11, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_Cmp32(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x39);
        pp.concat.addU8(modRM(0b11, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_Cmp64(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        pp.concat.addU8(0x48 | ((reg1 & 0b1000) >> 3) | ((reg2 & 0b1000) >> 1));
        pp.concat.addU8(0x39);
        pp.concat.addU8(modRM(0b11, reg2 & 0b111, reg1 & 0b111));
    }

    inline void emit_CmpF32(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x2F);
        pp.concat.addU8(modRM(0b11, reg1 & 0b111, reg2 & 0b111));
    }

    inline void emit_CmpF64(X64PerThread& pp, uint8_t reg1, uint8_t reg2)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x2F);
        pp.concat.addU8(modRM(0b11, reg1 & 0b111, reg2 & 0b111));
    }

    inline void emit_Cmp8_Indirect(X64PerThread& pp, uint32_t offsetStack, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x3A);
        emit_ModRM(pp, offsetStack, reg & 0b111, memReg & 0b111);
    }

    inline void emit_Cmp16_Indirect(X64PerThread& pp, uint32_t offsetStack, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x3B);
        emit_ModRM(pp, offsetStack, reg & 0b111, memReg & 0b111);
    }

    inline void emit_Cmp32_Indirect(X64PerThread& pp, uint32_t offsetStack, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x3B);
        emit_ModRM(pp, offsetStack, reg & 0b111, RDI);
    }

    inline void emit_Cmp64_Indirect(X64PerThread& pp, uint32_t offsetStack, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x48);
        pp.concat.addU8(0x3B);
        emit_ModRM(pp, offsetStack, reg & 0b111, memReg & 0b111);
    }

    inline void emit_Cmp8_IndirectDst(X64PerThread& pp, uint32_t offsetStack, uint32_t value)
    {
        pp.concat.addU8(0x80);
        emit_ModRM(pp, offsetStack, RDI, RDI);
        pp.concat.addU8((uint8_t) value);
    }

    inline void emit_Cmp32_IndirectDst(X64PerThread& pp, uint32_t offsetStack, uint32_t value)
    {
        if (value <= 0x7F)
            pp.concat.addU8(0x83);
        else
            pp.concat.addU8(0x81);

        emit_ModRM(pp, offsetStack, RDI, RDI);

        if (value <= 0x7F)
            pp.concat.addU8((uint8_t) value);
        else
            pp.concat.addU32((uint32_t) value);
    }

    inline void emit_Cmp16_IndirectDst(X64PerThread& pp, uint32_t offsetStack, uint32_t value)
    {
        pp.concat.addU8(0x66);
        emit_Cmp32_IndirectDst(pp, offsetStack, value);
    }

    inline void emit_Cmp64_IndirectDst(X64PerThread& pp, uint32_t offsetStack, uint32_t value)
    {
        pp.concat.addU8(0x48);
        emit_Cmp32_IndirectDst(pp, offsetStack, value);
    }

    inline void emit_CmpF32_Indirect(X64PerThread& pp, uint32_t offsetStack, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x2F);
        emit_ModRM(pp, offsetStack, reg & 0b111, memReg & 0b111);
    }

    inline void emit_CmpF64_Indirect(X64PerThread& pp, uint32_t offsetStack, uint8_t reg, uint8_t memReg)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0x2F);
        emit_ModRM(pp, offsetStack, reg & 0b111, memReg & 0b111);
    }

    inline void emit_LoadAddress_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg, uint8_t memReg)
    {
        if (stackOffset == 0)
        {
            emit_Copy64(pp, memReg, reg);
            return;
        }

        pp.concat.addU8(0x48 | ((memReg & 0b1000) >> 3) | ((reg & 0b1000) >> 1));
        pp.concat.addU8(0x8D);
        emit_ModRM(pp, stackOffset, reg & 0b111, memReg & 0b111);
    }

    inline void emit_Inc32_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg)
    {
        SWAG_ASSERT(reg < R8);
        pp.concat.addU8(0xFF);
        emit_ModRM(pp, stackOffset, 0, reg);
    }

    inline void emit_Dec32_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg)
    {
        SWAG_ASSERT(reg < R8);
        pp.concat.addU8(0xFF);
        emit_ModRM(pp, stackOffset, 1, reg);
    }

    inline void emit_Inc64_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg)
    {
        SWAG_ASSERT(reg < R8);
        pp.concat.addU8(0x48);
        pp.concat.addU8(0xFF);
        emit_ModRM(pp, stackOffset, 0, reg);
    }

    inline void emit_Dec64_Indirect(X64PerThread& pp, uint32_t stackOffset, uint8_t reg)
    {
        SWAG_ASSERT(reg < R8);
        pp.concat.addU8(0x48);
        pp.concat.addU8(0xFF);
        emit_ModRM(pp, stackOffset, 1, reg);
    }

    inline void emit_Op8_Indirect(X64PerThread& pp, uint32_t offsetStack, uint8_t reg, uint8_t memReg, X64Op instruction, bool lock = false)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        if (lock)
            pp.concat.addU8(0xF0);
        pp.concat.addU8((uint8_t) instruction & ~1);
        emit_ModRM(pp, offsetStack, reg & 0b111, memReg & 0b111);
    }

    inline void emit_Op16_Indirect(X64PerThread& pp, uint32_t offsetStack, uint8_t reg, uint8_t memReg, X64Op instruction, bool lock = false)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        if (lock)
            pp.concat.addU8(0xF0);
        pp.concat.addU8(0x66);
        pp.concat.addU8((uint8_t) instruction);
        emit_ModRM(pp, offsetStack, reg & 0b111, memReg & 0b111);
    }

    inline void emit_Op32_Indirect(X64PerThread& pp, uint32_t offsetStack, uint8_t reg, uint8_t memReg, X64Op instruction, bool lock = false)
    {
        SWAG_ASSERT(memReg < R8);
        if (lock)
            pp.concat.addU8(0xF0);
        if (reg >= R8)
            pp.concat.addU8(0x44);
        pp.concat.addU8((uint8_t) instruction);
        emit_ModRM(pp, offsetStack, reg & 0b111, memReg & 0b111);
    }

    inline void emit_Op64_IndirectDst(X64PerThread& pp, uint32_t offsetStack, uint8_t reg, uint8_t memReg, X64Op instruction, bool lock = false)
    {
        SWAG_ASSERT(memReg < R8 && reg < R8);
        if (lock)
            pp.concat.addU8(0xF0);
        pp.concat.addU8(0x48);
        pp.concat.addU8((uint8_t) instruction);
        emit_ModRM(pp, offsetStack, reg & 0b111, memReg & 0b111);
    }

    inline void emit_Op64_IndirectSrc(X64PerThread& pp, uint32_t offsetStack, uint8_t reg, uint8_t memReg, X64Op instruction, bool lock = false)
    {
        if (lock)
            pp.concat.addU8(0xF0);
        pp.concat.addU8(0x48);
        pp.concat.addU8((uint8_t) instruction | 2);
        emit_ModRM(pp, offsetStack, reg & 0b111, memReg & 0b111);
    }

    inline void emit_OpF32_Indirect(X64PerThread& pp, uint8_t reg, uint8_t memReg, X64Op instruction)
    {
        SWAG_ASSERT(reg == XMM1);
        SWAG_ASSERT(memReg < R8);
        emit_LoadF32_Indirect(pp, 0, XMM0, memReg);
        pp.concat.addU8(0xF3);
        pp.concat.addU8(0x0F);
        pp.concat.addU8((uint8_t) instruction);
        pp.concat.addU8(0xC1);
        emit_StoreF32_Indirect(pp, 0, XMM0, memReg);
    }

    inline void emit_OpF64_Indirect(X64PerThread& pp, uint8_t reg, uint8_t memReg, X64Op instruction)
    {
        SWAG_ASSERT(reg == XMM1);
        SWAG_ASSERT(memReg < R8);
        emit_LoadF64_Indirect(pp, 0, XMM0, memReg);
        pp.concat.addU8(0xF2);
        pp.concat.addU8(0x0F);
        pp.concat.addU8((uint8_t) instruction);
        pp.concat.addU8(0xC1);
        emit_StoreF64_Indirect(pp, 0, XMM0, memReg);
    }

    inline void emit_Op8(X64PerThread& pp, uint8_t reg1, uint8_t reg2, X64Op instruction)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8((uint8_t) instruction & ~1);
        pp.concat.addU8(modRM(0b11, reg1, reg2));
    }

    inline void emit_Op16(X64PerThread& pp, uint8_t reg1, uint8_t reg2, X64Op instruction)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8(0x66);
        pp.concat.addU8((uint8_t) instruction);
        pp.concat.addU8(modRM(0b11, reg1, reg2));
    }

    inline void emit_Op32(X64PerThread& pp, uint8_t reg1, uint8_t reg2, X64Op instruction)
    {
        SWAG_ASSERT(reg1 < R8 && reg2 < R8);
        pp.concat.addU8((uint8_t) instruction);
        pp.concat.addU8(modRM(0b11, reg1, reg2));
    }

    inline void emit_Op64(X64PerThread& pp, uint8_t reg1, uint8_t reg2, X64Op instruction)
    {
        pp.concat.addU8(0x48 | ((reg2 & 0b1000) >> 3) | ((reg1 & 0b1000) >> 1));
        pp.concat.addU8((uint8_t) instruction);
        pp.concat.addU8(modRM(0b11, reg1 & 0b111, reg2 & 0b111));
    }

    inline void emit_Add64_Immediate(X64PerThread& pp, uint64_t value, uint8_t reg)
    {
        if (!value)
            return;
        SWAG_ASSERT(reg == RAX || reg == RCX);
        SWAG_ASSERT(value <= 0x7FFFFFFF);
        pp.concat.addU8(0x48);
        if (value <= 0x7F)
        {
            pp.concat.addU8(0x83);
            pp.concat.addU8(0xC0 | reg);
            pp.concat.addU8((uint8_t) value);
        }
        else if (reg == RAX)
        {
            pp.concat.addU8(0x05);
            pp.concat.addU32((uint32_t) value);
        }
        else if (reg == RCX)
        {
            pp.concat.addU8(0x81);
            pp.concat.addU8(0xC1);
            pp.concat.addU32((uint32_t) value);
        }
    }

    /////////////////////////////////////////////////////////////////////

    inline void emit_Symbol_RelocationAddr(X64PerThread& pp, uint8_t reg, uint32_t symbolIndex, uint32_t offset)
    {
        auto& concat = pp.concat;

        concat.addU8(0x48);
        concat.addU8(0x8D);
        concat.addU8(0x05 | (reg << 3));

        CoffRelocation reloc;
        reloc.virtualAddress = concat.totalCount() - pp.textSectionOffset;
        reloc.symbolIndex    = symbolIndex;
        reloc.type           = IMAGE_REL_AMD64_REL32;
        pp.relocTableTextSection.table.push_back(reloc);
        concat.addU32(offset);
    }

    inline void emit_Symbol_RelocationValue(X64PerThread& pp, uint8_t reg, uint32_t symbolIndex, uint32_t offset)
    {
        auto& concat = pp.concat;

        concat.addU8(0x48);
        concat.addU8(0x8B);
        concat.addU8(0x05 | (reg << 3));

        CoffRelocation reloc;
        reloc.virtualAddress = concat.totalCount() - pp.textSectionOffset;
        reloc.symbolIndex    = symbolIndex;
        reloc.type           = IMAGE_REL_AMD64_REL32;
        pp.relocTableTextSection.table.push_back(reloc);
        concat.addU32(offset);
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

    /////////////////////////////////////////////////////////////////////

    // clang-format off
    inline void emit_SignedExtend_AL_To_AX(X64PerThread& pp) { pp.concat.addString2("\x66\x98"); } // cbw
    inline void emit_SignedExtend_AL_To_EAX(X64PerThread& pp) { pp.concat.addString3("\x0F\xBE\xC0"); } // movsx eax, al
    inline void emit_SignedExtend_AL_To_RAX(X64PerThread& pp) { pp.concat.addString4("\x48\x0F\xBE\xC0"); } // movsx rax, al
    inline void emit_SignedExtend_AX_To_EAX(X64PerThread& pp) { pp.concat.addU8(0x98); } // cwde
    inline void emit_SignedExtend_AX_To_RAX(X64PerThread& pp) { pp.concat.addString4("\x48\x0F\xBF\xC0"); } // movsx rax, ax
    inline void emit_SignedExtend_EAX_To_RAX(X64PerThread& pp) { pp.concat.addString2("\x48\x98"); } // cdqe
    inline void emit_SignedExtend_BX_To_EBX(X64PerThread& pp) { pp.concat.addString3("\x0f\xbf\xdb"); } // movsx ebx, bx
    inline void emit_SignedExtend_CL_To_ECX(X64PerThread& pp) { pp.concat.addString3("\x0f\xbe\xC9"); } // movsx ecx, cl
    inline void emit_SignedExtend_ECX_To_RCX(X64PerThread& pp) { pp.concat.addString3("\x48\x63\xC9"); } // movsx rcx, ecx

    inline void emit_SignedExtend_8_To_32(X64PerThread& pp, uint8_t reg)
    {
        switch (reg)
        {
        case RAX:
            emit_SignedExtend_AL_To_EAX(pp);
            break;
        case RCX:
            emit_SignedExtend_CL_To_ECX(pp);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
    }

    inline void emit_UnsignedExtend_8_To_32(X64PerThread& pp, uint8_t reg)
    {
        // movzx eax, al
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0xB6);
        pp.concat.addU8(modRM(REGREG, reg, reg));
    }

    inline void emit_UnsignedExtend_16_To_32(X64PerThread& pp, uint8_t reg)
    {
        // movzx rax, ax
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0xB7);
        pp.concat.addU8(modRM(REGREG, reg, reg));
    }

    inline void emit_UnsignedExtend_8_To_64(X64PerThread& pp, uint8_t reg)
    {
        // movzx rax, al
        pp.concat.addU8(0x48);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0xB6);
        pp.concat.addU8(modRM(REGREG, reg, reg));
    }

    inline void emit_UnsignedExtend_16_To_64(X64PerThread& pp, uint8_t reg)
    {
        // movzx rax, al
        pp.concat.addU8(0x48);
        pp.concat.addU8(0x0F);
        pp.concat.addU8(0xB7);
        pp.concat.addU8(modRM(REGREG, reg, reg));
    }

    // clang-format on

    /////////////////////////////////////////////////////////////////////

    inline void emit_BinOpFloat32(X64PerThread& pp, ByteCodeInstruction* ip, X64Op op)
    {
        if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
        {
            BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            pp.concat.addU8(0xF3);
            pp.concat.addU8(0x0F);
            pp.concat.addU8((uint8_t) op);
            emit_ModRM(pp, regOffset(ip->b.u32), XMM0, RDI);
        }
        else
        {
            if (ip->flags & BCI_IMM_A)
            {
                BackendX64Inst::emit_Load32_Immediate(pp, ip->a.u32, RAX);
                BackendX64Inst::emit_CopyF32(pp, RAX, XMM0);
            }
            else
                BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            if (ip->flags & BCI_IMM_B)
            {
                BackendX64Inst::emit_Load32_Immediate(pp, ip->b.u32, RAX);
                BackendX64Inst::emit_CopyF32(pp, RAX, XMM1);
            }
            else
                BackendX64Inst::emit_LoadF32_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            pp.concat.addU8(0xF3);
            pp.concat.addU8(0x0F);
            pp.concat.addU8((uint8_t) op);
            pp.concat.addU8(0xC1);
        }
    }

    inline void emit_BinOpFloat32_At_Reg(X64PerThread& pp, ByteCodeInstruction* ip, X64Op op)
    {
        emit_BinOpFloat32(pp, ip, op);
        BackendX64Inst::emit_StoreF32_Indirect(pp, regOffset(ip->c.u32), XMM0, RDI);
    }

    inline void emit_BinOpFloat64(X64PerThread& pp, ByteCodeInstruction* ip, X64Op op)
    {
        if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
        {
            BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            pp.concat.addU8(0xF2);
            pp.concat.addU8(0x0F);
            pp.concat.addU8((uint8_t) op);
            emit_ModRM(pp, regOffset(ip->b.u32), XMM0, RDI);
        }
        else
        {
            if (ip->flags & BCI_IMM_A)
            {
                BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u64, RAX);
                BackendX64Inst::emit_CopyF64(pp, RAX, XMM0);
            }
            else
                BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->a.u32), XMM0, RDI);
            if (ip->flags & BCI_IMM_B)
            {
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RAX);
                BackendX64Inst::emit_CopyF64(pp, RAX, XMM1);
            }
            else
                BackendX64Inst::emit_LoadF64_Indirect(pp, regOffset(ip->b.u32), XMM1, RDI);
            pp.concat.addU8(0xF2);
            pp.concat.addU8(0x0F);
            pp.concat.addU8((uint8_t) op);
            pp.concat.addU8(0xC1);
        }
    }

    inline void emit_BinOpFloat64_At_Reg(X64PerThread& pp, ByteCodeInstruction* ip, X64Op op)
    {
        emit_BinOpFloat64(pp, ip, op);
        BackendX64Inst::emit_StoreF64_Indirect(pp, regOffset(ip->c.u32), XMM0, RDI);
    }

    inline void emit_BinOpInt8(X64PerThread& pp, ByteCodeInstruction* ip, X64Op op)
    {
        SWAG_ASSERT(op == X64Op::AND || op == X64Op::OR);
        if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
        {
            BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            pp.concat.addU8((uint8_t) op + 1);
            emit_ModRM(pp, regOffset(ip->b.u32), RCX, RDI); // cl, [rdi+?]
        }
        else
        {
            if (ip->flags & BCI_IMM_A)
                BackendX64Inst::emit_Load8_Immediate(pp, ip->a.u8, RCX);
            else
                BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Load8_Immediate(pp, ip->b.u8, RAX);
            else
                BackendX64Inst::emit_Load8_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            emit_Op8(pp, RAX, RCX, op);
        }
    }

    inline void emit_BinOpInt16(X64PerThread& pp, ByteCodeInstruction* ip, X64Op op)
    {
        SWAG_ASSERT(op == X64Op::AND || op == X64Op::OR);
        if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
        {
            BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            pp.concat.addU8(0x66);
            pp.concat.addU8((uint8_t) op | 2);
            emit_ModRM(pp, regOffset(ip->b.u32), RCX, RDI); // cx, [rdi+?]
        }
        else
        {
            if (ip->flags & BCI_IMM_A)
                BackendX64Inst::emit_Load16_Immediate(pp, ip->a.u16, RCX);
            else
                BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Load16_Immediate(pp, ip->b.u16, RAX);
            else
                BackendX64Inst::emit_Load16_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            emit_Op16(pp, RAX, RCX, op);
        }
    }

    inline void emit_BinOpInt32(X64PerThread& pp, ByteCodeInstruction* ip, X64Op op)
    {
        if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
        {
            if (op == X64Op::MUL)
            {
                BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
                pp.concat.addString1("\xF7"); // mul
                // need to pass rsp here, don't know why, but the encoding should be 0x67 for a small stack offset
                emit_ModRM(pp, regOffset(ip->b.u32), RSP, RDI);
            }
            else
            {
                BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
                if (op == X64Op::IMUL)
                    pp.concat.addString2("\x0F\xAF"); // imul
                else
                    pp.concat.addU8((uint8_t) op | 2);
                emit_ModRM(pp, regOffset(ip->b.u32), RCX, RDI); // ecx, [rdi+?]
            }
        }
        else
        {
            if (ip->flags & BCI_IMM_A)
                BackendX64Inst::emit_Load32_Immediate(pp, ip->a.u32, RCX);
            else
                BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Load32_Immediate(pp, ip->b.u32, RAX);
            else
                BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            if (op == X64Op::MUL)
            {
                // mul rcx
                pp.concat.addString3("\x48\xF7\xE1");
            }
            else if (op == X64Op::IMUL)
            {
                // imul ecx, eax
                pp.concat.addString2("\x0F\xAF");
                pp.concat.addU8(0xC8);
            }
            else
                emit_Op32(pp, RAX, RCX, op);
        }
    }

    inline void emit_BinOpInt8_At_Reg(X64PerThread& pp, ByteCodeInstruction* ip, X64Op op)
    {
        emit_BinOpInt8(pp, ip, op);
        BackendX64Inst::emit_Store8_Indirect(pp, regOffset(ip->c.u32), RCX, RDI);
    }

    inline void emit_BinOpInt16_At_Reg(X64PerThread& pp, ByteCodeInstruction* ip, X64Op op)
    {
        emit_BinOpInt16(pp, ip, op);
        BackendX64Inst::emit_Store16_Indirect(pp, regOffset(ip->c.u32), RCX, RDI);
    }

    inline void emit_BinOpInt32_At_Reg(X64PerThread& pp, ByteCodeInstruction* ip, X64Op op)
    {
        emit_BinOpInt32(pp, ip, op);
        if (op == X64Op::MUL)
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
        else
            BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RCX, RDI);
    }

    inline void emit_BinOpInt64(X64PerThread& pp, ByteCodeInstruction* ip, X64Op op)
    {
        if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
        {
            if (op == X64Op::MUL)
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
                pp.concat.addU8(0x48);
                pp.concat.addString1("\xF7"); // mul
                // need to pass rsp here, don't know why, but the encoding should be 0x67 for a small stack offset
                emit_ModRM(pp, regOffset(ip->b.u32), RSP, RDI);
            }
            else
            {
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
                pp.concat.addU8(0x48);
                if (op == X64Op::IMUL)
                    pp.concat.addString2("\x0F\xAF"); // imul
                else
                    pp.concat.addU8((uint8_t) op | 2);
                emit_ModRM(pp, regOffset(ip->b.u32), RCX, RDI);
            }
        }
        else
        {
            if (ip->flags & BCI_IMM_A)
                BackendX64Inst::emit_Load64_Immediate(pp, ip->a.u64, RCX);
            else
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RCX, RDI);
            if (ip->flags & BCI_IMM_B)
                BackendX64Inst::emit_Load64_Immediate(pp, ip->b.u64, RAX);
            else
                BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->b.u32), RAX, RDI);
            if (op == X64Op::MUL)
            {
                // mul rcx
                pp.concat.addString3("\x48\xF7\xE1");
            }
            else if (op == X64Op::IMUL)
            {
                // imul rcx, rax
                pp.concat.addU8(0x48);
                pp.concat.addU8(0x0F);
                pp.concat.addU8(0xAF);
                pp.concat.addU8(0xC8);
            }
            else
                emit_Op64(pp, RAX, RCX, op);
        }
    }

    inline void emit_BinOpInt64_At_Reg(X64PerThread& pp, ByteCodeInstruction* ip, X64Op op)
    {
        emit_BinOpInt64(pp, ip, op);
        if (op == X64Op::MUL)
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
        else
            BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RCX, RDI);
    }

    /////////////////////////////////////////////////////////////////////
    inline void emit_imul64_RAX(X64PerThread& pp, uint64_t value)
    {
        if (value <= 0x7F)
        {
            pp.concat.addString3("\x48\x6B\xC0"); // imul rax, ??
            pp.concat.addU8((uint8_t) value);
        }
        else if (value <= 0x7FFFFFFF)
        {
            pp.concat.addString3("\x48\x69\xC0"); // imul rax, ????????
            pp.concat.addU32((uint32_t) value);
        }
        else
        {
            BackendX64Inst::emit_Load64_Immediate(pp, value, RCX);
            pp.concat.addString4("\x48\x0F\xAF\xC1"); // imul rax, rcx
            pp.concat.addU64(value);
        }
    }

    /////////////////////////////////////////////////////////////////////

    inline void emit_BinOpInt_Div_At_Reg(X64PerThread& pp, ByteCodeInstruction* ip, bool isSigned, uint32_t bits, bool modulo = false)
    {
        switch (bits)
        {
        case 32:
            BackendX64Inst::emit_Load32_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (isSigned)
                pp.concat.addString1("\x99"); // cdq
            else
                emit_Clear32(pp, RDX);
            break;
        case 64:
            BackendX64Inst::emit_Load64_Indirect(pp, regOffset(ip->a.u32), RAX, RDI);
            if (isSigned)
                pp.concat.addString2("\x48\x99"); // cqo
            else
                emit_Clear64(pp, RDX);
            pp.concat.addU8(0x48);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }

        pp.concat.addU8(0xF7);

        uint32_t offsetStack = ip->b.u32 * sizeof(Register);
        if (offsetStack == 0)
            pp.concat.addU8(0x37 | (isSigned ? 0b1000 : 0));
        else if (offsetStack <= 0x7F)
        {
            pp.concat.addU8(0x77 | (isSigned ? 0b1000 : 0));
            pp.concat.addU8((uint8_t) offsetStack);
        }
        else
        {
            pp.concat.addU8(0xB7 | (isSigned ? 0b1000 : 0));
            pp.concat.addU32(offsetStack);
        }

        if (modulo)
        {
            switch (bits)
            {
            case 32:
                BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RDX, RDI);
                break;
            case 64:
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RDX, RDI);
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
                BackendX64Inst::emit_Store32_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
                break;
            case 64:
                BackendX64Inst::emit_Store64_Indirect(pp, regOffset(ip->c.u32), RAX, RDI);
                break;
            default:
                SWAG_ASSERT(false);
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////
    inline void emit_NearJumpOp(X64PerThread& pp, JumpType jumpType)
    {
        switch (jumpType)
        {
        case JB:
            pp.concat.addU8(0x72);
            break;
        case JAE:
            pp.concat.addU8(0x73);
            break;
        case JZ:
            pp.concat.addU8(0x74);
            break;
        case JNZ:
            pp.concat.addU8(0x75);
            break;
        case JBE:
            pp.concat.addU8(0x76);
            break;
        case JA:
            pp.concat.addU8(0x77);
            break;
        case JL:
            pp.concat.addU8(0x7C);
            break;
        case JGE:
            pp.concat.addU8(0x7D);
            break;
        case JLE:
            pp.concat.addU8(0x7E);
            break;
        case JG:
            pp.concat.addU8(0x7F);
            break;
        case JUMP:
            pp.concat.addU8(0xEB);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
    }

    inline void emit_LongJumpOp(X64PerThread& pp, JumpType jumpType)
    {
        switch (jumpType)
        {
        case JB:
            pp.concat.addU8(0x0F);
            pp.concat.addU8(0x82);
            break;
        case JAE:
            pp.concat.addU8(0x0F);
            pp.concat.addU8(0x83);
            break;
        case JZ:
            pp.concat.addU8(0x0F);
            pp.concat.addU8(0x84);
            break;
        case JNZ:
            pp.concat.addU8(0x0F);
            pp.concat.addU8(0x85);
            break;
        case JBE:
            pp.concat.addU8(0x0F);
            pp.concat.addU8(0x86);
            break;
        case JA:
            pp.concat.addU8(0x0F);
            pp.concat.addU8(0x87);
            break;
        case JL:
            pp.concat.addU8(0x0F);
            pp.concat.addU8(0x8C);
            break;
        case JGE:
            pp.concat.addU8(0x0F);
            pp.concat.addU8(0x8D);
            break;
        case JLE:
            pp.concat.addU8(0x0F);
            pp.concat.addU8(0x8E);
            break;
        case JG:
            pp.concat.addU8(0x0F);
            pp.concat.addU8(0x8F);
            break;
        case JUMP:
            pp.concat.addU8(0xE9);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
    }

    inline void emit_Jump(X64PerThread& pp, JumpType jumpType, int32_t instructionCount, int32_t jumpOffset)
    {
        LabelToSolve label;
        label.ipDest = jumpOffset + instructionCount + 1;

        // Can we solve the label now ?
        auto it = pp.labels.find(label.ipDest);
        if (it != pp.labels.end())
        {
            label.currentOffset = (int32_t) pp.concat.totalCount() + 1;
            int relOffset       = it->second - (label.currentOffset + 1);
            if (relOffset >= -127 && relOffset <= 128)
            {
                emit_NearJumpOp(pp, jumpType);
                int8_t offset8 = (int8_t) relOffset;
                SWAG_ASSERT(offset8 >= -127 && offset8 <= 128);
                pp.concat.addU8(*(uint8_t*) &offset8);
            }
            else
            {
                emit_LongJumpOp(pp, jumpType);
                label.currentOffset = (int32_t) pp.concat.totalCount();
                relOffset           = it->second - (label.currentOffset + 4);
                pp.concat.addU32(*(uint32_t*) &relOffset);
            }

            return;
        }

        // Here we do not know the destination label, so we assume 32 bits of offset
        emit_LongJumpOp(pp, jumpType);
        label.currentOffset = (int32_t) pp.concat.totalCount();
        pp.concat.addU32(0);
        label.patch = pp.concat.getSeekPtr() - 4;
        pp.labelsToSolve.push_back(label);
    }

    inline void emitClearX(X64PerThread& pp, uint32_t toClear, uint32_t offset, uint8_t reg)
    {
        if (!toClear)
            return;
        SWAG_ASSERT(reg == RAX || reg == RDI);

        // SSE 16 octets
        if (toClear >= 16)
        {
            pp.concat.addString3("\x0F\x57\xC0"); // xorps xmm0, xmm0
            while (toClear >= 16)
            {
                pp.concat.addString2("\x0F\x11"); // movups [reg + ????????], xmm0
                if (offset <= 0x7F)
                {
                    pp.concat.addU8(0x40 | reg);
                    pp.concat.addU8((uint8_t) offset);
                }
                else
                {
                    pp.concat.addU8(0x80 | reg);
                    pp.concat.addU32(offset);
                }
                toClear -= 16;
                offset += 16;
            }
        }

        while (toClear >= 8)
        {
            emit_Store64_Immediate(pp, offset, 0, reg);
            toClear -= 8;
            offset += 8;
        }

        while (toClear >= 4)
        {
            emit_Store32_Immediate(pp, offset, 0, reg);
            toClear -= 4;
            offset += 4;
        }

        while (toClear >= 2)
        {
            emit_Store16_Immediate(pp, offset, 0, reg);
            toClear -= 2;
            offset += 2;
        }

        while (toClear >= 1)
        {
            emit_Store8_Immediate(pp, offset, 0, reg);
            toClear -= 1;
            offset += 1;
        }
    }
}; // namespace BackendX64Inst