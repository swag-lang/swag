// ReSharper disable CommentTypo
#include "pch.h"
#include "Backend/SCBE/Encoder/SCBE_X64.h"
#include "Core/Math.h"
#include "Semantic/Type/TypeManager.h"
#pragma optimize("", off)

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

    void emitREX(Concat& concat, OpBits opBits, CPUReg reg1 = CPUReg::RAX, CPUReg reg2 = CPUReg::RAX)
    {
        if (opBits == OpBits::B16 || opBits == OpBits::F64)
            concat.addU8(0x66);
        if (opBits == OpBits::B64 || reg1 >= CPUReg::R8 || reg2 >= CPUReg::R8)
            concat.addU8(getREX(opBits == OpBits::B64, reg1 >= CPUReg::R8, false, reg2 >= CPUReg::R8));
    }

    void emitValue(Concat& concat, uint64_t value, OpBits opBits)
    {
        if (opBits == OpBits::B8)
            concat.addU8(static_cast<uint8_t>(value));
        else if (opBits == OpBits::B16)
            concat.addU16(static_cast<uint16_t>(value));
        else if (opBits == OpBits::B32)
            concat.addU32(static_cast<uint32_t>(value));
        else
            concat.addU64(value);
    }

    void emitModRM(Concat& concat, CPUReg reg, CPUReg memReg)
    {
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(reg), static_cast<uint8_t>(memReg)));
    }

    void emitModRM(Concat& concat, uint64_t memOffset, CPUReg reg, CPUReg memReg, uint8_t op = 1)
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
            emitValue(concat, memOffset, OpBits::B8);
        }
        else
        {
            // mov al, byte ptr [rdi + ????????]
            concat.addU8(getModRM(Disp32, static_cast<uint8_t>(reg), static_cast<uint8_t>(memReg)) | op - 1);
            if (memReg == CPUReg::RSP || memReg == CPUReg::R12)
                concat.addU8(0x24);
            SWAG_ASSERT(memOffset <= 0x7FFFFFFF);
            emitValue(concat, memOffset, OpBits::B32);
        }
    }

    void emitSpecB8(Concat& concat, uint8_t value, OpBits opBits)
    {
        if (opBits == OpBits::B8)
            concat.addU8(value & ~1);
        else
            concat.addU8(value);
    }

    void emitSpecF64(Concat& concat, uint8_t value, OpBits opBits)
    {
        if (opBits == OpBits::F64)
            concat.addU8(value & ~1);
        else
            concat.addU8(value);
    }

    void emitCPUOp(Concat& concat, CPUOp op0)
    {
        concat.addU8(static_cast<uint8_t>(op0));
    }

    void emitSpecCPUOp(Concat& concat, CPUOp op, OpBits opBits)
    {
        emitSpecB8(concat, static_cast<uint8_t>(op), opBits);
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitCast(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, bool isSigned)
{
    if (numBitsSrc == OpBits::B8 && numBitsDst == OpBits::B64 && !isSigned)
    {
        emitREX(concat, OpBits::B64, regDst, regSrc);
        concat.addU8(0x0F);
        concat.addU8(0xB6);
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(regDst), static_cast<uint8_t>(regSrc)));
    }
    else if (numBitsSrc == OpBits::B16 && numBitsDst == OpBits::B64 && !isSigned)
    {
        emitREX(concat, OpBits::B64, regDst, regSrc);
        concat.addU8(0x0F);
        concat.addU8(0xB7);
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(regDst), static_cast<uint8_t>(regSrc)));
    }
    else if (numBitsSrc == OpBits::B64 && numBitsDst == OpBits::F64 && !isSigned)
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
    emitREX(concat, OpBits::B64, reg);
    concat.addU8(0x8D);
    concat.addU8(static_cast<uint8_t>(0x05 | ((static_cast<uint8_t>(reg) & 0b111)) << 3));
    addSymbolRelocation(concat.totalCount() - textSectionOffset, symbolIndex, IMAGE_REL_AMD64_REL32);
    concat.addU32(offset);
}

void SCBE_X64::emitSymbolRelocationValue(CPUReg reg, uint32_t symbolIndex, uint32_t offset)
{
    SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RDX || reg == CPUReg::R8 || reg == CPUReg::R9);
    emitREX(concat, OpBits::B64, reg);
    concat.addU8(0x8B);
    concat.addU8(static_cast<uint8_t>(0x05 | ((static_cast<uint8_t>(reg) & 0b111)) << 3));
    addSymbolRelocation(concat.totalCount() - textSectionOffset, symbolIndex, IMAGE_REL_AMD64_REL32);
    concat.addU32(offset);
}

void SCBE_X64::emitSymbolGlobalString(CPUReg reg, const Utf8& str)
{
    emitLoad64(reg, 0);
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

void SCBE_X64::emitLoad(CPUReg regDst, CPUReg regSrc, OpBits opBits)
{
    if (isFloat(opBits))
    {
        SWAG_ASSERT(regSrc == CPUReg::RAX);
        SWAG_ASSERT(regDst == CPUReg::XMM0 || regDst == CPUReg::XMM1 || regDst == CPUReg::XMM2 || regDst == CPUReg::XMM3);
        emitREX(concat, OpBits::F64);
        emitREX(concat, opBits == OpBits::F64 ? OpBits::B64 : OpBits::B32);
        concat.addU8(0x0F);
        emitCPUOp(concat, CPUOp::MOVD);
        emitModRM(concat, regDst, regSrc);
    }
    else
    {
        emitREX(concat, opBits, regSrc, regDst);
        emitSpecCPUOp(concat, CPUOp::MOV, opBits);
        emitModRM(concat, regSrc, regDst);
    }
}

void SCBE_X64::emitLoad([[maybe_unused]] CPUReg regDstSrc, [[maybe_unused]] OpBits opBits)
{
    SWAG_ASSERT(regDstSrc == CPUReg::RAX);
    SWAG_ASSERT(opBits == OpBits::B8);
    emitSpecCPUOp(concat, CPUOp::MOV, opBits);
    concat.addU8(0xE0);
}

void SCBE_X64::emitLoad(CPUReg reg, CPUReg memReg, uint64_t memOffset, uint64_t value, bool isImmediate, CPUOp op, OpBits opBits)
{
    if (op == CPUOp::DIV || op == CPUOp::MOD || op == CPUOp::IDIV || op == CPUOp::IMOD)
    {
        SWAG_ASSERT(reg == CPUReg::RAX);
        if (opBits == OpBits::B8)
        {
            if (isImmediate)
                emitLoad(CPUReg::RAX, value, OpBits::B32);
            else
                emitLoadExtend(CPUReg::RAX, memReg, memOffset, OpBits::B32, OpBits::B8, op == CPUOp::IDIV || op == CPUOp::IMOD);
        }
        else
        {
            if (isImmediate)
                emitLoad(CPUReg::RAX, value, opBits);
            else
                emitLoad(CPUReg::RAX, memReg, memOffset, opBits);

            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
            {
                emitREX(concat, opBits);
                emitCPUOp(concat, CPUOp::CDQ);
            }
            else
            {
                emitClear(CPUReg::RDX, opBits);
            }
        }
    }
    else if (isImmediate)
    {
        emitLoad(reg, value, opBits);
    }
    else
    {
        emitLoad(reg, memReg, memOffset, opBits);
    }
}

void SCBE_X64::emitLoad64(CPUReg reg, uint64_t value)
{
    emitREX(concat, OpBits::B64, CPUReg::RAX, reg);
    concat.addU8(0xB8 | static_cast<uint8_t>(reg));
    concat.addU64(value);
}

void SCBE_X64::emitLoad(CPUReg reg, uint64_t value, OpBits opBits)
{
    if (value == 0)
    {
        emitClear(reg, opBits);
    }
    else if (opBits == OpBits::F32)
    {
        concat.addU8(0xB8 | static_cast<int>(CPUReg::RAX));
        emitValue(concat, value, OpBits::B32);
        emitLoad(reg, CPUReg::RAX, OpBits::F32);
    }
    else if (opBits == OpBits::F64)
    {
        emitREX(concat, OpBits::B64);
        if (value <= 0x7FFFFFFF)
        {
            concat.addU8(0xC7);
            concat.addU8(0xC0 | (static_cast<int>(CPUReg::RAX) & 0b111));
            emitValue(concat, value, OpBits::B32);
        }
        else
        {
            concat.addU8(0xB8 | static_cast<int>(CPUReg::RAX));
            concat.addU64(value);
        }
        emitLoad(reg, CPUReg::RAX, OpBits::F64);
    }
    else if (opBits == OpBits::B64)
    {
        if (value <= 0x7FFFFFFF && reg < CPUReg::R8)
        {
            concat.addU8(0xB8 | static_cast<uint8_t>(reg));
            emitValue(concat, value, OpBits::B32);
        }
        else if (value <= 0x7FFFFFFF)
        {
            emitREX(concat, OpBits::B64, CPUReg::RAX, reg);
            concat.addU8(0xC7);
            concat.addU8(0xC0 | (static_cast<uint8_t>(reg) & 0b111));
            emitValue(concat, value, OpBits::B32);
        }
        else
        {
            emitREX(concat, OpBits::B64, CPUReg::RAX, reg);
            concat.addU8(0xB8 | static_cast<uint8_t>(reg));
            concat.addU64(value);
        }
    }
    else if (opBits == OpBits::B8)
    {
        emitREX(concat, opBits);
        concat.addU8(0xB0 | static_cast<uint8_t>(reg));
        emitValue(concat, value, opBits);
    }
    else
    {
        emitREX(concat, opBits);
        concat.addU8(0xB8 | static_cast<uint8_t>(reg));
        emitValue(concat, value, opBits);
    }
}

void SCBE_X64::emitLoadExtend(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, bool isSigned)
{
    if (numBitsSrc == numBitsDst)
    {
        emitLoad(reg, memReg, memOffset, numBitsSrc);
        return;
    }

    if (memOffset > 0x7FFFFFFF)
    {
        SWAG_ASSERT(memReg != CPUReg::RCX);
        emitLoad(CPUReg::RCX, memOffset, OpBits::B64);
        emitOpBinary(memReg, CPUReg::RCX, CPUOp::ADD, OpBits::B64);
        memOffset = 0;
    }

    if (numBitsSrc == OpBits::B8)
    {
        switch (numBitsDst)
        {
            case OpBits::B16:
                if (isSigned)
                {
                    emitREX(concat, OpBits::B16, reg, memReg);
                    concat.addU8(0x0F);
                    concat.addU8(0xBE);
                    emitModRM(concat, memOffset, reg, memReg);
                    return;
                }
                break;
            case OpBits::B32:
                if (isSigned)
                {
                    emitREX(concat, OpBits::B32, reg, memReg);
                    concat.addU8(0x0F);
                    concat.addU8(0xBE);
                    emitModRM(concat, memOffset, reg, memReg);
                    return;
                }

                emitREX(concat, OpBits::B32, reg, memReg);
                concat.addU8(0x0F);
                concat.addU8(0xB6);
                emitModRM(concat, memOffset, reg, memReg);
                return;

            case OpBits::B64:
                if (isSigned)
                {
                    emitREX(concat, OpBits::B64, reg, memReg);
                    concat.addU8(0x0F);
                    concat.addU8(0xBE);
                    emitModRM(concat, memOffset, reg, memReg);
                    return;
                }

                emitREX(concat, OpBits::B64, reg, memReg);
                concat.addU8(0x0F);
                concat.addU8(0xB6);
                emitModRM(concat, memOffset, reg, memReg);
                return;
        }
    }
    else if (numBitsSrc == OpBits::B16)
    {
        switch (numBitsDst)
        {
            case OpBits::B32:
                if (isSigned)
                {
                    emitREX(concat, OpBits::B32, reg, memReg);
                    concat.addU8(0x0F);
                    concat.addU8(0xBF);
                    emitModRM(concat, memOffset, reg, memReg);
                    return;
                }

                emitREX(concat, OpBits::B32, reg, memReg);
                concat.addU8(0x0F);
                concat.addU8(0xB7);
                emitModRM(concat, memOffset, reg, memReg);
                return;

            case OpBits::B64:
                if (isSigned)
                {
                    emitREX(concat, OpBits::B64, reg, memReg);
                    concat.addU8(0x0F);
                    concat.addU8(0xBF);
                    emitModRM(concat, memOffset, reg, memReg);
                    return;
                }

                emitREX(concat, OpBits::B64, reg, memReg);
                concat.addU8(0x0F);
                concat.addU8(0xB7);
                emitModRM(concat, memOffset, reg, memReg);
                return;
        }
    }
    else if (numBitsSrc == OpBits::B32)
    {
        SWAG_ASSERT(numBitsDst == OpBits::B64);
        if (isSigned)
        {
            emitREX(concat, OpBits::B64, reg, memReg);
            concat.addU8(0x63);
            emitModRM(concat, memOffset, reg, memReg);
        }
        else
        {
            emitLoad(reg, memReg, memOffset, numBitsSrc);
        }

        return;
    }

    SWAG_ASSERT(false);
}

void SCBE_X64::emitLoad(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits opBits)
{
    if (memOffset > 0x7FFFFFFF)
    {
        SWAG_ASSERT(memReg != CPUReg::RCX);
        emitLoad(CPUReg::RCX, memOffset, OpBits::B64);
        emitOpBinary(memReg, CPUReg::RCX, CPUOp::ADD, OpBits::B64);
        memOffset = 0;
    }

    if (isFloat(opBits))
    {
        SWAG_ASSERT(reg < CPUReg::R8 && memReg < CPUReg::R8);
        emitSpecF64(concat, 0xF3, opBits);
        concat.addU8(0x0F);
        concat.addU8(0x10);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else
    {
        emitREX(concat, opBits, reg, memReg);
        emitSpecB8(concat, 0x8B, opBits);
        emitModRM(concat, memOffset, reg, memReg);
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitLoadAddress(CPUReg reg, CPUReg memReg, uint64_t memOffset)
{
    if (memReg == CPUReg::RIP)
    {
        SWAG_ASSERT(memOffset == 0);
        SWAG_ASSERT(reg == CPUReg::RCX);
        emitREX(concat, OpBits::B64, reg, memReg);
        emitCPUOp(concat, CPUOp::LEA);
        concat.addU8(0x0D);
    }
    else if (memOffset == 0)
    {
        emitLoad(reg, memReg, OpBits::B64);
    }
    else
    {
        emitREX(concat, OpBits::B64, reg, memReg);
        emitCPUOp(concat, CPUOp::LEA);
        emitModRM(concat, memOffset, reg, memReg);
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitStore(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits)
{
    if (isFloat(opBits))
    {
        SWAG_ASSERT(reg < CPUReg::R8 && memReg < CPUReg::R8);
        emitSpecF64(concat, 0xF3, opBits);
        concat.addU8(0x0F);
        concat.addU8(0x11);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else
    {
        emitREX(concat, opBits, reg, memReg);
        emitSpecB8(concat, 0x89, opBits);
        emitModRM(concat, memOffset, reg, memReg);
    }
}

void SCBE_X64::emitStore(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    if (opBits == OpBits::B64 && value > 0x7FFFFFFF && value >> 32 != 0xFFFFFFFF)
    {
        SWAG_ASSERT(memReg != CPUReg::RCX);
        emitLoad(CPUReg::RCX, value, OpBits::B64);
        emitStore(memReg, memOffset, CPUReg::RCX, OpBits::B64);
    }
    else
    {
        emitREX(concat, opBits);
        emitSpecB8(concat, 0xC7, opBits);
        emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg);
        emitValue(concat, value, min(opBits, OpBits::B32));
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitClear(CPUReg reg, OpBits opBits)
{
    if (isFloat(opBits))
    {
        SWAG_ASSERT(reg == CPUReg::XMM0 || reg == CPUReg::XMM1);
        emitREX(concat, opBits);
        concat.addU8(0x0F);
        emitCPUOp(concat, CPUOp::FXOR);
        emitModRM(concat, reg, reg);
    }
    else
    {
        emitREX(concat, opBits, reg, reg);
        emitSpecCPUOp(concat, CPUOp::XOR, opBits);
        emitModRM(concat, reg, reg);
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitSet(CPUReg reg, CPUCondFlag setType)
{
    switch (setType)
    {
        case CPUCondFlag::A:
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::R8);
            if (reg >= CPUReg::R8)
                concat.addU8(0x41);
            concat.addU8(0x0F);
            concat.addU8(0x97);
            concat.addU8(0xC0 | (static_cast<uint8_t>(reg) & 0b111));
            break;

        case CPUCondFlag::O:
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::R8);
            if (reg >= CPUReg::R8)
                concat.addU8(0x41);
            concat.addU8(0x0F);
            concat.addU8(0x90);
            concat.addU8(0xC0 | (static_cast<uint8_t>(reg) & 0b111));
            break;

        case CPUCondFlag::AE:
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::R8);
            if (reg >= CPUReg::R8)
                concat.addU8(0x41);
            concat.addU8(0x0F);
            concat.addU8(0x93);
            concat.addU8(0xC0 | (static_cast<uint8_t>(reg) & 0b111));
            break;

        case CPUCondFlag::G:
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::R8);
            if (reg >= CPUReg::R8)
                concat.addU8(0x41);
            concat.addU8(0x0F);
            concat.addU8(0x9F);
            concat.addU8(0xC0 | (static_cast<uint8_t>(reg) & 0b111));
            break;

        case CPUCondFlag::NE:
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x0F);
            concat.addU8(0x95);
            concat.addU8(0xC0);
            break;

        case CPUCondFlag::NA:
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x0F);
            concat.addU8(0x96);
            concat.addU8(0xC0);
            break;

        case CPUCondFlag::B:
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x0F);
            concat.addU8(0x92);
            concat.addU8(0xC0);
            break;

        case CPUCondFlag::BE:
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x0F);
            concat.addU8(0x96);
            concat.addU8(0xC0);
            break;

        case CPUCondFlag::E:
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x0F);
            concat.addU8(0x94);
            concat.addU8(0xC0);
            break;

        case CPUCondFlag::EP:
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

        case CPUCondFlag::NEP:
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

        case CPUCondFlag::GE:
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x0F);
            concat.addU8(0x9D);
            concat.addU8(0xC0);
            break;

        case CPUCondFlag::L:
            SWAG_ASSERT(reg == CPUReg::RAX);
            concat.addU8(0x0F);
            concat.addU8(0x9C);
            concat.addU8(0xC0);
            break;

        case CPUCondFlag::LE:
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

void SCBE_X64::emitCmp(CPUReg reg0, CPUReg reg1, OpBits opBits)
{
    if (opBits == OpBits::F32)
    {
        SWAG_ASSERT(reg0 < CPUReg::R8 && reg1 < CPUReg::R8);
        concat.addU8(0x0F);
        concat.addU8(0x2F);
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(reg0), static_cast<uint8_t>(reg1)));
    }
    else if (opBits == OpBits::F64)
    {
        SWAG_ASSERT(reg0 < CPUReg::R8 && reg1 < CPUReg::R8);
        concat.addU8(0x66);
        concat.addU8(0x0F);
        concat.addU8(0x2F);
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(reg0), static_cast<uint8_t>(reg1)));
    }
    else
    {
        emitREX(concat, opBits, reg1, reg0);
        emitSpecB8(concat, static_cast<uint8_t>(CPUOp::CMP), opBits);
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(reg1), static_cast<uint8_t>(reg0)));
    }
}

void SCBE_X64::emitCmp(CPUReg reg, uint64_t value, OpBits opBits)
{
    SWAG_ASSERT(SCBE_CPU::isInt(opBits));
    maskValue(value, opBits);

    if (opBits == OpBits::B8)
    {
        SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX);
        if (reg == CPUReg::RAX)
        {
            concat.addU8(0x3C);
            emitValue(concat, value, OpBits::B8);
        }
        else if (reg == CPUReg::RCX)
        {
            concat.addU8(0x80);
            concat.addU8(0xF9);
            emitValue(concat, value, OpBits::B8);
        }
    }
    else if (value <= 0x7F ||
             (opBits == OpBits::B16 && value >= 0xFF80) ||
             (opBits == OpBits::B32 && value >= 0xFFFFFF80) ||
             (opBits == OpBits::B64 && value >= 0xFFFFFFFFFFFFFF80))
    {
        SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX);
        emitREX(concat, opBits);
        concat.addU8(0x83);
        concat.addU8(0xF8 | static_cast<uint8_t>(reg));
        emitValue(concat, value & 0xFF, OpBits::B8);
    }
    else if (opBits == OpBits::B16 && value <= 0x7FFF)
    {
        SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX);
        emitREX(concat, opBits);
        if (reg == CPUReg::RAX)
        {
            concat.addU8(0x3d);
            emitValue(concat, value, OpBits::B16);
        }
        else if (reg == CPUReg::RCX)
        {
            concat.addU8(0x81);
            concat.addU8(0xF9);
            emitValue(concat, value, OpBits::B16);
        }
    }
    else if (value <= 0x7FFFFFFF)
    {
        SWAG_ASSERT(reg == CPUReg::RAX);
        emitREX(concat, opBits);
        concat.addU8(0x3d);
        emitValue(concat, value, OpBits::B32);
    }
    else
    {
        SWAG_ASSERT(reg == CPUReg::RAX);
        emitLoad(CPUReg::RCX, value, opBits);
        emitCmp(reg, CPUReg::RCX, opBits);
    }
}

void SCBE_X64::emitCmp(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits)
{
    if (opBits == OpBits::F32)
    {
        SWAG_ASSERT(reg < CPUReg::R8 && memReg < CPUReg::R8);
        concat.addU8(0x0F);
        concat.addU8(0x2E);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (opBits == OpBits::F64)
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
        emitREX(concat, opBits);
        emitSpecB8(concat, 0x3B, opBits);
        emitModRM(concat, memOffset, reg, memReg);
    }
}

void SCBE_X64::emitCmp(CPUReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    SWAG_ASSERT(SCBE_CPU::isInt(opBits));
    maskValue(value, opBits);

    if (opBits == OpBits::B8)
    {
        emitREX(concat, opBits);
        concat.addU8(0x80);
        emitModRM(concat, memOffset, memReg, memReg, 0x39);
        emitValue(concat, value, OpBits::B8);
    }
    else if (value <= 0x7F ||
             (opBits == OpBits::B16 && value >= 0xFF80) ||
             (opBits == OpBits::B32 && value >= 0xFFFFFF80) ||
             (opBits == OpBits::B64 && value >= 0xFFFFFFFFFFFFFF80))
    {
        emitREX(concat, opBits);
        concat.addU8(0x83);
        emitModRM(concat, memOffset, memReg, memReg, 0x39);
        emitValue(concat, value, OpBits::B8);
    }
    else if (value > 0x7FFFFFFF)
    {
        emitLoad(CPUReg::RAX, memReg, memOffset, opBits);
        emitCmp(CPUReg::RAX, value, opBits);
    }
    else
    {
        emitREX(concat, opBits);
        concat.addU8(0x81);
        emitModRM(concat, memOffset, memReg, memReg, 0x39);
        if (opBits == OpBits::B16)
            emitValue(concat, value, OpBits::B16);
        else
            emitValue(concat, value, OpBits::B32);
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitOpUnary([[maybe_unused]] CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits)
{
    if (op == CPUOp::NOT)
    {
        SWAG_ASSERT(memReg == CPUReg::RDI);
        SWAG_ASSERT(memOffset <= 0x7FFFFFFF);

        emitREX(concat, opBits);
        emitSpecB8(concat, 0xF7, opBits);
        if (memOffset <= 0x7F)
        {
            concat.addU8(0x57);
            emitValue(concat, memOffset, OpBits::B8);
        }
        else
        {
            concat.addU8(0x97);
            emitValue(concat, memOffset, OpBits::B32);
        }
    }
    else if (op == CPUOp::NEG)
    {
        SWAG_ASSERT(memReg == CPUReg::RDI);
        SWAG_ASSERT(memOffset <= 0x7FFFFFFF);
        emitREX(concat, opBits);
        concat.addU8(0xF7);
        if (memOffset <= 0x7F)
        {
            concat.addU8(0x5F);
            emitValue(concat, memOffset, OpBits::B8);
        }
        else
        {
            concat.addU8(0x9F);
            emitValue(concat, memOffset, OpBits::B32);
        }
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void SCBE_X64::emitOpUnary(CPUReg reg, CPUOp op, OpBits opBits)
{
    if (op == CPUOp::NOT)
    {
        emitREX(concat, opBits);
        emitSpecB8(concat, static_cast<uint8_t>(op), opBits);
        concat.addU8(0xD0 | (static_cast<uint8_t>(reg) & 0b111));
    }
    else if (op == CPUOp::NEG)
    {
        if (isFloat(opBits))
        {
            SWAG_ASSERT(reg == CPUReg::XMM0);
            emitStore(offsetFLTReg, offsetFLT, opBits == OpBits::F32 ? 0x80000000 : 0x80000000'00000000, OpBits::B64);
            emitLoad(CPUReg::XMM1, offsetFLTReg, offsetFLT, opBits);
            emitOpBinary(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FXOR, opBits);
        }
        else
        {
            SWAG_ASSERT(reg < CPUReg::R8);
            SWAG_ASSERT(opBits == OpBits::B32 || opBits == OpBits::B64);
            emitREX(concat, opBits);
            concat.addU8(0xF7);
            concat.addU8(0xD8 | (static_cast<uint8_t>(reg) & 0b111));
        }
    }
    else if (op == CPUOp::BSWAP)
    {
        SWAG_ASSERT(reg == CPUReg::RAX);
        SWAG_ASSERT(opBits == OpBits::B16 || opBits == OpBits::B32 || opBits == OpBits::B64);

        emitREX(concat, opBits);
        if (opBits == OpBits::B16)
        {
            concat.addU8(0xC1);
            concat.addU8(0xC0);
            concat.addU8(0x08);
        }
        else
        {
            concat.addU8(0x0F);
            concat.addU8(0xC8);
        }
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void SCBE_X64::emitOpBinary(CPUReg regDst, CPUReg regSrc, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    if (opBits == OpBits::F32)
    {
        if (op != CPUOp::FSQRT &&
            op != CPUOp::FAND &&
            op != CPUOp::UCOMIF &&
            op != CPUOp::FXOR)
        {
            concat.addU8(0xF3);
            emitREX(concat, emitFlags.has(EMITF_B64) ? OpBits::B64 : OpBits::B32, regSrc);
        }

        concat.addU8(0x0F);
        emitCPUOp(concat, op);
        concat.addU8(static_cast<uint8_t>(0xC0 | static_cast<uint8_t>(regSrc) | static_cast<uint8_t>(regDst) << 3));
    }
    else if (opBits == OpBits::F64)
    {
        if (op != CPUOp::FSQRT &&
            op != CPUOp::FAND &&
            op != CPUOp::UCOMIF &&
            op != CPUOp::FXOR)
        {
            concat.addU8(0xF2);
            emitREX(concat, emitFlags.has(EMITF_B64) ? OpBits::B64 : OpBits::B32, regSrc);
        }
        else
        {
            concat.addU8(0x66);
        }

        concat.addU8(0x0F);
        emitCPUOp(concat, op);
        concat.addU8(static_cast<uint8_t>(0xC0 | static_cast<uint8_t>(regSrc) | static_cast<uint8_t>(regDst) << 3));
    }
    else if (op == CPUOp::DIV ||
             op == CPUOp::IDIV)
    {
        SWAG_ASSERT(regDst == CPUReg::RAX && regSrc == CPUReg::RCX);
        emitREX(concat, opBits, regSrc, regDst);
        emitSpecB8(concat, 0xF7, opBits);
        concat.addU8(static_cast<uint8_t>(op) & ~2);
    }
    else if (op == CPUOp::MOD ||
             op == CPUOp::IMOD)
    {
        SWAG_ASSERT(regDst == CPUReg::RAX && regSrc == CPUReg::RCX);
        emitREX(concat, opBits, regSrc, regDst);
        emitSpecB8(concat, 0xF7, opBits);
        concat.addU8(static_cast<uint8_t>(op) & ~2);
        if (opBits == OpBits::B8)
            emitLoad(CPUReg::RAX, opBits);
        else
            emitLoad(CPUReg::RAX, CPUReg::RDX, opBits);
    }
    else if (op == CPUOp::MUL ||
             op == CPUOp::IMUL)
    {
        SWAG_ASSERT(regDst == CPUReg::RAX && regSrc == CPUReg::RCX);
        emitREX(concat, opBits, regSrc, regDst);
        emitSpecB8(concat, 0xF7, opBits);
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
        emitREX(concat, opBits, regSrc, regDst);
        emitSpecB8(concat, 0xD3, opBits);
        emitCPUOp(concat, op);
    }
    else if (op == CPUOp::ADD ||
             op == CPUOp::SUB ||
             op == CPUOp::XOR ||
             op == CPUOp::AND ||
             op == CPUOp::OR)
    {
        emitREX(concat, opBits, regSrc, regDst);
        emitSpecB8(concat, static_cast<uint8_t>(op), opBits);
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(regSrc), static_cast<uint8_t>(regDst)));
    }
    else if (op == CPUOp::BSF ||
             op == CPUOp::BSR)
    {
        SWAG_ASSERT(regSrc == CPUReg::RAX && regDst == CPUReg::RAX);
        emitREX(concat, opBits, regSrc, regDst);
        concat.addU8(0x0F);
        emitCPUOp(concat, op);
        concat.addU8(0xC0);
    }
    else if (op == CPUOp::POPCNT)
    {
        SWAG_ASSERT(regSrc == CPUReg::RAX && regDst == CPUReg::RAX);
        if (opBits == OpBits::B16)
            emitREX(concat, opBits, regSrc, regDst);
        concat.addU8(0xF3);
        if (opBits == OpBits::B64)
            emitREX(concat, opBits, regSrc, regDst);
        concat.addU8(0x0F);
        emitCPUOp(concat, op);
        concat.addU8(0xC0);
    }
    else if (op == CPUOp::NOT)
    {
        SWAG_ASSERT(regSrc == regDst);
        emitREX(concat, opBits);
        emitSpecB8(concat, static_cast<uint8_t>(op), opBits);
        concat.addU8(0xD0 | (static_cast<uint8_t>(regSrc) & 0b111));
    }
    else if (op == CPUOp::ROL ||
             op == CPUOp::ROR)
    {
        SWAG_ASSERT(regDst == CPUReg::RAX && regSrc == CPUReg::RCX);
        emitREX(concat, opBits, regDst, regSrc);
        emitSpecB8(concat, 0xD3, opBits);
        if (op == CPUOp::ROL)
            concat.addU8(0xC0);
        else
            concat.addU8(0xC8);
    }
    else if (op == CPUOp::CMOVB ||
             op == CPUOp::CMOVE ||
             op == CPUOp::CMOVG ||
             op == CPUOp::CMOVL ||
             op == CPUOp::CMOVBE ||
             op == CPUOp::CMOVGE)
    {
        if (opBits == OpBits::B8 || opBits == OpBits::B16)
            opBits = OpBits::B32;
        emitREX(concat, opBits, regDst, regSrc);
        concat.addU8(0x0F);
        emitCPUOp(concat, op);
        concat.addU8(getModRM(RegReg, static_cast<uint8_t>(regDst), static_cast<uint8_t>(regSrc)));
    }
    else if (op == CPUOp::CMPXCHG)
    {
        SWAG_ASSERT(regDst == CPUReg::RCX && regSrc == CPUReg::RDX);
        // lock CMPXCHG [rcx], dl
        if (opBits == OpBits::B16)
            emitREX(concat, opBits);
        concat.addU8(0xF0);
        if (opBits == OpBits::B64)
            emitREX(concat, opBits);
        concat.addU8(0x0F);
        emitSpecB8(concat, 0xB1, opBits);
        concat.addU8(0x11);
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void SCBE_X64::emitOpBinary(CPUReg memReg, uint64_t memOffset, CPUReg reg, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    if (opBits == OpBits::F32 ||
        opBits == OpBits::F64)
    {
        SWAG_ASSERT(reg == CPUReg::XMM1);
        SWAG_ASSERT(memReg < CPUReg::R8);
        emitLoad(CPUReg::XMM0, memReg, memOffset, opBits);
        emitOpBinary(CPUReg::XMM0, reg, op, opBits, emitFlags);
        emitStore(memReg, memOffset, CPUReg::XMM0, opBits);
    }
    else if (op == CPUOp::IMUL ||
             op == CPUOp::MUL)
    {
        SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
        SWAG_ASSERT(reg == CPUReg::RCX);
        if (memReg == CPUReg::RAX)
        {
            emitLoad(CPUReg::R8, memReg, OpBits::B64);
            memReg = CPUReg::R8;
        }
        emitLoad(CPUReg::RAX, memReg, memOffset, opBits);
        emitOpBinary(CPUReg::RAX, reg, op, opBits, emitFlags);
        emitStore(memReg, memOffset, CPUReg::RAX, opBits);
    }
    else if (op == CPUOp::DIV ||
             op == CPUOp::IDIV ||
             op == CPUOp::MOD ||
             op == CPUOp::IMOD)
    {
        SWAG_ASSERT(memReg == CPUReg::RAX);
        SWAG_ASSERT(reg == CPUReg::RCX);
        if (memReg == CPUReg::RAX)
        {
            emitLoad(CPUReg::R8, memReg, OpBits::B64);
            memReg = CPUReg::R8;
        }
        emitLoad(CPUReg::RAX, memReg, memOffset, 0, false, op, opBits);
        emitOpBinary(CPUReg::RAX, reg, op, opBits, emitFlags);
        emitStore(memReg, memOffset, CPUReg::RAX, opBits);
    }
    else if (op == CPUOp::SAR ||
             op == CPUOp::SHR ||
             op == CPUOp::SHL)
    {
        SWAG_ASSERT(memReg == CPUReg::RAX && reg == CPUReg::RCX);
        if (emitFlags.has(EMITF_Lock))
            concat.addU8(0xF0);
        emitREX(concat, opBits, reg, memReg);
        emitSpecB8(concat, 0xD3, opBits);
        concat.addU8(static_cast<uint8_t>(op) & ~0xC0);
    }
    else
    {
        SWAG_ASSERT(memReg < CPUReg::R8);
        if (emitFlags.has(EMITF_Lock))
            concat.addU8(0xF0);
        emitREX(concat, opBits, reg, memReg);
        emitSpecB8(concat, static_cast<uint8_t>(op), opBits);
        emitModRM(concat, memOffset, reg, memReg);
    }
}

void SCBE_X64::emitOpBinary(CPUReg reg, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    if (isNoOp(value, op, opBits, emitFlags))
        return;

    ///////////////////////////////////////////

    if (value > 0x7FFFFFFF)
    {
        SWAG_ASSERT(reg == CPUReg::RAX);
        emitLoad(CPUReg::RCX, value, OpBits::B64);
        emitOpBinary(reg, CPUReg::RCX, op, opBits, emitFlags);
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::XOR || op == CPUOp::OR || op == CPUOp::AND)
    {
        if (opBits == OpBits::B8)
        {
            SWAG_ASSERT(reg == CPUReg::RAX);
            emitREX(concat, opBits);
            concat.addU8(static_cast<uint8_t>(op) + 3);
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(reg == CPUReg::RAX);
            emitREX(concat, opBits);
            concat.addU8(0x83);
            concat.addU8(static_cast<uint8_t>(op) + 0xBF);
            emitValue(concat, value, OpBits::B8);
        }
        else if (opBits == OpBits::B16)
        {
            SWAG_ASSERT(reg == CPUReg::RAX);
            emitREX(concat, opBits);
            concat.addU8(0x81);
            concat.addU8(static_cast<uint8_t>(op) + 0xBF);
            emitValue(concat, value, OpBits::B16);
        }
        else if (opBits == OpBits::B32 || opBits == OpBits::B64)
        {
            SWAG_ASSERT(reg == CPUReg::RAX);
            emitREX(concat, opBits);
            concat.addU8(0x81);
            concat.addU8(static_cast<uint8_t>(op) + 0xBF);
            emitValue(concat, value, OpBits::B32);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::ADD)
    {
        if (value == 1 && !emitFlags.has(EMITF_Overflow))
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RSP);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xFF, opBits);
            concat.addU8(0xC0 | static_cast<uint8_t>(reg)); // inc rax
        }
        else if (opBits == OpBits::B8)
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RSP);
            emitREX(concat, opBits);
            if (reg == CPUReg::RAX)
                concat.addU8(0x04);
            else
            {
                concat.addU8(0x80);
                concat.addU8(0xC0 | static_cast<uint8_t>(reg));
            }
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RSP);
            emitREX(concat, opBits);
            concat.addU8(0x83);
            concat.addU8(0xC0 | static_cast<uint8_t>(reg));
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RSP);
            emitREX(concat, opBits);
            if (reg == CPUReg::RAX)
                concat.addU8(0x05);
            else
            {
                concat.addU8(0x81);
                concat.addU8(0xC0 | static_cast<uint8_t>(reg));
            }
            emitValue(concat, value, opBits == OpBits::B16 ? OpBits::B16 : OpBits::B32);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::SUB)
    {
        if (value == 1 && !emitFlags.has(EMITF_Overflow))
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RSP);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xFF, opBits);
            concat.addU8(0xC8 | static_cast<uint8_t>(reg)); // dec rax
        }
        else if (opBits == OpBits::B8)
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RSP);
            emitREX(concat, opBits);
            if (reg == CPUReg::RAX)
                concat.addU8(0x2C);
            else
            {
                concat.addU8(0x80);
                concat.addU8(0xE8 | static_cast<uint8_t>(reg));
            }
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RSP);
            emitREX(concat, opBits);
            concat.addU8(0x83);
            concat.addU8(0xE8 | static_cast<uint8_t>(reg));
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RSP);
            emitREX(concat, opBits);
            if (reg == CPUReg::RAX)
                concat.addU8(0x2D);
            else
            {
                concat.addU8(0x81);
                concat.addU8(0xE8 | static_cast<uint8_t>(reg));
            }
            emitValue(concat, value, opBits == OpBits::B16 ? OpBits::B16 : OpBits::B32);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::MOD || op == CPUOp::IMOD)
    {
        if (Math::isPowerOfTwo(value))
        {
            emitOpBinary(reg, value - 1, CPUOp::AND, opBits, emitFlags);
        }
        else
        {
            SWAG_ASSERT(reg == CPUReg::RAX);
            emitLoad(CPUReg::RCX, value, opBits);
            emitOpBinary(reg, CPUReg::RCX, op, opBits, emitFlags);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::DIV)
    {
        if (value <= 0x7F && Math::isPowerOfTwo(value))
        {
            emitOpBinary(reg, static_cast<uint32_t>(log2(value)), CPUOp::SHR, opBits, emitFlags);
        }
        else
        {
            SWAG_ASSERT(reg == CPUReg::RAX);
            emitLoad(CPUReg::RCX, value, opBits);
            emitOpBinary(reg, CPUReg::RCX, op, opBits, emitFlags);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::IDIV)
    {
        if (value <= 0x7F && Math::isPowerOfTwo(value))
        {
            emitOpBinary(reg, static_cast<uint32_t>(log2(value)), CPUOp::SAR, opBits, emitFlags);
        }
        else
        {
            SWAG_ASSERT(reg == CPUReg::RAX);
            emitLoad(CPUReg::RCX, value, opBits);
            emitOpBinary(reg, CPUReg::RCX, op, opBits, emitFlags);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::MUL)
    {
        if (value == 0)
        {
            emitClear(reg, opBits);
        }
        else if (value <= 0x7F && Math::isPowerOfTwo(value))
        {
            emitOpBinary(reg, static_cast<uint32_t>(log2(value)), CPUOp::SHL, opBits, emitFlags);
        }
        else
        {
            SWAG_ASSERT(reg == CPUReg::RAX);
            emitLoad(CPUReg::RCX, value, opBits);
            emitOpBinary(reg, CPUReg::RCX, op, opBits, emitFlags);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::IMUL)
    {
        if (value == 0)
        {
            emitClear(reg, opBits);
        }
        else if (value <= 0x7F && Math::isPowerOfTwo(value))
        {
            emitOpBinary(reg, static_cast<uint32_t>(log2(value)), CPUOp::SHL, opBits, emitFlags);
        }
        else if (opBits == OpBits::B8)
        {
            SWAG_ASSERT(reg == CPUReg::RAX);
            emitLoad(CPUReg::RCX, value, opBits);
            emitOpBinary(reg, CPUReg::RCX, op, opBits, emitFlags);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX);
            emitREX(concat, opBits);
            concat.addU8(0x6B);
            concat.addU8(reg == CPUReg::RAX ? 0xC0 : 0xC9);
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX);
            emitREX(concat, opBits);
            concat.addU8(0x69);
            concat.addU8(reg == CPUReg::RAX ? 0xC0 : 0xC9);
            emitValue(concat, value, OpBits::B32);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::SAR || op == CPUOp::SHR || op == CPUOp::SHL)
    {
        if (value == 1)
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xD1, opBits);
            concat.addU8(static_cast<uint8_t>(op) | static_cast<uint8_t>(reg));
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX);
            emitREX(concat, opBits);
            value = min(value, SCBE_CPU::getNumBits(opBits) - 1);
            emitSpecB8(concat, 0xC1, opBits);
            concat.addU8(static_cast<uint8_t>(op) | static_cast<uint8_t>(reg));
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(false);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::BT)
    {
        if (value <= 0x7F)
        {
            emitREX(concat, opBits, CPUReg::RAX, reg);
            concat.addU8(0x0F);
            emitCPUOp(concat, op);
            concat.addU8(0xE2);
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(false);
        }
    }

    ///////////////////////////////////////////

    else
    {
        SWAG_ASSERT(false);
    }
}

void SCBE_X64::emitOpBinary(CPUReg memReg, uint64_t memOffset, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    SWAG_ASSERT(SCBE_CPU::isInt(opBits));
    if (isNoOp(value, op, opBits, emitFlags))
        return;
    maskValue(value, opBits);

    ///////////////////////////////////////////

    if (value > 0x7FFFFFFF)
    {
        SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
        emitLoad(CPUReg::RCX, value, opBits);
        emitOpBinary(memReg, memOffset, CPUReg::RCX, op, opBits, emitFlags);
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::IMOD || op == CPUOp::MOD)
    {
        if (Math::isPowerOfTwo(value))
        {
            emitOpBinary(memReg, memOffset, value - 1, CPUOp::AND, opBits, emitFlags);
        }
        else
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
            if (memReg == CPUReg::RAX)
            {
                emitLoad(CPUReg::R8, memReg, OpBits::B64);
                memReg = CPUReg::R8;
            }
            emitLoad(CPUReg::RAX, memReg, memOffset, 0, false, op, opBits);
            emitOpBinary(CPUReg::RAX, value, op, opBits, emitFlags);
            emitStore(memReg, memOffset, CPUReg::RAX, opBits);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::IDIV || op == CPUOp::DIV)
    {
        if (value <= 0x7F && Math::isPowerOfTwo(value))
        {
            emitOpBinary(memReg, memOffset, static_cast<uint32_t>(log2(value)), op == CPUOp::IDIV ? CPUOp::SAR : CPUOp::SHR, opBits, emitFlags);
        }
        else
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
            if (memReg == CPUReg::RAX)
            {
                emitLoad(CPUReg::R8, memReg, OpBits::B64);
                memReg = CPUReg::R8;
            }
            emitLoad(CPUReg::RAX, memReg, memOffset, 0, false, op, opBits);
            emitOpBinary(CPUReg::RAX, value, op, opBits, emitFlags);
            emitStore(memReg, memOffset, CPUReg::RAX, opBits);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::IMUL || op == CPUOp::MUL)
    {
        SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
        if (value <= 0x7F && Math::isPowerOfTwo(value))
        {
            emitOpBinary(memReg, memOffset, static_cast<uint32_t>(log2(value)), CPUOp::SHL, opBits, emitFlags);
        }
        else
        {
            if (memReg == CPUReg::RAX)
            {
                emitLoad(CPUReg::R8, memReg, OpBits::B64);
                memReg = CPUReg::R8;
            }
            emitLoad(CPUReg::RAX, memReg, memOffset, 0, false, op, opBits);
            emitOpBinary(CPUReg::RAX, value, op, opBits, emitFlags);
            emitStore(memReg, memOffset, CPUReg::RAX, opBits);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::SAR || op == CPUOp::SHR || op == CPUOp::SHL)
    {
        if (value == 1)
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xD1, opBits);
            emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg, 1 + static_cast<uint8_t>(op) & 0x3F);
        }
        else
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
            value = min(value, SCBE_CPU::getNumBits(opBits) - 1);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xC1, opBits);
            emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg, 1 + static_cast<uint8_t>(op) & 0x3F);
            emitValue(concat, value, OpBits::B8);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::ADD)
    {
        if (value == 1 && !emitFlags.has(EMITF_Overflow))
        {
            SWAG_ASSERT(memReg < CPUReg::R8);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xFF, opBits);
            emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg);
        }
        else if (opBits == OpBits::B8)
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
            emitREX(concat, opBits);
            concat.addU8(0x80);
            emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
            emitREX(concat, opBits);
            concat.addU8(0x83);
            emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
            emitREX(concat, opBits);
            concat.addU8(0x81);
            emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, min(opBits, OpBits::B32));
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::SUB)
    {
        if (value == 1 && !emitFlags.has(EMITF_Overflow))
        {
            SWAG_ASSERT(memReg < CPUReg::R8);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xFF, opBits);
            emitModRM(concat, memOffset, static_cast<CPUReg>(1), memReg);
        }
        else if (opBits == OpBits::B8)
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
            emitREX(concat, opBits);
            concat.addU8(0x80);
            emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
            emitREX(concat, opBits);
            concat.addU8(0x83);
            emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
            emitREX(concat, opBits);
            concat.addU8(0x81);
            emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, min(opBits, OpBits::B32));
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::OR || op == CPUOp::AND || op == CPUOp::XOR)
    {
        if (opBits == OpBits::B8)
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
            emitREX(concat, opBits);
            concat.addU8(0x80);
            emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
            emitREX(concat, opBits);
            concat.addU8(0x83);
            emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RDI);
            emitREX(concat, opBits);
            concat.addU8(0x81);
            emitModRM(concat, memOffset, static_cast<CPUReg>(0), memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, min(opBits, OpBits::B32));
        }
    }

    ///////////////////////////////////////////

    else
    {
        SWAG_ASSERT(false);
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitJumpTable([[maybe_unused]] CPUReg table, [[maybe_unused]] CPUReg offset)
{
    SWAG_ASSERT(table == CPUReg::RCX && offset == CPUReg::RAX);
    emitREX(concat, OpBits::B64);
    concat.addString3("\x63\x0C\x81"); // movsx rcx, dword ptr [rcx + rax*4]
}

CPUJump SCBE_X64::emitJump(CPUCondJump jumpType, OpBits opBits)
{
    SWAG_ASSERT(opBits == OpBits::B8 || opBits == OpBits::B32);

    if (opBits == OpBits::B8)
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

        CPUJump jump;
        jump.addr   = concat.getSeekPtr() - 1;
        jump.offset = concat.totalCount();
        jump.opBits = opBits;
        return jump;
    }

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
    CPUJump jump;
    jump.addr   = concat.getSeekPtr() - sizeof(uint32_t);
    jump.offset = concat.totalCount();
    jump.opBits = opBits;
    return jump;
}

void SCBE_X64::patchJump(const CPUJump& jump, uint64_t offsetDestination)
{
    const int32_t offset = static_cast<int32_t>(offsetDestination - jump.offset);
    if (jump.opBits == OpBits::B8)
    {
        SWAG_ASSERT(offset >= -127 && offset <= 128);
        *static_cast<uint8_t*>(jump.addr) = static_cast<int8_t>(offset);
    }
    else
    {
        *static_cast<uint32_t*>(jump.addr) = static_cast<int32_t>(offset);
    }
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
        emitLoad(CPUReg::RAX, regSrc, offset, OpBits::B64);
        emitStore(regDst, offset, CPUReg::RAX, OpBits::B64);
        count -= 8;
        offset += 8;
    }

    while (count >= 4)
    {
        emitLoad(CPUReg::RAX, regSrc, offset, OpBits::B32);
        emitStore(regDst, offset, CPUReg::RAX, OpBits::B32);
        count -= 4;
        offset += 4;
    }

    while (count >= 2)
    {
        emitLoad(CPUReg::RAX, regSrc, offset, OpBits::B16);
        emitStore(regDst, offset, CPUReg::RAX, OpBits::B16);
        count -= 2;
        offset += 2;
    }

    while (count >= 1)
    {
        emitLoad(CPUReg::RAX, regSrc, offset, OpBits::B8);
        emitStore(regDst, offset, CPUReg::RAX, OpBits::B8);
        count -= 1;
        offset += 1;
    }
}

void SCBE_X64::emitClear(CPUReg memReg, uint64_t memOffset, uint32_t count)
{
    if (!count)
        return;
    SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RCX || memReg == CPUReg::RDI);
    SWAG_ASSERT(memOffset <= 0x7FFFFFFF);

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
                emitValue(concat, memOffset, OpBits::B8);
            }
            else
            {
                concat.addU8(0x80 | static_cast<uint8_t>(memReg));
                emitValue(concat, memOffset, OpBits::B32);
            }
            count -= 16;
            memOffset += 16;
        }
    }

    while (count >= 8)
    {
        emitStore(memReg, static_cast<uint32_t>(memOffset), 0, OpBits::B64);
        count -= 8;
        memOffset += 8;
    }

    while (count >= 4)
    {
        emitStore(memReg, static_cast<uint32_t>(memOffset), 0, OpBits::B32);
        count -= 4;
        memOffset += 4;
    }

    while (count >= 2)
    {
        emitStore(memReg, static_cast<uint32_t>(memOffset), 0, OpBits::B16);
        count -= 2;
        memOffset += 2;
    }

    while (count >= 1)
    {
        emitStore(memReg, static_cast<uint32_t>(memOffset), 0, OpBits::B8);
        count -= 1;
        memOffset += 1;
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitCallExtern(const Utf8& symbolName)
{
    concat.addU8(0xFF);
    concat.addU8(0x15);
    const auto callSym = getOrAddSymbol(symbolName, CPUSymbolKind::Extern);
    addSymbolRelocation(concat.totalCount() - textSectionOffset, callSym->index, IMAGE_REL_AMD64_REL32);
    concat.addU32(0);
}

void SCBE_X64::emitCallLocal(const Utf8& symbolName)
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

void SCBE_X64::emitCallIndirect(CPUReg reg)
{
    SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::R10);
    if (reg == CPUReg::R10)
        concat.addU8(0x41);
    concat.addU8(0xFF);
    concat.addU8(0xD0 | (static_cast<uint8_t>(reg) & 0b111));
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitNop()
{
    concat.addU8(0x90);
}

// a*b+c
void SCBE_X64::emitMulAdd([[maybe_unused]] CPUReg regDst, [[maybe_unused]] CPUReg regMul, [[maybe_unused]] CPUReg regAdd, OpBits opBits)
{
    SWAG_ASSERT(regDst == CPUReg::XMM0);
    SWAG_ASSERT(regMul == CPUReg::XMM1);
    SWAG_ASSERT(regAdd == CPUReg::XMM2);

    emitSpecF64(concat, 0xF3, opBits);
    concat.addU8(0x0F);
    emitCPUOp(concat, CPUOp::FMUL);
    concat.addU8(0xC1);

    emitSpecF64(concat, 0xF3, opBits);
    concat.addU8(0x0F);
    emitCPUOp(concat, CPUOp::FADD);
    concat.addU8(0xC2);
}
