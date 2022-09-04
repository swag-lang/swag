#include "pch.h"
#include "Register.h"
#include "Math.h"
#include "ByteCode.h"
#include "CallConv.h"
#include "X64Gen.h"
#include "TypeManager.h"

uint8_t X64Gen::emit_modRM(uint8_t mod, uint8_t r, uint8_t m)
{
    return mod << 6 | r << 3 | m;
}

void X64Gen::emit_ModRM(uint32_t stackOffset, uint8_t reg, uint8_t memReg, uint8_t op)
{
    if (stackOffset == 0)
    {
        // mov al, byte ptr [rdi]
        concat.addU8(emit_modRM(0, reg, memReg) | (op - 1));
        if (memReg == RSP)
            concat.addU8(0x24);
    }
    else if (stackOffset <= 0x7F)
    {
        // mov al, byte ptr [rdi + ??]
        concat.addU8(emit_modRM(DISP8, reg, memReg) | (op - 1));
        if (memReg == RSP)
            concat.addU8(0x24);
        concat.addU8((uint8_t) stackOffset);
    }
    else
    {
        // mov al, byte ptr [rdi + ????????]
        concat.addU8(emit_modRM(DISP32, reg, memReg) | (op - 1));
        if (memReg == RSP)
            concat.addU8(0x24);
        concat.addU32(stackOffset);
    }
}

void X64Gen::emit_Load8_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(memReg < R8);
    if (reg >= R8)
        concat.addU8(0x44);
    concat.addU8(0x8A);
    emit_ModRM(stackOffset, (reg & 0b111), memReg);
}

void X64Gen::emit_Load16_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(memReg < R8);
    concat.addU8(0x66);
    if (reg >= R8)
        concat.addU8(0x44);
    concat.addU8(0x8B);
    emit_ModRM(stackOffset, (reg & 0b111), memReg);
}

void X64Gen::emit_Load32_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(memReg < R8);
    if (reg >= R8)
        concat.addU8(0x44);
    concat.addU8(0x8B);
    emit_ModRM(stackOffset, (reg & 0b111), memReg);
}

void X64Gen::emit_Load64_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(memReg < R8);
    concat.addU8(0x48 | ((reg & 0b1000) >> 1));
    concat.addU8(0x8B);
    emit_ModRM(stackOffset, (reg & 0b111), memReg);
}

void X64Gen::emit_LoadS8S16_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x66);
    concat.addU8(0x0F);
    concat.addU8(0xBE);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadS8S32_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x0F);
    concat.addU8(0xBE);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadS8S64_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x48);
    concat.addU8(0x0F);
    concat.addU8(0xBE);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadS16S32_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x0F);
    concat.addU8(0xBF);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadS16S64_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x48);
    concat.addU8(0x0F);
    concat.addU8(0xBF);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadS32S64_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x48);
    concat.addU8(0x63);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadU8U32_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadU16U32_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x0F);
    concat.addU8(0xB7);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadN_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg, uint8_t numBits)
{
    switch (numBits)
    {
    case 8:
        emit_Load8_Indirect(stackOffset, reg, memReg);
        break;
    case 16:
        emit_Load16_Indirect(stackOffset, reg, memReg);
        break;
    case 32:
        emit_Load32_Indirect(stackOffset, reg, memReg);
        break;
    case 64:
        emit_Load64_Indirect(stackOffset, reg, memReg);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

void X64Gen::emit_LoadF32_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF3);
    concat.addU8(0x0F);
    concat.addU8(0x10);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadF64_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF2);
    concat.addU8(0x0F);
    concat.addU8(0x10);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_Store8_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x88);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_Store16_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x66);
    concat.addU8(0x89);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_Store32_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x89);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_Store64_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    concat.addU8(0x48 | ((reg & 0b1000) >> 1) | ((memReg & 0b1000) >> 3));
    concat.addU8(0x89);
    emit_ModRM(stackOffset, (reg & 0b111), (memReg & 0b111));
}

void X64Gen::emit_StoreN_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg, uint8_t numBits)
{
    switch (numBits)
    {
    case 8:
        emit_Store8_Indirect(stackOffset, reg, memReg);
        break;
    case 16:
        emit_Store16_Indirect(stackOffset, reg, memReg);
        break;
    case 32:
        emit_Store32_Indirect(stackOffset, reg, memReg);
        break;
    case 64:
        emit_Store64_Indirect(stackOffset, reg, memReg);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

void X64Gen::emit_StoreF32_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF3);
    concat.addU8(0x0F);
    concat.addU8(0x11);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_StoreF64_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF2);
    concat.addU8(0x0F);
    concat.addU8(0x11);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_Store8_Immediate(uint32_t offset, uint8_t val, uint8_t reg)
{
    concat.addU8(0xC6);
    emit_ModRM(offset, 0, reg);
    concat.addU8(val);
}

void X64Gen::emit_Store16_Immediate(uint32_t offset, uint16_t val, uint8_t reg)
{
    concat.addU8(0x66);
    concat.addU8(0xC7);
    emit_ModRM(offset, 0, reg);
    concat.addU16(val);
}

void X64Gen::emit_Store32_Immediate(uint32_t offset, uint32_t val, uint8_t reg)
{
    concat.addU8(0xC7);
    emit_ModRM(offset, 0, reg);
    concat.addU32((uint32_t) val);
}

void X64Gen::emit_Store64_Immediate(uint32_t offset, uint64_t val, uint8_t reg)
{
    SWAG_ASSERT(val <= 0x7FFFFFFF);
    concat.addU8(0x48);
    concat.addU8(0xC7);
    emit_ModRM(offset, 0, reg);
    concat.addU32((uint32_t) val);
}

void X64Gen::emit_Clear8(uint8_t reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0x30);
    concat.addU8(emit_modRM(3, reg, reg));
}

void X64Gen::emit_Clear16(uint8_t reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0x66);
    concat.addU8(0x31);
    concat.addU8(emit_modRM(3, reg, reg));
}

void X64Gen::emit_Clear32(uint8_t reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0x31);
    concat.addU8(emit_modRM(3, reg, reg));
}

void X64Gen::emit_Clear64(uint8_t reg)
{
    concat.addU8(0x48 | ((reg & 0b1000) >> 1) | ((reg & 0b1000) >> 3));
    concat.addU8(0x31);
    concat.addU8(emit_modRM(3, (reg & 0b111), (reg & 0b111)));
}

void X64Gen::emit_ClearN(uint8_t reg, uint8_t numBits)
{
    switch (numBits)
    {
    case 8:
    case 16:
    case 32:
        emit_Clear32(reg);
        break;
    case 64:
        emit_Clear64(reg);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

void X64Gen::emit_Load8_Immediate(uint8_t val, uint8_t reg)
{
    if (val == 0)
    {
        emit_Clear8(reg);
        return;
    }

    concat.addU8(0xB0 | reg);
    concat.addU8(val);
}

void X64Gen::emit_Load16_Immediate(uint16_t val, uint8_t reg)
{
    if (val == 0)
    {
        emit_Clear16(reg);
        return;
    }

    concat.addU8(0x66);
    concat.addU8(0xB8 | reg);
    concat.addU16(val);
}

void X64Gen::emit_Load32_Immediate(uint32_t val, uint8_t reg)
{
    if (val == 0)
    {
        emit_Clear32(reg);
        return;
    }

    concat.addU8(0xB8 | reg);
    concat.addU32(val);
}

void X64Gen::emit_Load64_Immediate(uint64_t val, uint8_t reg, bool force64bits)
{
    if (force64bits)
    {
        concat.addU8(0x48 | ((reg & 0b1000) >> 3));
        concat.addU8(0xB8 | reg);
        concat.addU64(val);
        return;
    }

    if (val == 0)
    {
        emit_Clear64(reg);
        return;
    }

    if (val <= 0x7FFFFFFF && reg < R8)
    {
        emit_Load32_Immediate((uint32_t) val, reg);
        return;
    }

    concat.addU8(0x48 | ((reg & 0b1000) >> 3));
    if (val <= 0x7FFFFFFF)
    {
        concat.addU8(0xC7);
        concat.addU8(0xC0 | (reg & 0b111));
        concat.addU32((uint32_t) val);
    }
    else
    {
        concat.addU8(0xB8 | reg);
        concat.addU64(val);
    }
}

void X64Gen::emit_LoadN_Immediate(Register& val, uint8_t reg, uint8_t numBits)
{
    switch (numBits)
    {
    case 8:
        emit_Load8_Immediate(val.u8, reg);
        break;
    case 16:
        emit_Load16_Immediate(val.u16, reg);
        break;
    case 32:
        emit_Load32_Immediate(val.u32, reg);
        break;
    case 64:
        emit_Load64_Immediate(val.u64, reg);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

void X64Gen::emit_Push(uint8_t reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0x50 | (reg & 0b111));
}

void X64Gen::emit_Pop(uint8_t reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0x58 | (reg & 0b111));
}

void X64Gen::emit_Ret()
{
    concat.addU8(0xC3);
}

void X64Gen::emit_Copy64(uint8_t regSrc, uint8_t regDst)
{
    concat.addU8(0x48 | ((regDst & 0b1000) >> 3) | ((regSrc & 0b1000) >> 1));
    concat.addU8(0x89);
    concat.addU8(emit_modRM(0b11, regSrc & 0b111, regDst & 0b111));
}

void X64Gen::emit_Copy8(uint8_t regSrc, uint8_t regDst)
{
    SWAG_ASSERT(regDst <= R9 && regSrc < R8);
    if (regDst >= R8)
        concat.addU8(0x41);
    concat.addU8(0x88);
    concat.addU8(emit_modRM(0b11, regSrc & 0b111, regDst & 0b111));
}

void X64Gen::emit_Copy16(uint8_t regSrc, uint8_t regDst)
{
    SWAG_ASSERT(regDst <= R9 && regSrc < R8);
    concat.addU8(0x66);
    if (regDst >= R8)
        concat.addU8(0x41);
    concat.addU8(0x89);
    concat.addU8(emit_modRM(0b11, regSrc & 0b111, regDst & 0b111));
}

void X64Gen::emit_Copy32(uint8_t regSrc, uint8_t regDst)
{
    SWAG_ASSERT(regDst <= R9 && regSrc < R8);
    if (regDst >= R8)
        concat.addU8(0x41);
    concat.addU8(0x89);
    concat.addU8(emit_modRM(0b11, regSrc & 0b111, regDst & 0b111));
}

void X64Gen::emit_CopyF32(uint8_t regSrc, uint8_t regDst)
{
    SWAG_ASSERT(regSrc == RAX);
    SWAG_ASSERT(regDst == XMM0 || regDst == XMM1 || regDst == XMM2 || regDst == XMM3);
    concat.addU8(0x66);
    concat.addU8(0x0F);
    concat.addU8(0x6E);
    concat.addU8(0xC0 | (regDst << 3));
}

void X64Gen::emit_CopyF64(uint8_t regSrc, uint8_t regDst)
{
    SWAG_ASSERT(regSrc == RAX);
    SWAG_ASSERT(regDst == XMM0 || regDst == XMM1 || regDst == XMM2 || regDst == XMM3);
    concat.addU8(0x66);
    concat.addU8(0x48);
    concat.addU8(0x0F);
    concat.addU8(0x6E);
    concat.addU8(0xC0 | (regDst << 3));
}

void X64Gen::emit_SetNE()
{
    concat.addU8(0x0F);
    concat.addU8(0x95);
    concat.addU8(0xC0);
}

void X64Gen::emit_SetA(uint8_t regDst)
{
    SWAG_ASSERT(regDst < R8);
    concat.addU8(0x0F);
    concat.addU8(0x97);
    concat.addU8(0xC0 | regDst);
}

void X64Gen::emit_SetAE(uint8_t regDst)
{
    SWAG_ASSERT(regDst < R8);
    concat.addU8(0x0F);
    concat.addU8(0x93);
    concat.addU8(0xC0 | regDst);
}

void X64Gen::emit_SetNA()
{
    concat.addU8(0x0F);
    concat.addU8(0x96);
    concat.addU8(0xC0);
}

void X64Gen::emit_SetB()
{
    concat.addU8(0x0F);
    concat.addU8(0x92);
    concat.addU8(0xC0);
}

void X64Gen::emit_SetBE()
{
    concat.addU8(0x0F);
    concat.addU8(0x96);
    concat.addU8(0xC0);
}

void X64Gen::emit_SetE()
{
    concat.addU8(0x0F);
    concat.addU8(0x94);
    concat.addU8(0xC0);
}

void X64Gen::emit_SetG()
{
    concat.addU8(0x0F);
    concat.addU8(0x9F);
    concat.addU8(0xC0);
}

void X64Gen::emit_SetGE()
{
    concat.addU8(0x0F);
    concat.addU8(0x9D);
    concat.addU8(0xC0);
}

void X64Gen::emit_SetL()
{
    concat.addU8(0x0F);
    concat.addU8(0x9C);
    concat.addU8(0xC0);
}

void X64Gen::emit_SetLE()
{
    concat.addU8(0x0F);
    concat.addU8(0x9E);
    concat.addU8(0xC0);
}

void X64Gen::emit_Test64(uint8_t reg1, uint8_t reg2)
{
    concat.addU8(0x48 | ((reg1 & 0b1000) >> 3) | ((reg2 & 0b1000) >> 1));
    concat.addU8(0x85);
    concat.addU8(emit_modRM(0b11, reg2 & 0b111, reg1 & 0b111));
}

void X64Gen::emit_Test32(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x85);
    concat.addU8(emit_modRM(0b11, reg2 & 0b111, reg1 & 0b111));
}

void X64Gen::emit_Test16(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x66);
    concat.addU8(0x85);
    concat.addU8(emit_modRM(0b11, reg2 & 0b111, reg1 & 0b111));
}

void X64Gen::emit_Test8(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x84);
    concat.addU8(emit_modRM(0b11, reg2 & 0b111, reg1 & 0b111));
}

void X64Gen::emit_Cmp8(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x38);
    concat.addU8(emit_modRM(0b11, reg2 & 0b111, reg1 & 0b111));
}

void X64Gen::emit_Cmp16(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x66);
    concat.addU8(0x39);
    concat.addU8(emit_modRM(0b11, reg2 & 0b111, reg1 & 0b111));
}

void X64Gen::emit_Cmp32(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x39);
    concat.addU8(emit_modRM(0b11, reg2 & 0b111, reg1 & 0b111));
}

void X64Gen::emit_Cmp64(uint8_t reg1, uint8_t reg2)
{
    concat.addU8(0x48 | ((reg1 & 0b1000) >> 3) | ((reg2 & 0b1000) >> 1));
    concat.addU8(0x39);
    concat.addU8(emit_modRM(0b11, reg2 & 0b111, reg1 & 0b111));
}

void X64Gen::emit_Cmp64_Immediate(uint64_t value, uint8_t reg, uint8_t altReg)
{
    SWAG_ASSERT(reg == RAX || reg == RCX);

    if (value <= 0x7f)
    {
        concat.addU8(0x48);
        concat.addU8(0x83);
        concat.addU8(0xF8 | reg);
        concat.addU8((uint8_t) value);
    }
    else if (value <= 0x7fffffff)
    {
        concat.addU8(0x48);
        if (reg == RAX)
            concat.addU8(0x3d);
        else
        {
            concat.addU8(0x81);
            concat.addU8(0xF9);
        }

        concat.addU32((uint32_t) value);
    }
    else
    {
        emit_Load64_Immediate(value, altReg);
        emit_Cmp64(reg, altReg);
    }
}

void X64Gen::emit_CmpF32(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x0F);
    concat.addU8(0x2F);
    concat.addU8(emit_modRM(0b11, reg1 & 0b111, reg2 & 0b111));
}

void X64Gen::emit_CmpF64(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x66);
    concat.addU8(0x0F);
    concat.addU8(0x2F);
    concat.addU8(emit_modRM(0b11, reg1 & 0b111, reg2 & 0b111));
}

void X64Gen::emit_Cmp8_Indirect(uint32_t offsetStack, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x3A);
    emit_ModRM(offsetStack, reg & 0b111, memReg & 0b111);
}

void X64Gen::emit_Cmp16_Indirect(uint32_t offsetStack, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x66);
    concat.addU8(0x3B);
    emit_ModRM(offsetStack, reg & 0b111, memReg & 0b111);
}

void X64Gen::emit_Cmp32_Indirect(uint32_t offsetStack, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x3B);
    emit_ModRM(offsetStack, reg & 0b111, RDI);
}

void X64Gen::emit_Cmp64_Indirect(uint32_t offsetStack, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x48);
    concat.addU8(0x3B);
    emit_ModRM(offsetStack, reg & 0b111, memReg & 0b111);
}

void X64Gen::emit_Cmp8_IndirectDst(uint32_t offsetStack, uint32_t value)
{
    concat.addU8(0x80);
    emit_ModRM(offsetStack, RDI, RDI);
    concat.addU8((uint8_t) value);
}

void X64Gen::emit_Cmp16_IndirectDst(uint32_t offsetStack, uint32_t value)
{
    concat.addU8(0x66);
    if (value <= 0x7F)
    {
        concat.addU8(0x83);
        emit_ModRM(offsetStack, RDI, RDI);
        concat.addU8((uint8_t) value);
    }
    else
    {
        concat.addU8(0x81);
        emit_ModRM(offsetStack, RDI, RDI);
        concat.addU16((uint16_t) value);
    }
}

void X64Gen::emit_Cmp32_IndirectDst(uint32_t offsetStack, uint32_t value)
{
    if (value <= 0x7F)
    {
        concat.addU8(0x83);
        emit_ModRM(offsetStack, RDI, RDI);
        concat.addU8((uint8_t) value);
    }
    else
    {
        concat.addU8(0x81);
        emit_ModRM(offsetStack, RDI, RDI);
        concat.addU32((uint32_t) value);
    }
}

void X64Gen::emit_Cmp64_IndirectDst(uint32_t offsetStack, uint32_t value)
{
    SWAG_ASSERT(value <= 0x7FFFFFFF);
    concat.addU8(0x48);
    emit_Cmp32_IndirectDst(offsetStack, value);
}

void X64Gen::emit_CmpF32_Indirect(uint32_t offsetStack, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x0F);
    concat.addU8(0x2F);
    emit_ModRM(offsetStack, reg & 0b111, memReg & 0b111);
}

void X64Gen::emit_CmpF64_Indirect(uint32_t offsetStack, uint8_t reg, uint8_t memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x66);
    concat.addU8(0x0F);
    concat.addU8(0x2F);
    emit_ModRM(offsetStack, reg & 0b111, memReg & 0b111);
}

void X64Gen::emit_LoadAddress_Indirect(uint32_t stackOffset, uint8_t reg, uint8_t memReg)
{
    if (stackOffset == 0)
    {
        emit_Copy64(memReg, reg);
        return;
    }

    concat.addU8(0x48 | ((memReg & 0b1000) >> 3) | ((reg & 0b1000) >> 1));
    concat.addU8(0x8D);
    emit_ModRM(stackOffset, reg & 0b111, memReg & 0b111);
}

void X64Gen::emit_Inc32_Indirect(uint32_t stackOffset, uint8_t reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0xFF);
    emit_ModRM(stackOffset, 0, reg);
}

void X64Gen::emit_Dec32_Indirect(uint32_t stackOffset, uint8_t reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0xFF);
    emit_ModRM(stackOffset, 1, reg);
}

void X64Gen::emit_Inc64_Indirect(uint32_t stackOffset, uint8_t reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0x48);
    concat.addU8(0xFF);
    emit_ModRM(stackOffset, 0, reg);
}

void X64Gen::emit_Dec64_Indirect(uint32_t stackOffset, uint8_t reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0x48);
    concat.addU8(0xFF);
    emit_ModRM(stackOffset, 1, reg);
}

void X64Gen::emit_Op8_Indirect(uint32_t offsetStack, uint8_t reg, uint8_t memReg, X64Op instruction, bool lock)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    if (lock)
        concat.addU8(0xF0);
    concat.addU8((uint8_t) instruction & ~1);
    emit_ModRM(offsetStack, reg & 0b111, memReg & 0b111);
}

void X64Gen::emit_Op16_Indirect(uint32_t offsetStack, uint8_t reg, uint8_t memReg, X64Op instruction, bool lock)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    if (lock)
        concat.addU8(0xF0);
    concat.addU8(0x66);
    concat.addU8((uint8_t) instruction);
    emit_ModRM(offsetStack, reg & 0b111, memReg & 0b111);
}

void X64Gen::emit_Op32_Indirect(uint32_t offsetStack, uint8_t reg, uint8_t memReg, X64Op instruction, bool lock)
{
    SWAG_ASSERT(memReg < R8);
    if (lock)
        concat.addU8(0xF0);
    if (reg >= R8)
        concat.addU8(0x44);
    concat.addU8((uint8_t) instruction);
    emit_ModRM(offsetStack, reg & 0b111, memReg & 0b111);
}

void X64Gen::emit_Op64_IndirectDst(uint32_t offsetStack, uint8_t reg, uint8_t memReg, X64Op instruction, bool lock)
{
    SWAG_ASSERT(memReg < R8 && reg < R8);
    if (lock)
        concat.addU8(0xF0);
    concat.addU8(0x48);
    concat.addU8((uint8_t) instruction);
    emit_ModRM(offsetStack, reg & 0b111, memReg & 0b111);
}

void X64Gen::emit_Op64_IndirectSrc(uint32_t offsetStack, uint8_t reg, uint8_t memReg, X64Op instruction, bool lock)
{
    if (lock)
        concat.addU8(0xF0);
    concat.addU8(0x48);
    concat.addU8((uint8_t) instruction | 2);
    emit_ModRM(offsetStack, reg & 0b111, memReg & 0b111);
}

void X64Gen::emit_OpF32_Indirect(uint8_t reg, uint8_t memReg, X64Op instruction)
{
    SWAG_ASSERT(reg == XMM1);
    SWAG_ASSERT(memReg < R8);
    emit_LoadF32_Indirect(0, XMM0, memReg);
    concat.addU8(0xF3);
    concat.addU8(0x0F);
    concat.addU8((uint8_t) instruction);
    concat.addU8(0xC1);
    emit_StoreF32_Indirect(0, XMM0, memReg);
}

void X64Gen::emit_OpF64_Indirect(uint8_t reg, uint8_t memReg, X64Op instruction)
{
    SWAG_ASSERT(reg == XMM1);
    SWAG_ASSERT(memReg < R8);
    emit_LoadF64_Indirect(0, XMM0, memReg);
    concat.addU8(0xF2);
    concat.addU8(0x0F);
    concat.addU8((uint8_t) instruction);
    concat.addU8(0xC1);
    emit_StoreF64_Indirect(0, XMM0, memReg);
}

void X64Gen::emit_Op8(uint8_t reg1, uint8_t reg2, X64Op instruction)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8((uint8_t) instruction & ~1);
    concat.addU8(emit_modRM(0b11, reg1, reg2));
}

void X64Gen::emit_Op16(uint8_t reg1, uint8_t reg2, X64Op instruction)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x66);
    concat.addU8((uint8_t) instruction);
    concat.addU8(emit_modRM(0b11, reg1, reg2));
}

void X64Gen::emit_Op32(uint8_t reg1, uint8_t reg2, X64Op instruction)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8((uint8_t) instruction);
    concat.addU8(emit_modRM(0b11, reg1, reg2));
}

void X64Gen::emit_Op64(uint8_t reg1, uint8_t reg2, X64Op instruction)
{
    concat.addU8(0x48 | ((reg2 & 0b1000) >> 3) | ((reg1 & 0b1000) >> 1));
    concat.addU8((uint8_t) instruction);
    concat.addU8(emit_modRM(0b11, reg1 & 0b111, reg2 & 0b111));
}

void X64Gen::emit_Add64_Immediate(uint64_t value, uint8_t reg)
{
    if (!value)
        return;
    SWAG_ASSERT(reg == RAX || reg == RCX);
    SWAG_ASSERT(value <= 0x7FFFFFFF);
    concat.addU8(0x48);
    if (value <= 0x7F)
    {
        concat.addU8(0x83);
        concat.addU8(0xC0 | reg);
        concat.addU8((uint8_t) value);
    }
    else if (reg == RAX)
    {
        concat.addU8(0x05);
        concat.addU32((uint32_t) value);
    }
    else if (reg == RCX)
    {
        concat.addU8(0x81);
        concat.addU8(0xC1);
        concat.addU32((uint32_t) value);
    }
}

void X64Gen::emit_Sub64_Immediate(uint64_t value, uint8_t reg, uint8_t altReg)
{
    if (!value)
        return;

    SWAG_ASSERT(reg == RAX || reg == RCX);
    SWAG_ASSERT(altReg == RAX || altReg == RCX);

    if (value > 0x7FFFFFFF)
    {
        emit_Load64_Immediate(value, altReg);

        concat.addU8(0x48);
        concat.addU8(0x29);
        if (reg == RCX)
            concat.addU8(0xC1); // sub rcx, rax
        else
            concat.addU8(0xC8); // sub rax, rcx
    }
    else if (value <= 0x7F)
    {
        concat.addU8(0x48);
        concat.addU8(0x83);
        concat.addU8(0xE8 | reg);
        concat.addU8((uint8_t) value);
    }
    else if (reg == RAX)
    {
        concat.addU8(0x48);
        concat.addU8(0x2D);
        concat.addU32((uint32_t) value);
    }
    else if (reg == RCX)
    {
        concat.addU8(0x48);
        concat.addU8(0x81);
        concat.addU8(0xE9);
        concat.addU32((uint32_t) value);
    }
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_Symbol_RelocationAddr(uint8_t reg, uint32_t symbolIndex, uint32_t offset)
{
    SWAG_ASSERT(reg == RAX || reg == RCX || reg == RDX || reg == R8 || reg == R9);
    if (reg == R8 || reg == R9)
        concat.addU8(0x4C);
    else
        concat.addU8(0x48);
    concat.addU8(0x8D);
    concat.addU8(0x05 | ((reg & 0b111) << 3));

    CoffRelocation reloc;
    reloc.virtualAddress = concat.totalCount() - textSectionOffset;
    reloc.symbolIndex    = symbolIndex;
    reloc.type           = IMAGE_REL_AMD64_REL32;
    relocTableTextSection.table.push_back(reloc);
    concat.addU32(offset);
}

void X64Gen::emit_Symbol_RelocationValue(uint8_t reg, uint32_t symbolIndex, uint32_t offset)
{
    SWAG_ASSERT(reg == RAX || reg == RCX || reg == RDX || reg == R8 || reg == R9);

    if (reg == R8 || reg == R9)
        concat.addU8(0x4C);
    else
        concat.addU8(0x48);
    concat.addU8(0x8B);
    concat.addU8(0x05 | ((reg & 0b111) << 3));

    CoffRelocation reloc;
    reloc.virtualAddress = concat.totalCount() - textSectionOffset;
    reloc.symbolIndex    = symbolIndex;
    reloc.type           = IMAGE_REL_AMD64_REL32;
    relocTableTextSection.table.push_back(reloc);
    concat.addU32(offset);
}

void X64Gen::emit_Sub_Cst32_To_RSP(uint32_t value)
{
    if (value)
    {
        if (value <= 0x7F)
        {
            concat.addString3("\x48\x83\xEC"); // sub rsp, ??
            concat.addU8((uint8_t) value);
        }
        else
        {
            concat.addString3("\x48\x81\xEC"); // sub rsp, ????????
            concat.addU32(value);
        }
    }
}

void X64Gen::emit_Add_Cst32_To_RSP(uint32_t value)
{
    if (value)
    {
        if (value <= 0x7F)
        {
            concat.addString3("\x48\x83\xC4"); // add rsp, ??
            concat.addU8((uint8_t) value);
        }
        else
        {
            concat.addString3("\x48\x81\xC4"); // add rsp, ????????
            concat.addU32(value);
        }
    }
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_SignedExtend_AL_To_AX()
{
    concat.addString2("\x66\x98"); // cbw
}

void X64Gen::emit_SignedExtend_AL_To_EAX()
{
    concat.addString3("\x0F\xBE\xC0"); // movsx eax, al
}

void X64Gen::emit_SignedExtend_AL_To_RAX()
{
    concat.addString4("\x48\x0F\xBE\xC0"); // movsx rax, al
}

void X64Gen::emit_SignedExtend_AX_To_EAX()
{
    concat.addU8(0x98); // cwde
}

void X64Gen::emit_SignedExtend_AX_To_RAX()
{
    concat.addString4("\x48\x0F\xBF\xC0"); // movsx rax, ax
}

void X64Gen::emit_SignedExtend_EAX_To_RAX()
{
    concat.addString2("\x48\x98"); // cdqe
}

void X64Gen::emit_SignedExtend_BX_To_EBX()
{
    concat.addString3("\x0f\xbf\xdb"); // movsx ebx, bx
}

void X64Gen::emit_SignedExtend_CL_To_ECX()
{
    concat.addString3("\x0f\xbe\xC9"); // movsx ecx, cl
}

void X64Gen::emit_SignedExtend_CL_To_RCX()
{
    concat.addString4("\x48\x0F\xBE\xC9"); // movsx rcx, cl
}
void X64Gen::emit_SignedExtend_CX_To_RCX()
{
    concat.addString4("\x48\x0F\xBF\xC9"); // movsx rcx, cx
}

void X64Gen::emit_SignedExtend_ECX_To_RCX()
{
    concat.addString3("\x48\x63\xC9"); // movsx rcx, ecx
}

void X64Gen::emit_SignedExtend_8_To_32(uint8_t reg)
{
    switch (reg)
    {
    case RAX:
        emit_SignedExtend_AL_To_EAX();
        break;
    case RCX:
        emit_SignedExtend_CL_To_ECX();
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

void X64Gen::emit_UnsignedExtend_8_To_32(uint8_t reg)
{
    // movzx eax, al
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    concat.addU8(emit_modRM(REGREG, reg, reg));
}

void X64Gen::emit_UnsignedExtend_16_To_32(uint8_t reg)
{
    // movzx rax, ax
    concat.addU8(0x0F);
    concat.addU8(0xB7);
    concat.addU8(emit_modRM(REGREG, reg, reg));
}

void X64Gen::emit_UnsignedExtend_8_To_64(uint8_t reg)
{
    // movzx rax, al
    concat.addU8(0x48);
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    concat.addU8(emit_modRM(REGREG, reg, reg));
}

void X64Gen::emit_UnsignedExtend_16_To_64(uint8_t reg)
{
    // movzx rax, al
    concat.addU8(0x48);
    concat.addU8(0x0F);
    concat.addU8(0xB7);
    concat.addU8(emit_modRM(REGREG, reg, reg));
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_BinOpFloat32(ByteCodeInstruction* ip, X64Op op)
{
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
    {
        emit_LoadF32_Indirect(regOffset(ip->a.u32), XMM0, RDI);
        concat.addU8(0xF3);
        concat.addU8(0x0F);
        concat.addU8((uint8_t) op);
        emit_ModRM(regOffset(ip->b.u32), XMM0, RDI);
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
        {
            emit_Load32_Immediate(ip->a.u32, RAX);
            emit_CopyF32(RAX, XMM0);
        }
        else
            emit_LoadF32_Indirect(regOffset(ip->a.u32), XMM0, RDI);
        if (ip->flags & BCI_IMM_B)
        {
            emit_Load32_Immediate(ip->b.u32, RAX);
            emit_CopyF32(RAX, XMM1);
        }
        else
            emit_LoadF32_Indirect(regOffset(ip->b.u32), XMM1, RDI);
        concat.addU8(0xF3);
        concat.addU8(0x0F);
        concat.addU8((uint8_t) op);
        concat.addU8(0xC1);
    }
}

void X64Gen::emit_BinOpFloat32_At_Reg(ByteCodeInstruction* ip, X64Op op)
{
    emit_BinOpFloat32(ip, op);
    emit_StoreF32_Indirect(regOffset(ip->c.u32), XMM0, RDI);
}

void X64Gen::emit_BinOpFloat64(ByteCodeInstruction* ip, X64Op op)
{
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
    {
        emit_LoadF64_Indirect(regOffset(ip->a.u32), XMM0, RDI);
        concat.addU8(0xF2);
        concat.addU8(0x0F);
        concat.addU8((uint8_t) op);
        emit_ModRM(regOffset(ip->b.u32), XMM0, RDI);
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
        {
            emit_Load64_Immediate(ip->a.u64, RAX);
            emit_CopyF64(RAX, XMM0);
        }
        else
            emit_LoadF64_Indirect(regOffset(ip->a.u32), XMM0, RDI);
        if (ip->flags & BCI_IMM_B)
        {
            emit_Load64_Immediate(ip->b.u64, RAX);
            emit_CopyF64(RAX, XMM1);
        }
        else
            emit_LoadF64_Indirect(regOffset(ip->b.u32), XMM1, RDI);
        concat.addU8(0xF2);
        concat.addU8(0x0F);
        concat.addU8((uint8_t) op);
        concat.addU8(0xC1);
    }
}

void X64Gen::emit_BinOpFloat64_At_Reg(ByteCodeInstruction* ip, X64Op op)
{
    emit_BinOpFloat64(ip, op);
    emit_StoreF64_Indirect(regOffset(ip->c.u32), XMM0, RDI);
}

void X64Gen::emit_BinOpInt8(ByteCodeInstruction* ip, X64Op op)
{
    SWAG_ASSERT(op == X64Op::AND || op == X64Op::OR || op == X64Op::XOR);
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
    {
        emit_Load8_Indirect(regOffset(ip->a.u32), RCX, RDI);
        concat.addU8((uint8_t) op + 1);
        emit_ModRM(regOffset(ip->b.u32), RCX, RDI); // cl, [rdi+?]
    }
    else if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B))
    {
        emit_Load8_Indirect(regOffset(ip->a.u32), RCX, RDI);
        concat.addU8((uint8_t) 0x80);
        if (op == X64Op::XOR)
            concat.addU8(emit_modRM(REGREG, 6, RCX));
        else if (op == X64Op::AND)
            concat.addU8(emit_modRM(REGREG, 4, RCX));
        else
            concat.addU8(emit_modRM(REGREG, 1, RCX));
        concat.addU8(ip->b.u8);
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
            emit_Load8_Immediate(ip->a.u8, RCX);
        else
            emit_Load8_Indirect(regOffset(ip->a.u32), RCX, RDI);
        if (ip->flags & BCI_IMM_B)
            emit_Load8_Immediate(ip->b.u8, RAX);
        else
            emit_Load8_Indirect(regOffset(ip->b.u32), RAX, RDI);
        emit_Op8(RAX, RCX, op);
    }
}

void X64Gen::emit_BinOpInt16(ByteCodeInstruction* ip, X64Op op)
{
    SWAG_ASSERT(op == X64Op::AND || op == X64Op::OR || op == X64Op::XOR);
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
    {
        emit_Load16_Indirect(regOffset(ip->a.u32), RCX, RDI);
        concat.addU8(0x66);
        concat.addU8((uint8_t) op | 2);
        emit_ModRM(regOffset(ip->b.u32), RCX, RDI); // cx, [rdi+?]
    }
    else if (!(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B))
    {
        emit_Load16_Indirect(regOffset(ip->a.u32), RCX, RDI);
        concat.addU8((uint8_t) 0x66);
        if (ip->b.u16 <= 0x7F)
            concat.addU8((uint8_t) 0x83);
        else
            concat.addU8((uint8_t) 0x81);
        if (op == X64Op::AND)
            concat.addU8(emit_modRM(REGREG, 4, RCX));
        else
            concat.addU8(emit_modRM(REGREG, 1, RCX));
        if (ip->b.u16 <= 0x7F)
            concat.addU8(ip->b.u8);
        else
            concat.addU16(ip->b.u16);
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
            emit_Load16_Immediate(ip->a.u16, RCX);
        else
            emit_Load16_Indirect(regOffset(ip->a.u32), RCX, RDI);
        if (ip->flags & BCI_IMM_B)
            emit_Load16_Immediate(ip->b.u16, RAX);
        else
            emit_Load16_Indirect(regOffset(ip->b.u32), RAX, RDI);
        emit_Op16(RAX, RCX, op);
    }
}

void X64Gen::emit_BinOpInt32(ByteCodeInstruction* ip, X64Op op)
{
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
    {
        if (op == X64Op::MUL)
        {
            emit_Load32_Indirect(regOffset(ip->a.u32), RAX, RDI);
            concat.addString1("\xF7"); // mul
            emit_ModRM(regOffset(ip->b.u32), 4, RDI);
        }
        else
        {
            emit_Load32_Indirect(regOffset(ip->a.u32), RCX, RDI);
            if (op == X64Op::IMUL)
                concat.addString2("\x0F\xAF"); // imul
            else
                concat.addU8((uint8_t) op | 2);
            emit_ModRM(regOffset(ip->b.u32), RCX, RDI); // ecx, [rdi+?]
        }
    }
    // Mul by power of 2 => shift by log2
    else if (op == X64Op::MUL && !(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B) && isPowerOfTwo(ip->b.u32) && (ip->b.u32 < 32))
    {
        emit_Load32_Indirect(regOffset(ip->a.u32), RAX, RDI);
        emit_Load8_Immediate((uint8_t) log2(ip->b.u32), RCX);
        concat.addString2("\xD3\xE0"); // shl eax, cl
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
            emit_Load32_Immediate(ip->a.u32, RCX);
        else
            emit_Load32_Indirect(regOffset(ip->a.u32), RCX, RDI);
        if (ip->flags & BCI_IMM_B)
            emit_Load32_Immediate(ip->b.u32, RAX);
        else
            emit_Load32_Indirect(regOffset(ip->b.u32), RAX, RDI);
        if (op == X64Op::MUL)
        {
            // mul rcx
            concat.addString3("\x48\xF7\xE1");
        }
        else if (op == X64Op::IMUL)
        {
            // imul ecx, eax
            concat.addString2("\x0F\xAF");
            concat.addU8(0xC8);
        }
        else
            emit_Op32(RAX, RCX, op);
    }
}

void X64Gen::emit_BinOpInt64(ByteCodeInstruction* ip, X64Op op)
{
    if (!(ip->flags & BCI_IMM_A) && !(ip->flags & BCI_IMM_B))
    {
        if (op == X64Op::MUL)
        {
            emit_Load64_Indirect(regOffset(ip->a.u32), RAX, RDI);
            concat.addU8(0x48);
            concat.addString1("\xF7"); // mul
            emit_ModRM(regOffset(ip->b.u32), 4, RDI);
        }
        else
        {
            emit_Load64_Indirect(regOffset(ip->a.u32), RCX, RDI);
            concat.addU8(0x48);
            if (op == X64Op::IMUL)
                concat.addString2("\x0F\xAF"); // imul
            else
                concat.addU8((uint8_t) op | 2);
            emit_ModRM(regOffset(ip->b.u32), RCX, RDI);
        }
    }
    // Mul by power of 2 => shift by log2
    else if (op == X64Op::MUL && !(ip->flags & BCI_IMM_A) && (ip->flags & BCI_IMM_B) && isPowerOfTwo(ip->b.u64) && (ip->b.u64 < 64))
    {
        emit_Load64_Indirect(regOffset(ip->a.u32), RAX, RDI);
        emit_Load8_Immediate((uint8_t) log2(ip->b.u64), RCX);
        concat.addString3("\x48\xD3\xE0"); // shl rax, cl
    }
    else
    {
        if (ip->flags & BCI_IMM_A)
            emit_Load64_Immediate(ip->a.u64, RCX);
        else
            emit_Load64_Indirect(regOffset(ip->a.u32), RCX, RDI);
        if (ip->flags & BCI_IMM_B)
            emit_Load64_Immediate(ip->b.u64, RAX);
        else
            emit_Load64_Indirect(regOffset(ip->b.u32), RAX, RDI);
        if (op == X64Op::MUL)
        {
            // mul rcx
            concat.addString3("\x48\xF7\xE1");
        }
        else if (op == X64Op::IMUL)
        {
            // imul rcx, rax
            concat.addU8(0x48);
            concat.addU8(0x0F);
            concat.addU8(0xAF);
            concat.addU8(0xC8);
        }
        else
            emit_Op64(RAX, RCX, op);
    }
}

void X64Gen::emit_BinOpInt8_At_Reg(ByteCodeInstruction* ip, X64Op op)
{
    emit_BinOpInt8(ip, op);
    emit_Store8_Indirect(regOffset(ip->c.u32), RCX, RDI);
}

void X64Gen::emit_BinOpInt16_At_Reg(ByteCodeInstruction* ip, X64Op op)
{
    emit_BinOpInt16(ip, op);
    emit_Store16_Indirect(regOffset(ip->c.u32), RCX, RDI);
}

void X64Gen::emit_BinOpInt32_At_Reg(ByteCodeInstruction* ip, X64Op op)
{
    emit_BinOpInt32(ip, op);
    if (op == X64Op::MUL)
        emit_Store32_Indirect(regOffset(ip->c.u32), RAX, RDI);
    else
        emit_Store32_Indirect(regOffset(ip->c.u32), RCX, RDI);
}

void X64Gen::emit_BinOpInt64_At_Reg(ByteCodeInstruction* ip, X64Op op)
{
    emit_BinOpInt64(ip, op);
    if (op == X64Op::MUL)
        emit_Store64_Indirect(regOffset(ip->c.u32), RAX, RDI);
    else
        emit_Store64_Indirect(regOffset(ip->c.u32), RCX, RDI);
}

/////////////////////////////////////////////////////////////////////
void X64Gen::emit_imul64_RAX(uint64_t value)
{
    if (value == 2)
    {
        concat.addString3("\x48\xD1\xE0"); // shl rax, 1
    }
    else if (isPowerOfTwo(value))
    {
        concat.addString3("\x48\xC1\xE0"); // shl rax, ??
        concat.addU8((uint8_t) log2(value));
    }
    else if (value <= 0x7F)
    {
        concat.addString3("\x48\x6B\xC0"); // imul rax, ??
        concat.addU8((uint8_t) value);
    }
    else if (value <= 0x7FFFFFFF)
    {
        concat.addString3("\x48\x69\xC0"); // imul rax, ????????
        concat.addU32((uint32_t) value);
    }
    else
    {
        emit_Load64_Immediate(value, RCX);
        concat.addString4("\x48\x0F\xAF\xC1"); // imul rax, rcx
    }
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_BinOpInt_Div_At_Reg(ByteCodeInstruction* ip, bool isSigned, uint32_t bits, bool modulo)
{
    switch (bits)
    {
    case 32:
        emit_Load32_Indirect(regOffset(ip->a.u32), RAX, RDI);
        if (isSigned)
            concat.addString1("\x99"); // cdq
        else
            emit_Clear32(RDX);
        break;
    case 64:
        emit_Load64_Indirect(regOffset(ip->a.u32), RAX, RDI);
        if (isSigned)
            concat.addString2("\x48\x99"); // cqo
        else
            emit_Clear64(RDX);
        concat.addU8(0x48);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    concat.addU8(0xF7);

    uint32_t offsetStack = ip->b.u32 * sizeof(Register);
    if (offsetStack == 0)
        concat.addU8(0x37 | (isSigned ? 0b1000 : 0));
    else if (offsetStack <= 0x7F)
    {
        concat.addU8(0x77 | (isSigned ? 0b1000 : 0));
        concat.addU8((uint8_t) offsetStack);
    }
    else
    {
        concat.addU8(0xB7 | (isSigned ? 0b1000 : 0));
        concat.addU32(offsetStack);
    }

    if (modulo)
    {
        switch (bits)
        {
        case 32:
            emit_Store32_Indirect(regOffset(ip->c.u32), RDX, RDI);
            break;
        case 64:
            emit_Store64_Indirect(regOffset(ip->c.u32), RDX, RDI);
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
            emit_Store32_Indirect(regOffset(ip->c.u32), RAX, RDI);
            break;
        case 64:
            emit_Store64_Indirect(regOffset(ip->c.u32), RAX, RDI);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }
    }
}

/////////////////////////////////////////////////////////////////////
void X64Gen::emit_NearJumpOp(JumpType jumpType)
{
    switch (jumpType)
    {
    case JB:
        concat.addU8(0x72);
        break;
    case JAE:
        concat.addU8(0x73);
        break;
    case JZ:
        concat.addU8(0x74);
        break;
    case JNZ:
        concat.addU8(0x75);
        break;
    case JBE:
        concat.addU8(0x76);
        break;
    case JA:
        concat.addU8(0x77);
        break;
    case JL:
        concat.addU8(0x7C);
        break;
    case JGE:
        concat.addU8(0x7D);
        break;
    case JLE:
        concat.addU8(0x7E);
        break;
    case JG:
        concat.addU8(0x7F);
        break;
    case JUMP:
        concat.addU8(0xEB);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

void X64Gen::emit_LongJumpOp(JumpType jumpType)
{
    switch (jumpType)
    {
    case JB:
        concat.addU8(0x0F);
        concat.addU8(0x82);
        break;
    case JAE:
        concat.addU8(0x0F);
        concat.addU8(0x83);
        break;
    case JZ:
        concat.addU8(0x0F);
        concat.addU8(0x84);
        break;
    case JNZ:
        concat.addU8(0x0F);
        concat.addU8(0x85);
        break;
    case JBE:
        concat.addU8(0x0F);
        concat.addU8(0x86);
        break;
    case JA:
        concat.addU8(0x0F);
        concat.addU8(0x87);
        break;
    case JL:
        concat.addU8(0x0F);
        concat.addU8(0x8C);
        break;
    case JGE:
        concat.addU8(0x0F);
        concat.addU8(0x8D);
        break;
    case JLE:
        concat.addU8(0x0F);
        concat.addU8(0x8E);
        break;
    case JG:
        concat.addU8(0x0F);
        concat.addU8(0x8F);
        break;
    case JUMP:
        concat.addU8(0xE9);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

void X64Gen::emit_Jump(JumpType jumpType, int32_t instructionCount, int32_t jumpOffset)
{
    LabelToSolve label;
    label.ipDest = jumpOffset + instructionCount + 1;

    // Can we solve the label now ?
    auto it = labels.find(label.ipDest);
    if (it != labels.end())
    {
        auto currentOffset = (int32_t) concat.totalCount() + 1;
        int  relOffset     = it->second - (currentOffset + 1);
        if (relOffset >= -127 && relOffset <= 128)
        {
            emit_NearJumpOp(jumpType);
            int8_t offset8 = (int8_t) relOffset;
            concat.addU8(*(uint8_t*) &offset8);
        }
        else
        {
            emit_LongJumpOp(jumpType);
            currentOffset = (int32_t) concat.totalCount();
            relOffset     = it->second - (currentOffset + 4);
            concat.addU32(*(uint32_t*) &relOffset);
        }

        return;
    }

    // Here we do not know the destination label, so we assume 32 bits of offset
    emit_LongJumpOp(jumpType);
    concat.addU32(0);
    label.currentOffset = (int32_t) concat.totalCount();
    label.patch         = concat.getSeekPtr() - 4;
    labelsToSolve.push_back(label);
}

void X64Gen::emit_CopyX(uint32_t count, uint32_t offset, uint8_t regDst, uint8_t regSrc)
{
    if (!count)
        return;
    SWAG_ASSERT(regDst == RCX);
    SWAG_ASSERT(regSrc == RDX);

    // SSE 16 octets
    if (count >= 16)
    {
        while (count >= 16)
        {
            concat.addString2("\x0F\x10"); // movups xmm0, [rdx+??]
            if (offset <= 0x7F)
            {
                concat.addU8(0x40 | regSrc);
                concat.addU8((uint8_t) offset);
            }
            else
            {
                concat.addU8(0x80 | regSrc);
                concat.addU32(offset);
            }
            concat.addString2("\x0F\x11"); // movups [rcx+??], xmm0
            if (offset <= 0x7F)
            {
                concat.addU8(0x40 | regDst);
                concat.addU8((uint8_t) offset);
            }
            else
            {
                concat.addU8(0x80 | regDst);
                concat.addU32(offset);
            }

            count -= 16;
            offset += 16;
        }
    }

    while (count >= 8)
    {
        emit_Load64_Indirect(offset, RAX, regSrc);
        emit_Store64_Indirect(offset, RAX, regDst);
        count -= 8;
        offset += 8;
    }

    while (count >= 4)
    {
        emit_Load32_Indirect(offset, RAX, regSrc);
        emit_Store32_Indirect(offset, RAX, regDst);
        count -= 4;
        offset += 4;
    }

    while (count >= 2)
    {
        emit_Load16_Indirect(offset, RAX, regSrc);
        emit_Store16_Indirect(offset, RAX, regDst);
        count -= 2;
        offset += 2;
    }

    while (count >= 1)
    {
        emit_Load8_Indirect(offset, RAX, regSrc);
        emit_Store8_Indirect(offset, RAX, regDst);
        count -= 1;
        offset += 1;
    }
}

void X64Gen::emit_ClearX(uint32_t count, uint32_t offset, uint8_t reg)
{
    if (!count)
        return;
    SWAG_ASSERT(reg == RAX || reg == RCX || reg == RDI);

    // SSE 16 octets
    if (count >= 16)
    {
        concat.addString3("\x0F\x57\xC0"); // xorps xmm0, xmm0
        while (count >= 16)
        {
            concat.addString2("\x0F\x11"); // movups [reg + ????????], xmm0
            if (offset <= 0x7F)
            {
                concat.addU8(0x40 | reg);
                concat.addU8((uint8_t) offset);
            }
            else
            {
                concat.addU8(0x80 | reg);
                concat.addU32(offset);
            }
            count -= 16;
            offset += 16;
        }
    }

    while (count >= 8)
    {
        emit_Store64_Immediate(offset, 0, reg);
        count -= 8;
        offset += 8;
    }

    while (count >= 4)
    {
        emit_Store32_Immediate(offset, 0, reg);
        count -= 4;
        offset += 4;
    }

    while (count >= 2)
    {
        emit_Store16_Immediate(offset, 0, reg);
        count -= 2;
        offset += 2;
    }

    while (count >= 1)
    {
        emit_Store8_Immediate(offset, 0, reg);
        count -= 1;
        offset += 1;
    }
}

void X64Gen::emit_Call_Parameters(TypeInfoFuncAttr* typeFuncBC, VectorNative<uint32_t>& paramsRegisters, VectorNative<TypeInfo*>& paramsTypes, void* retCopy)
{
    const auto& cc           = g_CallConv[typeFuncBC->callConv];
    auto        returnType   = TypeManager::concreteReferenceType(typeFuncBC->returnType);
    bool        returnByCopy = returnType->flags & TYPEINFO_RETURN_BY_COPY;

    int callConvRegisters    = cc.byRegisterCount;
    int maxParamsPerRegister = (int) paramsRegisters.size();

    // Set the first N parameters. Can be return register, or function parameter.
    int i = 0;
    for (; i < min(callConvRegisters, maxParamsPerRegister); i++)
    {
        auto type = paramsTypes[i];
        auto r    = paramsRegisters[i];

        // This is a return register
        if (type == g_TypeMgr->typeInfoUndefined)
        {
            // r is an address to registerRR, for FFI
            if (retCopy)
                emit_Load64_Immediate((uint64_t) retCopy, cc.byRegisterInteger[i]);
            else if (returnByCopy)
                emit_Load64_Indirect(r, cc.byRegisterInteger[i], RDI);
            else
                emit_LoadAddress_Indirect(r, cc.byRegisterInteger[i], RDI);
        }

        // This is a normal parameter, which can be float or integer
        else
        {
            // Pass struct in a register if small enough
            if (cc.structByRegister && type->kind == TypeInfoKind::Struct && type->sizeOf <= sizeof(void*))
            {
                emit_Load64_Indirect(regOffset(r), RAX, RDI);
                switch (type->sizeOf)
                {
                case 1:
                    emit_Load8_Indirect(0, cc.byRegisterInteger[i], RAX);
                    break;
                case 2:
                    emit_Load16_Indirect(0, cc.byRegisterInteger[i], RAX);
                    break;
                case 4:
                    emit_Load32_Indirect(0, cc.byRegisterInteger[i], RAX);
                    break;
                case 8:
                    emit_Load64_Indirect(0, cc.byRegisterInteger[i], RAX);
                    break;
                }
            }
            else if (cc.useRegisterFloat && type->isNativeFloat())
            {
                emit_LoadF64_Indirect(regOffset(r), cc.byRegisterFloat[i], RDI);
            }
            else
            {
                emit_Load64_Indirect(regOffset(r), cc.byRegisterInteger[i], RDI);
            }
        }
    }

    // Store all parameters after N on the stack, with an offset of N * sizeof(uint64_t)
    uint32_t offsetStack = min(callConvRegisters, maxParamsPerRegister) * sizeof(uint64_t);
    for (; i < (int) paramsRegisters.size(); i++)
    {
        // This is a C variadic parameter
        if (i >= maxParamsPerRegister)
        {
            emit_Load64_Indirect(regOffset(paramsRegisters[i]), RAX, RDI);
            emit_Store64_Indirect(offsetStack, RAX, RSP);
        }

        // This is for a return value
        else if (paramsTypes[i] == g_TypeMgr->typeInfoUndefined)
        {
            // r is an address to registerRR, for FFI
            if (retCopy)
                emit_Load64_Immediate((uint64_t) retCopy, RAX);
            else if (returnByCopy)
                emit_Load64_Indirect(paramsRegisters[i], RAX, RDI);
            else
                emit_LoadAddress_Indirect(paramsRegisters[i], RAX, RDI);
            emit_Store64_Indirect(offsetStack, RAX, RSP);
        }

        // This is for a normal parameter
        else
        {
            auto sizeOf = paramsTypes[i]->sizeOf;

            // Struct by copy. Will be a pointer to the stack
            if (paramsTypes[i]->kind == TypeInfoKind::Struct)
            {
                emit_Load64_Indirect(regOffset(paramsRegisters[i]), RAX, RDI);

                // Store the content of the struct in the stack
                if (cc.structByRegister && sizeOf <= sizeof(void*))
                {
                    switch (sizeOf)
                    {
                    case 1:
                        emit_Load8_Indirect(0, RAX, RAX);
                        emit_Store8_Indirect(offsetStack, RAX, RSP);
                        break;
                    case 2:
                        emit_Load16_Indirect(0, RAX, RAX);
                        emit_Store16_Indirect(offsetStack, RAX, RSP);
                        break;
                    case 4:
                        emit_Load32_Indirect(0, RAX, RAX);
                        emit_Store32_Indirect(offsetStack, RAX, RSP);
                        break;
                    case 8:
                        emit_Load64_Indirect(0, RAX, RAX);
                        emit_Store64_Indirect(offsetStack, RAX, RSP);
                        break;
                    }
                }

                // Store the address of the struct in the stack
                else
                {
                    emit_Store64_Indirect(offsetStack, RAX, RSP);
                }
            }
            else
            {
                switch (sizeOf)
                {
                case 1:
                    emit_Load8_Indirect(regOffset(paramsRegisters[i]), RAX, RDI);
                    emit_Store8_Indirect(offsetStack, RAX, RSP);
                    break;
                case 2:
                    emit_Load16_Indirect(regOffset(paramsRegisters[i]), RAX, RDI);
                    emit_Store16_Indirect(offsetStack, RAX, RSP);
                    break;
                case 4:
                    emit_Load32_Indirect(regOffset(paramsRegisters[i]), RAX, RDI);
                    emit_Store32_Indirect(offsetStack, RAX, RSP);
                    break;
                case 8:
                    emit_Load64_Indirect(regOffset(paramsRegisters[i]), RAX, RDI);
                    emit_Store64_Indirect(offsetStack, RAX, RSP);
                    break;
                default:
                    SWAG_ASSERT(false);
                    return;
                }
            }
        }

        // Push is always aligned
        offsetStack += 8;
    }
}

void X64Gen::emit_Call_Parameters(TypeInfoFuncAttr* typeFuncBC, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, void* retCopy)
{
    int numCallParams = (int) typeFuncBC->parameters.size();

    VectorNative<uint32_t>  paramsRegisters;
    VectorNative<TypeInfo*> paramsTypes;

    int indexParam = (int) pushRAParams.size() - 1;

    // Variadic are first
    if (typeFuncBC->isVariadic())
    {
        auto index = pushRAParams[indexParam--];
        paramsRegisters.push_back(index);
        paramsTypes.push_back(g_TypeMgr->typeInfoU64);

        index = pushRAParams[indexParam--];
        paramsRegisters.push_back(index);
        paramsTypes.push_back(g_TypeMgr->typeInfoU64);
        numCallParams--;
    }
    else if (typeFuncBC->isCVariadic())
    {
        numCallParams--;
    }

    // All parameters
    for (int i = 0; i < (int) numCallParams; i++)
    {
        auto typeParam = TypeManager::concreteReferenceType(typeFuncBC->parameters[i]->typeInfo);

        auto index = pushRAParams[indexParam--];

        if (typeParam->kind == TypeInfoKind::Pointer ||
            typeParam->kind == TypeInfoKind::Lambda ||
            typeParam->kind == TypeInfoKind::Array)
        {
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
        else if (typeParam->kind == TypeInfoKind::Struct)
        {
            paramsRegisters.push_back(index);
            paramsTypes.push_back(typeParam);
        }
        else if (typeParam->kind == TypeInfoKind::Slice ||
                 typeParam->isNative(NativeTypeKind::String))
        {
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr->typeInfoU64);
            index = pushRAParams[indexParam--];
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
        else if (typeParam->isNative(NativeTypeKind::Any) ||
                 typeParam->kind == TypeInfoKind::Interface)
        {
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr->typeInfoU64);
            index = pushRAParams[indexParam--];
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
        else
        {
            SWAG_ASSERT(typeParam->sizeOf <= sizeof(void*));
            paramsRegisters.push_back(index);
            paramsTypes.push_back(typeParam);
        }
    }

    // Return by parameter
    if (typeFuncBC->returnByCopy())
    {
        paramsRegisters.push_back(offsetRT);
        paramsTypes.push_back(g_TypeMgr->typeInfoUndefined);
    }

    // Add all C variadic parameters
    if (typeFuncBC->isCVariadic())
    {
        for (int i = typeFuncBC->numParamsRegisters(); i < pushRAParams.size(); i++)
        {
            auto index = pushRAParams[indexParam--];
            paramsRegisters.push_back(index);
            paramsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
    }

    // If the closure is assigned to a lambda, then we must not use the first parameter (the first
    // parameter is the capture context, which does not exist in a normal function)
    // But as this is dynamic, we need to have two call path : one for the closure (normal call), and
    // one for the lambda (omit first parameter)
    if (typeFuncBC->isClosure())
    {
        auto reg = paramsRegisters[0];
        emit_Load64_Indirect(regOffset(reg), RAX, RDI);
        emit_Test64(RAX, RAX);

        // If not zero, jump to closure call
        emit_LongJumpOp(JZ);
        concat.addU32(0);
        auto seekPtrClosure = concat.getSeekPtr() - 4;
        auto seekJmpClosure = concat.totalCount();

        emit_Call_Parameters(typeFuncBC, paramsRegisters, paramsTypes, retCopy);

        // Jump to after closure call
        emit_LongJumpOp(JUMP);
        concat.addU32(0);
        auto seekPtrAfterClosure = concat.getSeekPtr() - 4;
        auto seekJmpAfterClosure = concat.totalCount();

        // Update jump to closure call
        *seekPtrClosure = (uint8_t) (concat.totalCount() - seekJmpClosure);

        paramsRegisters.erase(0);
        paramsTypes.erase(0);
        emit_Call_Parameters(typeFuncBC, paramsRegisters, paramsTypes, retCopy);

        *seekPtrAfterClosure = (uint8_t) (concat.totalCount() - seekJmpAfterClosure);
    }
    else
    {
        emit_Call_Parameters(typeFuncBC, paramsRegisters, paramsTypes, retCopy);
    }
}

void X64Gen::emit_Call_Result(TypeInfoFuncAttr* typeFunc, uint32_t offsetRT)
{
    if (!typeFunc->returnByValue())
        return;

    const auto& cc         = g_CallConv[typeFunc->callConv];
    auto        returnType = TypeManager::concreteReferenceType(typeFunc->returnType);

    if (cc.useReturnByRegisterFloat && returnType->isNativeFloat())
        emit_StoreF64_Indirect(offsetRT, cc.returnByRegisterFloat, RDI);
    else
        emit_Store64_Indirect(offsetRT, cc.returnByRegisterInteger, RDI);
}

void X64Gen::emit_Call_Indirect(uint8_t reg)
{
    SWAG_ASSERT(reg == RAX || reg == RCX || reg == R10);
    if (reg == R10)
        concat.addU8(0x41);
    concat.addU8(0xFF);
    concat.addU8(0xD0 | (reg & 0b111));
}