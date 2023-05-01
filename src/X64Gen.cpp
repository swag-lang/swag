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

void X64Gen::emit_REX(X64Bits numBits, CPURegister reg1, CPURegister reg2)
{
    if (numBits == X64Bits::B16)
        concat.addU8(0x66);
    if (numBits == X64Bits::B64 || reg1 >= R8 || reg2 >= R8)
        concat.addU8(getREX(numBits == X64Bits::B64, reg1 >= R8, false, reg2 >= R8));
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

void X64Gen::emit_Spec8(uint8_t value, X64Bits numBits)
{
    if (numBits == X64Bits::B8)
        concat.addU8(value & ~1);
    else
        concat.addU8(value);
}

/////////////////////////////////////////////////////////////////////

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

void X64Gen::emit_GlobalString(const Utf8& str, CPURegister reg)
{
    emit_Load64_Immediate(reg, 0, true);

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

void X64Gen::emit_Symbol_RelocationAddr(CPURegister reg, uint32_t symbolIndex, uint32_t offset)
{
    SWAG_ASSERT(reg == RAX || reg == RCX || reg == RDX || reg == R8 || reg == R9 || reg == RDI);

    emit_REX(X64Bits::B64, reg);
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

    emit_REX(X64Bits::B64, reg);
    concat.addU8(0x8B);
    concat.addU8(0x05 | ((reg & 0b111) << 3));

    CoffRelocation reloc;
    reloc.virtualAddress = concat.totalCount() - textSectionOffset;
    reloc.symbolIndex    = symbolIndex;
    reloc.type           = IMAGE_REL_AMD64_REL32;
    relocTableTextSection.table.push_back(reloc);
    concat.addU32(offset);
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
            emit_CopyN(RAX, RAX, X64Bits::B32);
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

void X64Gen::emit_LoadN_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg, X64Bits numBits)
{
    switch (numBits)
    {
    case X64Bits::B8:
        emit_Load8_Indirect(stackOffset, reg, memReg);
        break;
    case X64Bits::B16:
        emit_Load16_Indirect(stackOffset, reg, memReg);
        break;
    case X64Bits::B32:
        emit_Load32_Indirect(stackOffset, reg, memReg);
        break;
    case X64Bits::B64:
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
        emit_CopyN(reg, memReg, X64Bits::B64);
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

void X64Gen::emit_StoreN_Indirect(uint32_t stackOffset, CPURegister reg, CPURegister memReg, X64Bits numBits)
{
    switch (numBits)
    {
    case X64Bits::B8:
        emit_Store8_Indirect(stackOffset, reg, memReg);
        break;
    case X64Bits::B16:
        emit_Store16_Indirect(stackOffset, reg, memReg);
        break;
    case X64Bits::B32:
        emit_Store32_Indirect(stackOffset, reg, memReg);
        break;
    case X64Bits::B64:
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
    emit_REX(X64Bits::B64);
    concat.addU8(0xC7);
    emit_ModRM(stackOffset, 0, memReg);
    concat.addU32((uint32_t) val);
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_Load8_Immediate(CPURegister reg, uint8_t value)
{
    if (value == 0)
    {
        emit_ClearN(reg, X64Bits::B8);
        return;
    }

    concat.addU8(0xB0 | reg);
    concat.addU8(value);
}

void X64Gen::emit_Load16_Immediate(CPURegister reg, uint16_t value)
{
    if (value == 0)
    {
        emit_ClearN(reg, X64Bits::B16);
        return;
    }

    concat.addU8(0x66);
    concat.addU8(0xB8 | reg);
    concat.addU16(value);
}

void X64Gen::emit_Load32_Immediate(CPURegister reg, uint32_t value)
{
    if (value == 0)
    {
        emit_ClearN(reg, X64Bits::B32);
        return;
    }

    concat.addU8(0xB8 | reg);
    concat.addU32(value);
}

void X64Gen::emit_Load64_Immediate(CPURegister reg, uint64_t value, bool force64bits)
{
    if (force64bits)
    {
        concat.addU8(getREX(true, false, false, reg >= R8));
        concat.addU8(0xB8 | reg);
        concat.addU64(value);
        return;
    }

    if (value == 0)
    {
        emit_ClearN(reg, X64Bits::B64);
        return;
    }

    if (value <= 0x7FFFFFFF && reg < R8)
    {
        emit_Load32_Immediate(reg, (uint32_t) value);
        return;
    }

    concat.addU8(getREX(true, false, false, reg >= R8));
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

void X64Gen::emit_LoadN_Immediate(CPURegister reg, uint64_t value, X64Bits numBits)
{
    switch (numBits)
    {
    case X64Bits::B8:
        emit_Load8_Immediate(reg, (uint8_t) value);
        break;
    case X64Bits::B16:
        emit_Load16_Immediate(reg, (uint16_t) value);
        break;
    case X64Bits::B32:
        emit_Load32_Immediate(reg, (uint32_t) value);
        break;
    case X64Bits::B64:
        emit_Load64_Immediate(reg, value);
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_ClearN(CPURegister reg, X64Bits numBits)
{
    emit_REX(numBits, reg, reg);
    emit_Spec8((uint8_t) X64Op::XOR, numBits);
    concat.addU8(getModRM(REGREG, reg, reg));
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_CopyN(CPURegister regDst, CPURegister regSrc, X64Bits numBits)
{
    emit_REX(numBits, regSrc, regDst);
    if (numBits == X64Bits::B8)
        concat.addU8(0x88);
    else
        concat.addU8(0x89);
    concat.addU8(getModRM(REGREG, regSrc, regDst));
}

void X64Gen::emit_CopyF32(CPURegister regDst, CPURegister regSrc)
{
    SWAG_ASSERT(regSrc == RAX);
    SWAG_ASSERT(regDst == XMM0 || regDst == XMM1 || regDst == XMM2 || regDst == XMM3);
    concat.addU8(0x66);
    concat.addU8(0x0F);
    concat.addU8(0x6E);
    concat.addU8(0xC0 | (regDst << 3));
}

void X64Gen::emit_CopyF64(CPURegister regDst, CPURegister regSrc)
{
    SWAG_ASSERT(regSrc == RAX);
    SWAG_ASSERT(regDst == XMM0 || regDst == XMM1 || regDst == XMM2 || regDst == XMM3);
    concat.addU8(0x66);
    emit_REX(X64Bits::B64);
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

void X64Gen::emit_TestN(CPURegister reg1, CPURegister reg2, X64Bits numBits)
{
    emit_REX(numBits, reg2, reg1);
    if (numBits == X64Bits::B8)
        concat.addU8(0x84);
    else
        concat.addU8(0x85);
    concat.addU8(getModRM(REGREG, reg2, reg1));
}

void X64Gen::emit_CmpN(CPURegister regSrc, CPURegister regDst, X64Bits numBits)
{
    emit_REX(numBits, regDst, regSrc);
    emit_Spec8(0x39, numBits);
    concat.addU8(getModRM(REGREG, regDst, regSrc));
}

void X64Gen::emit_CmpF32(CPURegister reg1, CPURegister reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x0F);
    concat.addU8(0x2F);
    concat.addU8(getModRM(REGREG, reg1, reg2));
}

void X64Gen::emit_CmpF64(CPURegister reg1, CPURegister reg2)
{
    SWAG_ASSERT(reg1 < R8 && reg2 < R8);
    concat.addU8(0x66);
    concat.addU8(0x0F);
    concat.addU8(0x2F);
    concat.addU8(getModRM(REGREG, reg1, reg2));
}

void X64Gen::emit_CmpN_Immediate(CPURegister reg, uint64_t value, X64Bits numBits)
{
    if (value <= 0x7f)
    {
        SWAG_ASSERT(reg == RAX || reg == RCX);
        emit_REX(numBits);
        concat.addU8(0x83);
        concat.addU8(0xF8 | reg);
        concat.addU8((uint8_t) value);
    }
    else if (value <= 0x7fffffff)
    {
        SWAG_ASSERT(reg == RAX);
        emit_REX(numBits);
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

void X64Gen::emit_CmpN_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, X64Bits numBits)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    emit_REX(numBits);
    emit_Spec8(0x3B, numBits);
    emit_ModRM(offsetStack, reg, memReg);
}

void X64Gen::emit_CmpF32_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg)
{
    SWAG_ASSERT(reg < R8 && memReg < R8);
    concat.addU8(0x0F);
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

void X64Gen::emit_CmpN_IndirectDst(uint32_t offsetStack, uint32_t value, X64Bits numBits)
{
    emit_REX(numBits);
    if (numBits == X64Bits::B8)
    {
        concat.addU8(0x80);
        emit_ModRM(offsetStack, RDI, RDI);
        concat.addU8((uint8_t) value);
    }
    else if (value <= 0x7F)
    {
        concat.addU8(0x83);
        emit_ModRM(offsetStack, RDI, RDI);
        concat.addU8((uint8_t) value);
    }
    else
    {
        concat.addU8(0x81);
        emit_ModRM(offsetStack, RDI, RDI);
        if (numBits == X64Bits::B16)
            concat.addU16((uint16_t) value);
        else
            concat.addU32(value);
    }
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_OpN_Indirect(uint32_t offsetStack, CPURegister reg, CPURegister memReg, X64Op instruction, X64Bits numBits, bool lock)
{
    SWAG_ASSERT(memReg < R8);
    if (lock)
        concat.addU8(0xF0);
    emit_REX(numBits, reg);
    emit_Spec8((uint8_t) instruction, numBits);
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

void X64Gen::emit_OpN(CPURegister regSrc, CPURegister regDst, X64Op instruction, X64Bits numBits)
{
    emit_REX(numBits, regSrc, regDst);
    if (instruction == X64Op::DIV || instruction == X64Op::IDIV)
    {
        SWAG_ASSERT(regSrc == RAX and regDst == RCX);
        emit_Spec8(0xF7, numBits);
        concat.addU8((uint8_t) instruction);
    }
    else if (instruction == X64Op::MUL || instruction == X64Op::IMUL)
    {
        SWAG_ASSERT(regSrc == RAX and regDst == RCX);
        emit_Spec8(0xF7, numBits);
        concat.addU8(instruction == X64Op::IMUL ? 0xE9 : 0xE1);
    }
    else
    {
        emit_Spec8((uint8_t) instruction, numBits);
        concat.addU8(getModRM(REGREG, regSrc, regDst));
    }
}

void X64Gen::emit_OpF32(CPURegister regSrc, CPURegister regDst, X64Op instruction)
{
    SWAG_ASSERT(regSrc == XMM0 && regDst == XMM1);
    if (instruction == X64Op::MUL)
    {
        concat.addString4("\xF3\x0F\x59\xC1"); // mulss xmm0, xmm1
    }
    else if (instruction == X64Op::DIV)
    {
        concat.addString4("\xF3\x0F\x5E\xC1"); // divss xmm0, xmm1
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void X64Gen::emit_OpF64(CPURegister regSrc, CPURegister regDst, X64Op instruction)
{
    SWAG_ASSERT(regSrc == XMM0 && regDst == XMM1);
    if (instruction == X64Op::MUL)
    {
        concat.addString4("\xF2\x0F\x59\xC1"); // mulsd xmm0, xmm1
    }
    else if (instruction == X64Op::DIV)
    {
        concat.addString4("\xF2\x0F\x5E\xC1"); // divsd xmm0, xmm1
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_OpN_Immediate(CPURegister reg, uint64_t value, X64Op instruction, X64Bits numBits)
{
    emit_REX(numBits);
}

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

    emit_REX(X64Bits::B64);
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

    emit_REX(X64Bits::B64);
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
        emit_REX(X64Bits::B64);
        concat.addU8(0xFF);
        concat.addU8(0xC8); // dec rax
    }
    else if (value > 0x7FFFFFFF)
    {
        emit_Load64_Immediate(RCX, value);
        emit_REX(X64Bits::B64);
        concat.addU8(0x29);
        concat.addU8(0xC8); // sub rax, rcx
    }
    else if (value <= 0x7F)
    {
        emit_REX(X64Bits::B64);
        concat.addU8(0x83);
        concat.addU8(0xE8);
        concat.addU8((uint8_t) value);
    }
    else
    {
        emit_REX(X64Bits::B64);
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
        emit_Load64_Immediate(RCX, value);
        concat.addString4("\x48\x0F\xAF\xC1"); // imul rax, rcx
    }
}

/////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_Call_Parameters(TypeInfoFuncAttr* typeFuncBC, VectorNative<X64PushParam>& paramsRegisters, VectorNative<TypeInfo*>& paramsTypes, void* retCopyAddr)
{
    const auto& cc                = typeFuncBC->getCallConv();
    bool        returnByStackAddr = CallConv::returnByStackAddress(typeFuncBC);

    int callConvRegisters    = cc.paramByRegisterCount;
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
                SWAG_ASSERT(paramsRegisters[i].type == X64PushParamType::Reg);
                emit_Load64_Indirect(regOffset(reg), RAX);
                emit_Load64_Indirect(0, cc.paramByRegisterInteger[i], RAX);
            }
            else if (cc.useRegisterFloat && type->isNative(NativeTypeKind::F32))
            {
                if (paramsRegisters[i].type == X64PushParamType::Imm)
                {
                    SWAG_ASSERT(paramsRegisters[i].reg <= UINT32_MAX);
                    emit_Load32_Immediate(RAX, (uint32_t) paramsRegisters[i].reg);
                    emit_CopyF32(cc.paramByRegisterFloat[i], RAX);
                }
                else
                {
                    SWAG_ASSERT(paramsRegisters[i].type == X64PushParamType::Reg);
                    emit_LoadF32_Indirect(regOffset(reg), cc.paramByRegisterFloat[i]);
                }
            }
            else if (cc.useRegisterFloat && type->isNative(NativeTypeKind::F64))
            {
                if (paramsRegisters[i].type == X64PushParamType::Imm)
                {
                    emit_Load64_Immediate(RAX, paramsRegisters[i].reg);
                    emit_CopyF64(cc.paramByRegisterFloat[i], RAX);
                }
                else
                {
                    SWAG_ASSERT(paramsRegisters[i].type == X64PushParamType::Reg);
                    emit_LoadF64_Indirect(regOffset(reg), cc.paramByRegisterFloat[i]);
                }
            }
            else
            {
                switch (paramsRegisters[i].type)
                {
                case X64PushParamType::Imm:
                    if (paramsRegisters[i].reg == 0)
                        emit_ClearN(cc.paramByRegisterInteger[i], X64Bits::B64);
                    else
                        emit_Load64_Immediate(cc.paramByRegisterInteger[i], paramsRegisters[i].reg);
                    break;
                case X64PushParamType::Imm64:
                    emit_Load64_Immediate(cc.paramByRegisterInteger[i], paramsRegisters[i].reg, true);
                    break;
                case X64PushParamType::RelocV:
                    emit_Symbol_RelocationValue(cc.paramByRegisterInteger[i], (uint32_t) paramsRegisters[i].reg, 0);
                    break;
                case X64PushParamType::RelocAddr:
                    emit_Symbol_RelocationAddr(cc.paramByRegisterInteger[i], (uint32_t) paramsRegisters[i].reg, 0);
                    break;
                case X64PushParamType::Addr:
                    emit_LoadAddress_Indirect((uint32_t) paramsRegisters[i].reg, cc.paramByRegisterInteger[i], RDI);
                    break;
                case X64PushParamType::RegAdd:
                    emit_Load64_Indirect(regOffset(reg), cc.paramByRegisterInteger[i]);
                    emit_Add64_Immediate(paramsRegisters[i].val, cc.paramByRegisterInteger[i]);
                    break;
                case X64PushParamType::RegMul:
                    emit_Load64_Indirect(regOffset(reg), RAX);
                    emit_Mul64_RAX(paramsRegisters[i].val);
                    emit_CopyN(cc.paramByRegisterInteger[i], RAX, X64Bits::B64);
                    break;
                case X64PushParamType::GlobalString:
                    emit_GlobalString((const char*) paramsRegisters[i].reg, cc.paramByRegisterInteger[i]);
                    break;
                default:
                    SWAG_ASSERT(paramsRegisters[i].type == X64PushParamType::Reg);
                    emit_Load64_Indirect(regOffset(reg), cc.paramByRegisterInteger[i]);
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
            auto sizeOf = type->sizeOf;

            // Struct by copy. Will be a pointer to the stack
            if (type->isStruct())
            {
                emit_Load64_Indirect(regOffset(reg), RAX);

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

void X64Gen::emit_Call_Parameters(TypeInfoFuncAttr* typeFunc, const VectorNative<uint32_t>& pushRAParams, uint32_t offsetRT, void* retCopyAddr)
{
    pushParams2.clear();
    for (auto r : pushRAParams)
        pushParams2.push_back({X64PushParamType::Reg, r});
    emit_Call_Parameters(typeFunc, pushParams2, offsetRT, retCopyAddr);
}

void X64Gen::emit_Call_Parameters(TypeInfoFuncAttr* typeFunc, const VectorNative<X64PushParam>& pushRAParams, uint32_t offsetRT, void* retCopyAddr)
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
    if (CallConv::returnByAddress(typeFunc))
    {
        pushParams3.push_back({X64PushParamType::Reg, offsetRT});
        pushParamsTypes.push_back(g_TypeMgr->typeInfoUndefined);
    }

    // Add all C variadic parameters
    if (typeFunc->isCVariadic())
    {
        for (size_t i = typeFunc->numParamsRegisters(); i < pushRAParams.size(); i++)
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
        emit_TestN(RAX, RAX, X64Bits::B64);

        // If not zero, jump to closure call
        emit_LongJumpOp(JZ);
        concat.addU32(0);
        auto seekPtrClosure = concat.getSeekPtr() - 4;
        auto seekJmpClosure = concat.totalCount();

        emit_Call_Parameters(typeFunc, pushParams3, pushParamsTypes, retCopyAddr);

        // Jump to after closure call
        emit_LongJumpOp(JUMP);
        concat.addU32(0);
        auto seekPtrAfterClosure = concat.getSeekPtr() - 4;
        auto seekJmpAfterClosure = concat.totalCount();

        // Update jump to closure call
        *seekPtrClosure = (uint8_t) (concat.totalCount() - seekJmpClosure);

        pushParams3.erase(0);
        pushParamsTypes.erase(0);
        emit_Call_Parameters(typeFunc, pushParams3, pushParamsTypes, retCopyAddr);

        *seekPtrAfterClosure = (uint8_t) (concat.totalCount() - seekJmpAfterClosure);
    }
    else
    {
        emit_Call_Parameters(typeFunc, pushParams3, pushParamsTypes, retCopyAddr);
    }
}

void X64Gen::emit_Call_Result(TypeInfoFuncAttr* typeFunc, uint32_t offsetRT)
{
    if (CallConv::returnByValue(typeFunc))
    {
        const auto& cc         = typeFunc->getCallConv();
        auto        returnType = typeFunc->concreteReturnType();
        if (returnType->isNativeFloat())
            emit_StoreF64_Indirect(offsetRT, cc.returnByRegisterFloat, RDI);
        else
            emit_Store64_Indirect(offsetRT, cc.returnByRegisterInteger, RDI);
    }
}

void X64Gen::emit_Call_Indirect(CPURegister reg)
{
    SWAG_ASSERT(reg == RAX || reg == RCX || reg == R10);
    if (reg == R10)
        concat.addU8(0x41);
    concat.addU8(0xFF);
    concat.addU8(0xD0 | (reg & 0b111));
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_Cwd()
{
    emit_REX(X64Bits::B16);
    concat.addU8(0x99);
}

void X64Gen::emit_Cdq()
{
    concat.addU8(0x99);
}

void X64Gen::emit_Cqo()
{
    emit_REX(X64Bits::B64);
    concat.addU8(0x99);
}

/////////////////////////////////////////////////////////////////////

void X64Gen::emit_NotN(CPURegister reg, X64Bits numBits)
{
    SWAG_ASSERT(reg < R8);

    emit_REX(numBits);
    if (numBits == X64Bits::B8)
        concat.addU8(0xF6);
    else
        concat.addU8(0xF7);
    concat.addU8(0xD0 | (reg & 0b111));
}

void X64Gen::emit_NotN_Indirect(uint32_t stackOffset, CPURegister memReg, X64Bits numBits)
{
    SWAG_ASSERT(memReg == RDI);

    emit_REX(numBits);
    if (numBits == X64Bits::B8)
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

void X64Gen::emit_IncN_Indirect(uint32_t stackOffset, CPURegister memReg, X64Bits numBits)
{
    SWAG_ASSERT(memReg < R8);
    emit_REX(numBits);
    if (numBits == X64Bits::B8)
        concat.addU8(0xFE);
    else
        concat.addU8(0xFF);
    emit_ModRM(stackOffset, 0, memReg);
}

void X64Gen::emit_DecN_Indirect(uint32_t stackOffset, CPURegister memReg, X64Bits numBits)
{
    SWAG_ASSERT(memReg < R8);
    emit_REX(numBits);
    if (numBits == X64Bits::B8)
        concat.addU8(0xFE);
    else
        concat.addU8(0xFF);
    emit_ModRM(stackOffset, 1, memReg);
}

void X64Gen::emit_NegN(CPURegister reg, X64Bits numBits)
{
    SWAG_ASSERT(reg < R8);
    SWAG_ASSERT(numBits == X64Bits::B32 || numBits == X64Bits::B64);

    emit_REX(numBits);
    concat.addU8(0xF7);
    concat.addU8(0xD8 | (reg & 0b111));
}

void X64Gen::emit_NegN_Indirect(uint32_t stackOffset, CPURegister memReg, X64Bits numBits)
{
    SWAG_ASSERT(memReg == RDI);
    SWAG_ASSERT(numBits == X64Bits::B32 || numBits == X64Bits::B64);

    emit_REX(numBits);
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

void X64Gen::emit_CMovN(CPURegister regDst, CPURegister regSrc, X64Bits numBits, X64Op op)
{
    if (numBits < X64Bits::B32)
        numBits = X64Bits::B32;
    emit_REX(numBits, regDst, regSrc);
    concat.addU8(0x0F);
    concat.addU8((uint8_t) op);
    concat.addU8(getModRM(REGREG, regDst, regSrc));
}

void X64Gen::emit_BSwapN(CPURegister reg, X64Bits numBits)
{
    SWAG_ASSERT(reg == RAX);
    SWAG_ASSERT(numBits == X64Bits::B32 || numBits == X64Bits::B64);

    emit_REX(numBits);
    concat.addU8(0x0F);
    concat.addU8(0xC8);
}

void X64Gen::emit_ShiftN(CPURegister reg, uint32_t value, X64Bits numBits, X64Op op)
{
    SWAG_ASSERT(reg == RAX);
    value = min(value, (uint32_t) numBits - 1);

    emit_REX(numBits);
    if (value == 1)
    {
        emit_Spec8(0xD1, numBits);
        concat.addU8((uint8_t) op);
    }
    else
    {
        emit_Spec8(0xC1, numBits);
        concat.addU8((uint8_t) op);
        concat.addU8((uint8_t) value);
    }
}