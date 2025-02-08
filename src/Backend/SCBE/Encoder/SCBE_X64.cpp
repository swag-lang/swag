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

    void emitREX(Concat& concat, CPUBits numBits, CPUReg reg1 = CPUReg::RAX, CPUReg reg2 = CPUReg::RAX)
    {
        if (numBits == CPUBits::B16)
            concat.addU8(0x66);
        if (numBits == CPUBits::B64 || reg1 >= CPUReg::R8 || reg2 >= CPUReg::R8)
            concat.addU8(getREX(numBits == CPUBits::B64, reg1 >= CPUReg::R8, false, reg2 >= CPUReg::R8));
    }

    void emitModRM(Concat& concat, uint32_t memOffset, CPUReg reg, CPUReg memReg, uint8_t op = 1)
    {
        if (memOffset == 0 && (memReg < CPUReg::R8 || memReg == CPUReg::R12))
        {
            // mov al, byte ptr [rdi]
            concat.addU8(getModRM(0, static_cast<uint8_t>(reg), static_cast<uint8_t>(memReg)) | op - 1);
            if (memReg == CPUReg::RSP || memReg == CPUReg::R12)
                concat.addU8(0x24);
        }
        else if (memOffset <= 0x7F)
        {
            // mov al, byte ptr [rdi + ??]
            concat.addU8(getModRM(Disp8, static_cast<uint8_t>(reg), static_cast<uint8_t>(memReg)) | op - 1);
            if (memReg == CPUReg::RSP || memReg == CPUReg::R12)
                concat.addU8(0x24);
            concat.addU8(static_cast<uint8_t>(memOffset));
        }
        else
        {
            // mov al, byte ptr [rdi + ????????]
            concat.addU8(getModRM(Disp32, static_cast<uint8_t>(reg), static_cast<uint8_t>(memReg)) | op - 1);
            if (memReg == CPUReg::RSP || memReg == CPUReg::R12)
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

void SCBE_X64::emitCast(CPUReg regDst, CPUReg regSrc, CPUBits numBitsDst, CPUBits numBitsSrc, bool isSigned)
{
    if (numBitsSrc == CPUBits::B8 && numBitsDst == CPUBits::B64 && !isSigned)
    {
        emitREX(concat, CPUBits::B64, regDst, regSrc);
        concat.addU8(0x0F);
        concat.addU8(0xB6);
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(regDst), static_cast<uint8_t>(regSrc)));
    }
    else if (numBitsSrc == CPUBits::B16 && numBitsDst == CPUBits::B64 && !isSigned)
    {
        emitREX(concat, CPUBits::B64, regDst, regSrc);
        concat.addU8(0x0F);
        concat.addU8(0xB7);
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(regDst), static_cast<uint8_t>(regSrc)));
    }
    else if (numBitsSrc == CPUBits::B64 && numBitsDst == CPUBits::F64 && !isSigned)
    {
        SWAG_ASSERT(regSrc == CPUReg::RAX && regDst == CPUReg::XMM0);
        concat.addString5("\x66\x48\x0F\x6E\xC8"); // movq xmm1, rax
        emitSymbolRelocationAddr(CPUReg::RCX, symCst_U64F64, 0);
        concat.addString4("\x66\x0F\x62\x09");     // punpckldq xmm1, xmmword ptr [rcx]
        concat.addString5("\x66\x0F\x5C\x49\x10"); // subpd xmm1, xmmword ptr [rcx + 16]
        concat.addString4("\x66\x0F\x28\xC1");     // movapd xmm0, xmm1
        concat.addString4("\x66\x0F\x15\xC1");     // unpckhpd xmm0, xmm1
        concat.addString4("\xF2\x0F\x58\xC1");     // addsd xmm0, xmm1
    }
    else
    {
        SWAG_ASSERT(false);
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

void SCBE_X64::emitSymbolRelocationAddr(CPUReg reg, uint32_t symbolIndex, uint32_t offset)
{
    SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RDX || reg == CPUReg::R8 || reg == CPUReg::R9 || reg == CPUReg::RDI);
    emitREX(concat, CPUBits::B64, reg);
    concat.addU8(0x8D);
    concat.addU8(static_cast<uint8_t>(0x05 | ((static_cast<uint8_t>(reg) & 0b111)) << 3));
    addSymbolRelocation(concat.totalCount() - textSectionOffset, symbolIndex, IMAGE_REL_AMD64_REL32);
    concat.addU32(offset);
}

void SCBE_X64::emitSymbolRelocationValue(CPUReg reg, uint32_t symbolIndex, uint32_t offset)
{
    SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RDX || reg == CPUReg::R8 || reg == CPUReg::R9);
    emitREX(concat, CPUBits::B64, reg);
    concat.addU8(0x8B);
    concat.addU8(static_cast<uint8_t>(0x05 | ((static_cast<uint8_t>(reg) & 0b111)) << 3));
    addSymbolRelocation(concat.totalCount() - textSectionOffset, symbolIndex, IMAGE_REL_AMD64_REL32);
    concat.addU32(offset);
}

void SCBE_X64::emitSymbolGlobalString(CPUReg reg, const Utf8& str)
{
    emitLoad(reg, 0, CPUBits::B64, true);
    const auto sym = getOrCreateGlobalString(str);
    addSymbolRelocation(concat.totalCount() - 8 - textSectionOffset, sym->index, IMAGE_REL_AMD64_ADDR64);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitPush(CPUReg reg)
{
    if (reg < CPUReg::R8)
        concat.addU8(0x50 | (static_cast<uint8_t>(reg) & 0b111));
    else
    {
        concat.addU8(getREX(false, false, false, true));
        concat.addU8(0x50 | (static_cast<uint8_t>(reg) & 0b111));
    }
}

void SCBE_X64::emitPop(CPUReg reg)
{
    if (reg < CPUReg::R8)
        concat.addU8(0x58 | (static_cast<uint8_t>(reg) & 0b111));
    else
    {
        concat.addU8(getREX(false, false, false, true));
        concat.addU8(0x58 | (static_cast<uint8_t>(reg) & 0b111));
    }
}

void SCBE_X64::emitRet()
{
    concat.addU8(0xC3);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitLoad(CPUReg reg, CPUReg memReg, uint32_t memOffset, uint64_t value, bool isImmediate, CPUOp op, CPUBits numBits)
{
    if (op == CPUOp::DIV || op == CPUOp::MOD || op == CPUOp::IDIV || op == CPUOp::IMOD)
    {
        SWAG_ASSERT(reg == CPUReg::RAX);
        if (numBits == CPUBits::B8)
        {
            if (isImmediate)
                emitLoad(CPUReg::RAX, value, CPUBits::B32);
            else
                emitLoad(CPUReg::RAX, memReg, memOffset, CPUBits::B32, CPUBits::B8, op == CPUOp::IDIV || op == CPUOp::IMOD);
        }
        else
        {
            if (isImmediate)
                emitLoad(CPUReg::RAX, value, numBits);
            else
                emitLoad(CPUReg::RAX, memReg, memOffset, numBits);

            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
            {
                emitREX(concat, numBits);
                concat.addU8(static_cast<uint8_t>(CPUOp::CDQ));                
            }
            else
            {
                emitClear(CPUReg::RDX, numBits);
            }
        }
    }
    else if (isImmediate)
    {
        emitLoad(reg, value, numBits);
    }
    else
    {
        emitLoad(reg, memReg, memOffset, numBits);
    }
}

void SCBE_X64::emitLoad(CPUReg reg, uint64_t value, CPUBits numBits, bool force64Bits)
{
    if (force64Bits)
    {
        SWAG_ASSERT(numBits == CPUBits::B64);
        emitREX(concat, CPUBits::B64, CPUReg::RAX, reg);
        concat.addU8(0xB8 | static_cast<uint8_t>(reg));
        concat.addU64(value);
        return;
    }

    if (value == 0)
    {
        emitClear(reg, numBits);
        return;
    }

    switch (numBits)
    {
        case CPUBits::B8:
            concat.addU8(0xB0 | static_cast<uint8_t>(reg));
            concat.addU8(static_cast<uint8_t>(value));
            break;

        case CPUBits::B16:
            concat.addU8(0x66);
            concat.addU8(0xB8 | static_cast<uint8_t>(reg));
            concat.addU16(static_cast<uint16_t>(value));
            break;

        case CPUBits::B32:
            concat.addU8(0xB8 | static_cast<uint8_t>(reg));
            concat.addU32(static_cast<uint32_t>(value));
            break;

        case CPUBits::B64:
            if (value <= 0x7FFFFFFF && reg < CPUReg::R8)
            {
                concat.addU8(0xB8 | static_cast<uint8_t>(reg));
                concat.addU32(static_cast<uint32_t>(value));
            }
            else if (value <= 0x7FFFFFFF)
            {
                emitREX(concat, CPUBits::B64, CPUReg::RAX, reg);
                concat.addU8(0xC7);
                concat.addU8(0xC0 | (static_cast<uint8_t>(reg) & 0b111));
                concat.addU32(static_cast<uint32_t>(value));
            }
            else
            {
                emitREX(concat, CPUBits::B64, CPUReg::RAX, reg);
                concat.addU8(0xB8 | static_cast<uint8_t>(reg));
                concat.addU64(value);
            }
            break;

        case CPUBits::F32:
            concat.addU8(0xB8 | static_cast<int>(CPUReg::RAX));
            concat.addU32(static_cast<uint32_t>(value));
            emitCopy(reg, CPUReg::RAX, CPUBits::F32);
            break;

        case CPUBits::F64:
            emitREX(concat, CPUBits::B64, CPUReg::RAX, CPUReg::RAX);
            if (value <= 0x7FFFFFFF)
            {
                concat.addU8(0xC7);
                concat.addU8(0xC0 | (static_cast<int>(CPUReg::RAX) & 0b111));
                concat.addU32(static_cast<uint32_t>(value));
            }
            else
            {
                concat.addU8(0xB8 | static_cast<int>(CPUReg::RAX));
                concat.addU64(value);
            }
            emitCopy(reg, CPUReg::RAX, CPUBits::F64);
            break;

        default:
            SWAG_ASSERT(false);
            break;
    }
}

void SCBE_X64::emitLoad(CPUReg reg, CPUReg memReg, uint32_t memOffset, CPUBits numBitsDst, CPUBits numBitsSrc, bool isSigned)
{
    if (numBitsSrc == numBitsDst)
    {
        emitLoad(reg, memReg, memOffset, numBitsSrc);
        return;
    }

    if (numBitsSrc == CPUBits::B8)
    {
        switch (numBitsDst)
        {
            case CPUBits::B16:
                if (isSigned)
                {
                    emitREX(concat, CPUBits::B16, reg, memReg);
                    concat.addU8(0x0F);
                    concat.addU8(0xBE);
                    emitModRM(concat, memOffset, reg, memReg);
                    return;
                }
                break;
            case CPUBits::B32:
                if (isSigned)
                {
                    emitREX(concat, CPUBits::B32, reg, memReg);
                    concat.addU8(0x0F);
                    concat.addU8(0xBE);
                    emitModRM(concat, memOffset, reg, memReg);
                    return;
                }

                emitREX(concat, CPUBits::B32, reg, memReg);
                concat.addU8(0x0F);
                concat.addU8(0xB6);
                emitModRM(concat, memOffset, reg, memReg);
                return;

            case CPUBits::B64:
                if (isSigned)
                {
                    emitREX(concat, CPUBits::B64, reg, memReg);
                    concat.addU8(0x0F);
                    concat.addU8(0xBE);
                    emitModRM(concat, memOffset, reg, memReg);
                    return;
                }

                emitREX(concat, CPUBits::B64, reg, memReg);
                concat.addU8(0x0F);
                concat.addU8(0xB6);
                emitModRM(concat, memOffset, reg, memReg);
                return;
        }
    }
    else if (numBitsSrc == CPUBits::B16)
    {
        switch (numBitsDst)
        {
            case CPUBits::B32:
                if (isSigned)
                {
                    emitREX(concat, CPUBits::B32, reg, memReg);
                    concat.addU8(0x0F);
                    concat.addU8(0xBF);
                    emitModRM(concat, memOffset, reg, memReg);
                    return;
                }

                emitREX(concat, CPUBits::B32, reg, memReg);
                concat.addU8(0x0F);
                concat.addU8(0xB7);
                emitModRM(concat, memOffset, reg, memReg);
                return;

            case CPUBits::B64:
                if (isSigned)
                {
                    emitREX(concat, CPUBits::B64, reg, memReg);
                    concat.addU8(0x0F);
                    concat.addU8(0xBF);
                    emitModRM(concat, memOffset, reg, memReg);
                    return;
                }

                emitREX(concat, CPUBits::B64, reg, memReg);
                concat.addU8(0x0F);
                concat.addU8(0xB7);
                emitModRM(concat, memOffset, reg, memReg);
                return;
        }
    }
    else if (numBitsSrc == CPUBits::B32)
    {
        if (isSigned)
        {
            emitREX(concat, CPUBits::B64, reg, memReg);
            concat.addU8(0x63);
            emitModRM(concat, memOffset, reg, memReg);
            return;
        }
    }

    SWAG_ASSERT(false);
}

void SCBE_X64::emitLoad(CPUReg reg, CPUReg memReg, uint32_t memOffset, CPUBits numBits)
{
    if (isInt(numBits) &&
        storageRegStack == memOffset &&
        storageReg == reg &&
        storageMemReg == memReg &&
        storageRegBits >= countBits(numBits) &&
        storageRegCount == concat.totalCount())
    {
        if (numBits == CPUBits::B32 && storageRegBits > 32)
            emitCopy(CPUReg::RAX, CPUReg::RAX, CPUBits::B32);
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
            SWAG_ASSERT(reg < CPUReg::R8 && memReg < CPUReg::R8);
            concat.addU8(0xF3);
            concat.addU8(0x0F);
            concat.addU8(0x10);
            emitModRM(concat, memOffset, reg, memReg);
            break;

        case CPUBits::F64:
            SWAG_ASSERT(reg < CPUReg::R8 && memReg < CPUReg::R8);
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

void SCBE_X64::emitSetAddress(CPUReg reg, CPUReg memReg, uint32_t memOffset)
{
    if (memReg == CPUReg::RIP)
    {
        SWAG_ASSERT(memOffset == 0);
        SWAG_ASSERT(reg == CPUReg::RCX);
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

void SCBE_X64::emitStore(CPUReg memReg, uint32_t memOffset, CPUReg reg, CPUBits numBits)
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
            SWAG_ASSERT(reg < CPUReg::R8 && memReg < CPUReg::R8);
            concat.addU8(0xF3);
            concat.addU8(0x0F);
            concat.addU8(0x11);
            emitModRM(concat, memOffset, reg, memReg);
            break;

        case CPUBits::F64:
            SWAG_ASSERT(reg < CPUReg::R8 && memReg < CPUReg::R8);
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

void SCBE_X64::emitStore(CPUReg memReg, uint32_t memOffset, uint64_t value, CPUBits numBits)
{
    if (numBits == CPUBits::B8)
    {
        concat.addU8(0xC6);
        emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg);
        concat.addU8(static_cast<uint8_t>(value));
    }
    else if (numBits == CPUBits::B16)
    {
        concat.addU8(0x66);
        concat.addU8(0xC7);
        emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg);
        concat.addU16(static_cast<uint16_t>(value));
    }
    else if (numBits == CPUBits::B32)
    {
        concat.addU8(0xC7);
        emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg);
        concat.addU32(static_cast<uint32_t>(value));
    }
    else
    {
        SWAG_ASSERT(numBits == CPUBits::B64);
        SWAG_ASSERT(value <= 0xFFFFFFFF);
        emitREX(concat, CPUBits::B64);
        concat.addU8(0xC7);
        emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg);
        concat.addU32(static_cast<uint32_t>(value));
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitClear(CPUReg reg, CPUBits numBits)
{
    if (numBits == CPUBits::F32)
    {
        emitClear(CPUReg::RAX, CPUBits::B32);
        emitCopy(reg, CPUReg::RAX, CPUBits::F32);
    }
    else if (numBits == CPUBits::F64)
    {
        emitClear(CPUReg::RAX, CPUBits::B64);
        emitCopy(reg, CPUReg::RAX, CPUBits::F64);
    }
    else
    {
        emitREX(concat, numBits, reg, reg);
        emitSpec8(concat, static_cast<uint8_t>(CPUOp::XOR), numBits);
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(reg), static_cast<uint8_t>(reg)));
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitCopy(CPUReg regDst, CPUReg regSrc, CPUBits numBits)
{
    if (numBits == CPUBits::F32)
    {
        SWAG_ASSERT(regSrc == CPUReg::RAX);
        SWAG_ASSERT(regDst == CPUReg::XMM0 || regDst == CPUReg::XMM1 || regDst == CPUReg::XMM2 || regDst == CPUReg::XMM3);
        concat.addU8(0x66);
        concat.addU8(0x0F);
        concat.addU8(0x6E);
        concat.addU8(static_cast<uint8_t>(0xC0 | static_cast<uint8_t>(regDst) << 3));
    }
    else if (numBits == CPUBits::F64)
    {
        SWAG_ASSERT(regSrc == CPUReg::RAX);
        SWAG_ASSERT(regDst == CPUReg::XMM0 || regDst == CPUReg::XMM1 || regDst == CPUReg::XMM2 || regDst == CPUReg::XMM3);
        concat.addU8(0x66);
        emitREX(concat, CPUBits::B64);
        concat.addU8(0x0F);
        concat.addU8(0x6E);
        concat.addU8(static_cast<uint8_t>(0xC0 | static_cast<uint8_t>(regDst) << 3));
    }
    else
    {
        emitREX(concat, numBits, regSrc, regDst);
        emitSpec8(concat, 0x89, numBits);
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(regSrc), static_cast<uint8_t>(regDst)));
    }
}

void SCBE_X64::emitCopy([[maybe_unused]] CPUReg reg, [[maybe_unused]] CPUBits numBits)
{
    SWAG_ASSERT(reg == CPUReg::RAX);
    SWAG_ASSERT(numBits == CPUBits::B8);
    concat.addString2("\x88\xe0"); // mov al, ah
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitSet(CPUReg reg, CPUSet setType)
{
    switch (setType)
    {
        case CPUSet::SetA:
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::R8);
            if (reg >= CPUReg::R8)
                concat.addU8(0x41);
            concat.addU8(0x0F);
            concat.addU8(0x97);
            concat.addU8(0xC0 | (static_cast<uint8_t>(reg) & 0b111));
            break;

        case CPUSet::SetAE:
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::R8);
            if (reg >= CPUReg::R8)
                concat.addU8(0x41);
            concat.addU8(0x0F);
            concat.addU8(0x93);
            concat.addU8(0xC0 | (static_cast<uint8_t>(reg) & 0b111));
            break;

        case CPUSet::SetG:
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::R8);
            if (reg >= CPUReg::R8)
                concat.addU8(0x41);
            concat.addU8(0x0F);
            concat.addU8(0x9F);
            concat.addU8(0xC0 | (static_cast<uint8_t>(reg) & 0b111));
            break;

        case CPUSet::SetNE:
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x0F);
            concat.addU8(0x95);
            concat.addU8(0xC0);
            break;

        case CPUSet::SetNA:
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x0F);
            concat.addU8(0x96);
            concat.addU8(0xC0);
            break;

        case CPUSet::SetB:
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x0F);
            concat.addU8(0x92);
            concat.addU8(0xC0);
            break;

        case CPUSet::SetBE:
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x0F);
            concat.addU8(0x96);
            concat.addU8(0xC0);
            break;

        case CPUSet::SetE:
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x0F);
            concat.addU8(0x94);
            concat.addU8(0xC0);
            break;

        case CPUSet::SetEP:
            SWAG_ASSERT(reg == CPUReg::RAX);

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
            SWAG_ASSERT(reg == CPUReg::RAX);

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
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x0F);
            concat.addU8(0x9D);
            concat.addU8(0xC0);
            break;

        case CPUSet::SetL:
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x0F);
            concat.addU8(0x9C);
            concat.addU8(0xC0);
            break;

        case CPUSet::SetLE:
            SWAG_ASSERT(reg == CPUReg::RAX);
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

void SCBE_X64::emitTest(CPUReg reg0, CPUReg reg1, CPUBits numBits)
{
    emitREX(concat, numBits, reg0, reg1);
    emitSpec8(concat, static_cast<uint8_t>(CPUOp::TEST), numBits);
    concat.addU8(getModRM(RegReg, static_cast<uint8_t>(reg0), static_cast<uint8_t>(reg1)));
}

void SCBE_X64::emitCmp(CPUReg reg0, CPUReg reg1, CPUBits numBits)
{
    if (numBits == CPUBits::F32)
    {
        SWAG_ASSERT(reg0 < CPUReg::R8 && reg1 < CPUReg::R8);
        concat.addU8(0x0F);
        concat.addU8(0x2F);
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(reg0), static_cast<uint8_t>(reg1)));
    }
    else if (numBits == CPUBits::F64)
    {
        SWAG_ASSERT(reg0 < CPUReg::R8 && reg1 < CPUReg::R8);
        concat.addU8(0x66);
        concat.addU8(0x0F);
        concat.addU8(0x2F);
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(reg0), static_cast<uint8_t>(reg1)));
    }
    else
    {
        emitREX(concat, numBits, reg1, reg0);
        emitSpec8(concat, static_cast<uint8_t>(CPUOp::CMP), numBits);
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(reg1), static_cast<uint8_t>(reg0)));
    }
}

void SCBE_X64::emitCmp(CPUReg reg, uint64_t value, CPUBits numBits)
{
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));
    if (value <= 0x7f)
    {
        SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX);
        emitREX(concat, numBits);
        concat.addU8(0x83);
        concat.addU8(0xF8 | static_cast<uint8_t>(reg));
        concat.addU8(static_cast<uint8_t>(value));
    }
    else if (value <= 0x7fffffff)
    {
        SWAG_ASSERT(reg == CPUReg::RAX);
        emitREX(concat, numBits);
        concat.addU8(0x3d);
        concat.addU32(static_cast<uint32_t>(value));
    }
    else
    {
        SWAG_ASSERT(reg == CPUReg::RAX);
        emitLoad(CPUReg::RCX, value, numBits);
        emitCmp(reg, CPUReg::RCX, numBits);
    }
}

void SCBE_X64::emitCmp(CPUReg memReg, uint32_t memOffset, CPUReg reg, CPUBits numBits)
{
    if (numBits == CPUBits::F32)
    {
        SWAG_ASSERT(reg < CPUReg::R8 && memReg < CPUReg::R8);
        concat.addU8(0x0F);
        concat.addU8(0x2E);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (numBits == CPUBits::F64)
    {
        SWAG_ASSERT(reg < CPUReg::R8 && memReg < CPUReg::R8);
        concat.addU8(0x66);
        concat.addU8(0x0F);
        concat.addU8(0x2F);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else
    {
        SWAG_ASSERT(reg < CPUReg::R8 && memReg < CPUReg::R8);
        emitREX(concat, numBits);
        emitSpec8(concat, 0x3B, numBits);
        emitModRM(concat, memOffset, reg, memReg);
    }
}

void SCBE_X64::emitCmp(CPUReg memReg, uint32_t memOffset, uint64_t value, CPUBits numBits)
{
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));

    if (value > 0x7FFFFFFF)
    {
        emitLoad(CPUReg::RAX, memReg, memOffset, numBits);
        emitCmp(CPUReg::RAX, value, numBits);
    }
    else if (numBits == CPUBits::B8)
    {
        emitREX(concat, numBits);
        concat.addU8(0x80);
        emitModRM(concat, memOffset, memReg, memReg, 0x39);
        concat.addU8(static_cast<uint8_t>(value));
    }
    else if (value <= 0x7F)
    {
        emitREX(concat, numBits);
        concat.addU8(0x83);
        emitModRM(concat, memOffset, memReg, memReg, 0x39);
        concat.addU8(static_cast<uint8_t>(value));
    }
    else
    {
        emitREX(concat, numBits);
        concat.addU8(0x81);
        emitModRM(concat, memOffset, memReg, memReg, 0x39);
        if (numBits == CPUBits::B16)
            concat.addU16(static_cast<uint16_t>(value));
        else
            concat.addU32(static_cast<uint32_t>(value));
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitOp(CPUReg regDst, CPUReg regSrc, CPUOp op, CPUBits numBits, CPUBits srcBits)
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
        concat.addU8(static_cast<uint8_t>(0xC0 | static_cast<uint8_t>(regSrc) | static_cast<uint8_t>(regDst) << 3));
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
        concat.addU8(static_cast<uint8_t>(0xC0 | static_cast<uint8_t>(regSrc) | static_cast<uint8_t>(regDst) << 3));
    }
    else if (op == CPUOp::DIV ||
             op == CPUOp::IDIV)
    {
        SWAG_ASSERT(regDst == CPUReg::RAX && regSrc == CPUReg::RCX);
        emitREX(concat, numBits, regSrc, regDst);
        emitSpec8(concat, 0xF7, numBits);
        concat.addU8(static_cast<uint8_t>(op) & ~2);
    }
    else if (op == CPUOp::MOD ||
             op == CPUOp::IMOD)
    {
        SWAG_ASSERT(regDst == CPUReg::RAX && regSrc == CPUReg::RCX);
        emitREX(concat, numBits, regSrc, regDst);
        emitSpec8(concat, 0xF7, numBits);
        concat.addU8(static_cast<uint8_t>(op) & ~2);
        if (numBits == CPUBits::B8)
            emitCopy(CPUReg::RAX, numBits);
        else
            emitCopy(CPUReg::RAX, CPUReg::RDX, numBits);
    }
    else if (op == CPUOp::MUL ||
             op == CPUOp::IMUL)
    {
        SWAG_ASSERT(regDst == CPUReg::RAX && regSrc == CPUReg::RCX);
        emitREX(concat, numBits, regSrc, regDst);
        emitSpec8(concat, 0xF7, numBits);
        if (op == CPUOp::MUL)
            concat.addU8(0xE1);
        else if (op == CPUOp::IMUL)
            concat.addU8(0xE9);
    }
    else if (op == CPUOp::SAR ||
             op == CPUOp::SAL ||
             op == CPUOp::SHR ||
             op == CPUOp::SHL)
    {
        SWAG_ASSERT(regDst == CPUReg::RAX && regSrc == CPUReg::RCX);
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
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(regSrc), static_cast<uint8_t>(regDst)));
    }
    else if (op == CPUOp::BSF ||
             op == CPUOp::BSR)
    {
        SWAG_ASSERT(regSrc == CPUReg::RAX && regDst == CPUReg::RAX);
        emitREX(concat, numBits, regSrc, regDst);
        concat.addU8(0x0F);
        concat.addU8(static_cast<uint8_t>(op));
        concat.addU8(0xC0);
    }
    else if (op == CPUOp::POPCNT)
    {
        SWAG_ASSERT(regSrc == CPUReg::RAX && regDst == CPUReg::RAX);
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

void SCBE_X64::emitOp(CPUReg memReg, uint32_t memOffset, CPUReg reg, CPUOp op, CPUBits numBits, bool lock)
{
    if (numBits == CPUBits::F32 ||
        numBits == CPUBits::F64)
    {
        SWAG_ASSERT(reg == CPUReg::XMM1);
        SWAG_ASSERT(memReg < CPUReg::R8);
        emitLoad(CPUReg::XMM0, memReg, memOffset, numBits);
        emitOp(CPUReg::XMM0, reg, op, numBits, numBits);
        emitStore(memReg, memOffset, CPUReg::XMM0, numBits);
    }
    else if (op == CPUOp::IMUL ||
             op == CPUOp::MUL)
    {
        SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
        SWAG_ASSERT(reg == CPUReg::RCX);
        emitCopy(CPUReg::R8, memReg, CPUBits::B64);
        emitLoad(CPUReg::RAX, memReg, memOffset, numBits);
        emitOp(CPUReg::RAX, reg, op, numBits);
        emitStore(CPUReg::R8, memOffset, CPUReg::RAX, numBits);
    }
    else if (op == CPUOp::DIV ||
             op == CPUOp::IDIV ||
             op == CPUOp::MOD ||
             op == CPUOp::IMOD)
    {
        SWAG_ASSERT(memReg == CPUReg::RAX);
        SWAG_ASSERT(reg == CPUReg::RCX);
        emitCopy(CPUReg::R8, memReg, CPUBits::B64);
        emitLoad(CPUReg::RAX, memReg, memOffset, 0, false, op, numBits);
        emitOp(CPUReg::RAX, reg, op, numBits);
        emitStore(CPUReg::R8, memOffset, CPUReg::RAX, numBits);
    }
    else if (op == CPUOp::SAR ||
             op == CPUOp::SHR ||
             op == CPUOp::SHL)
    {
        SWAG_ASSERT(memReg == CPUReg::RAX && reg == CPUReg::RCX);
        if (lock)
            concat.addU8(0xF0);
        emitREX(concat, numBits, reg, memReg);

        if (numBits == CPUBits::B8)
            concat.addU8(0xD2);
        else
            concat.addU8(0xD3);
        concat.addU8(static_cast<uint8_t>(op) & ~0xC0);
    }
    else
    {
        SWAG_ASSERT(memReg < CPUReg::R8);
        if (lock)
            concat.addU8(0xF0);
        emitREX(concat, numBits, reg, memReg);
        emitSpec8(concat, static_cast<uint8_t>(op), numBits);
        emitModRM(concat, memOffset, reg, memReg);
    }
}

void SCBE_X64::emitOp(CPUReg reg, uint64_t value, CPUOp op, CPUBits numBits)
{
    if (value > 0x7FFFFFFF)
    {
        SWAG_ASSERT(reg == CPUReg::RAX);
        emitLoad(CPUReg::RCX, value, CPUBits::B64);
        emitOp(reg, CPUReg::RCX, op, numBits);
    }
    else if (op == CPUOp::XOR || op == CPUOp::OR || op == CPUOp::AND)
    {
        SWAG_ASSERT(reg == CPUReg::RAX);
        emitREX(concat, numBits);
        if (numBits == CPUBits::B8)
        {
            concat.addU8(static_cast<uint8_t>(op) + 3);
            concat.addU8(static_cast<uint8_t>(value));
        }
        else if (value <= 0x7F)
        {
            concat.addU8(0x83);
            concat.addU8(static_cast<uint8_t>(op) + 0xBF);
            concat.addU8(static_cast<uint8_t>(value));
        }
        else if (numBits == CPUBits::B16)
        {
            concat.addU8(0x81);
            concat.addU8(static_cast<uint8_t>(op) + 0xBF);
            concat.addU16(static_cast<uint16_t>(value));
        }
        else if (numBits == CPUBits::B32 || numBits == CPUBits::B64)
        {
            concat.addU8(0x81);
            concat.addU8(static_cast<uint8_t>(op) + 0xBF);
            concat.addU32(static_cast<uint32_t>(value));
        }
    }
    else if (op == CPUOp::ADD)
    {
        if (value == 0)
            return;
        SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RSP);
        emitREX(concat, numBits);
        if (value == 1)
        {
            emitSpec8(concat, 0xFF, numBits);
            concat.addU8(0xC0 | static_cast<uint8_t>(reg)); // inc rax
        }
        else if (numBits == CPUBits::B8)
        {
            if (reg == CPUReg::RAX)
                concat.addU8(0x04);
            else
            {
                concat.addU8(0x80);
                concat.addU8(0xC0 | static_cast<uint8_t>(reg));
            }
            concat.addU8(static_cast<uint8_t>(value));
        }
        else if (value <= 0x7F)
        {
            concat.addU8(0x83);
            concat.addU8(0xC0 | static_cast<uint8_t>(reg));
            concat.addU8(static_cast<uint8_t>(value));
        }
        else
        {
            if (reg == CPUReg::RAX)
                concat.addU8(0x05);
            else
            {
                concat.addU8(0x81);
                concat.addU8(0xC0 | static_cast<uint8_t>(reg));
            }
            if (numBits == CPUBits::B16)
                concat.addU16(static_cast<uint16_t>(value));
            else
                concat.addU32(static_cast<uint32_t>(value));
        }
    }
    else if (op == CPUOp::SUB)
    {
        if (value == 0)
            return;
        SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RSP);
        emitREX(concat, numBits);
        if (value == 1)
        {
            emitSpec8(concat, 0xFF, numBits);
            concat.addU8(0xC8 | static_cast<uint8_t>(reg)); // dec rax
        }
        else if (numBits == CPUBits::B8)
        {
            if (reg == CPUReg::RAX)
                concat.addU8(0x2C);
            else
            {
                concat.addU8(0x80);
                concat.addU8(0xE8 | static_cast<uint8_t>(reg));
            }
            concat.addU8(static_cast<uint8_t>(value));
        }
        else if (value <= 0x7F)
        {
            concat.addU8(0x83);
            concat.addU8(0xE8 | static_cast<uint8_t>(reg));
            concat.addU8(static_cast<uint8_t>(value));
        }
        else
        {
            if (reg == CPUReg::RAX)
                concat.addU8(0x2D);
            else
            {
                concat.addU8(0x81);
                concat.addU8(0xE8 | static_cast<uint8_t>(reg));
            }
            if (numBits == CPUBits::B16)
                concat.addU16(static_cast<uint16_t>(value));
            else
                concat.addU32(static_cast<uint32_t>(value));
        }
    }
    else if (op == CPUOp::IMUL && numBits != CPUBits::B8)
    {
        if (value == 1)
            return;
        if (value == 0)
        {
            emitClear(reg, numBits);
            return;
        }

        SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX);
        emitREX(concat, numBits);
        if (value == 2)
        {
            concat.addU8(0xD1);
            concat.addU8(0xE0 | static_cast<uint8_t>(reg)); // shl rax, 1
        }
        else if (value <= 0x7F && Math::isPowerOfTwo(value))
        {
            concat.addU8(0xC1);
            concat.addU8(0xE0 | static_cast<uint8_t>(reg)); // shl rax, ??
            concat.addU8(static_cast<uint8_t>(log2(value)));
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x6B);
            concat.addU8(0xC0);
            concat.addU8(static_cast<uint8_t>(value));
        }
        else
        {
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x69);
            concat.addU8(0xC0);
            concat.addU32(static_cast<uint32_t>(value));
        }
    }
    else if (op == CPUOp::SAR || op == CPUOp::SHR || op == CPUOp::SHL)
    {
        SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX);
        emitREX(concat, numBits);
        if (value == 1)
        {
            emitSpec8(concat, 0xD1, numBits);
            concat.addU8(static_cast<uint8_t>(op) | static_cast<uint8_t>(reg));
        }
        else if (value <= 0x7F)
        {
            value = min(value, SCBE_CPU::countBits(numBits) - 1);
            emitSpec8(concat, 0xC1, numBits);
            concat.addU8(static_cast<uint8_t>(op) | static_cast<uint8_t>(reg));
            concat.addU8(static_cast<uint8_t>(value));
        }
        else
        {
            SWAG_ASSERT(false);
        }
    }
    else if (op == CPUOp::BT)
    {
        if (value <= 0x7F)
        {
            emitREX(concat, numBits, CPUReg::RAX, reg);
            concat.addU8(0x0F);
            concat.addU8(static_cast<uint8_t>(op));
            concat.addU8(0xE2);
            concat.addU8(static_cast<uint8_t>(value));
        }
        else
        {
            SWAG_ASSERT(false);
        }
    }
    else
    {
        SWAG_ASSERT(reg == CPUReg::RAX);
        emitLoad(CPUReg::RCX, value, numBits);
        emitOp(reg, CPUReg::RCX, op, numBits);
    }
}

void SCBE_X64::emitOp(CPUReg memReg, uint32_t memOffset, uint64_t value, CPUOp op, CPUBits numBits)
{
    SWAG_ASSERT(SCBE_CPU::isInt(numBits));

    if (op == CPUOp::IDIV ||
        op == CPUOp::IMOD ||
        op == CPUOp::DIV ||
        op == CPUOp::MOD)
    {
        SWAG_ASSERT(memReg == CPUReg::RAX);
        emitCopy(CPUReg::R8, memReg, CPUBits::B64);
        emitLoad(CPUReg::RAX, memReg, memOffset, 0, false, op, numBits);
        emitLoad(CPUReg::RCX, value, numBits);
        emitOp(CPUReg::RAX, CPUReg::RCX, op, numBits);
        emitStore(CPUReg::R8, memOffset, CPUReg::RAX, numBits);
    }
    else if (op == CPUOp::IMUL ||
             op == CPUOp::MUL)
    {
        SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
        emitLoad(CPUReg::RCX, value, numBits);
        emitOp(memReg, memOffset, CPUReg::RCX, op, numBits);
    }
    else if (value > 0x7FFFFFFF)
    {
        SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
        emitLoad(CPUReg::RCX, value, numBits);
        emitOp(memReg, memOffset, CPUReg::RCX, op, numBits);
    }
    else if (op == CPUOp::SAR ||
             op == CPUOp::SHR ||
             op == CPUOp::SHL)
    {
        SWAG_ASSERT(memReg == CPUReg::RAX);
        value = min(value, SCBE_CPU::countBits(numBits) - 1);

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
    else
    {
        SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
        emitREX(concat, numBits);
        if (numBits == CPUBits::B8)
            concat.addU8(0x80);
        else if (value <= 0x7F)
            concat.addU8(0x83);
        else
            concat.addU8(0x81);
        if (memOffset == 0)
        {
            concat.addU8(static_cast<uint8_t>(op) - 1 + static_cast<uint8_t>(memReg));
        }
        else if (memOffset <= 0x7F)
        {
            concat.addU8(0x3F + static_cast<uint8_t>(memReg) + static_cast<uint8_t>(op));
            concat.addU8(static_cast<uint8_t>(memOffset));
        }
        else
        {
            concat.addU8(0x7F + static_cast<uint8_t>(memReg) + static_cast<uint8_t>(op));
            concat.addU32(memOffset);
        }

        if (value <= 0x7F || numBits == CPUBits::B8)
            concat.addU8(static_cast<uint8_t>(value));
        else if (numBits == CPUBits::B16)
            concat.addU16(static_cast<uint16_t>(value));
        else
            concat.addU32(static_cast<uint32_t>(value));
    }
}

/////////////////////////////////////////////////////////////////////

uint8_t* SCBE_X64::emitJumpNear(CPUJumpType jumpType)
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

uint32_t* SCBE_X64::emitJumpLong(CPUJumpType jumpType)
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

void SCBE_X64::emitJumpTable([[maybe_unused]] CPUReg table, [[maybe_unused]] CPUReg offset)
{
    SWAG_ASSERT(table == CPUReg::RCX && offset == CPUReg::RAX);
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
            const auto offsetPtr = emitJumpNear(jumpType);
            *offsetPtr           = static_cast<uint8_t>(it->second - concat.totalCount());
        }
        else
        {
            const auto offsetPtr = emitJumpLong(jumpType);
            *offsetPtr           = it->second - concat.totalCount();
        }

        return;
    }

    // Here we do not know the destination label, so we assume 32 bits of offset
    label.patch         = reinterpret_cast<uint8_t*>(emitJumpLong(jumpType));
    label.currentOffset = static_cast<int32_t>(concat.totalCount());
    labelsToSolve.push_back(label);
}

void SCBE_X64::emitJump([[maybe_unused]] CPUReg reg)
{
    SWAG_ASSERT(reg == CPUReg::RAX);
    concat.addString2("\xFF\xE0"); // jmp rax
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitCopy(CPUReg regDst, CPUReg regSrc, uint32_t count, uint32_t offset)
{
    if (!count)
        return;
    SWAG_ASSERT(regDst == CPUReg::RCX);
    SWAG_ASSERT(regSrc == CPUReg::RDX);

    // SSE 16 octets
    if (count >= 16)
    {
        while (count >= 16)
        {
            concat.addString2("\x0F\x10"); // movups xmm0, [rdx+??]
            if (offset <= 0x7F)
            {
                concat.addU8(0x40 | static_cast<uint8_t>(regSrc));
                concat.addU8(static_cast<uint8_t>(offset));
            }
            else
            {
                concat.addU8(0x80 | static_cast<uint8_t>(regSrc));
                concat.addU32(offset);
            }
            concat.addString2("\x0F\x11"); // movups [rcx+??], xmm0
            if (offset <= 0x7F)
            {
                concat.addU8(0x40 | static_cast<uint8_t>(regDst));
                concat.addU8(static_cast<uint8_t>(offset));
            }
            else
            {
                concat.addU8(0x80 | static_cast<uint8_t>(regDst));
                concat.addU32(offset);
            }

            count -= 16;
            offset += 16;
        }
    }

    while (count >= 8)
    {
        emitLoad(CPUReg::RAX, regSrc, offset, CPUBits::B64);
        emitStore(regDst, offset, CPUReg::RAX, CPUBits::B64);
        count -= 8;
        offset += 8;
    }

    while (count >= 4)
    {
        emitLoad(CPUReg::RAX, regSrc, offset, CPUBits::B32);
        emitStore(regDst, offset, CPUReg::RAX, CPUBits::B32);
        count -= 4;
        offset += 4;
    }

    while (count >= 2)
    {
        emitLoad(CPUReg::RAX, regSrc, offset, CPUBits::B16);
        emitStore(regDst, offset, CPUReg::RAX, CPUBits::B16);
        count -= 2;
        offset += 2;
    }

    while (count >= 1)
    {
        emitLoad(CPUReg::RAX, regSrc, offset, CPUBits::B8);
        emitStore(regDst, offset, CPUReg::RAX, CPUBits::B8);
        count -= 1;
        offset += 1;
    }
}

void SCBE_X64::emitClear(CPUReg memReg, uint32_t memOffset, uint32_t count)
{
    if (!count)
        return;
    SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RCX || memReg == CPUReg::RDI);

    // SSE 16 octets
    if (count >= 16)
    {
        concat.addString3("\x0F\x57\xC0"); // xorps xmm0, xmm0
        while (count >= 16)
        {
            concat.addString2("\x0F\x11"); // movups [reg + ????????], xmm0
            if (memOffset <= 0x7F)
            {
                concat.addU8(0x40 | static_cast<uint8_t>(memReg));
                concat.addU8(static_cast<uint8_t>(memOffset));
            }
            else
            {
                concat.addU8(0x80 | static_cast<uint8_t>(memReg));
                concat.addU32(memOffset);
            }
            count -= 16;
            memOffset += 16;
        }
    }

    while (count >= 8)
    {
        emitStore(memReg, memOffset, 0, CPUBits::B64);
        count -= 8;
        memOffset += 8;
    }

    while (count >= 4)
    {
        emitStore(memReg, memOffset, 0, CPUBits::B32);
        count -= 4;
        memOffset += 4;
    }

    while (count >= 2)
    {
        emitStore(memReg, memOffset, 0, CPUBits::B16);
        count -= 2;
        memOffset += 2;
    }

    while (count >= 1)
    {
        emitStore(memReg, memOffset, 0, CPUBits::B8);
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
                emitLoad(cc.paramByRegisterInteger[i], reinterpret_cast<uint64_t>(retCopyAddr), CPUBits::B64);
            else if (returnByStackAddr)
                emitLoad(cc.paramByRegisterInteger[i], CPUReg::RDI, reg, CPUBits::B64);
            else
                emitSetAddress(cc.paramByRegisterInteger[i], CPUReg::RDI, reg);
        }

        // This is a normal parameter, which can be float or integer
        else
        {
            // Pass struct in a register if small enough
            if (CallConv::structParamByValue(typeFuncBC, type))
            {
                SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), CPUBits::B64);
                emitLoad(cc.paramByRegisterInteger[i], CPUReg::RAX, 0, CPUBits::B64);
            }
            else if (cc.useRegisterFloat && type->isNative(NativeTypeKind::F32))
            {
                if (paramsRegisters[i].type == CPUPushParamType::Imm)
                {
                    SWAG_ASSERT(paramsRegisters[i].reg <= UINT32_MAX);
                    emitLoad(CPUReg::RAX, static_cast<uint32_t>(paramsRegisters[i].reg), CPUBits::B32);
                    emitCopy(cc.paramByRegisterFloat[i], CPUReg::RAX, CPUBits::F32);
                }
                else
                {
                    SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                    emitLoad(cc.paramByRegisterFloat[i], CPUReg::RDI, REG_OFFSET(reg), CPUBits::F32);
                }
            }
            else if (cc.useRegisterFloat && type->isNative(NativeTypeKind::F64))
            {
                if (paramsRegisters[i].type == CPUPushParamType::Imm)
                {
                    emitLoad(CPUReg::RAX, paramsRegisters[i].reg, CPUBits::B64);
                    emitCopy(cc.paramByRegisterFloat[i], CPUReg::RAX, CPUBits::F64);
                }
                else
                {
                    SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                    emitLoad(cc.paramByRegisterFloat[i], CPUReg::RDI, REG_OFFSET(reg), CPUBits::F64);
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
                            emitLoad(cc.paramByRegisterInteger[i], paramsRegisters[i].reg, CPUBits::B64);
                        break;
                    case CPUPushParamType::Imm64:
                        emitLoad(cc.paramByRegisterInteger[i], paramsRegisters[i].reg, CPUBits::B64, true);
                        break;
                    case CPUPushParamType::RelocV:
                        emitSymbolRelocationValue(cc.paramByRegisterInteger[i], static_cast<uint32_t>(paramsRegisters[i].reg), 0);
                        break;
                    case CPUPushParamType::RelocAddr:
                        emitSymbolRelocationAddr(cc.paramByRegisterInteger[i], static_cast<uint32_t>(paramsRegisters[i].reg), 0);
                        break;
                    case CPUPushParamType::Addr:
                        emitSetAddress(cc.paramByRegisterInteger[i], CPUReg::RDI, static_cast<uint32_t>(paramsRegisters[i].reg));
                        break;
                    case CPUPushParamType::RegAdd:
                        emitLoad(cc.paramByRegisterInteger[i], CPUReg::RDI, REG_OFFSET(reg), CPUBits::B64);
                        emitOp(cc.paramByRegisterInteger[i], paramsRegisters[i].val, CPUOp::ADD, CPUBits::B64);
                        break;
                    case CPUPushParamType::RegMul:
                        emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), CPUBits::B64);
                        emitOp(CPUReg::RAX, paramsRegisters[i].val, CPUOp::IMUL, CPUBits::B64);
                        emitCopy(cc.paramByRegisterInteger[i], CPUReg::RAX, CPUBits::B64);
                        break;
                    case CPUPushParamType::RAX:
                        emitCopy(cc.paramByRegisterInteger[i], CPUReg::RAX, CPUBits::B64);
                        break;
                    case CPUPushParamType::GlobalString:
                        emitSymbolGlobalString(cc.paramByRegisterInteger[i], reinterpret_cast<const char*>(paramsRegisters[i].reg));
                        break;
                    default:
                        SWAG_ASSERT(paramsRegisters[i].type == CPUPushParamType::Reg);
                        emitLoad(cc.paramByRegisterInteger[i], CPUReg::RDI, REG_OFFSET(reg), CPUBits::B64);
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
            emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), CPUBits::B64);
            emitStore(CPUReg::RSP, offsetStack, CPUReg::RAX, CPUBits::B64);
        }

        // This is for a return value
        else if (type == g_TypeMgr->typeInfoUndefined)
        {
            // r is an address to registerRR, for FFI
            if (retCopyAddr)
                emitLoad(CPUReg::RAX, reinterpret_cast<uint64_t>(retCopyAddr), CPUBits::B64);
            else if (returnByStackAddr)
                emitLoad(CPUReg::RAX, CPUReg::RDI, reg, CPUBits::B64);
            else
                emitSetAddress(CPUReg::RAX, CPUReg::RDI, reg);
            emitStore(CPUReg::RSP, offsetStack, CPUReg::RAX, CPUBits::B64);
        }

        // This is for a normal parameter
        else
        {
            const auto sizeOf = type->sizeOf;

            // Struct by copy. Will be a pointer to the stack
            if (type->isStruct())
            {
                emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), CPUBits::B64);

                // Store the content of the struct in the stack
                if (CallConv::structParamByValue(typeFuncBC, type))
                {
                    switch (sizeOf)
                    {
                        case 1:
                            emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B8);
                            emitStore(CPUReg::RSP, offsetStack, CPUReg::RAX, CPUBits::B8);
                            break;
                        case 2:
                            emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B16);
                            emitStore(CPUReg::RSP, offsetStack, CPUReg::RAX, CPUBits::B16);
                            break;
                        case 4:
                            emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B32);
                            emitStore(CPUReg::RSP, offsetStack, CPUReg::RAX, CPUBits::B32);
                            break;
                        case 8:
                            emitLoad(CPUReg::RAX, CPUReg::RAX, 0, CPUBits::B64);
                            emitStore(CPUReg::RSP, offsetStack, CPUReg::RAX, CPUBits::B64);
                            break;
                        default:
                            break;
                    }
                }

                // Store the address of the struct in the stack
                else
                {
                    emitStore(CPUReg::RSP, offsetStack, CPUReg::RAX, CPUBits::B64);
                }
            }
            else
            {
                switch (sizeOf)
                {
                    case 1:
                        emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), CPUBits::B8);
                        emitStore(CPUReg::RSP, offsetStack, CPUReg::RAX, CPUBits::B8);
                        break;
                    case 2:
                        emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), CPUBits::B16);
                        emitStore(CPUReg::RSP, offsetStack, CPUReg::RAX, CPUBits::B16);
                        break;
                    case 4:
                        emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), CPUBits::B32);
                        emitStore(CPUReg::RSP, offsetStack, CPUReg::RAX, CPUBits::B32);
                        break;
                    case 8:
                        emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), CPUBits::B64);
                        emitStore(CPUReg::RSP, offsetStack, CPUReg::RAX, CPUBits::B64);
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

        emitLoad(CPUReg::RAX, CPUReg::RDI, REG_OFFSET(reg), CPUBits::B64);
        emitTest(CPUReg::RAX, CPUReg::RAX, CPUBits::B64);

        // If not zero, jump to closure call
        const auto seekPtrClosure = emitJumpLong(JZ);
        const auto seekJmpClosure = concat.totalCount();

        emitCallParameters(typeFunc, pushParams3, pushParamsTypes, retCopyAddr);

        // Jump to after closure call
        const auto seekPtrAfterClosure = emitJumpLong(JUMP);
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
            emitStore(CPUReg::RDI, offsetRT, cc.returnByRegisterFloat, CPUBits::F64);
        else
            emitStore(CPUReg::RDI, offsetRT, cc.returnByRegisterInteger, CPUBits::B64);
    }
}

void SCBE_X64::emitCallIndirect(CPUReg reg)
{
    SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::R10);
    if (reg == CPUReg::R10)
        concat.addU8(0x41);
    concat.addU8(0xFF);
    concat.addU8(0xD0 | (static_cast<uint8_t>(reg) & 0b111));
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitNot(CPUReg reg, CPUBits numBits)
{
    SWAG_ASSERT(reg < CPUReg::R8);

    emitREX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0xF6);
    else
        concat.addU8(0xF7);
    concat.addU8(0xD0 | (static_cast<uint8_t>(reg) & 0b111));
}

void SCBE_X64::emitNot([[maybe_unused]] CPUReg memReg, uint32_t memOffset, CPUBits numBits)
{
    SWAG_ASSERT(memReg == CPUReg::RDI);

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

void SCBE_X64::emitInc(CPUReg memReg, uint32_t memOffset, CPUBits numBits)
{
    SWAG_ASSERT(memReg < CPUReg::R8);
    emitREX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0xFE);
    else
        concat.addU8(0xFF);
    emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg);
}

void SCBE_X64::emitDec(CPUReg memReg, uint32_t memOffset, CPUBits numBits)
{
    SWAG_ASSERT(memReg < CPUReg::R8);
    emitREX(concat, numBits);
    if (numBits == CPUBits::B8)
        concat.addU8(0xFE);
    else
        concat.addU8(0xFF);
    emitModRM(concat, memOffset, static_cast<CPUReg>(1), memReg);
}

void SCBE_X64::emitNeg(CPUReg reg, CPUBits numBits)
{
    SWAG_ASSERT(reg < CPUReg::R8);
    SWAG_ASSERT(numBits == CPUBits::B32 || numBits == CPUBits::B64);

    emitREX(concat, numBits);
    concat.addU8(0xF7);
    concat.addU8(0xD8 | (static_cast<uint8_t>(reg) & 0b111));
}

void SCBE_X64::emitNeg([[maybe_unused]] CPUReg memReg, uint32_t memOffset, CPUBits numBits)
{
    SWAG_ASSERT(memReg == CPUReg::RDI);
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

void SCBE_X64::emitCMov(CPUReg regDst, CPUReg regSrc, CPUOp op, CPUBits numBits)
{
    if (numBits == CPUBits::B8 || numBits == CPUBits::B16)
        numBits = CPUBits::B32;
    emitREX(concat, numBits, regDst, regSrc);
    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(op));
    concat.addU8(getModRM(RegReg, static_cast<uint8_t>(regDst), static_cast<uint8_t>(regSrc)));
}

void SCBE_X64::emitRotate(CPUReg regDst, CPUReg regSrc, CPUOp op, CPUBits numBits)
{
    SWAG_ASSERT(numBits >= CPUBits::B32);
    SWAG_ASSERT(regDst == CPUReg::RAX && regSrc == CPUReg::RCX);
    emitREX(concat, numBits, regDst, regSrc);
    concat.addU8(0xD3);
    concat.addU8(static_cast<uint8_t>(op));
}

void SCBE_X64::emitCmpXChg([[maybe_unused]] CPUReg regDst, [[maybe_unused]] CPUReg regSrc, CPUBits numBits)
{
    SWAG_ASSERT(regDst == CPUReg::RCX && regSrc == CPUReg::RDX);

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

void SCBE_X64::emitBSwap([[maybe_unused]] CPUReg reg, CPUBits numBits)
{
    SWAG_ASSERT(reg == CPUReg::RAX);
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

// a*b+c
void SCBE_X64::emitMulAdd([[maybe_unused]] CPUReg regDst, [[maybe_unused]] CPUReg regMul, [[maybe_unused]] CPUReg regAdd, CPUBits numBits)
{
    SWAG_ASSERT(regDst == CPUReg::XMM0);
    SWAG_ASSERT(regMul == CPUReg::XMM1);
    SWAG_ASSERT(regAdd == CPUReg::XMM2);

    concat.addU8(numBits == CPUBits::F32 ? 0xF3 : 0xF2);
    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(CPUOp::FMUL));
    concat.addU8(0xC1);

    concat.addU8(numBits == CPUBits::F32 ? 0xF3 : 0xF2);
    concat.addU8(0x0F);
    concat.addU8(static_cast<uint8_t>(CPUOp::FADD));
    concat.addU8(0xC2);
}
