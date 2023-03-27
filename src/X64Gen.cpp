#include "pch.h"
#include "pch.h"
#include "Register.h"
#include "Math.h"
#include "ByteCode.h"
#include "CallConv.h"
#include "X64Gen.h"
#include "TypeManager.h"

void X64Gen::clearInstructionCache()
{
    storageRegCount = UINT32_MAX;
    storageRegStack = 0;
    storageRegBits  = 0;
    storageMemReg   = RAX;
    storageReg      = RAX;
}

uint8_t X64Gen::getREX(bool w, bool r, bool x, bool b)
{
    uint8_t REX = 0x40;
    if (w) // 64 bits
        REX |= 8;
    if (r) // extended MODRM.reg
        REX |= 4;
    if (x) // extended SIB.index
        REX |= 2;
    if (b) // extended MODRM.rm
        REX |= 1;
    return REX;
}

uint8_t X64Gen::getModRM(uint8_t mod, uint8_t r, uint8_t m)
{
    return (mod << 6) | ((r & 0b111) << 3) | (m & 0b111);
}

void X64Gen::emit_REX()
{
    concat.addU8(getREX());
}

void X64Gen::emit_ModRM(uint32_t stackOffset, uint8_t reg, uint8_t memReg, uint8_t op)
{
    if (stackOffset == 0 && (memReg < R8 || memReg == R12))
    {
        // mov al, byte ptr [rdi]
        concat.addU8(getModRM(0, reg, memReg) | (op - 1));
        if (memReg == RSP || memReg == R12)
            concat.addU8(0x24);
    }
    else if (stackOffset <= 0x7F)
    {
        // mov al, byte ptr [rdi + ??]
        concat.addU8(getModRM(DISP8, reg, memReg) | (op - 1));
        if (memReg == RSP || memReg == R12)
            concat.addU8(0x24);
        concat.addU8((uint8_t) stackOffset);
    }
    else
    {
        // mov al, byte ptr [rdi + ????????]
        concat.addU8(getModRM(DISP32, reg, memReg) | (op - 1));
        if (memReg == RSP || memReg == R12)
            concat.addU8(0x24);
        concat.addU32(stackOffset);
    }
}

CoffSymbol* X64Gen::getSymbol(const Utf8& name)
{
    auto it = mapSymbols.find(name);
    if (it != mapSymbols.end())
        return &allSymbols[it->second];
    return nullptr;
}

CoffSymbol* X64Gen::getOrAddSymbol(const Utf8& name, CoffSymbolKind kind, uint32_t value, uint16_t sectionIdx)
{
    auto it = getSymbol(name);
    if (it)
    {
        if (it->kind == kind)
            return it;
        if (kind == CoffSymbolKind::Extern)
            return it;
        if (kind == CoffSymbolKind::Function && it->kind == CoffSymbolKind::Extern)
        {
            it->kind  = kind;
            it->value = value;
            return it;
        }

        SWAG_ASSERT(false);
    }

    CoffSymbol sym;
    sym.name       = name;
    sym.kind       = kind;
    sym.value      = value;
    sym.sectionIdx = sectionIdx;
    SWAG_ASSERT(allSymbols.size() < UINT32_MAX);
    sym.index = (uint32_t) allSymbols.size();
    allSymbols.emplace_back(std::move(sym));
    mapSymbols[name] = (uint32_t) allSymbols.size() - 1;
    return &allSymbols.back();
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_LoadS8S16_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    concat.addU8(0x66);
    if (reg >= R8 || memReg >= R8)
        concat.addU8(getREX(false, reg >= R8, false, memReg >= R8));
    concat.addU8(0x0F);
    concat.addU8(0xBE);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadS8S32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (reg >= R8 || memReg >= R8)
        concat.addU8(getREX(false, reg >= R8, false, memReg >= R8));
    concat.addU8(0x0F);
    concat.addU8(0xBE);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadS16S32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (reg >= R8 || memReg >= R8)
        concat.addU8(getREX(false, reg >= R8, false, memReg >= R8));
    concat.addU8(0x0F);
    concat.addU8(0xBF);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadS8S64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    concat.addU8(getREX(true, reg >= R8, false, memReg >= R8));
    concat.addU8(0x0F);
    concat.addU8(0xBE);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadS16S64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    concat.addU8(getREX(true, reg >= R8, false, memReg >= R8));
    concat.addU8(0x0F);
    concat.addU8(0xBF);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadS32S64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    concat.addU8(getREX(true, reg >= R8, false, memReg >= R8));
    concat.addU8(0x63);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadU8U32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (reg >= R8 || memReg >= R8)
        concat.addU8(getREX(false, reg >= R8, false, memReg >= R8));
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadU16U32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (reg >= R8 || memReg >= R8)
        concat.addU8(getREX(false, reg >= R8, false, memReg >= R8));
    concat.addU8(0x0F);
    concat.addU8(0xB7);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadU8U64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    concat.addU8(getREX(true, reg >= R8, false, memReg >= R8));
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadU16U64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    concat.addU8(getREX(true, reg >= R8, false, memReg >= R8));
    concat.addU8(0x0F);
    concat.addU8(0xB7);
    emit_ModRM(stackOffset, reg, memReg);
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_Load8_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (storageRegStack == stackOffset &&
        storageReg == reg &&
        storageMemReg == memReg &&
        storageRegCount == concat.totalCount())
    {
        return;
    }

    concat.addU8(getREX(false, reg >= R8, false, memReg >= R8));
    concat.addU8(0x8A);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_Load16_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (storageRegStack == stackOffset &&
        storageReg == reg &&
        storageMemReg == memReg &&
        storageRegCount == concat.totalCount())
    {
        return;
    }

    concat.addU8(0x66);
    concat.addU8(getREX(false, reg >= R8, false, memReg >= R8));
    concat.addU8(0x8B);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_Load32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (storageRegStack == stackOffset &&
        storageReg == reg &&
        storageMemReg == memReg &&
        storageRegCount == concat.totalCount())
    {
        if (storageRegBits > 32)
            emit_Copy32(RAX, RAX);
        return;
    }

    concat.addU8(getREX(false, reg >= R8, false, memReg >= R8));
    concat.addU8(0x8B);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_Load64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (storageRegStack == stackOffset &&
        storageReg == reg &&
        storageMemReg == memReg &&
        storageRegCount == concat.totalCount())
    {
        return;
    }

    concat.addU8(getREX(true, reg >= R8, false, memReg >= R8));
    concat.addU8(0x8B);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadN_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg, uint8_t numBits)
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

void X64Gen::emit_LoadF32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF3);
    concat.addU8(0x0F);
    concat.addU8(0x10);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_LoadF64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF2);
    concat.addU8(0x0F);
    concat.addU8(0x10);
    emit_ModRM(stackOffset, reg, memReg);
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_LoadAddress_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (stackOffset == 0)
        emit_Copy64(memReg, reg);
    else
    {
        concat.addU8(getREX(true, reg >= R8, false, memReg >= R8));
        concat.addU8(0x8D);
        emit_ModRM(stackOffset, reg, memReg);
    }
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_Store8_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x88);
    emit_ModRM(stackOffset, reg, memReg);

    storageRegCount = concat.totalCount();
    storageRegStack = stackOffset;
    storageReg      = reg;
    storageMemReg   = memReg;
    storageRegBits  = 8;
}

void X64Gen::emit_Store16_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x66);
    concat.addU8(0x89);
    emit_ModRM(stackOffset, reg, memReg);

    storageRegCount = concat.totalCount();
    storageRegStack = stackOffset;
    storageReg      = reg;
    storageMemReg   = memReg;
    storageRegBits  = 16;
}

void X64Gen::emit_Store32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x89);
    emit_ModRM(stackOffset, reg, memReg);

    storageRegCount = concat.totalCount();
    storageRegStack = stackOffset;
    storageReg      = reg;
    storageMemReg   = memReg;
    storageRegBits  = 32;
}

void X64Gen::emit_Store64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    concat.addU8(getREX(true, reg >= R8, false, memReg >= R8));
    concat.addU8(0x89);
    emit_ModRM(stackOffset, reg, memReg);

    storageRegCount = concat.totalCount();
    storageRegStack = stackOffset;
    storageReg      = reg;
    storageMemReg   = memReg;
    storageRegBits  = 64;
}

void X64Gen::emit_StoreN_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg, uint8_t numBits)
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

void X64Gen::emit_StoreF32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF3);
    concat.addU8(0x0F);
    concat.addU8(0x11);
    emit_ModRM(stackOffset, reg, memReg);
}

void X64Gen::emit_StoreF64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF2);
    concat.addU8(0x0F);
    concat.addU8(0x11);
    emit_ModRM(stackOffset, reg, memReg);
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_Store8_Immediate(uint32_t stackOffset, uint8_t val, CPURegister memReg)
{
    concat.addU8(0xC6);
    emit_ModRM(stackOffset, 0, memReg);
    concat.addU8(val);
}

void X64Gen::emit_Store16_Immediate(uint32_t stackOffset, uint16_t val, CPURegister memReg)
{
    concat.addU8(0x66);
    concat.addU8(0xC7);
    emit_ModRM(stackOffset, 0, memReg);
    concat.addU16(val);
}

void X64Gen::emit_Store32_Immediate(uint32_t stackOffset, uint32_t val, CPURegister memReg)
{
    concat.addU8(0xC7);
    emit_ModRM(stackOffset, 0, memReg);
    concat.addU32((uint32_t) val);
}

void X64Gen::emit_Store64_Immediate(uint32_t stackOffset, uint64_t val, CPURegister memReg)
{
    SWAG_ASSERT(val <= 0xFFFFFFFF);
    concat.addU8(getREX());
    concat.addU8(0xC7);
    emit_ModRM(stackOffset, 0, memReg);
    concat.addU32((uint32_t) val);
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_Load8_Immediate(uint8_t val, CPURegister reg)
{
    if (val == 0)
    {
        emit_Clear8(reg);
        return;
    }

    concat.addU8(0xB0 | reg);
    concat.addU8(val);
}

void X64Gen::emit_Load16_Immediate(uint16_t val, CPURegister reg)
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

void X64Gen::emit_Load32_Immediate(uint32_t val, CPURegister reg)
{
    if (val == 0)
    {
        emit_Clear32(reg);
        return;
    }

    concat.addU8(0xB8 | reg);
    concat.addU32(val);
}

void X64Gen::emit_Load64_Immediate(uint64_t val, CPURegister reg, bool force64bits)
{
    if (force64bits)
    {
        concat.addU8(getREX(true, false, false, reg >= R8));
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

    concat.addU8(getREX(true, false, false, reg >= R8));
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

void X64Gen::emit_LoadN_Immediate(Register& val, CPURegister reg, uint8_t numBits)
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

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_ClearN(CPURegister reg, uint8_t numBits)
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

void X64Gen::emit_Clear8(CPURegister reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0x30);
    concat.addU8(getModRM(REGREG, reg, reg));
}

void X64Gen::emit_Clear16(CPURegister reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0x66);
    concat.addU8(0x31);
    concat.addU8(getModRM(REGREG, reg, reg));
}

void X64Gen::emit_Clear32(CPURegister reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0x31);
    concat.addU8(getModRM(REGREG, reg, reg));
}

void X64Gen::emit_Clear64(CPURegister reg)
{
    concat.addU8(getREX(true, reg >= R8, false, reg >= R8));
    concat.addU8(0x31);
    concat.addU8(getModRM(REGREG, reg, reg));
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_Push(CPURegister reg)
{
    if (reg < R8)
        concat.addU8(0x50 | (reg & 0b111));
    else
    {
        concat.addU8(getREX(false, false, false, true));
        concat.addU8(0x50 | (reg & 0b111));
    }
}

void X64Gen::emit_Pop(CPURegister reg)
{
    if (reg < R8)
        concat.addU8(0x58 | (reg & 0b111));
    else
    {
        concat.addU8(getREX(false, false, false, true));
        concat.addU8(0x58 | (reg & 0b111));
    }
}

void X64Gen::emit_Ret()
{
    concat.addU8(0xC3);
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_Copy8(CPURegister regSrc, CPURegister regDst)
{
    if (regSrc >= R8 || regDst >= R8)
        concat.addU8(getREX(false, regSrc >= R8, false, regDst >= R8));
    concat.addU8(0x88);
    concat.addU8(getModRM(REGREG, regSrc, regDst));
}

void X64Gen::emit_Copy16(CPURegister regSrc, CPURegister regDst)
{
    concat.addU8(0x66);
    if (regSrc >= R8 || regDst >= R8)
        concat.addU8(getREX(false, regSrc >= R8, false, regDst >= R8));
    concat.addU8(0x89);
    concat.addU8(getModRM(REGREG, regSrc, regDst));
}

void X64Gen::emit_Copy32(CPURegister regSrc, CPURegister regDst)
{
    if (regSrc >= R8 || regDst >= R8)
        concat.addU8(getREX(false, regSrc >= R8, false, regDst >= R8));
    concat.addU8(0x89);
    concat.addU8(getModRM(REGREG, regSrc, regDst));
}

void X64Gen::emit_Copy64(CPURegister regSrc, CPURegister regDst)
{
    concat.addU8(getREX(true, regSrc >= R8, false, regDst >= R8));
    concat.addU8(0x89);
    concat.addU8(getModRM(REGREG, regSrc, regDst));
}

void X64Gen::emit_CopyF32(CPURegister regSrc, CPURegister regDst)
{
    SWAG_ASSERT(regSrc == RAX);
    SWAG_ASSERT(regDst == XMM0 || regDst == XMM1 || regDst == XMM2 || regDst == XMM3);
    concat.addU8(0x66);
    concat.addU8(0x0F);
    concat.addU8(0x6E);
    concat.addU8(0xC0 | (regDst << 3));
}

void X64Gen::emit_CopyF64(CPURegister regSrc, CPURegister regDst)
{
    SWAG_ASSERT(regSrc == RAX);
    SWAG_ASSERT(regDst == XMM0 || regDst == XMM1 || regDst == XMM2 || regDst == XMM3);
    concat.addU8(0x66);
    concat.addU8(getREX());
    concat.addU8(0x0F);
    concat.addU8(0x6E);
    concat.addU8(0xC0 | (regDst << 3));
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_SetNE()
{
    concat.addU8(0x0F);
    concat.addU8(0x95);
    concat.addU8(0xC0);
}

void X64Gen::emit_SetA(CPURegister reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0x0F);
    concat.addU8(0x97);
    concat.addU8(0xC0 | reg);
}

void X64Gen::emit_SetAE(CPURegister reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0x0F);
    concat.addU8(0x93);
    concat.addU8(0xC0 | reg);
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

void X64Gen::emit_SetEP()
{
    // sete al
    concat.addU8(0x0F);
    concat.addU8(0x94);
    concat.addU8(0xC0);

    // setnp ah
    concat.addU8(0x0F);
    concat.addU8(0x9B);
    concat.addU8(0xC4);

    // and al, ah
    concat.addU8(0x20);
    concat.addU8(0xE0);
}

void X64Gen::emit_SetNEP()
{
    // setne al
    concat.addU8(0x0F);
    concat.addU8(0x95);
    concat.addU8(0xC0);

    // setp ah
    concat.addU8(0x0F);
    concat.addU8(0x9A);
    concat.addU8(0xC4);

    // or al, ah
    concat.addU8(0x08);
    concat.addU8(0xE0);
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

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_Test64(uint8_t reg1, uint8_t reg2)
{
    concat.addU8(getREX(true, reg2 >= R8, false, reg1 >= R8));
    concat.addU8(0x85);
    concat.addU8(getModRM(REGREG, reg2, reg1));
}

void X64Gen::emit_Test32(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x85);
    concat.addU8(getModRM(REGREG, reg2, reg1));
}

void X64Gen::emit_Test16(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x66);
    concat.addU8(0x85);
    concat.addU8(getModRM(REGREG, reg2, reg1));
}

void X64Gen::emit_Test8(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x84);
    concat.addU8(getModRM(REGREG, reg2, reg1));
}

void X64Gen::emit_Cmp8(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x38);
    concat.addU8(getModRM(REGREG, reg2, reg1));
}

void X64Gen::emit_Cmp16(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x66);
    concat.addU8(0x39);
    concat.addU8(getModRM(REGREG, reg2, reg1));
}

void X64Gen::emit_Cmp32(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x39);
    concat.addU8(getModRM(REGREG, reg2, reg1));
}

void X64Gen::emit_Cmp64(uint8_t reg1, uint8_t reg2)
{
    concat.addU8(getREX(true, reg2 >= R8, false, reg1 >= R8));
    concat.addU8(0x39);
    concat.addU8(getModRM(REGREG, reg2, reg1));
}

void X64Gen::emit_Cmp64_Immediate(uint64_t value, CPURegister reg)
{
    SWAG_ASSERT(reg == RAX);

    if (value <= 0x7f)
    {
        concat.addU8(getREX());
        concat.addU8(0x83);
        concat.addU8(0xF8 | reg);
        concat.addU8((uint8_t) value);
    }
    else if (value <= 0x7fffffff)
    {
        concat.addU8(getREX());
        concat.addU8(0x3d);
        concat.addU32((uint32_t) value);
    }
    else
    {
        emit_Load64_Immediate(value, RCX);
        emit_Cmp64(reg, RCX);
    }
}

void X64Gen::emit_CmpF32(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x0F);
    concat.addU8(0x2F);
    concat.addU8(getModRM(REGREG, reg1, reg2));
}

void X64Gen::emit_CmpF64(uint8_t reg1, uint8_t reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x66);
    concat.addU8(0x0F);
    concat.addU8(0x2F);
    concat.addU8(getModRM(REGREG, reg1, reg2));
}

void X64Gen::emit_Cmp8_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x3A);
    emit_ModRM(offsetStack, reg, memReg);
}

void X64Gen::emit_Cmp16_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x66);
    concat.addU8(0x3B);
    emit_ModRM(offsetStack, reg, memReg);
}

void X64Gen::emit_Cmp32_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x3B);
    emit_ModRM(offsetStack, reg, RDI);
}

void X64Gen::emit_Cmp64_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(getREX());
    concat.addU8(0x3B);
    emit_ModRM(offsetStack, reg, memReg);
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
    concat.addU8(getREX());
    emit_Cmp32_IndirectDst(offsetStack, value);
}

void X64Gen::emit_CmpF32_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x0F);
    // concat.addU8(0x2F);
    concat.addU8(0x2E);
    emit_ModRM(offsetStack, reg, memReg);
}

void X64Gen::emit_CmpF64_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x66);
    concat.addU8(0x0F);
    concat.addU8(0x2F);
    emit_ModRM(offsetStack, reg, memReg);
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_Op8_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, X64Op instruction, bool lock)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    if (lock)
        concat.addU8(0xF0);
    concat.addU8((uint8_t) instruction & ~1);
    emit_ModRM(offsetStack, reg, memReg);
}

void X64Gen::emit_Op16_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, X64Op instruction, bool lock)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    if (lock)
        concat.addU8(0xF0);
    concat.addU8(0x66);
    concat.addU8((uint8_t) instruction);
    emit_ModRM(offsetStack, reg, memReg);
}

void X64Gen::emit_Op32_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, X64Op instruction, bool lock)
{
    SWAG_ASSERT(memReg < R8);
    if (lock)
        concat.addU8(0xF0);
    if (reg >= R8)
        concat.addU8(0x44);
    concat.addU8((uint8_t) instruction);
    emit_ModRM(offsetStack, reg, memReg);
}

void X64Gen::emit_Op64_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, X64Op instruction, bool lock)
{
    SWAG_ASSERT(memReg < R8 && reg < R8);
    if (lock)
        concat.addU8(0xF0);
    concat.addU8(getREX());
    concat.addU8((uint8_t) instruction);
    emit_ModRM(offsetStack, reg, memReg);
}

void X64Gen::emit_OpF32_Indirect(CPURegister reg, CPURegister memReg, X64Op instruction)
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

void X64Gen::emit_OpF64_Indirect(CPURegister reg, CPURegister memReg, X64Op instruction)
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
    concat.addU8(getModRM(REGREG, reg1, reg2));
}

void X64Gen::emit_Op16(uint8_t reg1, uint8_t reg2, X64Op instruction)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x66);
    concat.addU8((uint8_t) instruction);
    concat.addU8(getModRM(REGREG, reg1, reg2));
}

void X64Gen::emit_Op32(uint8_t reg1, uint8_t reg2, X64Op instruction)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8((uint8_t) instruction);
    concat.addU8(getModRM(REGREG, reg1, reg2));
}

void X64Gen::emit_Op64(uint8_t reg1, uint8_t reg2, X64Op instruction)
{
    concat.addU8(getREX(true, reg1 >= R8, false, reg2 >= R8));
    concat.addU8((uint8_t) instruction);
    concat.addU8(getModRM(REGREG, reg1, reg2));
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_Add64_Immediate(uint64_t value, CPURegister reg)
{
    switch (reg)
    {
    case RAX:
        emit_Add64_RAX(value);
        break;
    case RCX:
        emit_Add64_RCX(value);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

void X64Gen::emit_Add64_RAX(uint64_t value)
{
    if (!value)
        return;

    concat.addU8(getREX());
    if (value == 1)
    {
        concat.addU8(0xFF);
        concat.addU8(0xC0); // inc rax
    }
    else if (value <= 0x7F)
    {
        concat.addU8(0x83);
        concat.addU8(0xC0);
        concat.addU8((uint8_t) value);
    }
    else
    {
        concat.addU8(0x05);
        concat.addU32((uint32_t) value);
    }
}

void X64Gen::emit_Add64_RCX(uint64_t value)
{
    if (!value)
        return;

    concat.addU8(getREX());
    if (value == 1)
    {
        concat.addU8(0xFF);
        concat.addU8(0xC1); // inc rcx
    }
    else if (value <= 0x7F)
    {
        concat.addU8(0x83);
        concat.addU8(0xC1);
        concat.addU8((uint8_t) value);
    }
    else
    {
        concat.addU8(0x81);
        concat.addU8(0xC1);
        concat.addU32((uint32_t) value);
    }
}

void X64Gen::emit_Sub64_RAX(uint64_t value)
{
    if (!value)
        return;

    if (value == 1)
    {
        concat.addU8(getREX());
        concat.addU8(0xFF);
        concat.addU8(0xC8); // dec rax
    }
    else if (value > 0x7FFFFFFF)
    {
        emit_Load64_Immediate(value, RCX);
        concat.addU8(getREX());
        concat.addU8(0x29);
        concat.addU8(0xC8); // sub rax, rcx
    }
    else if (value <= 0x7F)
    {
        concat.addU8(getREX());
        concat.addU8(0x83);
        concat.addU8(0xE8);
        concat.addU8((uint8_t) value);
    }
    else
    {
        concat.addU8(getREX());
        concat.addU8(0x2D);
        concat.addU32((uint32_t) value);
    }
}

void X64Gen::emit_Mul64_RAX(uint64_t value)
{
    if (value == 1)
        return;

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

void X64Gen::emit_Symbol_RelocationAddr(CPURegister reg, uint32_t symbolIndex, uint32_t offset)
{
    SWAG_ASSERT(reg == RAX || reg == RCX || reg == RDX || reg == R8 || reg == R9 || reg == RDI);
    if (reg == R8 || reg == R9)
        concat.addU8(0x4C);
    else
        concat.addU8(getREX());
    concat.addU8(0x8D);
    concat.addU8(0x05 | ((reg & 0b111) << 3));

    CoffRelocation reloc;
    reloc.virtualAddress = concat.totalCount() - textSectionOffset;
    reloc.symbolIndex    = symbolIndex;
    reloc.type           = IMAGE_REL_AMD64_REL32;
    relocTableTextSection.table.push_back(reloc);
    concat.addU32(offset);
}

void X64Gen::emit_Symbol_RelocationValue(CPURegister reg, uint32_t symbolIndex, uint32_t offset)
{
    SWAG_ASSERT(reg == RAX || reg == RCX || reg == RDX || reg == R8 || reg == R9);

    if (reg == R8 || reg == R9)
        concat.addU8(0x4C);
    else
        concat.addU8(getREX());
    concat.addU8(0x8B);
    concat.addU8(0x05 | ((reg & 0b111) << 3));

    CoffRelocation reloc;
    reloc.virtualAddress = concat.totalCount() - textSectionOffset;
    reloc.symbolIndex    = symbolIndex;
    reloc.type           = IMAGE_REL_AMD64_REL32;
    relocTableTextSection.table.push_back(reloc);
    concat.addU32(offset);
}

void X64Gen::emit_Sub32_RSP(uint32_t value)
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

void X64Gen::emit_Add32_RSP(uint32_t value)
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

void X64Gen::emit_Extend_S8S32(CPURegister reg)
{
    switch (reg)
    {
    case RAX:
        concat.addString3("\x0F\xBE\xC0"); // movsx eax, al
        break;
    case RCX:
        concat.addString3("\x0f\xbe\xC9"); // movsx ecx, cl
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

void X64Gen::emit_Extend_S8S16(CPURegister reg)
{
    switch (reg)
    {
    case RAX:
        concat.addString4("\x66\x0F\xBE\xC0"); // movsx ax, al
        break;
    case RCX:
        concat.addString4("\x66\x0f\xbe\xC9"); // movsx cx, cl
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

void X64Gen::emit_Extend_U8U32(CPURegister reg)
{
    // movzx eax, al
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    concat.addU8(getModRM(REGREG, reg, reg));
}

void X64Gen::emit_Extend_U16U32(CPURegister reg)
{
    // movzx rax, ax
    concat.addU8(0x0F);
    concat.addU8(0xB7);
    concat.addU8(getModRM(REGREG, reg, reg));
}

void X64Gen::emit_Extend_U8U64(CPURegister regSrc, CPURegister regDst)
{
    // movzx regDst.64, regSrc.8
    concat.addU8(getREX(true, regDst >= R8, false, regSrc >= R8));
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    concat.addU8(getModRM(REGREG, regDst, regSrc));
}

void X64Gen::emit_Extend_U16U64(CPURegister regSrc, CPURegister regDst)
{
    // movzx regDst.64, regSrc.16
    concat.addU8(getREX(true, regDst >= R8, false, regSrc >= R8));
    concat.addU8(0x0F);
    concat.addU8(0xB7);
    concat.addU8(getModRM(REGREG, regDst, regSrc));
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
    case JP:
        concat.addU8(0x7A);
        break;
    case JNP:
        concat.addU8(0x7B);
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
    case JNO:
        concat.addU8(0x0F);
        concat.addU8(0x81);
        break;
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
    case JP:
        concat.addU8(0x0F);
        concat.addU8(0x8A);
        break;
    case JNP:
        concat.addU8(0x0F);
        concat.addU8(0x8B);
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

void X64Gen::emit_CopyX(uint32_t count, uint32_t offset, CPURegister regDst, CPURegister regSrc)
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

void X64Gen::emit_ClearX(uint32_t count, uint32_t offset, CPURegister reg)
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

void X64Gen::emit_Call_Parameters(TypeInfoFuncAttr* typeFuncBC, VectorNative<X64PushParam>& paramsRegisters, VectorNative<TypeInfo*>& paramsTypes, void* retCopy)
{
    const auto& cc           = g_CallConv[typeFuncBC->callConv];
    auto        returnType   = TypeManager::concreteType(typeFuncBC->returnType);
    bool        returnByCopy = returnType->flags & TYPEINFO_RETURN_BY_COPY;

    int callConvRegisters    = cc.byRegisterCount;
    int maxParamsPerRegister = (int) paramsRegisters.size();

    // Set the first N parameters. Can be return register, or function parameter.
    int i = 0;
    for (; i < min(callConvRegisters, maxParamsPerRegister); i++)
    {
        auto type = paramsTypes[i];
        auto reg  = (uint32_t) paramsRegisters[i].reg;

        if (type->isAutoConstPointerRef())
            type = TypeManager::concretePtrRef(type);

        // This is a return register
        if (type == g_TypeMgr->typeInfoUndefined)
        {
            SWAG_ASSERT(paramsRegisters[i].type == X64PushParamType::Reg);
            if (retCopy)
                emit_Load64_Immediate((uint64_t) retCopy, cc.byRegisterInteger[i]);
            else if (returnByCopy)
                emit_Load64_Indirect(reg, cc.byRegisterInteger[i], RDI);
            else
                emit_LoadAddress_Indirect(reg, cc.byRegisterInteger[i], RDI);
        }

        // This is a normal parameter, which can be float or integer
        else
        {
            // Pass struct in a register if small enough
            if (cc.structByRegister && type->isStruct() && type->sizeOf <= sizeof(void*))
            {
                SWAG_ASSERT(paramsRegisters[i].type == X64PushParamType::Reg);
                emit_Load64_Indirect(regOffset(reg), RAX);
                emit_Load64_Indirect(0, cc.byRegisterInteger[i], RAX);
            }
            else if (cc.useRegisterFloat && type->isNative(NativeTypeKind::F32))
            {
                if (paramsRegisters[i].type == X64PushParamType::Imm)
                {
                    SWAG_ASSERT(paramsRegisters[i].reg <= UINT32_MAX);
                    emit_Load32_Immediate((uint32_t) paramsRegisters[i].reg, RAX);
                    emit_CopyF32(RAX, cc.byRegisterFloat[i]);
                }
                else
                {
                    SWAG_ASSERT(paramsRegisters[i].type == X64PushParamType::Reg);
                    emit_LoadF32_Indirect(regOffset(reg), cc.byRegisterFloat[i]);
                }
            }
            else if (cc.useRegisterFloat && type->isNative(NativeTypeKind::F64))
            {
                if (paramsRegisters[i].type == X64PushParamType::Imm)
                {
                    emit_Load64_Immediate(paramsRegisters[i].reg, RAX);
                    emit_CopyF64(RAX, cc.byRegisterFloat[i]);
                }
                else
                {
                    SWAG_ASSERT(paramsRegisters[i].type == X64PushParamType::Reg);
                    emit_LoadF64_Indirect(regOffset(reg), cc.byRegisterFloat[i]);
                }
            }
            else
            {
                switch (paramsRegisters[i].type)
                {
                case X64PushParamType::Imm:
                    if (paramsRegisters[i].reg == 0)
                        emit_Clear64(cc.byRegisterInteger[i]);
                    else
                        emit_Load64_Immediate(paramsRegisters[i].reg, cc.byRegisterInteger[i]);
                    break;
                case X64PushParamType::Imm64:
                    emit_Load64_Immediate(paramsRegisters[i].reg, cc.byRegisterInteger[i], true);
                    break;
                case X64PushParamType::RelocV:
                    emit_Symbol_RelocationValue(cc.byRegisterInteger[i], (uint32_t) paramsRegisters[i].reg, 0);
                    break;
                case X64PushParamType::RelocAddr:
                    emit_Symbol_RelocationAddr(cc.byRegisterInteger[i], (uint32_t) paramsRegisters[i].reg, 0);
                    break;
                case X64PushParamType::Addr:
                    emit_LoadAddress_Indirect((uint32_t) paramsRegisters[i].reg, cc.byRegisterInteger[i], RDI);
                    break;
                case X64PushParamType::RegAdd:
                    emit_Load64_Indirect(regOffset(reg), cc.byRegisterInteger[i]);
                    emit_Add64_Immediate(paramsRegisters[i].val, cc.byRegisterInteger[i]);
                    break;
                case X64PushParamType::RegMul:
                    emit_Load64_Indirect(regOffset(reg), RAX);
                    emit_Mul64_RAX(paramsRegisters[i].val);
                    emit_Copy64(RAX, cc.byRegisterInteger[i]);
                    break;
                case X64PushParamType::GlobalString:
                    emit_GlobalString((const char*) paramsRegisters[i].reg, cc.byRegisterInteger[i]);
                    break;
                default:
                    SWAG_ASSERT(paramsRegisters[i].type == X64PushParamType::Reg);
                    emit_Load64_Indirect(regOffset(reg), cc.byRegisterInteger[i]);
                    break;
                }
            }
        }
    }

    // Store all parameters after N on the stack, with an offset of N * sizeof(uint64_t)
    uint32_t offsetStack = min(callConvRegisters, maxParamsPerRegister) * sizeof(uint64_t);
    for (; i < (int) paramsRegisters.size(); i++)
    {
        auto type = paramsTypes[i];
        if (type->isAutoConstPointerRef())
            type = TypeManager::concretePtrRef(type);

        auto reg = (uint32_t) paramsRegisters[i].reg;
        SWAG_ASSERT(paramsRegisters[i].type == X64PushParamType::Reg);

        // This is a C variadic parameter
        if (i >= maxParamsPerRegister)
        {
            emit_Load64_Indirect(regOffset(reg), RAX);
            emit_Store64_Indirect(offsetStack, RAX, RSP);
        }

        // This is for a return value
        else if (type == g_TypeMgr->typeInfoUndefined)
        {
            // r is an address to registerRR, for FFI
            if (retCopy)
                emit_Load64_Immediate((uint64_t) retCopy, RAX);
            else if (returnByCopy)
                emit_Load64_Indirect(reg, RAX, RDI);
            else
                emit_LoadAddress_Indirect(reg, RAX, RDI);
            emit_Store64_Indirect(offsetStack, RAX, RSP);
        }

        // This is for a normal parameter
        else
        {
            auto sizeOf = type->sizeOf;

            // Struct by copy. Will be a pointer to the stack
            if (type->isStruct())
            {
                emit_Load64_Indirect(regOffset(reg), RAX);

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
                    emit_Load8_Indirect(regOffset(reg), RAX);
                    emit_Store8_Indirect(offsetStack, RAX, RSP);
                    break;
                case 2:
                    emit_Load16_Indirect(regOffset(reg), RAX);
                    emit_Store16_Indirect(offsetStack, RAX, RSP);
                    break;
                case 4:
                    emit_Load32_Indirect(regOffset(reg), RAX);
                    emit_Store32_Indirect(offsetStack, RAX, RSP);
                    break;
                case 8:
                    emit_Load64_Indirect(regOffset(reg), RAX);
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

void X64Gen::emit_Call_Parameters(TypeInfoFuncAttr* typeFunc, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, void* retCopy)
{
    pushParams2.clear();
    for (auto r : pushRAParams)
        pushParams2.push_back({X64PushParamType::Reg, r});
    emit_Call_Parameters(typeFunc, pushParams2, offsetRT, retCopy);
}

void X64Gen::emit_Call_Parameters(TypeInfoFuncAttr* typeFunc, const VectorNative<X64PushParam>& pushRAParams, uint32_t offsetRT, void* retCopy)
{
    int numCallParams = (int) typeFunc->parameters.size();
    pushParams3.clear();
    pushParamsTypes.clear();

    int indexParam = (int) pushRAParams.size() - 1;

    // Variadic are first
    if (typeFunc->isVariadic())
    {
        auto index = pushRAParams[indexParam--];
        pushParams3.push_back(index);
        pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);

        index = pushRAParams[indexParam--];
        pushParams3.push_back(index);
        pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
        numCallParams--;
    }
    else if (typeFunc->isCVariadic())
    {
        numCallParams--;
    }

    // All parameters
    for (int i = 0; i < (int) numCallParams; i++)
    {
        auto typeParam = TypeManager::concreteType(typeFunc->parameters[i]->typeInfo);
        if (typeParam->isAutoConstPointerRef())
            typeParam = TypeManager::concretePtrRef(typeParam);

        auto index = pushRAParams[indexParam--];

        if (typeParam->isPointer() ||
            typeParam->isLambdaClosure() ||
            typeParam->isArray())
        {
            pushParams3.push_back(index);
            pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
        else if (typeParam->isStruct())
        {
            pushParams3.push_back(index);
            pushParamsTypes.push_back(typeParam);
        }
        else if (typeParam->isSlice() ||
                 typeParam->isString())
        {
            pushParams3.push_back(index);
            pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
            index = pushRAParams[indexParam--];
            pushParams3.push_back(index);
            pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
        else if (typeParam->isAny() ||
                 typeParam->isInterface())
        {
            pushParams3.push_back(index);
            pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
            index = pushRAParams[indexParam--];
            pushParams3.push_back(index);
            pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
        else
        {
            SWAG_ASSERT(typeParam->sizeOf <= sizeof(void*));
            pushParams3.push_back(index);
            pushParamsTypes.push_back(typeParam);
        }
    }

    // Return by parameter
    if (typeFunc->returnByCopy())
    {
        pushParams3.push_back({X64PushParamType::Reg, offsetRT});
        pushParamsTypes.push_back(g_TypeMgr->typeInfoUndefined);
    }

    // Add all C variadic parameters
    if (typeFunc->isCVariadic())
    {
        for (int i = typeFunc->numParamsRegisters(); i < pushRAParams.size(); i++)
        {
            auto index = pushRAParams[indexParam--];
            pushParams3.push_back(index);
            pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
    }

    // If the closure is assigned to a lambda, then we must not use the first parameter (the first
    // parameter is the capture context, which does not exist in a normal function)
    // But as this is dynamic, we need to have two call path : one for the closure (normal call), and
    // one for the lambda (omit first parameter)
    if (typeFunc->isClosure())
    {
        SWAG_ASSERT(pushParams3[0].type == X64PushParamType::Reg);
        auto reg = (uint32_t) pushParams3[0].reg;

        emit_Load64_Indirect(regOffset(reg), RAX);
        emit_Test64(RAX, RAX);

        // If not zero, jump to closure call
        emit_LongJumpOp(JZ);
        concat.addU32(0);
        auto seekPtrClosure = concat.getSeekPtr() - 4;
        auto seekJmpClosure = concat.totalCount();

        emit_Call_Parameters(typeFunc, pushParams3, pushParamsTypes, retCopy);

        // Jump to after closure call
        emit_LongJumpOp(JUMP);
        concat.addU32(0);
        auto seekPtrAfterClosure = concat.getSeekPtr() - 4;
        auto seekJmpAfterClosure = concat.totalCount();

        // Update jump to closure call
        *seekPtrClosure = (uint8_t) (concat.totalCount() - seekJmpClosure);

        pushParams3.erase(0);
        pushParamsTypes.erase(0);
        emit_Call_Parameters(typeFunc, pushParams3, pushParamsTypes, retCopy);

        *seekPtrAfterClosure = (uint8_t) (concat.totalCount() - seekJmpAfterClosure);
    }
    else
    {
        emit_Call_Parameters(typeFunc, pushParams3, pushParamsTypes, retCopy);
    }
}

void X64Gen::emit_Call_Result(TypeInfoFuncAttr* typeFunc, uint32_t offsetRT)
{
    if (!typeFunc->returnByValue())
        return;

    const auto& cc         = g_CallConv[typeFunc->callConv];
    auto        returnType = TypeManager::concreteType(typeFunc->returnType);

    if (cc.useReturnByRegisterFloat && returnType->isNativeFloat())
        emit_StoreF64_Indirect(offsetRT, cc.returnByRegisterFloat, RDI);
    else
        emit_Store64_Indirect(offsetRT, cc.returnByRegisterInteger, RDI);
}

void X64Gen::emit_Call_Indirect(CPURegister reg)
{
    SWAG_ASSERT(reg == RAX || reg == RCX || reg == R10);
    if (reg == R10)
        concat.addU8(0x41);
    concat.addU8(0xFF);
    concat.addU8(0xD0 | (reg & 0b111));
}

void X64Gen::emit_GlobalString(const Utf8& str, CPURegister reg)
{
    emit_Load64_Immediate(0, reg, true);

    auto        it  = globalStrings.find(str);
    CoffSymbol* sym = nullptr;
    if (it != globalStrings.end())
        sym = &allSymbols[it->second];
    else
    {
        Utf8 symName       = Fmt("__str%u", (uint32_t) globalStrings.size());
        sym                = getOrAddSymbol(symName, CoffSymbolKind::GlobalString);
        globalStrings[str] = sym->index;
        sym->value         = stringSegment.addStringNoLock(str);
    }

    CoffRelocation reloc;
    reloc.virtualAddress = (concat.totalCount() - 8) - textSectionOffset;
    reloc.symbolIndex    = sym->index;
    reloc.type           = IMAGE_REL_AMD64_ADDR64;
    relocTableTextSection.table.push_back(reloc);
}

void X64Gen::emit_Cwd()
{
    concat.addString2("\x66\x99"); // cwd
}

void X64Gen::emit_Cdq()
{
    concat.addU8(0x99); // cdq
}

void X64Gen::emit_Cqo()
{
    concat.addString2("\x48\x99"); // cqo
}

void X64Gen::emit_Not8(CPURegister reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0xF6);
    concat.addU8(0xD0 | (reg & 0b111));
}

void X64Gen::emit_Not16(CPURegister reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0x66);
    concat.addU8(0xF7);
    concat.addU8(0xD0 | (reg & 0b111));
}

void X64Gen::emit_Not32(CPURegister reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0xF7);
    concat.addU8(0xD0 | (reg & 0b111));
}

void X64Gen::emit_Not64(CPURegister reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(getREX());
    concat.addU8(0xF7);
    concat.addU8(0xD0 | (reg & 0b111));
}

void X64Gen::emit_Not8_Indirect(uint32_t stackOffset, CPURegister memReg)
{
    SWAG_ASSERT(memReg == RDI);

    concat.addU8(0xF6);
    if (stackOffset <= 0x7F)
    {
        concat.addU8(0x57);
        concat.addU8((uint8_t) stackOffset);
    }
    else
    {
        concat.addU8(0x97);
        concat.addU32(stackOffset);
    }
}

void X64Gen::emit_Not16_Indirect(uint32_t stackOffset, CPURegister memReg)
{
    SWAG_ASSERT(memReg == RDI);

    concat.addU8(0x66);
    concat.addU8(0xF7);
    if (stackOffset <= 0x7F)
    {
        concat.addU8(0x57);
        concat.addU8((uint8_t) stackOffset);
    }
    else
    {
        concat.addU8(0x97);
        concat.addU32(stackOffset);
    }
}

void X64Gen::emit_Not32_Indirect(uint32_t stackOffset, CPURegister memReg)
{
    SWAG_ASSERT(memReg == RDI);

    concat.addU8(0xF7);
    if (stackOffset <= 0x7F)
    {
        concat.addU8(0x57);
        concat.addU8((uint8_t) stackOffset);
    }
    else
    {
        concat.addU8(0x97);
        concat.addU32(stackOffset);
    }
}

void X64Gen::emit_Not64_Indirect(uint32_t stackOffset, CPURegister memReg)
{
    SWAG_ASSERT(memReg == RDI);

    concat.addU8(getREX());
    concat.addU8(0xF7);
    if (stackOffset <= 0x7F)
    {
        concat.addU8(0x57);
        concat.addU8((uint8_t) stackOffset);
    }
    else
    {
        concat.addU8(0x97);
        concat.addU32(stackOffset);
    }
}

void X64Gen::emit_Inc8_Indirect(uint32_t stackOffset, CPURegister memReg)
{
    SWAG_ASSERT(memReg < R8);
    concat.addU8(0xFE);
    emit_ModRM(stackOffset, 0, memReg);
}

void X64Gen::emit_Dec8_Indirect(uint32_t stackOffset, CPURegister memReg)
{
    SWAG_ASSERT(memReg < R8);
    concat.addU8(0xFE);
    emit_ModRM(stackOffset, 1, memReg);
}

void X64Gen::emit_Inc16_Indirect(uint32_t stackOffset, CPURegister memReg)
{
    SWAG_ASSERT(memReg < R8);
    concat.addU8(0x66);
    concat.addU8(0xFF);
    emit_ModRM(stackOffset, 0, memReg);
}

void X64Gen::emit_Dec16_Indirect(uint32_t stackOffset, CPURegister memReg)
{
    SWAG_ASSERT(memReg < R8);
    concat.addU8(0x66);
    concat.addU8(0xFF);
    emit_ModRM(stackOffset, 1, memReg);
}

void X64Gen::emit_Inc32_Indirect(uint32_t stackOffset, CPURegister memReg)
{
    SWAG_ASSERT(memReg < R8);
    concat.addU8(0xFF);
    emit_ModRM(stackOffset, 0, memReg);
}

void X64Gen::emit_Dec32_Indirect(uint32_t stackOffset, CPURegister memReg)
{
    SWAG_ASSERT(memReg < R8);
    concat.addU8(0xFF);
    emit_ModRM(stackOffset, 1, memReg);
}

void X64Gen::emit_Inc64_Indirect(uint32_t stackOffset, CPURegister memReg)
{
    SWAG_ASSERT(memReg < R8);
    concat.addU8(getREX());
    concat.addU8(0xFF);
    emit_ModRM(stackOffset, 0, memReg);
}

void X64Gen::emit_Dec64_Indirect(uint32_t stackOffset, CPURegister memReg)
{
    SWAG_ASSERT(memReg < R8);
    concat.addU8(getREX());
    concat.addU8(0xFF);
    emit_ModRM(stackOffset, 1, memReg);
}

void X64Gen::emit_Neg32(CPURegister reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(0xF7);
    concat.addU8(0xD8 | (reg & 0b111));
}

void X64Gen::emit_Neg64(CPURegister reg)
{
    SWAG_ASSERT(reg < R8);
    concat.addU8(getREX());
    concat.addU8(0xF7);
    concat.addU8(0xD8 | (reg & 0b111));
}

void X64Gen::emit_Neg32_Indirect(uint32_t stackOffset, CPURegister memReg)
{
    SWAG_ASSERT(memReg == RDI);

    concat.addU8(0xF7);
    if (stackOffset <= 0x7F)
    {
        concat.addU8(0x5F);
        concat.addU8((uint8_t)stackOffset);
    }
    else
    {
        concat.addU8(0x9F);
        concat.addU32(stackOffset);
    }
}

void X64Gen::emit_Neg64_Indirect(uint32_t stackOffset, CPURegister memReg)
{
    SWAG_ASSERT(memReg == RDI);

    concat.addU8(getREX());
    concat.addU8(0xF7);
    if (stackOffset <= 0x7F)
    {
        concat.addU8(0x5F);
        concat.addU8((uint8_t)stackOffset);
    }
    else
    {
        concat.addU8(0x9F);
        concat.addU32(stackOffset);
    }
}
