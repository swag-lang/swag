// ReSharper disable CommentTypo
#include "pch.h"
#include "Backend/SCBE/Encoder/SCBE_X64.h"
#include "Core/Math.h"
#include "Semantic/Type/TypeManager.h"

enum X64DispMode
{
    Disp8  = 0b01,
    Disp32 = 0b10,
    RegReg = 0b11,
};

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

    void emitModRM(Concat& concat, uint32_t memOffset, uint8_t reg, uint8_t memReg, uint8_t op = 1)
    {
        if (memOffset == 0 && (memReg < R8 || memReg == R12))
        {
            // mov al, byte ptr [rdi]
            concat.addU8(getModRM(0, reg, memReg) | op - 1);
            if (memReg == RSP || memReg == R12)
                concat.addU8(0x24);
        }
        else if (memOffset <= 0x7F)
        {
            // mov al, byte ptr [rdi + ??]
            concat.addU8(getModRM(Disp8, reg, memReg) | op - 1);
            if (memReg == RSP || memReg == R12)
                concat.addU8(0x24);
            concat.addU8(static_cast<uint8_t>(memOffset));
        }
        else
        {
            // mov al, byte ptr [rdi + ????????]
            concat.addU8(getModRM(Disp32, reg, memReg) | op - 1);
            if (memReg == RSP || memReg == R12)
                concat.addU8(0x24);
            concat.addU32(memOffset);
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
    emitLoadImmediate(reg, 0, CPUBits::B64, true);
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

void SCBE_X64::emitLoadIndirect(CPUSignedType srcType, CPUSignedType dstType, CPURegister reg, CPURegister memReg, uint32_t memOffset)
{
    if (srcType == CPUSignedType::S8 && dstType == CPUSignedType::S16)
    {
        emitREX(concat, CPUBits::B16, reg, memReg);
        concat.addU8(0x0F);
        concat.addU8(0xBE);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (srcType == CPUSignedType::S8 && dstType == CPUSignedType::S32)
    {
        emitREX(concat, CPUBits::B32, reg, memReg);
        concat.addU8(0x0F);
        concat.addU8(0xBE);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void SCBE_X64::emitLoadS16S32Indirect(CPURegister reg, CPURegister memReg, uint32_t memOffset)
{
    emitREX(concat, CPUBits::B32, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xBF);
    emitModRM(concat, memOffset, reg, memReg);
}

void SCBE_X64::emitLoadS8S64Indirect(CPURegister reg, CPURegister memReg, uint32_t memOffset)
{
    emitREX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xBE);
    emitModRM(concat, memOffset, reg, memReg);
}

void SCBE_X64::emitLoadS16S64Indirect(CPURegister reg, CPURegister memReg, uint32_t memOffset)
{
    emitREX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xBF);
    emitModRM(concat, memOffset, reg, memReg);
}

void SCBE_X64::emitLoadS32S64Indirect(CPURegister reg, CPURegister memReg, uint32_t memOffset)
{
    emitREX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x63);
    emitModRM(concat, memOffset, reg, memReg);
}

void SCBE_X64::emitLoadU8U32Indirect(uint32_t memOffset, CPURegister reg, CPURegister memReg)
{
    emitREX(concat, CPUBits::B32, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    emitModRM(concat, memOffset, reg, memReg);
}

void SCBE_X64::emitLoadU16U32Indirect(CPURegister reg, CPURegister memReg, uint32_t memOffset)
{
    emitREX(concat, CPUBits::B32, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xB7);
    emitModRM(concat, memOffset, reg, memReg);
}

void SCBE_X64::emitLoadU8U64Indirect(CPURegister reg, CPURegister memReg, uint32_t memOffset)
{
    emitREX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    emitModRM(concat, memOffset, reg, memReg);
}

void SCBE_X64::emitLoadU16U64Indirect(CPURegister reg, CPURegister memReg, uint32_t memOffset)
{
    emitREX(concat, CPUBits::B64, reg, memReg);
    concat.addU8(0x0F);
    concat.addU8(0xB7);
    emitModRM(concat, memOffset, reg, memReg);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitLoadIndirect(CPURegister reg, CPURegister memReg, uint32_t memOffset, CPUBits numBits)
{
    if (isInt(numBits) &&
        storageRegStack == memOffset &&
        storageReg == reg &&
        storageMemReg == memReg &&
        storageRegBits >= countBits(numBits) &&
        storageRegCount == concat.totalCount())
    {
        if (numBits == CPUBits::B32 && storageRegBits > 32)
            emitCopy(RAX, RAX, CPUBits::B32);
        return;
    }

    switch (numBits)
    {
        case CPUBits::B8:
            emitREX(concat, CPUBits::B8, reg, memReg);
            concat.addU8(0x8A);
            emitModRM(concat, memOffset, reg, memReg);
            break;

        case CPUBits::B16:
            emitREX(concat, CPUBits::B16, reg, memReg);
            concat.addU8(0x8B);
            emitModRM(concat, memOffset, reg, memReg);
            break;

        case CPUBits::B32:
            emitREX(concat, CPUBits::B32, reg, memReg);
            concat.addU8(0x8B);
            emitModRM(concat, memOffset, reg, memReg);
            break;

        case CPUBits::B64:
            emitREX(concat, CPUBits::B64, reg, memReg);
            concat.addU8(0x8B);
            emitModRM(concat, memOffset, reg, memReg);
            break;

        case CPUBits::F32:
            SWAG_ASSERT(reg < R8 && memReg < R8);
            concat.addU8(0xF3);
            concat.addU8(0x0F);
            concat.addU8(0x10);
            emitModRM(concat, memOffset, reg, memReg);
            break;

        case CPUBits::F64:
            SWAG_ASSERT(reg < R8 && memReg < R8);
            concat.addU8(0xF2);
            concat.addU8(0x0F);
            concat.addU8(0x10);
            emitModRM(concat, memOffset, reg, memReg);
            break;

        default:
            SWAG_ASSERT(false);
            break;
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitLoadAddressIndirect(CPURegister reg, CPURegister memReg, uint32_t memOffset)
{
    if (memReg == RIP)
    {
        SWAG_ASSERT(memOffset == 0);
        SWAG_ASSERT(reg == RCX);
        emitREX(concat, CPUBits::B64, reg, memReg);
        concat.addU8(0x8D);
        concat.addU8(0x0D);
    }
    else if (memOffset == 0)
    {
        emitCopy(reg, memReg, CPUBits::B64);
    }
    else
    {
        emitREX(concat, CPUBits::B64, reg, memReg);
        concat.addU8(0x8D);
        emitModRM(concat, memOffset, reg, memReg);
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitStoreIndirect(CPURegister memReg, uint32_t memOffset, CPURegister reg, CPUBits numBits)
{
    switch (numBits)
    {
        case CPUBits::B8:
            emitREX(concat, CPUBits::B8, reg, memReg);
            concat.addU8(0x88);
            emitModRM(concat, memOffset, reg, memReg);

            storageRegCount = concat.totalCount();
            storageRegStack = memOffset;
            storageReg      = reg;
            storageMemReg   = memReg;
            storageRegBits  = 8;
            break;

        case CPUBits::B16:
            emitREX(concat, CPUBits::B16, reg, memReg);
            concat.addU8(0x89);
            emitModRM(concat, memOffset, reg, memReg);

            storageRegCount = concat.totalCount();
            storageRegStack = memOffset;
            storageReg      = reg;
            storageMemReg   = memReg;
            storageRegBits  = 16;
            break;

        case CPUBits::B32:
            emitREX(concat, CPUBits::B32, reg, memReg);
            concat.addU8(0x89);
            emitModRM(concat, memOffset, reg, memReg);

            storageRegCount = concat.totalCount();
            storageRegStack = memOffset;
            storageReg      = reg;
            storageMemReg   = memReg;
            storageRegBits  = 32;
            break;

        case CPUBits::B64:
            emitREX(concat, CPUBits::B64, reg, memReg);
            concat.addU8(0x89);
            emitModRM(concat, memOffset, reg, memReg);

            storageRegCount = concat.totalCount();
            storageRegStack = memOffset;
            storageReg      = reg;
            storageMemReg   = memReg;
            storageRegBits  = 64;
            break;

        case CPUBits::F32:
            SWAG_ASSERT(reg < R8 && memReg < R8);
            concat.addU8(0xF3);
            concat.addU8(0x0F);
            concat.addU8(0x11);
            emitModRM(concat, memOffset, reg, memReg);
            break;

        case CPUBits::F64:
            SWAG_ASSERT(reg < R8 && memReg < R8);
            concat.addU8(0xF2);
            concat.addU8(0x0F);
            concat.addU8(0x11);
            emitModRM(concat, memOffset, reg, memReg);
            break;

        default:
            SWAG_ASSERT(false);
            break;
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitStoreImmediate(CPURegister memReg, uint32_t memOffset, uint64_t value, CPUBits numBits)
{
    if (numBits == CPUBits::B8)
    {
        concat.addU8(0xC6);
        emitModRM(concat, memOffset, 0, memReg);
        concat.addU8(static_cast<uint8_t>(value));
    }
    else if (numBits == CPUBits::B16)
    {
        concat.addU8(0x66);
        concat.addU8(0xC7);
        emitModRM(concat, memOffset, 0, memReg);
        concat.addU16(static_cast<uint16_t>(value));
    }
    else if (numBits == CPUBits::B32)
    {
        concat.addU8(0xC7);
        emitModRM(concat, memOffset, 0, memReg);
        concat.addU32(static_cast<uint32_t>(value));
    }
    else
    {
        SWAG_ASSERT(numBits == CPUBits::B64);
        SWAG_ASSERT(value <= 0xFFFFFFFF);
        emitREX(concat, CPUBits::B64);
        concat.addU8(0xC7);
        emitModRM(concat, memOffset, 0, memReg);
        concat.addU32(static_cast<uint32_t>(value));
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitLoadImmediate(CPURegister reg, uint64_t value, CPUBits numBits, bool force64Bits)
{
    if (force64Bits)
    {
        SWAG_ASSERT(numBits == CPUBits::B64);
        emitREX(concat, CPUBits::B64, RAX, reg);
        concat.addU8(0xB8 | reg);
        concat.addU64(value);
        return;
    }

    if (value == 0)
    {
        if (numBits == CPUBits::F32)
        {
            emitClear(RAX, CPUBits::B32);
            emitCopy(reg, RAX, CPUBits::F32);
        }
        else if (numBits == CPUBits::F64)
        {
            emitClear(RAX, CPUBits::B64);
            emitCopy(reg, RAX, CPUBits::F64);
        }
        else
            emitClear(reg, numBits);
        return;
    }

    switch (numBits)
    {
        case CPUBits::B8:
            concat.addU8(0xB0 | reg);
            concat.addU8(static_cast<uint8_t>(value));
            break;

        case CPUBits::B16:
            concat.addU8(0x66);
            concat.addU8(0xB8 | reg);
            concat.addU16(static_cast<uint16_t>(value));
            break;

        case CPUBits::B32:
            concat.addU8(0xB8 | reg);
            concat.addU32(static_cast<uint32_t>(value));
            break;

        case CPUBits::B64:
            if (value <= 0x7FFFFFFF && reg < R8)
            {
                concat.addU8(0xB8 | reg);
                concat.addU32(static_cast<uint32_t>(value));
            }
            else if (value <= 0x7FFFFFFF)
            {
                emitREX(concat, CPUBits::B64, RAX, reg);
                concat.addU8(0xC7);
                concat.addU8(0xC0 | (reg & 0b111));
                concat.addU32(static_cast<uint32_t>(value));
            }
            else
            {
                emitREX(concat, CPUBits::B64, RAX, reg);
                concat.addU8(0xB8 | reg);
                concat.addU64(value);
            }
            break;

        case CPUBits::F32:
            concat.addU8(0xB8 | RAX);
            concat.addU32(static_cast<uint32_t>(value));
            emitCopy(reg, RAX, CPUBits::F32);
            break;

        case CPUBits::F64:
            emitREX(concat, CPUBits::B64, RAX, RAX);
            if (value <= 0x7FFFFFFF)
            {
                concat.addU8(0xC7);
                concat.addU8(0xC0 | (RAX & 0b111));
                concat.addU32(static_cast<uint32_t>(value));
            }
            else
            {
                concat.addU8(0xB8 | RAX);
                concat.addU64(value);
            }
            emitCopy(reg, RAX, CPUBits::F64);
            break;

        default:
            SWAG_ASSERT(false);
            break;
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitClear(CPURegister reg, CPUBits numBits)
{
    emitREX(concat, numBits, reg, reg);
    emitSpec8(concat, static_cast<uint8_t>(CPUOp::XOR), numBits);
    concat.addU8(getModRM(RegReg, reg, reg));
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitCopy(CPURegister regDst, CPURegister regSrc, CPUBits numBits)
{
    if (numBits == CPUBits::F32)
    {
        SWAG_ASSERT(regSrc == RAX);
        SWAG_ASSERT(regDst == XMM0 || regDst == XMM1 || regDst == XMM2 || regDst == XMM3);
        concat.addU8(0x66);
        concat.addU8(0x0F);
        concat.addU8(0x6E);
        concat.addU8(static_cast<uint8_t>(0xC0 | regDst << 3));
    }
    else if (numBits == CPUBits::F64)
    {
        SWAG_ASSERT(regSrc == RAX);
        SWAG_ASSERT(regDst == XMM0 || regDst == XMM1 || regDst == XMM2 || regDst == XMM3);
        concat.addU8(0x66);
        emitREX(concat, CPUBits::B64);
        concat.addU8(0x0F);
        concat.addU8(0x6E);
        concat.addU8(static_cast<uint8_t>(0xC0 | regDst << 3));
    }
    else
    {
        emitREX(concat, numBits, regSrc, regDst);
        if (numBits == CPUBits::B8)
            concat.addU8(0x88);
        else
            concat.addU8(0x89);
        concat.addU8(getModRM(RegReg, regSrc, regDst));
    }
}

void SCBE_X64::emitCopyDownUp([[maybe_unused]] CPURegister reg, [[maybe_unused]] CPUBits numBits)
{
    SWAG_ASSERT(reg == RAX);
    SWAG_ASSERT(numBits == CPUBits::B8);
    concat.addString2("\x88\xe0"); // mov al, ah
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitSet(CPURegister reg, CPUSet setType)
{
    switch (setType)
    {
        case CPUSet::SetA:
            SWAG_ASSERT(reg == RAX || reg == R8);
            if (reg >= R8)
                concat.addU8(0x41);
            concat.addU8(0x0F);
            concat.addU8(0x97);
            concat.addU8(0xC0 | (reg & 0b111));
            break;

        case CPUSet::SetAE:
            SWAG_ASSERT(reg == RAX || reg == R8);
            if (reg >= R8)
                concat.addU8(0x41);
            concat.addU8(0x0F);
            concat.addU8(0x93);
            concat.addU8(0xC0 | (reg & 0b111));
            break;

        case CPUSet::SetG:
            SWAG_ASSERT(reg == RAX || reg == R8);
            if (reg >= R8)
                concat.addU8(0x41);
            concat.addU8(0x0F);
            concat.addU8(0x9F);
            concat.addU8(0xC0 | (reg & 0b111));
            break;

        case CPUSet::SetNE:
            SWAG_ASSERT(reg == RAX);
            concat.addU8(0x0F);
            concat.addU8(0x95);
            concat.addU8(0xC0);
            break;

        case CPUSet::SetNA:
            SWAG_ASSERT(reg == RAX);
            concat.addU8(0x0F);
            concat.addU8(0x96);
            concat.addU8(0xC0);
            break;

        case CPUSet::SetB:
            SWAG_ASSERT(reg == RAX);
            concat.addU8(0x0F);
            concat.addU8(0x92);
            concat.addU8(0xC0);
            break;

        case CPUSet::SetBE:
            SWAG_ASSERT(reg == RAX);
            concat.addU8(0x0F);
            concat.addU8(0x96);
            concat.addU8(0xC0);
            break;

        case CPUSet::SetE:
            SWAG_ASSERT(reg == RAX);
            concat.addU8(0x0F);
            concat.addU8(0x94);
            concat.addU8(0xC0);
            break;

        case CPUSet::SetEP:
            SWAG_ASSERT(reg == RAX);

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
            break;

        case CPUSet::SetNEP:
            SWAG_ASSERT(reg == RAX);

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
            break;

        case CPUSet::SetGE:
            SWAG_ASSERT(reg == RAX);
            concat.addU8(0x0F);
            concat.addU8(0x9D);
            concat.addU8(0xC0);
            break;

        case CPUSet::SetL:
            SWAG_ASSERT(reg == RAX);
            concat.addU8(0x0F);
            concat.addU8(0x9C);
            concat.addU8(0xC0);
            break;

        case CPUSet::SetLE:
            SWAG_ASSERT(reg == RAX);
            concat.addU8(0x0F);
            concat.addU8(0x9E);
            concat.addU8(0xC0);
            break;

        default:
            SWAG_ASSERT(false);
            break;
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitTest(CPURegister regDst, CPURegister regSrc, CPUBits numBits)
{
    emitREX(concat, numBits, regDst, regSrc);
    if (numBits == CPUBits::B8)
        concat.addU8(0x84);
    else
        concat.addU8(0x85);
    concat.addU8(getModRM(RegReg, regDst, regSrc));
}

void SCBE_X64::emitCmp(CPURegister reg0, CPURegister reg1, CPUBits numBits)
{
    if (numBits == CPUBits::F32)
    {
        SWAG_ASSERT(reg0 < R8 && reg1 < R8);
        concat.addU8(0x0F);
        concat.addU8(0x2F);
        concat.addU8(getModRM(RegReg, reg0, reg1));
    }
    else if (numBits == CPUBits::F64)
    {
        SWAG_ASSERT(reg0 < R8 && reg1 < R8);
        concat.addU8(0x66);
        concat.addU8(0x0F);
        concat.addU8(0x2F);
        concat.addU8(getModRM(RegReg, reg0, reg1));
    }
    else
    {
        emitREX(concat, numBits, reg1, reg0);
        emitSpec8(concat, 0x39, numBits);
        concat.addU8(getModRM(RegReg, reg1, reg0));
    }
}

void SCBE_X64::emitCmpImmediate(CPURegister reg, uint64_t value, CPUBits numBits)
{
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));
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
        emitLoadImmediate(RCX, value, numBits);
        emitCmp(reg, RCX, numBits);
    }
}

void SCBE_X64::emitCmpIndirect(CPURegister memReg, uint32_t memOffset, CPURegister reg, CPUBits numBits)
{
    if (numBits == CPUBits::F32)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        concat.addU8(0x0F);
        concat.addU8(0x2E);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (numBits == CPUBits::F64)
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        concat.addU8(0x66);
        concat.addU8(0x0F);
        concat.addU8(0x2F);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else
    {
        SWAG_ASSERT(reg < R8 && memReg < R8);
        emitREX(concat, numBits);
        emitSpec8(concat, 0x3B, numBits);
        emitModRM(concat, memOffset, reg, memReg);
    }
}

void SCBE_X64::emitCmpIndirectDst(CPURegister memReg, uint32_t memOffset, uint32_t value, CPUBits numBits)
{
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));
    emitREX(concat, numBits);

    if (numBits == CPUBits::B8)
    {
        concat.addU8(0x80);
        emitModRM(concat, memOffset, memReg, memReg, 0x39);
        concat.addU8(static_cast<uint8_t>(value));
    }
    else if (value <= 0x7F)
    {
        concat.addU8(0x83);
        emitModRM(concat, memOffset, memReg, memReg, 0x39);
        concat.addU8(static_cast<uint8_t>(value));
    }
    else
    {
        concat.addU8(0x81);
        emitModRM(concat, memOffset, memReg, memReg, 0x39);
        if (numBits == CPUBits::B16)
            concat.addU16(static_cast<uint16_t>(value));
        else
            concat.addU32(value);
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitOp(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits, CPUBits srcBits)
{
    if (numBits == CPUBits::F32)
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
    else if (numBits == CPUBits::F64)
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
    else
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
}

void SCBE_X64::emitOpIndirectSrc(CPURegister reg, CPURegister memReg, uint32_t memOffset, CPUOp op, CPUBits numBits)
{
    if (numBits == CPUBits::F32)
    {
        SWAG_ASSERT(reg == XMM0);
        concat.addU8(0xF3);
        concat.addU8(0x0F);
        concat.addU8(static_cast<uint8_t>(op));
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (numBits == CPUBits::F64)
    {
        SWAG_ASSERT(reg == XMM0);
        concat.addU8(0xF2);
        concat.addU8(0x0F);
        concat.addU8(static_cast<uint8_t>(op));
        emitModRM(concat, memOffset, reg, memReg);
    }
    else
    {
        SWAG_ASSERT(reg == RAX);
        emitREX(concat, numBits);
        if (op == CPUOp::MUL)
        {
            emitSpec8(concat, 0xF7, numBits);
            emitModRM(concat, memOffset, 4, memReg);
        }
        else if (op == CPUOp::IMUL)
        {
            if (numBits == CPUBits::B8)
            {
                concat.addU8(0xF6);
                emitModRM(concat, memOffset, 5, memReg);
            }
            else if (numBits == CPUBits::B16)
            {
                concat.addU8(0xF7);
                emitModRM(concat, memOffset, 5, memReg);
            }
            else
            {
                concat.addString2("\x0F\xAF");
                emitModRM(concat, memOffset, RAX, memReg);
            }
        }
        else
        {
            if (numBits == CPUBits::B8)
                concat.addU8(static_cast<uint8_t>(op) + 1);
            else
                concat.addU8(static_cast<uint8_t>(op) | 2);
            emitModRM(concat, memOffset, RAX, memReg);
        }
    }
}

void SCBE_X64::emitOpIndirect(CPURegister memReg, uint32_t memOffset, CPURegister reg, CPUOp op, CPUBits numBits, bool lock)
{
    if (numBits == CPUBits::F32)
    {
        SWAG_ASSERT(reg == XMM1);
        SWAG_ASSERT(memReg < R8);
        emitLoadIndirect(XMM0, memReg, memOffset, CPUBits::F32);
        concat.addU8(0xF3);
        concat.addU8(0x0F);
        concat.addU8(static_cast<uint8_t>(op));
        concat.addU8(0xC1);
        emitStoreIndirect(memReg, memOffset, XMM0, CPUBits::F32);
    }
    else if (numBits == CPUBits::F64)
    {
        SWAG_ASSERT(reg == XMM1);
        SWAG_ASSERT(memReg < R8);
        emitLoadIndirect(XMM0, memReg, memOffset, CPUBits::F64);
        concat.addU8(0xF2);
        concat.addU8(0x0F);
        concat.addU8(static_cast<uint8_t>(op));
        concat.addU8(0xC1);
        emitStoreIndirect(memReg, memOffset, XMM0, CPUBits::F64);
    }
    else
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

            if (memOffset == 0)
            {
                concat.addU8((static_cast<uint8_t>(op) & ~2) - 0xBA);
            }
            else if (memOffset <= 0x7F)
            {
                concat.addU8((static_cast<uint8_t>(op) & ~2) - 0x7A);
                concat.addU8(static_cast<uint8_t>(memOffset));
            }
            else
            {
                concat.addU8((static_cast<uint8_t>(op) & ~2) - 0x3A);
                concat.addU32(memOffset);
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
            emitModRM(concat, memOffset, reg, memReg);
        }
    }
}

void SCBE_X64::emitOpIndirectDst(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits)
{
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));
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

void SCBE_X64::emitOpIndirectDst([[maybe_unused]] CPURegister reg, uint64_t value, CPUOp op, CPUBits numBits)
{
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));
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

void SCBE_X64::emitOpImmediate(CPURegister reg, uint64_t value, CPUOp op, CPUBits numBits)
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
                    emitClear(reg, numBits);
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
            emitLoadImmediate(RCX, value, CPUBits::B64);
            emitOp(reg, RCX, op, numBits);
        }
        else
        {
            SWAG_ASSERT(reg != RSP);
            emitLoadImmediate(R8, value, CPUBits::B64);
            emitOp(reg, R8, op, numBits);
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

void SCBE_X64::emitOpIndirectDst(CPURegister memReg, uint32_t memOffset, uint64_t value, CPUOp op, CPUBits numBits)
{
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));
    SWAG_ASSERT(memReg == RAX || memReg == RDI);

    if (value > 0x7FFFFFFF)
    {
        emitLoadImmediate(RCX, value, numBits);
        emitOpIndirect(memReg, memOffset, RCX, op, numBits);
        return;
    }

    emitREX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0x80);
    else if (value <= 0x7F)
        concat.addU8(0x83);
    else
        concat.addU8(0x81);
    if (memOffset == 0)
    {
        concat.addU8(static_cast<uint8_t>(op) - 1 + memReg);
    }
    else if (memOffset <= 0x7F)
    {
        concat.addU8(0x3F + memReg + static_cast<uint8_t>(op));
        concat.addU8(static_cast<uint8_t>(memOffset));
    }
    else
    {
        concat.addU8(0x7F + memReg + static_cast<uint8_t>(op));
        concat.addU32(memOffset);
    }

    if (value <= 0x7F || numBits == CPUBits::B8)
        concat.addU8(static_cast<uint8_t>(value));
    else if (numBits == CPUBits::B16)
        concat.addU16(static_cast<uint16_t>(value));
    else
        concat.addU32(static_cast<uint32_t>(value));
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitCastU8U64(CPURegister regDst, CPURegister regSrc)
{
    emitREX(concat, CPUBits::B64, regDst, regSrc);
    concat.addU8(0x0F);
    concat.addU8(0xB6);
    concat.addU8(getModRM(RegReg, regDst, regSrc));
}

void SCBE_X64::emitCastU16U64(CPURegister regDst, CPURegister regSrc)
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

void SCBE_X64::emitCopy(CPURegister regDst, CPURegister regSrc, uint32_t count, uint32_t offset)
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
        emitLoadIndirect(RAX, regSrc, offset, CPUBits::B64);
        emitStoreIndirect(regDst, offset, RAX, CPUBits::B64);
        count -= 8;
        offset += 8;
    }

    while (count >= 4)
    {
        emitLoadIndirect(RAX, regSrc, offset, CPUBits::B32);
        emitStoreIndirect(regDst, offset, RAX, CPUBits::B32);
        count -= 4;
        offset += 4;
    }

    while (count >= 2)
    {
        emitLoadIndirect(RAX, regSrc, offset, CPUBits::B16);
        emitStoreIndirect(regDst, offset, RAX, CPUBits::B16);
        count -= 2;
        offset += 2;
    }

    while (count >= 1)
    {
        emitLoadIndirect(RAX, regSrc, offset, CPUBits::B8);
        emitStoreIndirect(regDst, offset, RAX, CPUBits::B8);
        count -= 1;
        offset += 1;
    }
}

void SCBE_X64::emitClear(CPURegister memReg, uint32_t memOffset, uint32_t count)
{
    if (!count)
        return;
    SWAG_ASSERT(memReg == RAX || memReg == RCX || memReg == RDI);

    // SSE 16 octets
    if (count >= 16)
    {
        concat.addString3("\x0F\x57\xC0"); // xorps xmm0, xmm0
        while (count >= 16)
        {
            concat.addString2("\x0F\x11"); // movups [reg + ????????], xmm0
            if (memOffset <= 0x7F)
            {
                concat.addU8(0x40 | memReg);
                concat.addU8(static_cast<uint8_t>(memOffset));
            }
            else
            {
                concat.addU8(0x80 | memReg);
                concat.addU32(memOffset);
            }
            count -= 16;
            memOffset += 16;
        }
    }

    while (count >= 8)
    {
        emitStoreImmediate(memReg, memOffset, 0, CPUBits::B64);
        count -= 8;
        memOffset += 8;
    }

    while (count >= 4)
    {
        emitStoreImmediate(memReg, memOffset, 0, CPUBits::B32);
        count -= 4;
        memOffset += 4;
    }

    while (count >= 2)
    {
        emitStoreImmediate(memReg, memOffset, 0, CPUBits::B16);
        count -= 2;
        memOffset += 2;
    }

    while (count >= 1)
    {
        emitStoreImmediate(memReg, memOffset, 0, CPUBits::B8);
        count -= 1;
        memOffset += 1;
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
                emitLoadImmediate(cc.paramByRegisterInteger[i], reinterpret_cast<uint64_t>(retCopyAddr), CPUBits::B64);
            else if (returnByStackAddr)
                emitLoadIndirect(cc.paramByRegisterInteger[i], RDI, reg, CPUBits::B64);
            else
                emitLoadAddressIndirect(cc.paramByRegisterInteger[i], RDI, reg);
        }

        // This is a normal parameter, which can be float or integer
        else
        {
            // Pass struct in a register if small enough
            if (CallConv::structParamByValue(typeFuncBC, type))
            {
                SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                emitLoadIndirect(RAX, RDI, REG_OFFSET(reg), CPUBits::B64);
                emitLoadIndirect(cc.paramByRegisterInteger[i], RAX, 0, CPUBits::B64);
            }
            else if (cc.useRegisterFloat && type->isNative(NativeTypeKind::F32))
            {
                if (paramsRegisters[i].type == CPUPushParamType::Imm)
                {
                    SWAG_ASSERT(paramsRegisters[i].reg <= UINT32_MAX);
                    emitLoadImmediate(RAX, static_cast<uint32_t>(paramsRegisters[i].reg), CPUBits::B32);
                    emitCopy(cc.paramByRegisterFloat[i], RAX, CPUBits::F32);
                }
                else
                {
                    SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                    emitLoadIndirect(cc.paramByRegisterFloat[i], RDI, REG_OFFSET(reg), CPUBits::F32);
                }
            }
            else if (cc.useRegisterFloat && type->isNative(NativeTypeKind::F64))
            {
                if (paramsRegisters[i].type == CPUPushParamType::Imm)
                {
                    emitLoadImmediate(RAX, paramsRegisters[i].reg, CPUBits::B64);
                    emitCopy(cc.paramByRegisterFloat[i], RAX, CPUBits::F64);
                }
                else
                {
                    SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                    emitLoadIndirect(cc.paramByRegisterFloat[i], RDI, REG_OFFSET(reg), CPUBits::F64);
                }
            }
            else
            {
                switch (paramsRegisters[i].type)
                {
                    case CPUPushParamType::Imm:
                        if (paramsRegisters[i].reg == 0)
                            emitClear(cc.paramByRegisterInteger[i], CPUBits::B64);
                        else
                            emitLoadImmediate(cc.paramByRegisterInteger[i], paramsRegisters[i].reg, CPUBits::B64);
                        break;
                    case CPUPushParamType::Imm64:
                        emitLoadImmediate(cc.paramByRegisterInteger[i], paramsRegisters[i].reg, CPUBits::B64, true);
                        break;
                    case CPUPushParamType::RelocV:
                        emitSymbolRelocationValue(cc.paramByRegisterInteger[i], static_cast<uint32_t>(paramsRegisters[i].reg), 0);
                        break;
                    case CPUPushParamType::RelocAddr:
                        emitSymbolRelocationAddr(cc.paramByRegisterInteger[i], static_cast<uint32_t>(paramsRegisters[i].reg), 0);
                        break;
                    case CPUPushParamType::Addr:
                        emitLoadAddressIndirect(cc.paramByRegisterInteger[i], RDI, static_cast<uint32_t>(paramsRegisters[i].reg));
                        break;
                    case CPUPushParamType::RegAdd:
                        emitLoadIndirect(cc.paramByRegisterInteger[i], RDI, REG_OFFSET(reg), CPUBits::B64);
                        emitOpImmediate(cc.paramByRegisterInteger[i], paramsRegisters[i].val, CPUOp::ADD, CPUBits::B64);
                        break;
                    case CPUPushParamType::RegMul:
                        emitLoadIndirect(RAX, RDI, REG_OFFSET(reg), CPUBits::B64);
                        emitOpImmediate(RAX, paramsRegisters[i].val, CPUOp::IMUL, CPUBits::B64);
                        emitCopy(cc.paramByRegisterInteger[i], RAX, CPUBits::B64);
                        break;
                    case CPUPushParamType::RAX:
                        emitCopy(cc.paramByRegisterInteger[i], RAX, CPUBits::B64);
                        break;
                    case CPUPushParamType::GlobalString:
                        emitSymbolGlobalString(cc.paramByRegisterInteger[i], reinterpret_cast<const char*>(paramsRegisters[i].reg));
                        break;
                    default:
                        SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                        emitLoadIndirect(cc.paramByRegisterInteger[i], RDI, REG_OFFSET(reg), CPUBits::B64);
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
            emitLoadIndirect(RAX, RDI, REG_OFFSET(reg), CPUBits::B64);
            emitStoreIndirect(RSP, offsetStack, RAX, CPUBits::B64);
        }

        // This is for a return value
        else if (type == g_TypeMgr->typeInfoUndefined)
        {
            // r is an address to registerRR, for FFI
            if (retCopyAddr)
                emitLoadImmediate(RAX, reinterpret_cast<uint64_t>(retCopyAddr), CPUBits::B64);
            else if (returnByStackAddr)
                emitLoadIndirect(RAX, RDI, reg, CPUBits::B64);
            else
                emitLoadAddressIndirect(RAX, RDI, reg);
            emitStoreIndirect(RSP, offsetStack, RAX, CPUBits::B64);
        }

        // This is for a normal parameter
        else
        {
            const auto sizeOf = type->sizeOf;

            // Struct by copy. Will be a pointer to the stack
            if (type->isStruct())
            {
                emitLoadIndirect(RAX, RDI, REG_OFFSET(reg), CPUBits::B64);

                // Store the content of the struct in the stack
                if (CallConv::structParamByValue(typeFuncBC, type))
                {
                    switch (sizeOf)
                    {
                        case 1:
                            emitLoadIndirect(RAX, RAX, 0, CPUBits::B8);
                            emitStoreIndirect(RSP, offsetStack, RAX, CPUBits::B8);
                            break;
                        case 2:
                            emitLoadIndirect(RAX, RAX, 0, CPUBits::B16);
                            emitStoreIndirect(RSP, offsetStack, RAX, CPUBits::B16);
                            break;
                        case 4:
                            emitLoadIndirect(RAX, RAX, 0, CPUBits::B32);
                            emitStoreIndirect(RSP, offsetStack, RAX, CPUBits::B32);
                            break;
                        case 8:
                            emitLoadIndirect(RAX, RAX, 0, CPUBits::B64);
                            emitStoreIndirect(RSP, offsetStack, RAX, CPUBits::B64);
                            break;
                        default:
                            break;
                    }
                }

                // Store the address of the struct in the stack
                else
                {
                    emitStoreIndirect(RSP, offsetStack, RAX, CPUBits::B64);
                }
            }
            else
            {
                switch (sizeOf)
                {
                    case 1:
                        emitLoadIndirect(RAX, RDI, REG_OFFSET(reg), CPUBits::B8);
                        emitStoreIndirect(RSP, offsetStack, RAX, CPUBits::B8);
                        break;
                    case 2:
                        emitLoadIndirect(RAX, RDI, REG_OFFSET(reg), CPUBits::B16);
                        emitStoreIndirect(RSP, offsetStack, RAX, CPUBits::B16);
                        break;
                    case 4:
                        emitLoadIndirect(RAX, RDI, REG_OFFSET(reg), CPUBits::B32);
                        emitStoreIndirect(RSP, offsetStack, RAX, CPUBits::B32);
                        break;
                    case 8:
                        emitLoadIndirect(RAX, RDI, REG_OFFSET(reg), CPUBits::B64);
                        emitStoreIndirect(RSP, offsetStack, RAX, CPUBits::B64);
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

        emitLoadIndirect(RAX, RDI, REG_OFFSET(reg), CPUBits::B64);
        emitTest(RAX, RAX, CPUBits::B64);

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
            emitStoreIndirect(RDI, offsetRT, cc.returnByRegisterFloat, CPUBits::F64);
        else
            emitStoreIndirect(RDI, offsetRT, cc.returnByRegisterInteger, CPUBits::B64);
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

void SCBE_X64::emitNot(CPURegister reg, CPUBits numBits)
{
    SWAG_ASSERT(reg < R8);

    emitREX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0xF6);
    else
        concat.addU8(0xF7);
    concat.addU8(0xD0 | (reg & 0b111));
}

void SCBE_X64::emitNotIndirect([[maybe_unused]] CPURegister memReg, uint32_t memOffset, CPUBits numBits)
{
    SWAG_ASSERT(memReg == RDI);

    emitREX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0xF6);
    else
        concat.addU8(0xF7);

    if (memOffset <= 0x7F)
    {
        concat.addU8(0x57);
        concat.addU8(static_cast<uint8_t>(memOffset));
    }
    else
    {
        concat.addU8(0x97);
        concat.addU32(memOffset);
    }
}

void SCBE_X64::emitIncIndirect(CPURegister memReg, uint32_t memOffset, CPUBits numBits)
{
    SWAG_ASSERT(memReg < R8);
    emitREX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0xFE);
    else
        concat.addU8(0xFF);
    emitModRM(concat, memOffset, 0, memReg);
}

void SCBE_X64::emitDecIndirect(CPURegister memReg, uint32_t memOffset, CPUBits numBits)
{
    SWAG_ASSERT(memReg < R8);
    emitREX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0xFE);
    else
        concat.addU8(0xFF);
    emitModRM(concat, memOffset, 1, memReg);
}

void SCBE_X64::emitNeg(CPURegister reg, CPUBits numBits)
{
    SWAG_ASSERT(reg < R8);
    SWAG_ASSERT(numBits == CPUBits::B32 || numBits == CPUBits::B64);

    emitREX(concat, numBits);
    concat.addU8(0xF7);
    concat.addU8(0xD8 | (reg & 0b111));
}

void SCBE_X64::emitNegIndirect([[maybe_unused]] CPURegister memReg, uint32_t memOffset, CPUBits numBits)
{
    SWAG_ASSERT(memReg == RDI);
    SWAG_ASSERT(numBits == CPUBits::B32 || numBits == CPUBits::B64);

    emitREX(concat, numBits);
    concat.addU8(0xF7);
    if (memOffset <= 0x7F)
    {
        concat.addU8(0x5F);
        concat.addU8(static_cast<uint8_t>(memOffset));
    }
    else
    {
        concat.addU8(0x9F);
        concat.addU32(memOffset);
    }
}

void SCBE_X64::emitCMov(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits)
{
    if (numBits == CPUBits::B8 || numBits == CPUBits::B16)
        numBits = CPUBits::B32;
    emitREX(concat, numBits, regDst, regSrc);
    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(op));
    concat.addU8(getModRM(RegReg, regDst, regSrc));
}

void SCBE_X64::emitRotate(CPURegister regDst, CPURegister regSrc, CPUOp op, CPUBits numBits)
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

void SCBE_X64::emitBSwap([[maybe_unused]] CPURegister reg, CPUBits numBits)
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
void SCBE_X64::emitMulAdd([[maybe_unused]] CPURegister regDst, [[maybe_unused]] CPURegister regMul, [[maybe_unused]] CPURegister regAdd, CPUBits numBits)
{
    SWAG_ASSERT(regDst == XMM0);
    SWAG_ASSERT(regMul == XMM1);
    SWAG_ASSERT(regAdd == XMM2);

    concat.addU8(numBits == CPUBits::F32 ? 0xF3 : 0xF2);
    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(CPUOp::FMUL));
    concat.addU8(0xC1);

    concat.addU8(numBits == CPUBits::F32 ? 0xF3 : 0xF2);
    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(CPUOp::FADD));
    concat.addU8(0xC2);
}
