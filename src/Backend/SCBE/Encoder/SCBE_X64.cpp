// ReSharper disable CommentTypo
#include "pch.h"
#include "Backend/SCBE/Encoder/SCBE_X64.h"
#include "Core/Math.h"
#include "Semantic/Type/TypeManager.h"

namespace
{
    uint8_t getREX(bool w, bool r, bool x, bool b)
    {
        uint8_t rex = 0x40;
        if (w) // 64 bits
            rex |= 8;
        if (r) // extended MODRM.reg
            rex |= 4;
        if (x) // extended SIB.index
            rex |= 2;
        if (b) // extended MODRM.rm
            rex |= 1;
        return rex;
    }

    uint8_t getModRM(uint8_t mod, uint8_t r, uint8_t m)
    {
        return static_cast<uint8_t>(mod << 6 | ((r & 0b111) << 3) | (m & 0b111));
    }

    void emitREX(Concat& concat, CPUBits numBits, CPURegister reg1 = RAX, CPURegister reg2 = RAX)
    {
        if (numBits == CPUBits::B16)
            concat.addU8(0x66);
        if (numBits == CPUBits::B64 || reg1 >= R8 || reg2 >= R8)
            concat.addU8(getREX(numBits == CPUBits::B64, reg1 >= R8, false, reg2 >= R8));
    }

    void emitModRM(Concat& concat, uint32_t stackOffset, uint8_t reg, uint8_t memReg, uint8_t op = 1)
    {
        if (stackOffset == 0 && (memReg < R8 || memReg == R12))
        {
            // mov al, byte ptr [rdi]
            concat.addU8(getModRM(0, reg, memReg) | op - 1);
            if (memReg == RSP || memReg == R12)
                concat.addU8(0x24);
        }
        else if (stackOffset <= 0x7F)
        {
            // mov al, byte ptr [rdi + ??]
            concat.addU8(getModRM(Disp8, reg, memReg) | op - 1);
            if (memReg == RSP || memReg == R12)
                concat.addU8(0x24);
            concat.addU8(static_cast<uint8_t>(stackOffset));
        }
        else
        {
            // mov al, byte ptr [rdi + ????????]
            concat.addU8(getModRM(Disp32, reg, memReg) | op - 1);
            if (memReg == RSP || memReg == R12)
                concat.addU8(0x24);
            concat.addU32(stackOffset);
        }
    }

    void emitSpec8(Concat& concat, uint8_t value, CPUBits numBits)
    {
        if (numBits == CPUBits::B8)
            concat.addU8(value & ~1);
        else
            concat.addU8(value);
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitSymbolRelocationRef(const Utf8& name)
{
    const auto callSym = getOrAddSymbol(name, CPUSymbolKind::Extern);
    if (callSym->kind == CPUSymbolKind::Function)
    {
        concat.addS32(static_cast<int32_t>(callSym->value + textSectionOffset - (concat.totalCount() + 4)));
    }
    else
    {
        addSymbolRelocation(concat.totalCount() - textSectionOffset, callSym->index, IMAGE_REL_AMD64_REL32);
        concat.addU32(0);
    }
}

void SCBE_X64::emitSymbolRelocationAddr(CPURegister reg, uint32_t symbolIndex, uint32_t offset)
{
    SWAG_ASSERT(reg == RAX || reg == RCX || reg == RDX || reg == R8 || reg == R9 || reg == RDI);
    emitREX(concat, CPUBits::B64, reg);
    concat.addU8(0x8D);
    concat.addU8(static_cast<uint8_t>(0x05 | ((reg & 0b111)) << 3));
    addSymbolRelocation(concat.totalCount() - textSectionOffset, symbolIndex, IMAGE_REL_AMD64_REL32);
    concat.addU32(offset);
}

void SCBE_X64::emitSymbolRelocationValue(CPURegister reg, uint32_t symbolIndex, uint32_t offset)
{
    SWAG_ASSERT(reg == RAX || reg == RCX || reg == RDX || reg == R8 || reg == R9);
    emitREX(concat, CPUBits::B64, reg);
    concat.addU8(0x8B);
    concat.addU8(static_cast<uint8_t>(0x05 | ((reg & 0b111)) << 3));
    addSymbolRelocation(concat.totalCount() - textSectionOffset, symbolIndex, IMAGE_REL_AMD64_REL32);
    concat.addU32(offset);
}

void SCBE_X64::emitSymbolGlobalString(CPURegister reg, const Utf8& str)
{
    emitLoad64Immediate(reg, 0, true);
    const auto sym = getOrCreateGlobalString(str);
    addSymbolRelocation(concat.totalCount() - 8 - textSectionOffset, sym->index, IMAGE_REL_AMD64_ADDR64);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitPush(CPURegister reg)
{
    if (reg < R8)
        concat.addU8(0x50 | (reg & 0b111));
    else
    {
        concat.addU8(getREX(false, false, false, true));
        concat.addU8(0x50 | (reg & 0b111));
    }
}

void SCBE_X64::emitPop(CPURegister reg)
{
    if (reg < R8)
        concat.addU8(0x58 | (reg & 0b111));
    else
    {
        concat.addU8(getREX(false, false, false, true));
        concat.addU8(0x58 | (reg & 0b111));
    }
}

void SCBE_X64::emitRet()
{
    concat.addU8(0xC3);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitLoadS8S16Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emitREX(concat, CPUBits::B16, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xBE);
    emitModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emitLoadS8S32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emitREX(concat, CPUBits::B32, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xBE);
    emitModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emitLoadS16S32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emitREX(concat, CPUBits::B32, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xBF);
    emitModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emitLoadS8S64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emitREX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xBE);
    emitModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emitLoadS16S64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emitREX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xBF);
    emitModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emitLoadS32S64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emitREX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x63);
    emitModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emitLoadU8U32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emitREX(concat, CPUBits::B32, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    emitModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emitLoadU16U32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emitREX(concat, CPUBits::B32, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xB7);
    emitModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emitLoadU8U64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emitREX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    emitModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emitLoadU16U64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emitREX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xB7);
    emitModRM(concat, stackOffset, reg, memReg);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitLoad8Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (storageRegStack == stackOffset &&
        storageReg == reg &&
        storageMemReg == memReg &&
        storageRegBits >= 8 &&
        storageRegCount == concat.totalCount())
    {
        return;
    }

    emitREX(concat, CPUBits::B8, reg, memReg);
    concat.addU8(0x8A);
    emitModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emitLoad16Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (storageRegStack == stackOffset &&
        storageReg == reg &&
        storageMemReg == memReg &&
        storageRegBits >= 16 &&
        storageRegCount == concat.totalCount())
    {
        return;
    }

    emitREX(concat, CPUBits::B16, reg, memReg);
    concat.addU8(0x8B);
    emitModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emitLoad32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (storageRegStack == stackOffset &&
        storageReg == reg &&
        storageMemReg == memReg &&
        storageRegBits >= 32 &&
        storageRegCount == concat.totalCount())
    {
        if (storageRegBits > 32)
            emitCopyN(RAX, RAX, CPUBits::B32);
        return;
    }

    emitREX(concat, CPUBits::B32, reg, memReg);
    concat.addU8(0x8B);
    emitModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emitLoad64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (storageRegStack == stackOffset &&
        storageReg == reg &&
        storageMemReg == memReg &&
        storageRegBits == 64 &&
        storageRegCount == concat.totalCount())
    {
        return;
    }

    emitREX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x8B);
    emitModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emitLoadNIndirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg, CPUBits numBits)
{
    switch (numBits)
    {
        case CPUBits::B8:
            emitLoad8Indirect(stackOffset, reg, memReg);
            break;
        case CPUBits::B16:
            emitLoad16Indirect(stackOffset, reg, memReg);
            break;
        case CPUBits::B32:
            emitLoad32Indirect(stackOffset, reg, memReg);
            break;
        case CPUBits::B64:
            emitLoad64Indirect(stackOffset, reg, memReg);
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }
}

void SCBE_X64::emitLoadF32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF3);
    concat.addU8(0x0F);
    concat.addU8(0x10);
    emitModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emitLoadF64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF2);
    concat.addU8(0x0F);
    concat.addU8(0x10);
    emitModRM(concat, stackOffset, reg, memReg);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitLoadAddressIndirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    if (memReg == RIP)
    {
        SWAG_ASSERT(stackOffset == 0);
        SWAG_ASSERT(reg == RCX);
        emitREX(concat, CPUBits::B64, reg, memReg);
        concat.addU8(0x8D);
        concat.addU8(0x0D);
    }
    else if (stackOffset == 0)
    {
        emitCopyN(reg, memReg, CPUBits::B64);
    }
    else
    {
        emitREX(concat, CPUBits::B64, reg, memReg);
        concat.addU8(0x8D);
        emitModRM(concat, stackOffset, reg, memReg);
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitStore8Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emitREX(concat, CPUBits::B8, reg, memReg);
    concat.addU8(0x88);
    emitModRM(concat, stackOffset, reg, memReg);

    storageRegCount = concat.totalCount();
    storageRegStack = stackOffset;
    storageReg      = reg;
    storageMemReg   = memReg;
    storageRegBits  = 8;
}

void SCBE_X64::emitStore16Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emitREX(concat, CPUBits::B16, reg, memReg);
    concat.addU8(0x89);
    emitModRM(concat, stackOffset, reg, memReg);

    storageRegCount = concat.totalCount();
    storageRegStack = stackOffset;
    storageReg      = reg;
    storageMemReg   = memReg;
    storageRegBits  = 16;
}

void SCBE_X64::emitStore32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emitREX(concat, CPUBits::B32, reg, memReg);
    concat.addU8(0x89);
    emitModRM(concat, stackOffset, reg, memReg);

    storageRegCount = concat.totalCount();
    storageRegStack = stackOffset;
    storageReg      = reg;
    storageMemReg   = memReg;
    storageRegBits  = 32;
}

void SCBE_X64::emitStore64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    emitREX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x89);
    emitModRM(concat, stackOffset, reg, memReg);

    storageRegCount = concat.totalCount();
    storageRegStack = stackOffset;
    storageReg      = reg;
    storageMemReg   = memReg;
    storageRegBits  = 64;
}

void SCBE_X64::emitStoreNIndirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg, CPUBits numBits)
{
    switch (numBits)
    {
        case CPUBits::B8:
            emitStore8Indirect(stackOffset, reg, memReg);
            break;
        case CPUBits::B16:
            emitStore16Indirect(stackOffset, reg, memReg);
            break;
        case CPUBits::B32:
            emitStore32Indirect(stackOffset, reg, memReg);
            break;
        case CPUBits::B64:
            emitStore64Indirect(stackOffset, reg, memReg);
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }
}

void SCBE_X64::emitStoreF32Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF3);
    concat.addU8(0x0F);
    concat.addU8(0x11);
    emitModRM(concat, stackOffset, reg, memReg);
}

void SCBE_X64::emitStoreF64Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0xF2);
    concat.addU8(0x0F);
    concat.addU8(0x11);
    emitModRM(concat, stackOffset, reg, memReg);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitStore8Immediate(uint32_t stackOffset, uint8_t val, CPURegister memReg)
{
    concat.addU8(0xC6);
    emitModRM(concat, stackOffset, 0, memReg);
    concat.addU8(val);
}

void SCBE_X64::emitStore16Immediate(uint32_t stackOffset, uint16_t val, CPURegister memReg)
{
    concat.addU8(0x66);
    concat.addU8(0xC7);
    emitModRM(concat, stackOffset, 0, memReg);
    concat.addU16(val);
}

void SCBE_X64::emitStore32Immediate(uint32_t stackOffset, uint32_t val, CPURegister memReg)
{
    concat.addU8(0xC7);
    emitModRM(concat, stackOffset, 0, memReg);
    concat.addU32(val);
}

void SCBE_X64::emitStore64Immediate(uint32_t stackOffset, uint64_t val, CPURegister memReg)
{
    SWAG_ASSERT(val <= 0xFFFFFFFF);
    emitREX(concat, CPUBits::B64);
    concat.addU8(0xC7);
    emitModRM(concat, stackOffset, 0, memReg);
    concat.addU32(static_cast<uint32_t>(val));
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitLoad8Immediate(CPURegister reg, uint8_t value)
{
    if (value == 0)
    {
        emitClearN(reg, CPUBits::B8);
        return;
    }

    concat.addU8(0xB0 | reg);
    concat.addU8(value);
}

void SCBE_X64::emitLoad16Immediate(CPURegister reg, uint16_t value)
{
    if (value == 0)
    {
        emitClearN(reg, CPUBits::B16);
        return;
    }

    concat.addU8(0x66);
    concat.addU8(0xB8 | reg);
    concat.addU16(value);
}

void SCBE_X64::emitLoad32Immediate(CPURegister reg, uint32_t value)
{
    if (value == 0)
    {
        emitClearN(reg, CPUBits::B32);
        return;
    }

    concat.addU8(0xB8 | reg);
    concat.addU32(value);
}

void SCBE_X64::emitLoad64Immediate(CPURegister reg, uint64_t value, bool force64Bits)
{
    if (force64Bits)
    {
        emitREX(concat, CPUBits::B64, RAX, reg);
        concat.addU8(0xB8 | reg);
        concat.addU64(value);
        return;
    }

    if (value == 0)
    {
        emitClearN(reg, CPUBits::B64);
        return;
    }

    if (value <= 0x7FFFFFFF && reg < R8)
    {
        emitLoad32Immediate(reg, static_cast<uint32_t>(value));
        return;
    }

    emitREX(concat, CPUBits::B64, RAX, reg);
    if (value <= 0x7FFFFFFF)
    {
        concat.addU8(0xC7);
        concat.addU8(0xC0 | (reg & 0b111));
        concat.addU32(static_cast<uint32_t>(value));
    }
    else
    {
        concat.addU8(0xB8 | reg);
        concat.addU64(value);
    }
}

void SCBE_X64::emitLoadNImmediate(CPURegister reg, uint64_t value, CPUBits numBits)
{
    switch (numBits)
    {
        case CPUBits::B8:
            emitLoad8Immediate(reg, static_cast<uint8_t>(value));
            break;
        case CPUBits::B16:
            emitLoad16Immediate(reg, static_cast<uint16_t>(value));
            break;
        case CPUBits::B32:
            emitLoad32Immediate(reg, static_cast<uint32_t>(value));
            break;
        case CPUBits::B64:
            emitLoad64Immediate(reg, value);
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitClearN(CPURegister reg, CPUBits numBits)
{
    emitREX(concat, numBits, reg, reg);
    emitSpec8(concat, static_cast<uint8_t>(CPUOp::XOR), numBits);
    concat.addU8(getModRM(RegReg, reg, reg));
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitCopyN(CPURegister regDst, CPURegister regSrc, CPUBits numBits)
{
    emitREX(concat, numBits, regSrc, regDst);
    if (numBits == CPUBits::B8)
        concat.addU8(0x88);
    else
        concat.addU8(0x89);
    concat.addU8(getModRM(RegReg, regSrc, regDst));
}

void SCBE_X64::emitCopyF32(CPURegister regDst, [[maybe_unused]] CPURegister regSrc)
{
    SWAG_ASSERT(regSrc == RAX);
    SWAG_ASSERT(regDst == XMM0 || regDst == XMM1 || regDst == XMM2 || regDst == XMM3);
    concat.addU8(0x66);
    concat.addU8(0x0F);
    concat.addU8(0x6E);
    concat.addU8(static_cast<uint8_t>(0xC0 | regDst << 3));
}

void SCBE_X64::emitCopyF64(CPURegister regDst, [[maybe_unused]] CPURegister regSrc)
{
    SWAG_ASSERT(regSrc == RAX);
    SWAG_ASSERT(regDst == XMM0 || regDst == XMM1 || regDst == XMM2 || regDst == XMM3);
    concat.addU8(0x66);
    emitREX(concat, CPUBits::B64);
    concat.addU8(0x0F);
    concat.addU8(0x6E);
    concat.addU8(static_cast<uint8_t>(0xC0 | regDst << 3));
}

void SCBE_X64::emitCopyDownUp([[maybe_unused]] CPURegister reg, [[maybe_unused]] CPUBits numBits)
{
    SWAG_ASSERT(reg == RAX);
    SWAG_ASSERT(numBits == CPUBits::B8);
    concat.addString2("\x88\xe0"); // mov al, ah
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitSetA(CPURegister reg)
{
    SWAG_ASSERT(reg == RAX || reg == R8);
    if (reg >= R8)
        concat.addU8(0x41);
    concat.addU8(0x0F);
    concat.addU8(0x97);
    concat.addU8(0xC0 | (reg & 0b111));
}

void SCBE_X64::emitSetAE(CPURegister reg)
{
    SWAG_ASSERT(reg == RAX || reg == R8);
    if (reg >= R8)
        concat.addU8(0x41);
    concat.addU8(0x0F);
    concat.addU8(0x93);
    concat.addU8(0xC0 | (reg & 0b111));
}

void SCBE_X64::emitSetG(CPURegister reg)
{
    SWAG_ASSERT(reg == RAX || reg == R8);
    if (reg >= R8)
        concat.addU8(0x41);
    concat.addU8(0x0F);
    concat.addU8(0x9F);
    concat.addU8(0xC0 | (reg & 0b111));
}

void SCBE_X64::emitSetNE()
{
    concat.addU8(0x0F);
    concat.addU8(0x95);
    concat.addU8(0xC0);
}

void SCBE_X64::emitSetNA()
{
    concat.addU8(0x0F);
    concat.addU8(0x96);
    concat.addU8(0xC0);
}

void SCBE_X64::emitSetB()
{
    concat.addU8(0x0F);
    concat.addU8(0x92);
    concat.addU8(0xC0);
}

void SCBE_X64::emitSetBE()
{
    concat.addU8(0x0F);
    concat.addU8(0x96);
    concat.addU8(0xC0);
}

void SCBE_X64::emitSetE()
{
    concat.addU8(0x0F);
    concat.addU8(0x94);
    concat.addU8(0xC0);
}

void SCBE_X64::emitSetEP()
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

void SCBE_X64::emitSetNEP()
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

void SCBE_X64::emitSetGE()
{
    concat.addU8(0x0F);
    concat.addU8(0x9D);
    concat.addU8(0xC0);
}

void SCBE_X64::emitSetL()
{
    concat.addU8(0x0F);
    concat.addU8(0x9C);
    concat.addU8(0xC0);
}

void SCBE_X64::emitSetLE()
{
    concat.addU8(0x0F);
    concat.addU8(0x9E);
    concat.addU8(0xC0);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitTestN(CPURegister regDst, CPURegister regSrc, CPUBits numBits)
{
    emitREX(concat, numBits, regDst, regSrc);
    if (numBits == CPUBits::B8)
        concat.addU8(0x84);
    else
        concat.addU8(0x85);
    concat.addU8(getModRM(RegReg, regDst, regSrc));
}

void SCBE_X64::emitCmpN(CPURegister regSrc, CPURegister regDst, CPUBits numBits)
{
    emitREX(concat, numBits, regDst, regSrc);
    emitSpec8(concat, 0x39, numBits);
    concat.addU8(getModRM(RegReg, regDst, regSrc));
}

void SCBE_X64::emitCmpF32(CPURegister regSrc, CPURegister regDst)
{
    SWAG_ASSERT(regSrc < R8 && regDst < R8);
    concat.addU8(0x0F);
    concat.addU8(0x2F);
    concat.addU8(getModRM(RegReg, regSrc, regDst));
}

void SCBE_X64::emitCmpF64(CPURegister regSrc, CPURegister regDst)
{
    SWAG_ASSERT(regSrc < R8 && regDst < R8);
    concat.addU8(0x66);
    concat.addU8(0x0F);
    concat.addU8(0x2F);
    concat.addU8(getModRM(RegReg, regSrc, regDst));
}

void SCBE_X64::emitCmpNImmediate(CPURegister reg, uint64_t value, CPUBits numBits)
{
    if (value <= 0x7f)
    {
        SWAG_ASSERT(reg == RAX || reg == RCX);
        emitREX(concat, numBits);
        concat.addU8(0x83);
        concat.addU8(0xF8 | reg);
        concat.addU8(static_cast<uint8_t>(value));
    }
    else if (value <= 0x7fffffff)
    {
        SWAG_ASSERT(reg == RAX);
        emitREX(concat, numBits);
        concat.addU8(0x3d);
        concat.addU32(static_cast<uint32_t>(value));
    }
    else
    {
        SWAG_ASSERT(reg == RAX);
        emitLoadNImmediate(RCX, value, numBits);
        emitCmpN(reg, RCX, numBits);
    }
}

void SCBE_X64::emitCmpNIndirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUBits numBits)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    emitREX(concat, numBits);
    emitSpec8(concat, 0x3B, numBits);
    emitModRM(concat, offsetStack, reg, memReg);
}

void SCBE_X64::emitCmpF32Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x0F);
    concat.addU8(0x2E);
    emitModRM(concat, offsetStack, reg, memReg);
}

void SCBE_X64::emitCmpF64Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x66);
    concat.addU8(0x0F);
    concat.addU8(0x2F);
    emitModRM(concat, offsetStack, reg, memReg);
}

void SCBE_X64::emitCmpNIndirectDst(uint32_t offsetStack, uint32_t value, CPUBits numBits)
{
    emitREX(concat, numBits);
    if (numBits == CPUBits::B8)
    {
        concat.addU8(0x80);
        emitModRM(concat, offsetStack, RDI, RDI);
        concat.addU8(static_cast<uint8_t>(value));
    }
    else if (value <= 0x7F)
    {
        concat.addU8(0x83);
        emitModRM(concat, offsetStack, RDI, RDI);
        concat.addU8(static_cast<uint8_t>(value));
    }
    else
    {
        concat.addU8(0x81);
        emitModRM(concat, offsetStack, RDI, RDI);
        if (numBits == CPUBits::B16)
            concat.addU16(static_cast<uint16_t>(value));
        else
            concat.addU32(value);
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitOpN(CPURegister regSrc, CPURegister regDst, CPUOp op, CPUBits numBits)
{
    if (op == CPUOp::DIV ||
        op == CPUOp::IDIV ||
        op == CPUOp::MOD ||
        op == CPUOp::IMOD)
    {
        emitREX(concat, numBits, regSrc, regDst);
        emitSpec8(concat, 0xF7, numBits);
        concat.addU8(static_cast<uint8_t>(op) & ~2);

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
        emitREX(concat, numBits, regSrc, regDst);
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
            emitSpec8(concat, 0xF7, numBits);
            concat.addU8(op == CPUOp::IMUL ? 0xE9 : 0xE1);
        }
    }
    else if (op == CPUOp::SAR ||
             op == CPUOp::SAL ||
             op == CPUOp::SHR ||
             op == CPUOp::SHL)
    {
        SWAG_ASSERT(regDst == RAX && regSrc == RCX);
        emitREX(concat, numBits, regSrc, regDst);
        emitSpec8(concat, 0xD3, numBits);
        concat.addU8(static_cast<uint8_t>(op));
    }
    else if (op == CPUOp::ADD ||
             op == CPUOp::SUB ||
             op == CPUOp::XOR ||
             op == CPUOp::AND ||
             op == CPUOp::OR)
    {
        emitREX(concat, numBits, regSrc, regDst);
        emitSpec8(concat, static_cast<uint8_t>(op), numBits);
        concat.addU8(getModRM(RegReg, regSrc, regDst));
    }
    else if (op == CPUOp::BSF ||
             op == CPUOp::BSR)
    {
        SWAG_ASSERT(regSrc == RAX && regDst == RAX);
        emitREX(concat, numBits, regSrc, regDst);
        concat.addU8(0x0F);
        concat.addU8(static_cast<uint8_t>(op));
        concat.addU8(0xC0);
    }
    else if (op == CPUOp::POPCNT)
    {
        SWAG_ASSERT(regSrc == RAX && regDst == RAX);
        if (numBits == CPUBits::B16)
            emitREX(concat, numBits, regSrc, regDst);
        concat.addU8(0xF3);
        if (numBits == CPUBits::B64)
            emitREX(concat, numBits, regSrc, regDst);
        concat.addU8(0x0F);
        concat.addU8(static_cast<uint8_t>(op));
        concat.addU8(0xC0);
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void SCBE_X64::emitOpF32(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits srcBits)
{
    if (op != CPUOp::FSQRT &&
        op != CPUOp::FAND &&
        op != CPUOp::UCOMIF &&
        op != CPUOp::FXOR)
    {
        concat.addU8(0xF3);
        emitREX(concat, srcBits, regSrc);
    }

    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(op));
    concat.addU8(static_cast<uint8_t>(0xC0 | regSrc | regDst << 3));
}

void SCBE_X64::emitOpF64(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits srcBits)
{
    if (op != CPUOp::FSQRT &&
        op != CPUOp::FAND &&
        op != CPUOp::UCOMIF &&
        op != CPUOp::FXOR)
    {
        concat.addU8(0xF2);
        emitREX(concat, srcBits, regSrc);
    }
    else
    {
        concat.addU8(0x66);
    }

    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(op));
    concat.addU8(static_cast<uint8_t>(0xC0 | regSrc | regDst << 3));
}

void SCBE_X64::emitOpN(uint32_t offsetStack, CPUOp op, CPUBits numBits)
{
    emitREX(concat, numBits);
    if (op == CPUOp::MUL)
    {
        emitSpec8(concat, 0xF7, numBits);
        emitModRM(concat, offsetStack, 4, RDI);
    }
    else if (op == CPUOp::IMUL)
    {
        if (numBits == CPUBits::B8)
        {
            concat.addU8(0xF6);
            emitModRM(concat, offsetStack, 5, RDI);
        }
        else if (numBits == CPUBits::B16)
        {
            concat.addU8(0xF7);
            emitModRM(concat, offsetStack, 5, RDI);
        }
        else
        {
            concat.addString2("\x0F\xAF");
            emitModRM(concat, offsetStack, RAX, RDI);
        }
    }
    else
    {
        if (numBits == CPUBits::B8)
            concat.addU8(static_cast<uint8_t>(op) + 1);
        else
            concat.addU8(static_cast<uint8_t>(op) | 2);
        emitModRM(concat, offsetStack, RAX, RDI);
    }
}

void SCBE_X64::emitOpF32(uint32_t offsetStack, CPUOp op)
{
    concat.addU8(0xF3);
    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(op));
    emitModRM(concat, offsetStack, XMM0, RDI);
}

void SCBE_X64::emitOpF64(uint32_t offsetStack, CPUOp op)
{
    concat.addU8(0xF2);
    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(op));
    emitModRM(concat, offsetStack, XMM0, RDI);
}

void SCBE_X64::emitOpNIndirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, CPUOp op, CPUBits numBits, bool lock)
{
    SWAG_ASSERT(memReg < R8);
    if (lock)
        concat.addU8(0xF0);

    emitREX(concat, numBits, reg);
    if (op == CPUOp::DIV ||
        op == CPUOp::IDIV ||
        op == CPUOp::MOD ||
        op == CPUOp::IMOD)
    {
        SWAG_ASSERT(reg == RAX);

        emitSpec8(concat, 0xF7, numBits);

        if (offsetStack == 0)
        {
            concat.addU8((static_cast<uint8_t>(op) & ~2) - 0xBA);
        }
        else if (offsetStack <= 0x7F)
        {
            concat.addU8((static_cast<uint8_t>(op) & ~2) - 0x7A);
            concat.addU8(static_cast<uint8_t>(offsetStack));
        }
        else
        {
            concat.addU8((static_cast<uint8_t>(op) & ~2) - 0x3A);
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
        emitSpec8(concat, static_cast<uint8_t>(op), numBits);
        emitModRM(concat, offsetStack, reg, memReg);
    }
}

void SCBE_X64::emitOpF32Indirect(uint32_t offsetStack, [[maybe_unused]] CPURegister reg, CPURegister memReg, CPUOp op)
{
    SWAG_ASSERT(reg == XMM1);
    SWAG_ASSERT(memReg < R8);
    emitLoadF32Indirect(offsetStack, XMM0, memReg);
    concat.addU8(0xF3);
    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(op));
    concat.addU8(0xC1);
    emitStoreF32Indirect(offsetStack, XMM0, memReg);
}

void SCBE_X64::emitOpF64Indirect(uint32_t offsetStack, [[maybe_unused]] CPURegister reg, CPURegister memReg, CPUOp op)
{
    SWAG_ASSERT(reg == XMM1);
    SWAG_ASSERT(memReg < R8);
    emitLoadF64Indirect(offsetStack, XMM0, memReg);
    concat.addU8(0xF2);
    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(op));
    concat.addU8(0xC1);
    emitStoreF64Indirect(offsetStack, XMM0, memReg);
}

void SCBE_X64::emitOpNIndirectDst(CPURegister regSrc, CPURegister regDst, CPUOp op, CPUBits numBits)
{
    emitREX(concat, numBits, regSrc, regDst);
    if (op == CPUOp::SAR ||
        op == CPUOp::SHR ||
        op == CPUOp::SHL)
    {
        SWAG_ASSERT(regDst == RAX && regSrc == RCX);
        if (numBits == CPUBits::B8)
            concat.addU8(0xD2);
        else
            concat.addU8(0xD3);
        concat.addU8(static_cast<uint8_t>(op) & ~0xC0);
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void SCBE_X64::emitOpNIndirectDst([[maybe_unused]] CPURegister reg, uint64_t value, CPUOp op, CPUBits numBits)
{
    if (op == CPUOp::SAR ||
        op == CPUOp::SHR ||
        op == CPUOp::SHL)
    {
        SWAG_ASSERT(reg == RAX);
        value = min(value, static_cast<uint32_t>(numBits) - 1);

        emitREX(concat, numBits);
        if (value == 1)
        {
            emitSpec8(concat, 0xD1, numBits);
            concat.addU8(static_cast<uint8_t>(op) & ~0xC0);
        }
        else
        {
            emitSpec8(concat, 0xC1, numBits);
            concat.addU8(static_cast<uint8_t>(op) & ~0xC0);
            concat.addU8(static_cast<uint8_t>(value));
        }
    }
    else if (op == CPUOp::AND || op == CPUOp::OR || op == CPUOp::XOR || op == CPUOp::ADD || op == CPUOp::SUB)
    {
        emitREX(concat, numBits);
        if (value <= 0x7F)
        {
            if (numBits == CPUBits::B8)
            {
                concat.addU8(static_cast<uint8_t>(op) + 3);
            }
            else
            {
                concat.addU8(0x83);
                concat.addU8(0xBF + static_cast<uint8_t>(op));
            }

            concat.addU8(static_cast<uint8_t>(value));
        }
        else
        {
            SWAG_ASSERT(value <= 0x7FFFFFFF);
            switch (numBits)
            {
                case CPUBits::B8:
                    concat.addU8(static_cast<uint8_t>(op) + 3);
                    concat.addU8(static_cast<uint8_t>(value));
                    break;
                case CPUBits::B16:
                    concat.addU8(0x81);
                    concat.addU8(0xBF + static_cast<uint8_t>(op));
                    concat.addU16(static_cast<uint16_t>(value));
                    break;
                case CPUBits::B32:
                case CPUBits::B64:
                    concat.addU8(0x81);
                    concat.addU8(0xBF + static_cast<uint8_t>(op));
                    concat.addU32(static_cast<uint32_t>(value));
                    break;
            }
        }
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void SCBE_X64::emitOpNImmediate(CPURegister reg, uint64_t value, CPUOp op, CPUBits numBits)
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
                emitREX(concat, numBits);
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
                    concat.addU8(static_cast<uint8_t>(value));
                }
                break;

            case CPUOp::SUB:
                if (value == 0)
                    return;
                emitREX(concat, numBits);
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
                    concat.addU8(static_cast<uint8_t>(value));
                }
                break;

            case CPUOp::IMUL:
                if (value == 1)
                    return;
                if (value == 0)
                {
                    emitClearN(reg, numBits);
                    return;
                }

                emitREX(concat, numBits);
                if (value == 2)
                {
                    concat.addU8(0xD1);
                    concat.addU8(0xE0 | reg); // shl rax, 1
                }
                else if (Math::isPowerOfTwo(value))
                {
                    concat.addU8(0xC1);
                    concat.addU8(0xE0 | reg); // shl rax, ??
                    concat.addU8(static_cast<uint8_t>(log2(value)));
                }
                else
                {
                    SWAG_ASSERT(reg == RAX);
                    concat.addU8(0x6B);
                    concat.addU8(0xC0);
                    concat.addU8(static_cast<uint8_t>(value));
                }
                break;

            case CPUOp::XOR:
                emitREX(concat, numBits);
                if (numBits == CPUBits::B8)
                    concat.addU8(0x34);
                else
                    concat.addString2("\x83\xF0");
                concat.addU8(static_cast<uint8_t>(value));
                break;

            case CPUOp::SAR:
            case CPUOp::SHR:
            case CPUOp::SHL:
                emitREX(concat, numBits);
                value = min(value, static_cast<uint32_t>(numBits) - 1);
                if (value == 1)
                {
                    emitSpec8(concat, 0xD1, numBits);
                    concat.addU8(static_cast<uint8_t>(op) | reg);
                }
                else
                {
                    emitSpec8(concat, 0xC1, numBits);
                    concat.addU8(static_cast<uint8_t>(op) | reg);
                    concat.addU8(static_cast<uint8_t>(value));
                }
                break;

            case CPUOp::BT:
                emitREX(concat, numBits, RAX, reg);
                concat.addU8(0x0F);
                concat.addU8(static_cast<uint8_t>(op));
                concat.addU8(0xE2);
                concat.addU8(static_cast<uint8_t>(value));
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
            emitLoad64Immediate(RCX, value);
            emitOpN(RCX, reg, op, numBits);
        }
        else
        {
            SWAG_ASSERT(reg != RSP);
            emitLoad64Immediate(R8, value);
            emitOpN(R8, reg, op, numBits);
        }
    }
    else
    {
        emitREX(concat, numBits);
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

        concat.addU32(static_cast<uint32_t>(value));
    }
}

void SCBE_X64::emitOpNIndirectDst(uint32_t offsetStack, uint64_t value, CPURegister memReg, CPUOp op, CPUBits numBits)
{
    SWAG_ASSERT(memReg == RAX || memReg == RDI);

    if (value > 0x7FFFFFFF)
    {
        emitLoadNImmediate(RCX, value, numBits);
        emitOpNIndirect(offsetStack, RCX, memReg, op, numBits);
        return;
    }

    emitREX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0x80);
    else if (value <= 0x7F)
        concat.addU8(0x83);
    else
        concat.addU8(0x81);
    if (offsetStack == 0)
    {
        concat.addU8(static_cast<uint8_t>(op) - 1 + memReg);
    }
    else if (offsetStack <= 0x7F)
    {
        concat.addU8(0x3F + memReg + static_cast<uint8_t>(op));
        concat.addU8(static_cast<uint8_t>(offsetStack));
    }
    else
    {
        concat.addU8(0x7F + memReg + static_cast<uint8_t>(op));
        concat.addU32(offsetStack);
    }

    if (value <= 0x7F || numBits == CPUBits::B8)
        concat.addU8(static_cast<uint8_t>(value));
    else if (numBits == CPUBits::B16)
        concat.addU16(static_cast<uint16_t>(value));
    else
        concat.addU32(static_cast<uint32_t>(value));
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitExtendU8U64(CPURegister regDst, CPURegister regSrc)
{
    emitREX(concat, CPUBits::B64, regDst, regSrc);
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    concat.addU8(getModRM(RegReg, regDst, regSrc));
}

void SCBE_X64::emitExtendU16U64(CPURegister regDst, CPURegister regSrc)
{
    emitREX(concat, CPUBits::B64, regDst, regSrc);
    concat.addU8(0x0F);
    concat.addU8(0xB7);
    concat.addU8(getModRM(RegReg, regDst, regSrc));
}

/////////////////////////////////////////////////////////////////////
uint8_t* SCBE_X64::emitNearJumpOp(CPUJumpType jumpType)
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

uint32_t* SCBE_X64::emitLongJumpOp(CPUJumpType jumpType)
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

void SCBE_X64::emitJumpTable([[maybe_unused]] CPURegister table, [[maybe_unused]] CPURegister offset)
{
    SWAG_ASSERT(table == RCX && offset == RAX);
    emitREX(concat, CPUBits::B64);
    concat.addString3("\x63\x0C\x81"); // movsx rcx, dword ptr [rcx + rax*4]
}

void SCBE_X64::emitJump(CPUJumpType jumpType, int32_t instructionCount, int32_t jumpOffset)
{
    CPULabelToSolve label;
    label.ipDest = jumpOffset + instructionCount + 1;

    // Can we solve the label now ?
    const auto it = labels.find(label.ipDest);
    if (it != labels.end())
    {
        const auto currentOffset = static_cast<int32_t>(concat.totalCount()) + 1;
        const int  relOffset     = it->second - (currentOffset + 1);
        if (relOffset >= -127 && relOffset <= 128)
        {
            const auto offsetPtr = emitNearJumpOp(jumpType);
            *offsetPtr           = static_cast<uint8_t>(it->second - concat.totalCount());
        }
        else
        {
            const auto offsetPtr = emitLongJumpOp(jumpType);
            *offsetPtr           = it->second - concat.totalCount();
        }

        return;
    }

    // Here we do not know the destination label, so we assume 32 bits of offset
    label.patch         = reinterpret_cast<uint8_t*>(emitLongJumpOp(jumpType));
    label.currentOffset = static_cast<int32_t>(concat.totalCount());
    labelsToSolve.push_back(label);
}

void SCBE_X64::emitJump([[maybe_unused]] CPURegister reg)
{
    SWAG_ASSERT(reg == RAX);
    concat.addString2("\xFF\xE0"); // jmp rax
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitCopyX(CPURegister regDst, CPURegister regSrc, uint32_t count, uint32_t offset)
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
                concat.addU8(static_cast<uint8_t>(offset));
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
                concat.addU8(static_cast<uint8_t>(offset));
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
        emitLoad64Indirect(offset, RAX, regSrc);
        emitStore64Indirect(offset, RAX, regDst);
        count -= 8;
        offset += 8;
    }

    while (count >= 4)
    {
        emitLoad32Indirect(offset, RAX, regSrc);
        emitStore32Indirect(offset, RAX, regDst);
        count -= 4;
        offset += 4;
    }

    while (count >= 2)
    {
        emitLoad16Indirect(offset, RAX, regSrc);
        emitStore16Indirect(offset, RAX, regDst);
        count -= 2;
        offset += 2;
    }

    while (count >= 1)
    {
        emitLoad8Indirect(offset, RAX, regSrc);
        emitStore8Indirect(offset, RAX, regDst);
        count -= 1;
        offset += 1;
    }
}

void SCBE_X64::emitClearX(uint32_t count, uint32_t offset, CPURegister reg)
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
                concat.addU8(static_cast<uint8_t>(offset));
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
        emitStore64Immediate(offset, 0, reg);
        count -= 8;
        offset += 8;
    }

    while (count >= 4)
    {
        emitStore32Immediate(offset, 0, reg);
        count -= 4;
        offset += 4;
    }

    while (count >= 2)
    {
        emitStore16Immediate(offset, 0, reg);
        count -= 2;
        offset += 2;
    }

    while (count >= 1)
    {
        emitStore8Immediate(offset, 0, reg);
        count -= 1;
        offset += 1;
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitCallFar(const Utf8& symbolName)
{
    concat.addU8(0xFF);
    concat.addU8(0x15);
    const auto callSym = getOrAddSymbol(symbolName, CPUSymbolKind::Extern);
    addSymbolRelocation(concat.totalCount() - textSectionOffset, callSym->index, IMAGE_REL_AMD64_REL32);
    concat.addU32(0);
}

void SCBE_X64::emitCall(const Utf8& symbolName)
{
    concat.addU8(0xE8);

    const auto callSym = getOrAddSymbol(symbolName, CPUSymbolKind::Extern);
    if (callSym->kind == CPUSymbolKind::Function)
    {
        concat.addS32(static_cast<int32_t>(callSym->value + textSectionOffset - (concat.totalCount() + 4)));
    }
    else
    {
        addSymbolRelocation(concat.totalCount() - textSectionOffset, callSym->index, IMAGE_REL_AMD64_REL32);
        concat.addU32(0);
    }
}

void SCBE_X64::emitCallParameters(const TypeInfoFuncAttr* typeFuncBC, VectorNative<CPUPushParam>& paramsRegisters, VectorNative<TypeInfo*>& paramsTypes, void* retCopyAddr)
{
    const auto& cc                = typeFuncBC->getCallConv();
    const bool  returnByStackAddr = CallConv::returnByStackAddress(typeFuncBC);

    const uint32_t callConvRegisters    = cc.paramByRegisterCount;
    const uint32_t maxParamsPerRegister = paramsRegisters.size();

    // Set the first N parameters. Can be return register, or function parameter.
    uint32_t i = 0;
    for (; i < min(callConvRegisters, maxParamsPerRegister); i++)
    {
        auto       type = paramsTypes[i];
        const auto reg  = static_cast<uint32_t>(paramsRegisters[i].reg);

        if (type->isAutoConstPointerRef())
            type = TypeManager::concretePtrRef(type);

        // This is a return register
        if (type == g_TypeMgr->typeInfoUndefined)
        {
            SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
            if (retCopyAddr)
                emitLoad64Immediate(cc.paramByRegisterInteger[i], reinterpret_cast<uint64_t>(retCopyAddr));
            else if (returnByStackAddr)
                emitLoad64Indirect(reg, cc.paramByRegisterInteger[i], RDI);
            else
                emitLoadAddressIndirect(reg, cc.paramByRegisterInteger[i], RDI);
        }

        // This is a normal parameter, which can be float or integer
        else
        {
            // Pass struct in a register if small enough
            if (CallConv::structParamByValue(typeFuncBC, type))
            {
                SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                emitLoad64Indirect(REG_OFFSET(reg), RAX);
                emitLoad64Indirect(0, cc.paramByRegisterInteger[i], RAX);
            }
            else if (cc.useRegisterFloat && type->isNative(NativeTypeKind::F32))
            {
                if (paramsRegisters[i].type == CPUPushParamType::Imm)
                {
                    SWAG_ASSERT(paramsRegisters[i].reg <= UINT32_MAX);
                    emitLoad32Immediate(RAX, static_cast<uint32_t>(paramsRegisters[i].reg));
                    emitCopyF32(cc.paramByRegisterFloat[i], RAX);
                }
                else
                {
                    SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                    emitLoadF32Indirect(REG_OFFSET(reg), cc.paramByRegisterFloat[i]);
                }
            }
            else if (cc.useRegisterFloat && type->isNative(NativeTypeKind::F64))
            {
                if (paramsRegisters[i].type == CPUPushParamType::Imm)
                {
                    emitLoad64Immediate(RAX, paramsRegisters[i].reg);
                    emitCopyF64(cc.paramByRegisterFloat[i], RAX);
                }
                else
                {
                    SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                    emitLoadF64Indirect(REG_OFFSET(reg), cc.paramByRegisterFloat[i]);
                }
            }
            else
            {
                switch (paramsRegisters[i].type)
                {
                    case CPUPushParamType::Imm:
                        if (paramsRegisters[i].reg == 0)
                            emitClearN(cc.paramByRegisterInteger[i], CPUBits::B64);
                        else
                            emitLoad64Immediate(cc.paramByRegisterInteger[i], paramsRegisters[i].reg);
                        break;
                    case CPUPushParamType::Imm64:
                        emitLoad64Immediate(cc.paramByRegisterInteger[i], paramsRegisters[i].reg, true);
                        break;
                    case CPUPushParamType::RelocV:
                        emitSymbolRelocationValue(cc.paramByRegisterInteger[i], static_cast<uint32_t>(paramsRegisters[i].reg), 0);
                        break;
                    case CPUPushParamType::RelocAddr:
                        emitSymbolRelocationAddr(cc.paramByRegisterInteger[i], static_cast<uint32_t>(paramsRegisters[i].reg), 0);
                        break;
                    case CPUPushParamType::Addr:
                        emitLoadAddressIndirect(static_cast<uint32_t>(paramsRegisters[i].reg), cc.paramByRegisterInteger[i], RDI);
                        break;
                    case CPUPushParamType::RegAdd:
                        emitLoad64Indirect(REG_OFFSET(reg), cc.paramByRegisterInteger[i]);
                        emitOpNImmediate(cc.paramByRegisterInteger[i], paramsRegisters[i].val, CPUOp::ADD, CPUBits::B64);
                        break;
                    case CPUPushParamType::RegMul:
                        emitLoad64Indirect(REG_OFFSET(reg), RAX);
                        emitOpNImmediate(RAX, paramsRegisters[i].val, CPUOp::IMUL, CPUBits::B64);
                        emitCopyN(cc.paramByRegisterInteger[i], RAX, CPUBits::B64);
                        break;
                    case CPUPushParamType::RAX:
                        emitCopyN(cc.paramByRegisterInteger[i], RAX, CPUBits::B64);
                        break;
                    case CPUPushParamType::GlobalString:
                        emitSymbolGlobalString(cc.paramByRegisterInteger[i], reinterpret_cast<const char*>(paramsRegisters[i].reg));
                        break;
                    default:
                        SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                        emitLoad64Indirect(REG_OFFSET(reg), cc.paramByRegisterInteger[i]);
                        break;
                }
            }
        }
    }

    // Store all parameters after N on the stack, with an offset of N * sizeof(uint64_t)
    uint32_t offsetStack = min(callConvRegisters, maxParamsPerRegister) * sizeof(uint64_t);
    for (; i < paramsRegisters.size(); i++)
    {
        auto type = paramsTypes[i];
        if (type->isAutoConstPointerRef())
            type = TypeManager::concretePtrRef(type);

        const auto reg = static_cast<uint32_t>(paramsRegisters[i].reg);
        SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);

        // This is a C variadic parameter
        if (i >= maxParamsPerRegister)
        {
            emitLoad64Indirect(REG_OFFSET(reg), RAX);
            emitStore64Indirect(offsetStack, RAX, RSP);
        }

        // This is for a return value
        else if (type == g_TypeMgr->typeInfoUndefined)
        {
            // r is an address to registerRR, for FFI
            if (retCopyAddr)
                emitLoad64Immediate(RAX, reinterpret_cast<uint64_t>(retCopyAddr));
            else if (returnByStackAddr)
                emitLoad64Indirect(reg, RAX, RDI);
            else
                emitLoadAddressIndirect(reg, RAX, RDI);
            emitStore64Indirect(offsetStack, RAX, RSP);
        }

        // This is for a normal parameter
        else
        {
            const auto sizeOf = type->sizeOf;

            // Struct by copy. Will be a pointer to the stack
            if (type->isStruct())
            {
                emitLoad64Indirect(REG_OFFSET(reg), RAX);

                // Store the content of the struct in the stack
                if (CallConv::structParamByValue(typeFuncBC, type))
                {
                    switch (sizeOf)
                    {
                        case 1:
                            emitLoad8Indirect(0, RAX, RAX);
                            emitStore8Indirect(offsetStack, RAX, RSP);
                            break;
                        case 2:
                            emitLoad16Indirect(0, RAX, RAX);
                            emitStore16Indirect(offsetStack, RAX, RSP);
                            break;
                        case 4:
                            emitLoad32Indirect(0, RAX, RAX);
                            emitStore32Indirect(offsetStack, RAX, RSP);
                            break;
                        case 8:
                            emitLoad64Indirect(0, RAX, RAX);
                            emitStore64Indirect(offsetStack, RAX, RSP);
                            break;
                        default:
                            break;
                    }
                }

                // Store the address of the struct in the stack
                else
                {
                    emitStore64Indirect(offsetStack, RAX, RSP);
                }
            }
            else
            {
                switch (sizeOf)
                {
                    case 1:
                        emitLoad8Indirect(REG_OFFSET(reg), RAX);
                        emitStore8Indirect(offsetStack, RAX, RSP);
                        break;
                    case 2:
                        emitLoad16Indirect(REG_OFFSET(reg), RAX);
                        emitStore16Indirect(offsetStack, RAX, RSP);
                        break;
                    case 4:
                        emitLoad32Indirect(REG_OFFSET(reg), RAX);
                        emitStore32Indirect(offsetStack, RAX, RSP);
                        break;
                    case 8:
                        emitLoad64Indirect(REG_OFFSET(reg), RAX);
                        emitStore64Indirect(offsetStack, RAX, RSP);
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

void SCBE_X64::emitCallParameters(TypeInfoFuncAttr* typeFunc, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, void* retCopyAddr)
{
    pushParams2.clear();
    for (const auto r : pushRAParams)
        pushParams2.push_back({CPUPushParamType::Reg, r});
    emitCallParameters(typeFunc, pushParams2, offsetRT, retCopyAddr);
}

void SCBE_X64::emitCallParameters(TypeInfoFuncAttr* typeFunc, const VectorNative<CPUPushParam>& pushRAParams, uint32_t offsetRT, void* retCopyAddr)
{
    uint32_t numCallParams = typeFunc->parameters.size();
    pushParams3.clear();
    pushParamsTypes.clear();

    uint32_t indexParam = pushRAParams.size() - 1;

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
    for (uint32_t i = 0; i < numCallParams; i++)
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
        for (uint32_t i = typeFunc->numParamsRegisters(); i < pushRAParams.size(); i++)
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
        uint32_t reg = static_cast<uint32_t>(pushParams3[0].reg);
        if (typeFunc->isFctVariadic())
            reg = static_cast<uint32_t>(pushParams3[2].reg);

        emitLoad64Indirect(REG_OFFSET(reg), RAX);
        emitTestN(RAX, RAX, CPUBits::B64);

        // If not zero, jump to closure call
        const auto seekPtrClosure = emitLongJumpOp(JZ);
        const auto seekJmpClosure = concat.totalCount();

        emitCallParameters(typeFunc, pushParams3, pushParamsTypes, retCopyAddr);

        // Jump to after closure call
        const auto seekPtrAfterClosure = emitLongJumpOp(JUMP);
        const auto seekJmpAfterClosure = concat.totalCount();

        // Update jump to closure call
        *seekPtrClosure = static_cast<uint8_t>(concat.totalCount() - seekJmpClosure);

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
        emitCallParameters(typeFunc, pushParams3, pushParamsTypes, retCopyAddr);

        *seekPtrAfterClosure = static_cast<uint8_t>(concat.totalCount() - seekJmpAfterClosure);
    }
    else
    {
        emitCallParameters(typeFunc, pushParams3, pushParamsTypes, retCopyAddr);
    }
}

void SCBE_X64::emitCallResult(const TypeInfoFuncAttr* typeFunc, uint32_t offsetRT)
{
    if (CallConv::returnByValue(typeFunc))
    {
        const auto& cc         = typeFunc->getCallConv();
        const auto  returnType = typeFunc->concreteReturnType();
        if (returnType->isNativeFloat())
            emitStoreF64Indirect(offsetRT, cc.returnByRegisterFloat, RDI);
        else
            emitStore64Indirect(offsetRT, cc.returnByRegisterInteger, RDI);
    }
}

void SCBE_X64::emitCallIndirect(CPURegister reg)
{
    SWAG_ASSERT(reg == RAX || reg == RCX || reg == R10);
    if (reg == R10)
        concat.addU8(0x41);
    concat.addU8(0xFF);
    concat.addU8(0xD0 | (reg & 0b111));
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitCwd()
{
    emitREX(concat, CPUBits::B16);
    concat.addU8(0x99);
}

void SCBE_X64::emitCdq()
{
    concat.addU8(0x99);
}

void SCBE_X64::emitCqo()
{
    emitREX(concat, CPUBits::B64);
    concat.addU8(0x99);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitNotN(CPURegister reg, CPUBits numBits)
{
    SWAG_ASSERT(reg < R8);

    emitREX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0xF6);
    else
        concat.addU8(0xF7);
    concat.addU8(0xD0 | (reg & 0b111));
}

void SCBE_X64::emitNotNIndirect(uint32_t stackOffset, [[maybe_unused]] CPURegister memReg, CPUBits numBits)
{
    SWAG_ASSERT(memReg == RDI);

    emitREX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0xF6);
    else
        concat.addU8(0xF7);

    if (stackOffset <= 0x7F)
    {
        concat.addU8(0x57);
        concat.addU8(static_cast<uint8_t>(stackOffset));
    }
    else
    {
        concat.addU8(0x97);
        concat.addU32(stackOffset);
    }
}

void SCBE_X64::emitIncNIndirect(uint32_t stackOffset, CPURegister memReg, CPUBits numBits)
{
    SWAG_ASSERT(memReg < R8);
    emitREX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0xFE);
    else
        concat.addU8(0xFF);
    emitModRM(concat, stackOffset, 0, memReg);
}

void SCBE_X64::emitDecNIndirect(uint32_t stackOffset, CPURegister memReg, CPUBits numBits)
{
    SWAG_ASSERT(memReg < R8);
    emitREX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0xFE);
    else
        concat.addU8(0xFF);
    emitModRM(concat, stackOffset, 1, memReg);
}

void SCBE_X64::emitNegN(CPURegister reg, CPUBits numBits)
{
    SWAG_ASSERT(reg < R8);
    SWAG_ASSERT(numBits == CPUBits::B32 || numBits == CPUBits::B64);

    emitREX(concat, numBits);
    concat.addU8(0xF7);
    concat.addU8(0xD8 | (reg & 0b111));
}

void SCBE_X64::emitNegNIndirect(uint32_t stackOffset, [[maybe_unused]] CPURegister memReg, CPUBits numBits)
{
    SWAG_ASSERT(memReg == RDI);
    SWAG_ASSERT(numBits == CPUBits::B32 || numBits == CPUBits::B64);

    emitREX(concat, numBits);
    concat.addU8(0xF7);
    if (stackOffset <= 0x7F)
    {
        concat.addU8(0x5F);
        concat.addU8(static_cast<uint8_t>(stackOffset));
    }
    else
    {
        concat.addU8(0x9F);
        concat.addU32(stackOffset);
    }
}

void SCBE_X64::emitCMovN(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits)
{
    if (numBits < CPUBits::B32)
        numBits = CPUBits::B32;
    emitREX(concat, numBits, regDst, regSrc);
    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(op));
    concat.addU8(getModRM(RegReg, regDst, regSrc));
}

void SCBE_X64::emitRotateN(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits)
{
    SWAG_ASSERT(numBits >= CPUBits::B32);
    SWAG_ASSERT(regDst == RAX && regSrc == RCX);
    emitREX(concat, numBits, regDst, regSrc);
    concat.addU8(0xD3);
    concat.addU8(static_cast<uint8_t>(op));
}

void SCBE_X64::emitCmpXChg([[maybe_unused]] CPURegister regDst, [[maybe_unused]] CPURegister regSrc, CPUBits numBits)
{
    SWAG_ASSERT(regDst == RCX && regSrc == RDX);

    // lock CMPXCHG [rcx], dl
    if (numBits == CPUBits::B16)
        emitREX(concat, numBits);
    concat.addU8(0xF0);
    if (numBits == CPUBits::B64)
        emitREX(concat, numBits);
    concat.addU8(0x0F);
    emitSpec8(concat, 0xB1, numBits);
    concat.addU8(0x11);
}

void SCBE_X64::emitBSwapN([[maybe_unused]] CPURegister reg, CPUBits numBits)
{
    SWAG_ASSERT(reg == RAX);
    SWAG_ASSERT(numBits == CPUBits::B16 || numBits == CPUBits::B32 || numBits == CPUBits::B64);

    emitREX(concat, numBits);
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

void SCBE_X64::emitNop()
{
    concat.addU8(0x90);
}

void SCBE_X64::emitCastU64F64([[maybe_unused]] CPURegister regDst, [[maybe_unused]] CPURegister regSrc)
{
    SWAG_ASSERT(regSrc == RAX && regDst == XMM0);
    concat.addString5("\x66\x48\x0F\x6E\xC8"); // movq xmm1, rax
    emitSymbolRelocationAddr(RCX, symCst_U64F64, 0);
    concat.addString4("\x66\x0F\x62\x09");     // punpckldq xmm1, xmmword ptr [rcx]
    concat.addString5("\x66\x0F\x5C\x49\x10"); // subpd xmm1, xmmword ptr [rcx + 16]
    concat.addString4("\x66\x0F\x28\xC1");     // movapd xmm0, xmm1
    concat.addString4("\x66\x0F\x15\xC1");     // unpckhpd xmm0, xmm1
    concat.addString4("\xF2\x0F\x58\xC1");     // addsd xmm0, xmm1
}

// a*b+c
void SCBE_X64::emitMulAddF32(CPURegister a, CPURegister b, CPURegister c)
{
    SWAG_ASSERT(a == XMM0);
    SWAG_ASSERT(b == XMM1);
    SWAG_ASSERT(c == XMM2);

    concat.addU8(0xF3);
    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(CPUOp::FMUL));
    concat.addU8(0xC1);

    concat.addU8(0xF3);
    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(CPUOp::FADD));
    concat.addU8(0xC2);
}

void SCBE_X64::emitMulAddF64(CPURegister a, CPURegister b, CPURegister c)
{
    SWAG_ASSERT(a == XMM0);
    SWAG_ASSERT(b == XMM1);
    SWAG_ASSERT(c == XMM2);

    concat.addU8(0xF2);
    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(CPUOp::FMUL));
    concat.addU8(0xC1);

    concat.addU8(0xF2);
    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(CPUOp::FADD));
    concat.addU8(0xC2);
}
