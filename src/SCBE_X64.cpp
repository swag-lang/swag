#include "pch.h"
#include "SCBE_X64.h"
#include "Math.h"
#include "TypeManager.h"

namespace
{
    uint8_t getREX(bool w, bool r, bool x, bool b)
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

    uint8_t getModRM(uint8_t mod, uint8_t r, uint8_t m)
    {
        return (mod << 6) | ((r & 0b111) << 3) | (m & 0b111);
    }

    void emit_REX(Concat& concat, CPUBits numBits, CPURegister reg1 = RAX, CPURegister reg2 = RAX)
    {
        if (numBits == CPUBits::B16)
            concat.addU8(0x66);
        if (numBits == CPUBits::B64 || reg1 >= R8 || reg2 >= R8)
            concat.addU8(getREX(numBits == CPUBits::B64, reg1 >= R8, false, reg2 >= R8));
    }

    void emit_ModRM(Concat& concat, uint32_t stackOffset, uint8_t reg, uint8_t memReg, uint8_t op = 1)
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

    void emit_Spec8(Concat& concat, uint8_t value, CPUBits numBits)
    {
        if (numBits == CPUBits::B8)
            concat.addU8(value & ~1);
        else
            concat.addU8(value);
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_Symbol_RelocationRef(const Utf8& name)
{
    const auto callSym = getOrAddSymbol(name, CPUSymbolKind::Extern);
    if (callSym->kind == CPUSymbolKind::Function)
    {
        concat.addS32((callSym->value + textSectionOffset) - (concat.totalCount() + 4));
    }
    else
    {
        addSymbolRelocation(concat.totalCount() - textSectionOffset, callSym->index, IMAGE_REL_AMD64_REL32);
        concat.addU32(0);
    }
}

void SCBE_X64::emit_Symbol_RelocationAddr(CPURegister reg, uint32_t symbolIndex, uint32_t offset)
{
    SWAG_ASSERT(reg == RAX || reg == RCX || reg == RDX || reg == R8 || reg == R9 || reg == RDI);
    emit_REX(concat, CPUBits::B64, reg);
    concat.addU8(0x8D);
    concat.addU8(0x05 | ((reg & 0b111) << 3));
    addSymbolRelocation(concat.totalCount() - textSectionOffset, symbolIndex, IMAGE_REL_AMD64_REL32);
    concat.addU32(offset);
}

void SCBE_X64::emit_Symbol_RelocationValue(CPURegister reg, uint32_t symbolIndex, uint32_t offset)
{
    SWAG_ASSERT(reg == RAX || reg == RCX || reg == RDX || reg == R8 || reg == R9);
    emit_REX(concat, CPUBits::B64, reg);
    concat.addU8(0x8B);
    concat.addU8(0x05 | ((reg & 0b111) << 3));
    addSymbolRelocation(concat.totalCount() - textSectionOffset, symbolIndex, IMAGE_REL_AMD64_REL32);
    concat.addU32(offset);
}

void SCBE_X64::emit_Symbol_GlobalString(CPURegister reg, const Utf8& str)
{
    emit_Load64_Immediate(reg, 0, true);
    const auto sym = getOrCreateGlobalString(str);
    addSymbolRelocation((concat.totalCount() - 8) - textSectionOffset, sym->index, IMAGE_REL_AMD64_ADDR64);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_Push(CPURegister reg)
{
    if (reg < R8)
        concat.addU8(0x50 | (reg & 0b111));
    else
    {
        concat.addU8(getREX(false, false, false, true));
        concat.addU8(0x50 | (reg & 0b111));
    }
}

void SCBE_X64::emit_Pop(CPURegister reg)
{
    if (reg < R8)
        concat.addU8(0x58 | (reg & 0b111));
    else
    {
        concat.addU8(getREX(false, false, false, true));
        concat.addU8(0x58 | (reg & 0b111));
    }
}

void SCBE_X64::emit_Ret()
{
    concat.addU8(0xC3);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_LoadS8S16_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emit_REX(concat, CPUBits::B16, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xBE);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emit_LoadS8S32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emit_REX(concat, CPUBits::B32, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xBE);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emit_LoadS16S32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emit_REX(concat, CPUBits::B32, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xBF);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emit_LoadS8S64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emit_REX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xBE);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emit_LoadS16S64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emit_REX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xBF);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emit_LoadS32S64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emit_REX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x63);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emit_LoadU8U32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emit_REX(concat, CPUBits::B32, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emit_LoadU16U32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emit_REX(concat, CPUBits::B32, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xB7);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emit_LoadU8U64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emit_REX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emit_LoadU16U64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emit_REX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xB7);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_Load8_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (storageRegStack == stackOffset &&
        storageReg == reg &&
        storageMemReg == memReg &&
        storageRegBits >= 8 &&
        storageRegCount == concat.totalCount())
    {
        return;
    }

    emit_REX(concat, CPUBits::B8, reg, memReg);
    concat.addU8(0x8A);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emit_Load16_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (storageRegStack == stackOffset &&
        storageReg == reg &&
        storageMemReg == memReg &&
        storageRegBits >= 16 &&
        storageRegCount == concat.totalCount())
    {
        return;
    }

    emit_REX(concat, CPUBits::B16, reg, memReg);
    concat.addU8(0x8B);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emit_Load32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (storageRegStack == stackOffset &&
        storageReg == reg &&
        storageMemReg == memReg &&
        storageRegBits >= 32 &&
        storageRegCount == concat.totalCount())
    {
        if (storageRegBits > 32)
            emit_CopyN(RAX, RAX, CPUBits::B32);
        return;
    }

    emit_REX(concat, CPUBits::B32, reg, memReg);
    concat.addU8(0x8B);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emit_Load64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (storageRegStack == stackOffset &&
        storageReg == reg &&
        storageMemReg == memReg &&
        storageRegBits == 64 &&
        storageRegCount == concat.totalCount())
    {
        return;
    }

    emit_REX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x8B);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emit_LoadN_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg, CPUBits numBits)
{
    switch (numBits)
    {
    case CPUBits::B8:
        emit_Load8_Indirect(stackOffset, reg, memReg);
        break;
    case CPUBits::B16:
        emit_Load16_Indirect(stackOffset, reg, memReg);
        break;
    case CPUBits::B32:
        emit_Load32_Indirect(stackOffset, reg, memReg);
        break;
    case CPUBits::B64:
        emit_Load64_Indirect(stackOffset, reg, memReg);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

void SCBE_X64::emit_LoadF32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF3);
    concat.addU8(0x0F);
    concat.addU8(0x10);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emit_LoadF64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF2);
    concat.addU8(0x0F);
    concat.addU8(0x10);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_LoadAddress_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (memReg == RIP)
    {
        SWAG_ASSERT(stackOffset == 0);
        SWAG_ASSERT(reg == RCX);
        emit_REX(concat, CPUBits::B64, reg, memReg);
        concat.addU8(0x8D);
        concat.addU8(0x0D);
    }
    else if (stackOffset == 0)
    {
        emit_CopyN(reg, memReg, CPUBits::B64);
    }
    else
    {
        emit_REX(concat, CPUBits::B64, reg, memReg);
        concat.addU8(0x8D);
        emit_ModRM(concat, stackOffset, reg, memReg);
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_Store8_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emit_REX(concat, CPUBits::B8, reg, memReg);
    concat.addU8(0x88);
    emit_ModRM(concat, stackOffset, reg, memReg);

    storageRegCount = concat.totalCount();
    storageRegStack = stackOffset;
    storageReg      = reg;
    storageMemReg   = memReg;
    storageRegBits  = 8;
}

void SCBE_X64::emit_Store16_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emit_REX(concat, CPUBits::B16, reg, memReg);
    concat.addU8(0x89);
    emit_ModRM(concat, stackOffset, reg, memReg);

    storageRegCount = concat.totalCount();
    storageRegStack = stackOffset;
    storageReg      = reg;
    storageMemReg   = memReg;
    storageRegBits  = 16;
}

void SCBE_X64::emit_Store32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emit_REX(concat, CPUBits::B32, reg, memReg);
    concat.addU8(0x89);
    emit_ModRM(concat, stackOffset, reg, memReg);

    storageRegCount = concat.totalCount();
    storageRegStack = stackOffset;
    storageReg      = reg;
    storageMemReg   = memReg;
    storageRegBits  = 32;
}

void SCBE_X64::emit_Store64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emit_REX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x89);
    emit_ModRM(concat, stackOffset, reg, memReg);

    storageRegCount = concat.totalCount();
    storageRegStack = stackOffset;
    storageReg      = reg;
    storageMemReg   = memReg;
    storageRegBits  = 64;
}

void SCBE_X64::emit_StoreN_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg, CPUBits numBits)
{
    switch (numBits)
    {
    case CPUBits::B8:
        emit_Store8_Indirect(stackOffset, reg, memReg);
        break;
    case CPUBits::B16:
        emit_Store16_Indirect(stackOffset, reg, memReg);
        break;
    case CPUBits::B32:
        emit_Store32_Indirect(stackOffset, reg, memReg);
        break;
    case CPUBits::B64:
        emit_Store64_Indirect(stackOffset, reg, memReg);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

void SCBE_X64::emit_StoreF32_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF3);
    concat.addU8(0x0F);
    concat.addU8(0x11);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emit_StoreF64_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF2);
    concat.addU8(0x0F);
    concat.addU8(0x11);
    emit_ModRM(concat, stackOffset, reg, memReg);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_Store8_Immediate(uint32_t stackOffset, uint8_t val, CPURegister memReg)
{
    concat.addU8(0xC6);
    emit_ModRM(concat, stackOffset, 0, memReg);
    concat.addU8(val);
}

void SCBE_X64::emit_Store16_Immediate(uint32_t stackOffset, uint16_t val, CPURegister memReg)
{
    concat.addU8(0x66);
    concat.addU8(0xC7);
    emit_ModRM(concat, stackOffset, 0, memReg);
    concat.addU16(val);
}

void SCBE_X64::emit_Store32_Immediate(uint32_t stackOffset, uint32_t val, CPURegister memReg)
{
    concat.addU8(0xC7);
    emit_ModRM(concat, stackOffset, 0, memReg);
    concat.addU32(val);
}

void SCBE_X64::emit_Store64_Immediate(uint32_t stackOffset, uint64_t val, CPURegister memReg)
{
    SWAG_ASSERT(val <= 0xFFFFFFFF);
    emit_REX(concat, CPUBits::B64);
    concat.addU8(0xC7);
    emit_ModRM(concat, stackOffset, 0, memReg);
    concat.addU32((uint32_t) val);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_Load8_Immediate(CPURegister reg, uint8_t value)
{
    if (value == 0)
    {
        emit_ClearN(reg, CPUBits::B8);
        return;
    }

    concat.addU8(0xB0 | reg);
    concat.addU8(value);
}

void SCBE_X64::emit_Load16_Immediate(CPURegister reg, uint16_t value)
{
    if (value == 0)
    {
        emit_ClearN(reg, CPUBits::B16);
        return;
    }

    concat.addU8(0x66);
    concat.addU8(0xB8 | reg);
    concat.addU16(value);
}

void SCBE_X64::emit_Load32_Immediate(CPURegister reg, uint32_t value)
{
    if (value == 0)
    {
        emit_ClearN(reg, CPUBits::B32);
        return;
    }

    concat.addU8(0xB8 | reg);
    concat.addU32(value);
}

void SCBE_X64::emit_Load64_Immediate(CPURegister reg, uint64_t value, bool force64bits)
{
    if (force64bits)
    {
        emit_REX(concat, CPUBits::B64, RAX, reg);
        concat.addU8(0xB8 | reg);
        concat.addU64(value);
        return;
    }

    if (value == 0)
    {
        emit_ClearN(reg, CPUBits::B64);
        return;
    }

    if (value <= 0x7FFFFFFF && reg < R8)
    {
        emit_Load32_Immediate(reg, (uint32_t) value);
        return;
    }

    emit_REX(concat, CPUBits::B64, RAX, reg);
    if (value <= 0x7FFFFFFF)
    {
        concat.addU8(0xC7);
        concat.addU8(0xC0 | (reg & 0b111));
        concat.addU32((uint32_t) value);
    }
    else
    {
        concat.addU8(0xB8 | reg);
        concat.addU64(value);
    }
}

void SCBE_X64::emit_LoadN_Immediate(CPURegister reg, uint64_t value, CPUBits numBits)
{
    switch (numBits)
    {
    case CPUBits::B8:
        emit_Load8_Immediate(reg, (uint8_t) value);
        break;
    case CPUBits::B16:
        emit_Load16_Immediate(reg, (uint16_t) value);
        break;
    case CPUBits::B32:
        emit_Load32_Immediate(reg, (uint32_t) value);
        break;
    case CPUBits::B64:
        emit_Load64_Immediate(reg, value);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_ClearN(CPURegister reg, CPUBits numBits)
{
    emit_REX(concat, numBits, reg, reg);
    emit_Spec8(concat, (uint8_t) CPUOp::XOR, numBits);
    concat.addU8(getModRM(REGREG, reg, reg));
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_CopyN(CPURegister regDst, CPURegister regSrc, CPUBits numBits)
{
    emit_REX(concat, numBits, regSrc, regDst);
    if (numBits == CPUBits::B8)
        concat.addU8(0x88);
    else
        concat.addU8(0x89);
    concat.addU8(getModRM(REGREG, regSrc, regDst));
}

void SCBE_X64::emit_CopyF32(CPURegister regDst, CPURegister regSrc)
{
    SWAG_ASSERT(regSrc == RAX);
    SWAG_ASSERT(regDst == XMM0 || regDst == XMM1 || regDst == XMM2 || regDst == XMM3);
    concat.addU8(0x66);
    concat.addU8(0x0F);
    concat.addU8(0x6E);
    concat.addU8(0xC0 | (regDst << 3));
}

void SCBE_X64::emit_CopyF64(CPURegister regDst, CPURegister regSrc)
{
    SWAG_ASSERT(regSrc == RAX);
    SWAG_ASSERT(regDst == XMM0 || regDst == XMM1 || regDst == XMM2 || regDst == XMM3);
    concat.addU8(0x66);
    emit_REX(concat, CPUBits::B64);
    concat.addU8(0x0F);
    concat.addU8(0x6E);
    concat.addU8(0xC0 | (regDst << 3));
}

void SCBE_X64::emit_CopyDownUp(CPURegister reg, CPUBits numBits)
{
    SWAG_ASSERT(reg == RAX);
    SWAG_ASSERT(numBits == CPUBits::B8);
    concat.addString2("\x88\xe0"); // mov al, ah
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_SetA(CPURegister reg)
{
    SWAG_ASSERT(reg == RAX || reg == R8);
    if (reg >= R8)
        concat.addU8(0x41);
    concat.addU8(0x0F);
    concat.addU8(0x97);
    concat.addU8(0xC0 | (reg & 0b111));
}

void SCBE_X64::emit_SetAE(CPURegister reg)
{
    SWAG_ASSERT(reg == RAX || reg == R8);
    if (reg >= R8)
        concat.addU8(0x41);
    concat.addU8(0x0F);
    concat.addU8(0x93);
    concat.addU8(0xC0 | (reg & 0b111));
}

void SCBE_X64::emit_SetG(CPURegister reg)
{
    SWAG_ASSERT(reg == RAX || reg == R8);
    if (reg >= R8)
        concat.addU8(0x41);
    concat.addU8(0x0F);
    concat.addU8(0x9F);
    concat.addU8(0xC0 | (reg & 0b111));
}

void SCBE_X64::emit_SetNE()
{
    concat.addU8(0x0F);
    concat.addU8(0x95);
    concat.addU8(0xC0);
}

void SCBE_X64::emit_SetNA()
{
    concat.addU8(0x0F);
    concat.addU8(0x96);
    concat.addU8(0xC0);
}

void SCBE_X64::emit_SetB()
{
    concat.addU8(0x0F);
    concat.addU8(0x92);
    concat.addU8(0xC0);
}

void SCBE_X64::emit_SetBE()
{
    concat.addU8(0x0F);
    concat.addU8(0x96);
    concat.addU8(0xC0);
}

void SCBE_X64::emit_SetE()
{
    concat.addU8(0x0F);
    concat.addU8(0x94);
    concat.addU8(0xC0);
}

void SCBE_X64::emit_SetEP()
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

void SCBE_X64::emit_SetNEP()
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

void SCBE_X64::emit_SetGE()
{
    concat.addU8(0x0F);
    concat.addU8(0x9D);
    concat.addU8(0xC0);
}

void SCBE_X64::emit_SetL()
{
    concat.addU8(0x0F);
    concat.addU8(0x9C);
    concat.addU8(0xC0);
}

void SCBE_X64::emit_SetLE()
{
    concat.addU8(0x0F);
    concat.addU8(0x9E);
    concat.addU8(0xC0);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_TestN(CPURegister regDst, CPURegister regSrc, CPUBits numBits)
{
    emit_REX(concat, numBits, regDst, regSrc);
    if (numBits == CPUBits::B8)
        concat.addU8(0x84);
    else
        concat.addU8(0x85);
    concat.addU8(getModRM(REGREG, regDst, regSrc));
}

void SCBE_X64::emit_CmpN(CPURegister regSrc, CPURegister regDst, CPUBits numBits)
{
    emit_REX(concat, numBits, regDst, regSrc);
    emit_Spec8(concat, 0x39, numBits);
    concat.addU8(getModRM(REGREG, regDst, regSrc));
}

void SCBE_X64::emit_CmpF32(CPURegister regSrc, CPURegister regDst)
{
    SWAG_ASSERT(regSrc < R8 && regDst < R8);
    concat.addU8(0x0F);
    concat.addU8(0x2F);
    concat.addU8(getModRM(REGREG, regSrc, regDst));
}

void SCBE_X64::emit_CmpF64(CPURegister regSrc, CPURegister regDst)
{
    SWAG_ASSERT(regSrc < R8 && regDst < R8);
    concat.addU8(0x66);
    concat.addU8(0x0F);
    concat.addU8(0x2F);
    concat.addU8(getModRM(REGREG, regSrc, regDst));
}

void SCBE_X64::emit_CmpN_Immediate(CPURegister reg, uint64_t value, CPUBits numBits)
{
    if (value <= 0x7f)
    {
        SWAG_ASSERT(reg == RAX || reg == RCX);
        emit_REX(concat, numBits);
        concat.addU8(0x83);
        concat.addU8(0xF8 | reg);
        concat.addU8((uint8_t) value);
    }
    else if (value <= 0x7fffffff)
    {
        SWAG_ASSERT(reg == RAX);
        emit_REX(concat, numBits);
        concat.addU8(0x3d);
        concat.addU32((uint32_t) value);
    }
    else
    {
        SWAG_ASSERT(reg == RAX);
        emit_LoadN_Immediate(RCX, value, numBits);
        emit_CmpN(reg, RCX, numBits);
    }
}

void SCBE_X64::emit_CmpN_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUBits numBits)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    emit_REX(concat, numBits);
    emit_Spec8(concat, 0x3B, numBits);
    emit_ModRM(concat, offsetStack, reg, memReg);
}

void SCBE_X64::emit_CmpF32_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x0F);
    concat.addU8(0x2E);
    emit_ModRM(concat, offsetStack, reg, memReg);
}

void SCBE_X64::emit_CmpF64_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x66);
    concat.addU8(0x0F);
    concat.addU8(0x2F);
    emit_ModRM(concat, offsetStack, reg, memReg);
}

void SCBE_X64::emit_CmpN_IndirectDst(uint32_t offsetStack, uint32_t value, CPUBits numBits)
{
    emit_REX(concat, numBits);
    if (numBits == CPUBits::B8)
    {
        concat.addU8(0x80);
        emit_ModRM(concat, offsetStack, RDI, RDI);
        concat.addU8((uint8_t) value);
    }
    else if (value <= 0x7F)
    {
        concat.addU8(0x83);
        emit_ModRM(concat, offsetStack, RDI, RDI);
        concat.addU8((uint8_t) value);
    }
    else
    {
        concat.addU8(0x81);
        emit_ModRM(concat, offsetStack, RDI, RDI);
        if (numBits == CPUBits::B16)
            concat.addU16((uint16_t) value);
        else
            concat.addU32(value);
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_OpN(CPURegister regSrc, CPURegister regDst, CPUOp op, CPUBits numBits)
{
    if (op == CPUOp::DIV ||
        op == CPUOp::IDIV ||
        op == CPUOp::MOD ||
        op == CPUOp::IMOD)
    {
        emit_REX(concat, numBits, regSrc, regDst);
        emit_Spec8(concat, 0xF7, numBits);
        concat.addU8((uint8_t) op & ~2);

        if (op == CPUOp::MOD || op == CPUOp::IMOD)
        {
            // modulo in 8 bits stores the reminder in AH and not RDX
            if (numBits == CPUBits::B8)
                concat.addString2("\x88\xE0"); // mov al, ah
            else
                concat.addString3("\x48\x89\xd0"); // mov rax, rdx
        }
    }
    else if (op == CPUOp::MUL ||
             op == CPUOp::IMUL)
    {
        emit_REX(concat, numBits, regSrc, regDst);
        if (op == CPUOp::MUL && regSrc == RCX && regDst == RAX)
        {
            if (numBits == CPUBits::B8 || numBits == CPUBits::B16)
                concat.addU8(0xF6);
            else
                concat.addU8(0xF7);
            concat.addU8(0xE1);
        }
        else if (op == CPUOp::IMUL && regSrc == RCX && regDst == RAX)
        {
            if (numBits == CPUBits::B8 || numBits == CPUBits::B16)
                concat.addString2("\xF6\xE9");
            else
                concat.addString3("\x0F\xAF\xC1");
        }
        else
        {
            SWAG_ASSERT(regSrc == RAX && regDst == RCX);
            emit_Spec8(concat, 0xF7, numBits);
            concat.addU8(op == CPUOp::IMUL ? 0xE9 : 0xE1);
        }
    }
    else if (op == CPUOp::SAR ||
             op == CPUOp::SAL ||
             op == CPUOp::SHR ||
             op == CPUOp::SHL)
    {
        SWAG_ASSERT(regDst == RAX && regSrc == RCX);
        emit_REX(concat, numBits, regSrc, regDst);
        emit_Spec8(concat, 0xD3, numBits);
        concat.addU8((uint8_t) op);
    }
    else if (op == CPUOp::ADD ||
             op == CPUOp::SUB ||
             op == CPUOp::XOR ||
             op == CPUOp::AND ||
             op == CPUOp::OR)
    {
        emit_REX(concat, numBits, regSrc, regDst);
        emit_Spec8(concat, (uint8_t) op, numBits);
        concat.addU8(getModRM(REGREG, regSrc, regDst));
    }
    else if (op == CPUOp::BSF ||
             op == CPUOp::BSR)
    {
        SWAG_ASSERT(regSrc == RAX && regDst == RAX);
        emit_REX(concat, numBits, regSrc, regDst);
        concat.addU8(0x0F);
        concat.addU8((uint8_t) op);
        concat.addU8(0xC0);
    }
    else if (op == CPUOp::POPCNT)
    {
        SWAG_ASSERT(regSrc == RAX && regDst == RAX);
        if (numBits == CPUBits::B16)
            emit_REX(concat, numBits, regSrc, regDst);
        concat.addU8(0xF3);
        if (numBits == CPUBits::B64)
            emit_REX(concat, numBits, regSrc, regDst);
        concat.addU8(0x0F);
        concat.addU8((uint8_t) op);
        concat.addU8(0xC0);
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void SCBE_X64::emit_OpF32(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits srcBits)
{
    if (op != CPUOp::FSQRT &&
        op != CPUOp::FAND &&
        op != CPUOp::UCOMIF &&
        op != CPUOp::FXOR)
    {
        concat.addU8(0xF3);
        emit_REX(concat, srcBits, regSrc);
    }

    concat.addU8(0x0F);
    concat.addU8((uint8_t) op);
    concat.addU8(0xC0 | regSrc | regDst << 3);
}

void SCBE_X64::emit_OpF64(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits srcBits)
{
    if (op != CPUOp::FSQRT &&
        op != CPUOp::FAND &&
        op != CPUOp::UCOMIF &&
        op != CPUOp::FXOR)
    {
        concat.addU8(0xF2);
        emit_REX(concat, srcBits, regSrc);
    }
    else
    {
        concat.addU8(0x66);
    }

    concat.addU8(0x0F);
    concat.addU8((uint8_t) op);
    concat.addU8(0xC0 | regSrc | regDst << 3);
}

void SCBE_X64::emit_OpN(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUOp op, CPUBits numBits)
{
    emit_REX(concat, numBits);
    if (op == CPUOp::MUL)
    {
        emit_Spec8(concat, 0xF7, numBits);
        emit_ModRM(concat, offsetStack, 4, RDI);
    }
    else if (op == CPUOp::IMUL)
    {
        if (numBits == CPUBits::B8)
        {
            concat.addU8(0xF6);
            emit_ModRM(concat, offsetStack, 5, RDI);
        }
        else if (numBits == CPUBits::B16)
        {
            concat.addU8(0xF7);
            emit_ModRM(concat, offsetStack, 5, RDI);
        }
        else
        {
            concat.addString2("\x0F\xAF");
            emit_ModRM(concat, offsetStack, RAX, RDI);
        }
    }
    else
    {
        if (numBits == CPUBits::B8)
            concat.addU8((uint8_t) op + 1);
        else
            concat.addU8((uint8_t) op | 2);
        emit_ModRM(concat, offsetStack, RAX, RDI);
    }
}

void SCBE_X64::emit_OpF32(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUOp op)
{
    concat.addU8(0xF3);
    concat.addU8(0x0F);
    concat.addU8((uint8_t) op);
    emit_ModRM(concat, offsetStack, XMM0, RDI);
}

void SCBE_X64::emit_OpF64(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUOp op)
{
    concat.addU8(0xF2);
    concat.addU8(0x0F);
    concat.addU8((uint8_t) op);
    emit_ModRM(concat, offsetStack, XMM0, RDI);
}

void SCBE_X64::emit_OpN_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUOp op, CPUBits numBits, bool lock)
{
    SWAG_ASSERT(memReg < R8);
    if (lock)
        concat.addU8(0xF0);

    emit_REX(concat, numBits, reg);
    if (op == CPUOp::DIV ||
        op == CPUOp::IDIV ||
        op == CPUOp::MOD ||
        op == CPUOp::IMOD)
    {
        SWAG_ASSERT(reg == RAX);

        emit_Spec8(concat, 0xF7, numBits);

        if (offsetStack == 0)
        {
            concat.addU8(((uint8_t) op & ~2) - 0xBA);
        }
        else if (offsetStack <= 0x7F)
        {
            concat.addU8(((uint8_t) op & ~2) - 0x7A);
            concat.addU8((uint8_t) offsetStack);
        }
        else
        {
            concat.addU8(((uint8_t) op & ~2) - 0x3A);
            concat.addU32(offsetStack);
        }

        if (op == CPUOp::MOD || op == CPUOp::IMOD)
        {
            // modulo in 8 bits stores the reminder in AH and not RDX
            if (numBits == CPUBits::B8)
                concat.addString2("\x88\xE0"); // mov al, ah
            else
                concat.addString3("\x48\x89\xd0"); // mov rax, rdx
        }
    }
    else
    {
        emit_Spec8(concat, (uint8_t) op, numBits);
        emit_ModRM(concat, offsetStack, reg, memReg);
    }
}

void SCBE_X64::emit_OpF32_Indirect(CPURegister reg, CPURegister memReg, CPUOp op)
{
    SWAG_ASSERT(reg == XMM1);
    SWAG_ASSERT(memReg < R8);
    emit_LoadF32_Indirect(0, XMM0, memReg);
    concat.addU8(0xF3);
    concat.addU8(0x0F);
    concat.addU8((uint8_t) op);
    concat.addU8(0xC1);
    emit_StoreF32_Indirect(0, XMM0, memReg);
}

void SCBE_X64::emit_OpF64_Indirect(CPURegister reg, CPURegister memReg, CPUOp op)
{
    SWAG_ASSERT(reg == XMM1);
    SWAG_ASSERT(memReg < R8);
    emit_LoadF64_Indirect(0, XMM0, memReg);
    concat.addU8(0xF2);
    concat.addU8(0x0F);
    concat.addU8((uint8_t) op);
    concat.addU8(0xC1);
    emit_StoreF64_Indirect(0, XMM0, memReg);
}

void SCBE_X64::emit_OpN_IndirectDst(CPURegister regSrc, CPURegister regDst, CPUOp op, CPUBits numBits)
{
    emit_REX(concat, numBits, regSrc, regDst);
    if (op == CPUOp::SAR ||
        op == CPUOp::SHR ||
        op == CPUOp::SHL)
    {
        SWAG_ASSERT(regDst == RAX && regSrc == RCX);
        if (numBits == CPUBits::B8)
            concat.addU8(0xD2);
        else
            concat.addU8(0xD3);
        concat.addU8(((uint8_t) op) & ~0xC0);
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void SCBE_X64::emit_OpN_IndirectDst(CPURegister reg, uint64_t value, CPUOp op, CPUBits numBits)
{
    if (op == CPUOp::SAR ||
        op == CPUOp::SHR ||
        op == CPUOp::SHL)
    {
        SWAG_ASSERT(reg == RAX);
        value = min(value, (uint32_t) numBits - 1);

        emit_REX(concat, numBits);
        if (value == 1)
        {
            emit_Spec8(concat, 0xD1, numBits);
            concat.addU8((uint8_t) op & ~0xC0);
        }
        else
        {
            emit_Spec8(concat, 0xC1, numBits);
            concat.addU8((uint8_t) op & ~0xC0);
            concat.addU8((uint8_t) value);
        }
    }
    else if (op == CPUOp::AND || op == CPUOp::OR || op == CPUOp::XOR || op == CPUOp::ADD || op == CPUOp::SUB)
    {
        emit_REX(concat, numBits);
        if (value <= 0x7F)
        {
            if (numBits == CPUBits::B8)
            {
                concat.addU8((uint8_t) op + 3);
            }
            else
            {
                concat.addU8(0x83);
                concat.addU8(0xBF + (uint8_t) op);
            }

            concat.addU8((uint8_t) value);
        }
        else
        {
            SWAG_ASSERT(value <= 0x7FFFFFFF);
            switch (numBits)
            {
            case CPUBits::B8:
                concat.addU8((uint8_t) op + 3);
                concat.addU8((uint8_t) value);
                break;
            case CPUBits::B16:
                concat.addU8(0x81);
                concat.addU8(0xBF + (uint8_t) op);
                concat.addU16((uint16_t) value);
                break;
            case CPUBits::B32:
            case CPUBits::B64:
                concat.addU8(0x81);
                concat.addU8(0xBF + (uint8_t) op);
                concat.addU32((uint32_t) value);
                break;
            }
        }
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void SCBE_X64::emit_OpN_Immediate(CPURegister reg, uint64_t value, CPUOp op, CPUBits numBits)
{
    switch (op)
    {
    case CPUOp::ADD:
        SWAG_ASSERT(numBits == CPUBits::B64);
        SWAG_ASSERT(reg == RAX || reg == RCX || reg == RSP);
        break;

    case CPUOp::SUB:
        SWAG_ASSERT(numBits == CPUBits::B64);
        SWAG_ASSERT(reg == RAX || reg == RCX || reg == RSP);
        break;

    case CPUOp::IMUL:
        SWAG_ASSERT(numBits == CPUBits::B64);
        SWAG_ASSERT(reg == RAX || reg == RCX);
        break;

    case CPUOp::XOR:
        SWAG_ASSERT(reg == RAX);
        break;

    case CPUOp::SAR:
    case CPUOp::SHR:
    case CPUOp::SHL:
        SWAG_ASSERT(reg == RAX || reg == RCX);
        SWAG_ASSERT(value <= 0x7F);
        break;

    case CPUOp::BT:
        SWAG_ASSERT(value <= 0x7F);
        break;

    default:
        SWAG_ASSERT(false);
        break;
    }

    if (value <= 0x7F)
    {
        switch (op)
        {
        case CPUOp::ADD:
            if (value == 0)
                return;
            emit_REX(concat, numBits);
            if (value == 1)
            {
                SWAG_ASSERT(reg != RSP);
                concat.addU8(0xFF);
                concat.addU8(0xC0 | reg); // inc rax
            }
            else
            {
                concat.addU8(0x83);
                concat.addU8(0xC0 | reg);
                concat.addU8((uint8_t) value);
            }
            break;

        case CPUOp::SUB:
            if (value == 0)
                return;
            emit_REX(concat, numBits);
            if (value == 1)
            {
                SWAG_ASSERT(reg != RSP);
                concat.addU8(0xFF);
                concat.addU8(0xC8 | reg); // dec rax
            }
            else
            {
                concat.addU8(0x83);
                concat.addU8(0xE8 | reg);
                concat.addU8((uint8_t) value);
            }
            break;

        case CPUOp::IMUL:
            if (value == 1)
                return;
            if (value == 0)
            {
                emit_ClearN(reg, numBits);
                return;
            }

            emit_REX(concat, numBits);
            if (value == 2)
            {
                concat.addU8(0xD1);
                concat.addU8(0xE0 | reg); // shl rax, 1
            }
            else if (Math::isPowerOfTwo(value))
            {
                concat.addU8(0xC1);
                concat.addU8(0xE0 | reg); // shl rax, ??
                concat.addU8((uint8_t) log2(value));
            }
            else
            {
                SWAG_ASSERT(reg == RAX);
                concat.addU8(0x6B);
                concat.addU8(0xC0);
                concat.addU8((uint8_t) value);
            }
            break;

        case CPUOp::XOR:
            emit_REX(concat, numBits);
            if (numBits == CPUBits::B8)
                concat.addU8(0x34);
            else
                concat.addString("\x83\xF0");
            concat.addU8((uint8_t) value);
            break;

        case CPUOp::SAR:
        case CPUOp::SHR:
        case CPUOp::SHL:
            emit_REX(concat, numBits);
            value = min(value, (uint32_t) numBits - 1);
            if (value == 1)
            {
                emit_Spec8(concat, 0xD1, numBits);
                concat.addU8((uint8_t) op | reg);
            }
            else
            {
                emit_Spec8(concat, 0xC1, numBits);
                concat.addU8((uint8_t) op | reg);
                concat.addU8((uint8_t) value);
            }
            break;

        case CPUOp::BT:
            emit_REX(concat, numBits, RAX, reg);
            concat.addU8(0x0F);
            concat.addU8((uint8_t) op);
            concat.addU8(0xE2);
            concat.addU8((uint8_t) value);
            break;

        default:
            SWAG_ASSERT(false);
            break;
        }
    }
    else if (value > 0x7FFFFFFF)
    {
        if (op == CPUOp::IMUL)
        {
            SWAG_ASSERT(reg == RAX);
            emit_Load64_Immediate(RCX, value);
            emit_OpN(RCX, reg, op, numBits);
        }
        else
        {
            SWAG_ASSERT(reg != RSP);
            emit_Load64_Immediate(R8, value);
            emit_OpN(R8, reg, op, numBits);
        }
    }
    else
    {
        emit_REX(concat, numBits);
        switch (op)
        {
        case CPUOp::ADD:
            if (reg == RAX)
                concat.addU8(0x05);
            else
            {
                concat.addU8(0x81);
                concat.addU8(0xC0 | reg);
            }
            break;
        case CPUOp::SUB:
            if (reg == RAX)
                concat.addU8(0x2D);
            else
            {
                concat.addU8(0x81);
                concat.addU8(0xE8 | reg);
            }
            break;
        case CPUOp::IMUL:
            SWAG_ASSERT(reg == RAX);
            concat.addU8(0x69);
            concat.addU8(0xC0);
            break;
        default:
            SWAG_ASSERT(false);
            break;
        }

        concat.addU32((uint32_t) value);
    }
}

void SCBE_X64::emit_OpN_IndirectDst(uint32_t offsetStack, uint64_t value, CPURegister memReg, CPUOp op, CPUBits numBits)
{
    SWAG_ASSERT(memReg == RAX || memReg == RDI);

    if (value > 0x7FFFFFFF)
    {
        emit_LoadN_Immediate(RCX, value, numBits);
        emit_OpN_Indirect(offsetStack, RCX, memReg, op, numBits);
        return;
    }

    emit_REX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0x80);
    else if (value <= 0x7F)
        concat.addU8(0x83);
    else
        concat.addU8(0x81);
    if (offsetStack == 0)
    {
        concat.addU8((uint8_t) op - 1 + memReg);
    }
    else if (offsetStack <= 0x7F)
    {
        concat.addU8(0x3F + memReg + (uint8_t) op);
        concat.addU8((uint8_t) offsetStack);
    }
    else
    {
        concat.addU8(0x7F + memReg + (uint8_t) op);
        concat.addU32(offsetStack);
    }

    if (value <= 0x7F || numBits == CPUBits::B8)
        concat.addU8((uint8_t) value);
    else if (numBits == CPUBits::B16)
        concat.addU16((uint16_t) value);
    else
        concat.addU32((uint32_t) value);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_Extend_U8U64(CPURegister regDst, CPURegister regSrc)
{
    emit_REX(concat, CPUBits::B64, regDst, regSrc);
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    concat.addU8(getModRM(REGREG, regDst, regSrc));
}

void SCBE_X64::emit_Extend_U16U64(CPURegister regDst, CPURegister regSrc)
{
    emit_REX(concat, CPUBits::B64, regDst, regSrc);
    concat.addU8(0x0F);
    concat.addU8(0xB7);
    concat.addU8(getModRM(REGREG, regDst, regSrc));
}

/////////////////////////////////////////////////////////////////////
uint8_t* SCBE_X64::emit_NearJumpOp(CPUJumpType jumpType)
{
    switch (jumpType)
    {
    case JNO:
        concat.addU8(0x71);
        break;
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

    concat.addU8(0);
    return concat.getSeekPtr() - 1;
}

uint32_t* SCBE_X64::emit_LongJumpOp(CPUJumpType jumpType)
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

    concat.addU32(0);
    return reinterpret_cast<uint32_t*>(concat.getSeekPtr()) - 1;
}

void SCBE_X64::emit_JumpTable(CPURegister table, CPURegister offset)
{
    SWAG_ASSERT(table == RCX && offset == RAX);
    emit_REX(concat, CPUBits::B64);
    concat.addString3("\x63\x0C\x81"); // movsx rcx, dword ptr [rcx + rax*4]
}

void SCBE_X64::emit_Jump(CPUJumpType jumpType, int32_t instructionCount, int32_t jumpOffset)
{
    CPULabelToSolve label;
    label.ipDest = jumpOffset + instructionCount + 1;

    // Can we solve the label now ?
    const auto it = labels.find(label.ipDest);
    if (it != labels.end())
    {
        const auto currentOffset = (int32_t) concat.totalCount() + 1;
        const int  relOffset     = it->second - (currentOffset + 1);
        if (relOffset >= -127 && relOffset <= 128)
        {
            const auto offsetPtr = emit_NearJumpOp(jumpType);
            *offsetPtr           = (uint8_t) (it->second - concat.totalCount());
        }
        else
        {
            const auto offsetPtr = emit_LongJumpOp(jumpType);
            *offsetPtr           = it->second - concat.totalCount();
        }

        return;
    }

    // Here we do not know the destination label, so we assume 32 bits of offset
    label.patch         = (uint8_t*) emit_LongJumpOp(jumpType);
    label.currentOffset = (int32_t) concat.totalCount();
    labelsToSolve.push_back(label);
}

void SCBE_X64::emit_Jump(CPURegister reg)
{
    SWAG_ASSERT(reg == RAX);
    concat.addString2("\xFF\xE0"); // jmp rax
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_CopyX(CPURegister regDst, CPURegister regSrc, uint32_t count, uint32_t offset)
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

void SCBE_X64::emit_ClearX(uint32_t count, uint32_t offset, CPURegister reg)
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

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_Call_Far(const Utf8& symbolName)
{
    concat.addU8(0xFF);
    concat.addU8(0x15);
    const auto callSym = getOrAddSymbol(symbolName, CPUSymbolKind::Extern);
    addSymbolRelocation(concat.totalCount() - textSectionOffset, callSym->index, IMAGE_REL_AMD64_REL32);
    concat.addU32(0);
}

void SCBE_X64::emit_Call(const Utf8& symbolName)
{
    concat.addU8(0xE8);

    const auto callSym = getOrAddSymbol(symbolName, CPUSymbolKind::Extern);
    if (callSym->kind == CPUSymbolKind::Function)
    {
        concat.addS32((callSym->value + textSectionOffset) - (concat.totalCount() + 4));
    }
    else
    {
        addSymbolRelocation(concat.totalCount() - textSectionOffset, callSym->index, IMAGE_REL_AMD64_REL32);
        concat.addU32(0);
    }
}

void SCBE_X64::emit_Call_Parameters(TypeInfoFuncAttr* typeFuncBC, VectorNative<CPUPushParam>& paramsRegisters, VectorNative<TypeInfo*>& paramsTypes, void* retCopyAddr)
{
    const auto& cc                = typeFuncBC->getCallConv();
    const bool  returnByStackAddr = CallConv::returnByStackAddress(typeFuncBC);

    const int callConvRegisters    = cc.paramByRegisterCount;
    const int maxParamsPerRegister = (int) paramsRegisters.size();

    // Set the first N parameters. Can be return register, or function parameter.
    int i = 0;
    for (; i < min(callConvRegisters, maxParamsPerRegister); i++)
    {
        auto       type = paramsTypes[i];
        const auto reg  = (uint32_t) paramsRegisters[i].reg;

        if (type->isAutoConstPointerRef())
            type = TypeManager::concretePtrRef(type);

        // This is a return register
        if (type == g_TypeMgr->typeInfoUndefined)
        {
            SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
            if (retCopyAddr)
                emit_Load64_Immediate(cc.paramByRegisterInteger[i], (uint64_t) retCopyAddr);
            else if (returnByStackAddr)
                emit_Load64_Indirect(reg, cc.paramByRegisterInteger[i], RDI);
            else
                emit_LoadAddress_Indirect(reg, cc.paramByRegisterInteger[i], RDI);
        }

        // This is a normal parameter, which can be float or integer
        else
        {
            // Pass struct in a register if small enough
            if (CallConv::structParamByValue(typeFuncBC, type))
            {
                SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                emit_Load64_Indirect(REG_OFFSET(reg), RAX);
                emit_Load64_Indirect(0, cc.paramByRegisterInteger[i], RAX);
            }
            else if (cc.useRegisterFloat && type->isNative(NativeTypeKind::F32))
            {
                if (paramsRegisters[i].type == CPUPushParamType::Imm)
                {
                    SWAG_ASSERT(paramsRegisters[i].reg <= UINT32_MAX);
                    emit_Load32_Immediate(RAX, (uint32_t) paramsRegisters[i].reg);
                    emit_CopyF32(cc.paramByRegisterFloat[i], RAX);
                }
                else
                {
                    SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                    emit_LoadF32_Indirect(REG_OFFSET(reg), cc.paramByRegisterFloat[i]);
                }
            }
            else if (cc.useRegisterFloat && type->isNative(NativeTypeKind::F64))
            {
                if (paramsRegisters[i].type == CPUPushParamType::Imm)
                {
                    emit_Load64_Immediate(RAX, paramsRegisters[i].reg);
                    emit_CopyF64(cc.paramByRegisterFloat[i], RAX);
                }
                else
                {
                    SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                    emit_LoadF64_Indirect(REG_OFFSET(reg), cc.paramByRegisterFloat[i]);
                }
            }
            else
            {
                switch (paramsRegisters[i].type)
                {
                case CPUPushParamType::Imm:
                    if (paramsRegisters[i].reg == 0)
                        emit_ClearN(cc.paramByRegisterInteger[i], CPUBits::B64);
                    else
                        emit_Load64_Immediate(cc.paramByRegisterInteger[i], paramsRegisters[i].reg);
                    break;
                case CPUPushParamType::Imm64:
                    emit_Load64_Immediate(cc.paramByRegisterInteger[i], paramsRegisters[i].reg, true);
                    break;
                case CPUPushParamType::RelocV:
                    emit_Symbol_RelocationValue(cc.paramByRegisterInteger[i], (uint32_t) paramsRegisters[i].reg, 0);
                    break;
                case CPUPushParamType::RelocAddr:
                    emit_Symbol_RelocationAddr(cc.paramByRegisterInteger[i], (uint32_t) paramsRegisters[i].reg, 0);
                    break;
                case CPUPushParamType::Addr:
                    emit_LoadAddress_Indirect((uint32_t) paramsRegisters[i].reg, cc.paramByRegisterInteger[i], RDI);
                    break;
                case CPUPushParamType::RegAdd:
                    emit_Load64_Indirect(REG_OFFSET(reg), cc.paramByRegisterInteger[i]);
                    emit_OpN_Immediate(cc.paramByRegisterInteger[i], paramsRegisters[i].val, CPUOp::ADD, CPUBits::B64);
                    break;
                case CPUPushParamType::RegMul:
                    emit_Load64_Indirect(REG_OFFSET(reg), RAX);
                    emit_OpN_Immediate(RAX, paramsRegisters[i].val, CPUOp::IMUL, CPUBits::B64);
                    emit_CopyN(cc.paramByRegisterInteger[i], RAX, CPUBits::B64);
                    break;
                case CPUPushParamType::RAX:
                    emit_CopyN(cc.paramByRegisterInteger[i], RAX, CPUBits::B64);
                    break;
                case CPUPushParamType::GlobalString:
                    emit_Symbol_GlobalString(cc.paramByRegisterInteger[i], (const char*) paramsRegisters[i].reg);
                    break;
                default:
                    SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                    emit_Load64_Indirect(REG_OFFSET(reg), cc.paramByRegisterInteger[i]);
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

        const auto reg = (uint32_t) paramsRegisters[i].reg;
        SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);

        // This is a C variadic parameter
        if (i >= maxParamsPerRegister)
        {
            emit_Load64_Indirect(REG_OFFSET(reg), RAX);
            emit_Store64_Indirect(offsetStack, RAX, RSP);
        }

        // This is for a return value
        else if (type == g_TypeMgr->typeInfoUndefined)
        {
            // r is an address to registerRR, for FFI
            if (retCopyAddr)
                emit_Load64_Immediate(RAX, (uint64_t) retCopyAddr);
            else if (returnByStackAddr)
                emit_Load64_Indirect(reg, RAX, RDI);
            else
                emit_LoadAddress_Indirect(reg, RAX, RDI);
            emit_Store64_Indirect(offsetStack, RAX, RSP);
        }

        // This is for a normal parameter
        else
        {
            const auto sizeOf = type->sizeOf;

            // Struct by copy. Will be a pointer to the stack
            if (type->isStruct())
            {
                emit_Load64_Indirect(REG_OFFSET(reg), RAX);

                // Store the content of the struct in the stack
                if (CallConv::structParamByValue(typeFuncBC, type))
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
                    default:
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
                    emit_Load8_Indirect(REG_OFFSET(reg), RAX);
                    emit_Store8_Indirect(offsetStack, RAX, RSP);
                    break;
                case 2:
                    emit_Load16_Indirect(REG_OFFSET(reg), RAX);
                    emit_Store16_Indirect(offsetStack, RAX, RSP);
                    break;
                case 4:
                    emit_Load32_Indirect(REG_OFFSET(reg), RAX);
                    emit_Store32_Indirect(offsetStack, RAX, RSP);
                    break;
                case 8:
                    emit_Load64_Indirect(REG_OFFSET(reg), RAX);
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

void SCBE_X64::emit_Call_Parameters(TypeInfoFuncAttr* typeFunc, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, void* retCopyAddr)
{
    pushParams2.clear();
    for (const auto r : pushRAParams)
        pushParams2.push_back({CPUPushParamType::Reg, r});
    emit_Call_Parameters(typeFunc, pushParams2, offsetRT, retCopyAddr);
}

void SCBE_X64::emit_Call_Parameters(TypeInfoFuncAttr* typeFunc, const VectorNative<CPUPushParam>& pushRAParams, uint32_t offsetRT, void* retCopyAddr)
{
    int numCallParams = (int) typeFunc->parameters.size();
    pushParams3.clear();
    pushParamsTypes.clear();

    int indexParam = (int) pushRAParams.size() - 1;

    // Variadic are first
    if (typeFunc->isFctVariadic())
    {
        auto index = pushRAParams[indexParam--];
        pushParams3.push_back(index);
        pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);

        index = pushRAParams[indexParam--];
        pushParams3.push_back(index);
        pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
        numCallParams--;
    }
    else if (typeFunc->isFctCVariadic())
    {
        numCallParams--;
    }

    // All parameters
    for (int i = 0; i < numCallParams; i++)
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
    if (CallConv::returnByAddress(typeFunc))
    {
        pushParams3.push_back({CPUPushParamType::Reg, offsetRT});
        pushParamsTypes.push_back(g_TypeMgr->typeInfoUndefined);
    }

    // Add all C variadic parameters
    if (typeFunc->isFctCVariadic())
    {
        for (size_t i = typeFunc->numParamsRegisters(); i < pushRAParams.size(); i++)
        {
            auto index = pushRAParams[indexParam--];
            pushParams3.push_back(index);
            pushParamsTypes.push_back(g_TypeMgr->typeInfoU64);
        }
    }

    // If a lambda is assigned to a closure, then we must not use the first parameter (the first
    // parameter is the capture context, which does not exist in a normal lambda function).
    // But as this is dynamic, we need to have two call path : one for the closure (normal call), and
    // one for the lambda (omit first parameter)
    if (typeFunc->isClosure())
    {
        SWAG_ASSERT(pushParams3[0].type == CPUPushParamType::Reg);

        // First register is closure context, except if variadic, where we have 2 registers for the slice first
        // :VariadicAndClosure
        uint32_t reg = (uint32_t) pushParams3[0].reg;
        if (typeFunc->isFctVariadic())
            reg = (uint32_t) pushParams3[2].reg;

        emit_Load64_Indirect(REG_OFFSET(reg), RAX);
        emit_TestN(RAX, RAX, CPUBits::B64);

        // If not zero, jump to closure call
        const auto seekPtrClosure = emit_LongJumpOp(JZ);
        const auto seekJmpClosure = concat.totalCount();

        emit_Call_Parameters(typeFunc, pushParams3, pushParamsTypes, retCopyAddr);

        // Jump to after closure call
        const auto seekPtrAfterClosure = emit_LongJumpOp(JUMP);
        const auto seekJmpAfterClosure = concat.totalCount();

        // Update jump to closure call
        *seekPtrClosure = (uint8_t) (concat.totalCount() - seekJmpClosure);

        // First register is closure context, except if variadic, where we have 2 registers for the slice first
        // :VariadicAndClosure
        if (typeFunc->isFctVariadic())
        {
            pushParams3.erase(2);
            pushParamsTypes.erase(2);
        }
        else
        {
            pushParams3.erase(0);
            pushParamsTypes.erase(0);
        }
        emit_Call_Parameters(typeFunc, pushParams3, pushParamsTypes, retCopyAddr);

        *seekPtrAfterClosure = (uint8_t) (concat.totalCount() - seekJmpAfterClosure);
    }
    else
    {
        emit_Call_Parameters(typeFunc, pushParams3, pushParamsTypes, retCopyAddr);
    }
}

void SCBE_X64::emit_Call_Result(TypeInfoFuncAttr* typeFunc, uint32_t offsetRT)
{
    if (CallConv::returnByValue(typeFunc))
    {
        const auto& cc         = typeFunc->getCallConv();
        const auto  returnType = typeFunc->concreteReturnType();
        if (returnType->isNativeFloat())
            emit_StoreF64_Indirect(offsetRT, cc.returnByRegisterFloat, RDI);
        else
            emit_Store64_Indirect(offsetRT, cc.returnByRegisterInteger, RDI);
    }
}

void SCBE_X64::emit_Call_Indirect(CPURegister reg)
{
    SWAG_ASSERT(reg == RAX || reg == RCX || reg == R10);
    if (reg == R10)
        concat.addU8(0x41);
    concat.addU8(0xFF);
    concat.addU8(0xD0 | (reg & 0b111));
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_Cwd()
{
    emit_REX(concat, CPUBits::B16);
    concat.addU8(0x99);
}

void SCBE_X64::emit_Cdq()
{
    concat.addU8(0x99);
}

void SCBE_X64::emit_Cqo()
{
    emit_REX(concat, CPUBits::B64);
    concat.addU8(0x99);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_NotN(CPURegister reg, CPUBits numBits)
{
    SWAG_ASSERT(reg < R8);

    emit_REX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0xF6);
    else
        concat.addU8(0xF7);
    concat.addU8(0xD0 | (reg & 0b111));
}

void SCBE_X64::emit_NotN_Indirect(uint32_t stackOffset, CPURegister memReg, CPUBits numBits)
{
    SWAG_ASSERT(memReg == RDI);

    emit_REX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0xF6);
    else
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

void SCBE_X64::emit_IncN_Indirect(uint32_t stackOffset, CPURegister memReg, CPUBits numBits)
{
    SWAG_ASSERT(memReg < R8);
    emit_REX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0xFE);
    else
        concat.addU8(0xFF);
    emit_ModRM(concat, stackOffset, 0, memReg);
}

void SCBE_X64::emit_DecN_Indirect(uint32_t stackOffset, CPURegister memReg, CPUBits numBits)
{
    SWAG_ASSERT(memReg < R8);
    emit_REX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0xFE);
    else
        concat.addU8(0xFF);
    emit_ModRM(concat, stackOffset, 1, memReg);
}

void SCBE_X64::emit_NegN(CPURegister reg, CPUBits numBits)
{
    SWAG_ASSERT(reg < R8);
    SWAG_ASSERT(numBits == CPUBits::B32 || numBits == CPUBits::B64);

    emit_REX(concat, numBits);
    concat.addU8(0xF7);
    concat.addU8(0xD8 | (reg & 0b111));
}

void SCBE_X64::emit_NegN_Indirect(uint32_t stackOffset, CPURegister memReg, CPUBits numBits)
{
    SWAG_ASSERT(memReg == RDI);
    SWAG_ASSERT(numBits == CPUBits::B32 || numBits == CPUBits::B64);

    emit_REX(concat, numBits);
    concat.addU8(0xF7);
    if (stackOffset <= 0x7F)
    {
        concat.addU8(0x5F);
        concat.addU8((uint8_t) stackOffset);
    }
    else
    {
        concat.addU8(0x9F);
        concat.addU32(stackOffset);
    }
}

void SCBE_X64::emit_CMovN(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits)
{
    if (numBits < CPUBits::B32)
        numBits = CPUBits::B32;
    emit_REX(concat, numBits, regDst, regSrc);
    concat.addU8(0x0F);
    concat.addU8((uint8_t) op);
    concat.addU8(getModRM(REGREG, regDst, regSrc));
}

void SCBE_X64::emit_RotateN(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits)
{
    SWAG_ASSERT(numBits >= CPUBits::B32);
    SWAG_ASSERT(regDst == RAX && regSrc == RCX);
    emit_REX(concat, numBits, regDst, regSrc);
    concat.addU8(0xD3);
    concat.addU8((uint8_t) op);
}

void SCBE_X64::emit_CmpXChg(CPURegister regDst, CPURegister regSrc, CPUBits numBits)
{
    SWAG_ASSERT(regDst == RCX && regSrc == RDX);

    // lock CMPXCHG [rcx], dl
    if (numBits == CPUBits::B16)
        emit_REX(concat, numBits);
    concat.addU8(0xF0);
    if (numBits == CPUBits::B64)
        emit_REX(concat, numBits);
    concat.addU8(0x0F);
    emit_Spec8(concat, 0xB1, numBits);
    concat.addU8(0x11);
}

void SCBE_X64::emit_BSwapN(CPURegister reg, CPUBits numBits)
{
    SWAG_ASSERT(reg == RAX);
    SWAG_ASSERT(numBits == CPUBits::B16 || numBits == CPUBits::B32 || numBits == CPUBits::B64);

    emit_REX(concat, numBits);
    if (numBits == CPUBits::B16)
    {
        concat.addU8(0xC1);
        concat.addU8(0xC0);
        concat.addU8(8);
    }
    else
    {
        concat.addU8(0x0F);
        concat.addU8(0xC8);
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emit_Nop()
{
    concat.addU8(0x90);
}

void SCBE_X64::emit_CastU64F64(CPURegister regDst, CPURegister regSrc)
{
    SWAG_ASSERT(regSrc == RAX && regDst == XMM0);
    concat.addString5("\x66\x48\x0F\x6E\xC8"); // movq xmm1, rax
    emit_Symbol_RelocationAddr(RCX, symCst_U64F64, 0);
    concat.addString4("\x66\x0F\x62\x09");     // punpckldq xmm1, xmmword ptr [rcx]
    concat.addString5("\x66\x0F\x5C\x49\x10"); // subpd xmm1, xmmword ptr [rcx + 16]
    concat.addString4("\x66\x0F\x28\xC1");     // movapd xmm0, xmm1
    concat.addString4("\x66\x0F\x15\xC1");     // unpckhpd xmm0, xmm1
    concat.addString4("\xF2\x0F\x58\xC1");     // addsd xmm0, xmm1
}
