// ReSharper disable CommentTypo
#include "pch.h"
#include "Backend/SCBE/Encoder/X64/SCBE_X64.h"
#include "Core/Math.h"
#include "Semantic/Type/TypeManager.h"
#include "Wmf/Module.h"
#pragma warning(disable : 4063)

enum class ModRMMode : uint8_t
{
    Memory         = 0b00,
    Displacement8  = 0b01,
    Displacement32 = 0b10,
    Register       = 0b11,
};

constexpr auto REX_REG_NONE = static_cast<CPUReg>(250);
constexpr auto MODRM_REG_0  = static_cast<CPUReg>(251);
constexpr auto MODRM_REG_1  = static_cast<CPUReg>(252);
constexpr auto MODRM_REG_2  = static_cast<CPUReg>(253);
constexpr auto MODRM_REG_3  = static_cast<CPUReg>(254);
constexpr auto MODRM_REG_4  = static_cast<CPUReg>(255);

constexpr uint8_t MODRM_RM_SID = 0b100;
constexpr uint8_t MODRM_RM_RIP = 0b101;

namespace
{
    uint8_t encodeReg(CPUReg reg)
    {
        switch (reg)
        {
            case CPUReg::RAX:
                return 0b0000;
            case CPUReg::RBX:
                return 0b0011;
            case CPUReg::RCX:
                return 0b0001;
            case CPUReg::RDX:
                return 0b0010;
            case CPUReg::RSP:
                return 0b0100;
            case CPUReg::RBP:
                return 0b0101;
            case CPUReg::RSI:
                return 0b0110;
            case CPUReg::RDI:
                return 0b0111;
            case CPUReg::R8:
                return 0b1000;
            case CPUReg::R9:
                return 0b1001;
            case CPUReg::R10:
                return 0b1010;
            case CPUReg::R11:
                return 0b1011;
            case CPUReg::R12:
                return 0b1100;
            case CPUReg::R13:
                return 0b1101;
            case CPUReg::R14:
                return 0b1110;
            case CPUReg::R15:
                return 0b1111;
            case CPUReg::XMM0:
                return 0b0000;
            case CPUReg::XMM1:
                return 0b0001;
            case CPUReg::XMM2:
                return 0b0010;
            case CPUReg::XMM3:
                return 0b0011;
            case CPUReg::RIP:
                return 0b10000;

            case REX_REG_NONE:
            case MODRM_REG_0:
                return 0;
            case MODRM_REG_1:
                return 1;
            case MODRM_REG_2:
                return 2;
            case MODRM_REG_3:
                return 3;
            case MODRM_REG_4:
                return 4;

            default:
                SWAG_ASSERT(false);
                return 0;
        }
    }

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

    // Addressing mode
    uint8_t getModRM(ModRMMode mod, CPUReg reg, uint8_t rm)
    {
        const auto result = static_cast<uint32_t>(mod) << 6 | ((encodeReg(reg) & 0b111) << 3) | (rm & 0b111);
        return static_cast<uint8_t>(result);
    }

    // Scaled index addressing
    uint8_t getSid(uint8_t scale, CPUReg regIndex, CPUReg regBase)
    {
        const auto result = static_cast<uint32_t>(scale) << 6 | ((encodeReg(regIndex) & 0b111) << 3) | (encodeReg(regBase) & 0b111);
        return static_cast<uint8_t>(result);
    }

    void emitREX(Concat& concat, OpBits opBits, CPUReg reg0 = REX_REG_NONE, CPUReg reg1 = REX_REG_NONE)
    {
        if (opBits == OpBits::B16 || opBits == OpBits::F64)
            concat.addU8(0x66);
        const bool b1 = reg0 >= CPUReg::R8 && reg0 <= CPUReg::R15;
        const bool b2 = reg1 >= CPUReg::R8 && reg1 <= CPUReg::R15;
        if (opBits == OpBits::B64 || b1 || b2)
            concat.addU8(getREX(opBits == OpBits::B64, b1, false, b2));
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

    void emitModRM(Concat& concat, ModRMMode mod, CPUReg reg, uint8_t rm)
    {
        concat.addU8(getModRM(mod, reg, rm));
    }

    void emitModRM(Concat& concat, CPUReg reg, CPUReg memReg)
    {
        emitModRM(concat, ModRMMode::Register, reg, encodeReg(memReg));
    }

    void emitModRM(Concat& concat, uint64_t memOffset, CPUReg reg, CPUReg memReg, uint8_t op = 1)
    {
        if (memOffset == 0 && (memReg < CPUReg::R8 || memReg == CPUReg::R12))
        {
            // mov al, byte ptr [rdi]
            concat.addU8(getModRM(ModRMMode::Memory, reg, encodeReg(memReg)) | op - 1);
            if (memReg == CPUReg::RSP || memReg == CPUReg::R12)
                concat.addU8(0x24);
        }
        else if (memOffset <= 0x7F)
        {
            // mov al, byte ptr [rdi + ??]
            concat.addU8(getModRM(ModRMMode::Displacement8, reg, encodeReg(memReg)) | op - 1);
            if (memReg == CPUReg::RSP || memReg == CPUReg::R12)
                concat.addU8(0x24);
            emitValue(concat, memOffset, OpBits::B8);
        }
        else
        {
            // mov al, byte ptr [rdi + ????????]
            concat.addU8(getModRM(ModRMMode::Displacement32, reg, encodeReg(memReg)) | op - 1);
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

    void emitCPUOp(Concat& concat, CPUOp op)
    {
        concat.addU8(static_cast<uint8_t>(op));
    }

    void emitCPUOp(Concat& concat, uint8_t op)
    {
        concat.addU8(op);
    }

    void emitCPUOp(Concat& concat, uint8_t op, CPUReg reg)
    {
        concat.addU8(op | (encodeReg(reg) & 0b111));
    }

    void emitCPUOp(Concat& concat, CPUOp op, CPUReg reg)
    {
        concat.addU8(static_cast<uint8_t>(op) | (encodeReg(reg) & 0b111));
    }

    void emitSpecCPUOp(Concat& concat, CPUOp op, OpBits opBits)
    {
        emitSpecB8(concat, static_cast<uint8_t>(op), opBits);
    }

    void emitSpecCPUOp(Concat& concat, uint8_t op, OpBits opBits)
    {
        emitSpecB8(concat, op, opBits);
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

void SCBE_X64::emitSymbolRelocationAddress(CPUReg reg, uint32_t symbolIndex, uint32_t offset)
{
    emitREX(concat, OpBits::B64, reg);
    emitCPUOp(concat, 0x8D); // LEA
    emitModRM(concat, ModRMMode::Memory, reg, MODRM_RM_RIP);
    addSymbolRelocation(concat.totalCount() - textSectionOffset, symbolIndex, IMAGE_REL_AMD64_REL32);
    concat.addU32(offset);
}

void SCBE_X64::emitSymbolRelocationValue(CPUReg reg, uint32_t symbolIndex, uint32_t offset)
{
    SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RDX || reg == CPUReg::R8 || reg == CPUReg::R9);
    emitREX(concat, OpBits::B64, reg);
    emitCPUOp(concat, 0x8B); // MOV
    emitModRM(concat, ModRMMode::Memory, reg, MODRM_RM_RIP);
    addSymbolRelocation(concat.totalCount() - textSectionOffset, symbolIndex, IMAGE_REL_AMD64_REL32);
    concat.addU32(offset);
}

void SCBE_X64::emitSymbolGlobalString(CPUReg reg, const Utf8& str)
{
    emitLoad(reg, 0);
    const auto sym = getOrCreateGlobalString(str);
    addSymbolRelocation(concat.totalCount() - 8 - textSectionOffset, sym->index, IMAGE_REL_AMD64_ADDR64);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitPush(CPUReg reg)
{
    emitREX(concat, OpBits::Zero, REX_REG_NONE, reg);
    emitCPUOp(concat, 0x50, reg);
}

void SCBE_X64::emitPop(CPUReg reg)
{
    emitREX(concat, OpBits::Zero, REX_REG_NONE, reg);
    emitCPUOp(concat, 0x58, reg);
}

void SCBE_X64::emitRet()
{
    emitCPUOp(concat, 0xC3);
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

void SCBE_X64::emitLoad(CPUReg regDstSrc, OpBits opBits)
{
    SWAG_ASSERT(regDstSrc == CPUReg::RAX);
    SWAG_ASSERT(opBits == OpBits::B8);
    emitSpecCPUOp(concat, CPUOp::MOV, opBits);
    concat.addU8(0xE0);
}

void SCBE_X64::emitLoad(CPUReg reg, uint64_t value)
{
    emitREX(concat, OpBits::B64, REX_REG_NONE, reg);
    emitCPUOp(concat, 0xB8, reg);
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
        emitCPUOp(concat, 0xB8, CPUReg::RAX);
        emitValue(concat, value, OpBits::B32);
        emitLoad(reg, CPUReg::RAX, OpBits::F32);
    }
    else if (opBits == OpBits::F64)
    {
        emitREX(concat, OpBits::B64);
        if (value <= 0x7FFFFFFF)
        {
            concat.addU8(0xC7);
            emitCPUOp(concat, 0xC0, CPUReg::RAX);
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
            emitCPUOp(concat, 0xB8, reg);
            emitValue(concat, value, OpBits::B32);
        }
        else if (value <= 0x7FFFFFFF)
        {
            emitREX(concat, OpBits::B64, CPUReg::RAX, reg);
            concat.addU8(0xC7);
            emitCPUOp(concat, 0xC0, reg);
            emitValue(concat, value, OpBits::B32);
        }
        else
        {
            emitREX(concat, OpBits::B64, CPUReg::RAX, reg);
            emitCPUOp(concat, 0xB8, reg);
            concat.addU64(value);
        }
    }
    else if (opBits == OpBits::B8)
    {
        emitREX(concat, opBits);
        emitCPUOp(concat, 0xB0, reg);
        emitValue(concat, value, opBits);
    }
    else
    {
        emitREX(concat, opBits);
        emitCPUOp(concat, 0xB8, reg);
        emitValue(concat, value, opBits);
    }
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
        SWAG_ASSERT(memReg < CPUReg::R8);
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

void SCBE_X64::emitLoadSignedExtend(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
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

    if (numBitsSrc == OpBits::B8 && numBitsDst == OpBits::B16)
    {
        emitREX(concat, OpBits::B16, reg, memReg);
        concat.addU8(0x0F);
        concat.addU8(0xBE);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (numBitsSrc == OpBits::B8 && numBitsDst == OpBits::B32)
    {
        emitREX(concat, OpBits::B32, reg, memReg);
        concat.addU8(0x0F);
        concat.addU8(0xBE);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (numBitsSrc == OpBits::B8 && numBitsDst == OpBits::B64)
    {
        emitREX(concat, OpBits::B64, reg, memReg);
        concat.addU8(0x0F);
        concat.addU8(0xBE);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (numBitsSrc == OpBits::B16 && numBitsDst == OpBits::B32)
    {
        emitREX(concat, OpBits::B32, reg, memReg);
        concat.addU8(0x0F);
        concat.addU8(0xBF);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (numBitsSrc == OpBits::B16 && numBitsDst == OpBits::B64)
    {
        emitREX(concat, OpBits::B64, reg, memReg);
        concat.addU8(0x0F);
        concat.addU8(0xBF);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (numBitsSrc == OpBits::B32)
    {
        SWAG_ASSERT(numBitsDst == OpBits::B64);
        emitREX(concat, OpBits::B64, reg, memReg);
        emitCPUOp(concat, 0x63);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void SCBE_X64::emitLoadZeroExtend(CPUReg reg, CPUReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
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

    if (numBitsSrc == OpBits::B8 && numBitsDst == OpBits::B32)
    {
        emitREX(concat, OpBits::B32, reg, memReg);
        concat.addU8(0x0F);
        concat.addU8(0xB6);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (numBitsSrc == OpBits::B8 && numBitsDst == OpBits::B64)
    {
        emitREX(concat, OpBits::B64, reg, memReg);
        concat.addU8(0x0F);
        concat.addU8(0xB6);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (numBitsSrc == OpBits::B16 && numBitsDst == OpBits::B32)
    {
        emitREX(concat, OpBits::B32, reg, memReg);
        concat.addU8(0x0F);
        concat.addU8(0xB7);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (numBitsSrc == OpBits::B16 && numBitsDst == OpBits::B64)
    {
        emitREX(concat, OpBits::B64, reg, memReg);
        concat.addU8(0x0F);
        concat.addU8(0xB7);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (numBitsSrc == OpBits::B32)
    {
        SWAG_ASSERT(numBitsDst == OpBits::B64);
        emitLoad(reg, memReg, memOffset, numBitsSrc);
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void SCBE_X64::emitLoadZeroExtend(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    if (numBitsSrc == OpBits::B8 && (numBitsDst == OpBits::B32 || numBitsDst == OpBits::B64))
    {
        emitREX(concat, numBitsDst, regDst, regSrc);
        concat.addU8(0x0F);
        concat.addU8(0xB6);
        concat.addU8(getModRM(ModRMMode::Register, regDst, encodeReg(regSrc)));
    }
    else if (numBitsSrc == OpBits::B16 && numBitsDst == OpBits::B64)
    {
        emitREX(concat, OpBits::B64, regDst, regSrc);
        concat.addU8(0x0F);
        concat.addU8(0xB7);
        concat.addU8(getModRM(ModRMMode::Register, regDst, encodeReg(regSrc)));
    }
    else if (numBitsSrc == OpBits::B64 && numBitsDst == OpBits::F64)
    {
        SWAG_ASSERT(regSrc == CPUReg::RAX && regDst == CPUReg::XMM0);
        concat.addString5("\x66\x48\x0F\x6E\xC8"); // movq xmm1, rax
        emitSymbolRelocationAddress(CPUReg::RCX, symCst_U64F64, 0);
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

void SCBE_X64::emitLoadSignedExtend(CPUReg regDst, CPUReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    SWAG_ASSERT(regDst == CPUReg::RAX);
    SWAG_ASSERT(regSrc == CPUReg::RAX);
    if (numBitsDst == OpBits::B32 && numBitsSrc == OpBits::B8)
    {
        concat.addU8(0x0F);
        emitCPUOp(concat, 0xBE);
        concat.addU8(getModRM(ModRMMode::Register, regDst, encodeReg(regSrc)));
    }
    else
    {
        SWAG_ASSERT(false);
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
        emitCPUOp(concat, 0x8D);
        concat.addU8(0x0D);
    }
    else if (memOffset == 0)
    {
        emitLoad(reg, memReg, OpBits::B64);
    }
    else
    {
        emitREX(concat, OpBits::B64, reg, memReg);
        emitCPUOp(concat, 0x8D);
        emitModRM(concat, memOffset, reg, memReg);
    }
}

void SCBE_X64::emitLoadAddress(CPUReg regDst, CPUReg regSrc1, CPUReg regSrc2, uint64_t mulValue, OpBits opBits)
{
    SWAG_ASSERT(regDst == CPUReg::RAX);
    SWAG_ASSERT(regSrc1 == CPUReg::RAX);
    SWAG_ASSERT(regSrc2 == CPUReg::RAX);
    SWAG_ASSERT(opBits == OpBits::B32 || opBits == OpBits::B64);

    // lea regDst, [regSrc1 + regSrc2 * mulValue]
    concat.addU8(opBits == OpBits::B32 ? 0x67 : 0x48);
    concat.addU8(0x8D);
    concat.addU8(0x04);
    if (mulValue == 2)
        concat.addU8(0x40);
    else if (mulValue == 4)
        concat.addU8(0x80);
    else if (mulValue == 8)
        concat.addU8(0xC0);
    else
        SWAG_ASSERT(false);
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitStore(CPUReg memReg, uint64_t memOffset, CPUReg reg, OpBits opBits)
{
    if (isFloat(opBits))
    {
        SWAG_ASSERT(memReg < CPUReg::R8);
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
        emitModRM(concat, memOffset, MODRM_REG_0, memReg);
        emitValue(concat, value, std::min(opBits, OpBits::B32));
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
            if (reg >= CPUReg::R8 && reg <= CPUReg::R15)
                concat.addU8(0x41);
            concat.addU8(0x0F);
            concat.addU8(0x97);
            emitCPUOp(concat, 0xC0, reg);
            break;

        case CPUCondFlag::O:
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::R8);
            if (reg >= CPUReg::R8 && reg <= CPUReg::R15)
                concat.addU8(0x41);
            concat.addU8(0x0F);
            concat.addU8(0x90);
            emitCPUOp(concat, 0xC0, reg);
            break;

        case CPUCondFlag::AE:
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::R8);
            if (reg >= CPUReg::R8 && reg <= CPUReg::R15)
                concat.addU8(0x41);
            concat.addU8(0x0F);
            concat.addU8(0x93);
            emitCPUOp(concat, 0xC0, reg);
            break;

        case CPUCondFlag::G:
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::R8);
            if (reg >= CPUReg::R8 && reg <= CPUReg::R15)
                concat.addU8(0x41);
            concat.addU8(0x0F);
            concat.addU8(0x9F);
            emitCPUOp(concat, 0xC0, reg);
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
        concat.addU8(0x0F);
        concat.addU8(0x2F);
        concat.addU8(getModRM(ModRMMode::Register, reg0, encodeReg(reg1)));
    }
    else if (opBits == OpBits::F64)
    {
        concat.addU8(0x66);
        concat.addU8(0x0F);
        concat.addU8(0x2F);
        concat.addU8(getModRM(ModRMMode::Register, reg0, encodeReg(reg1)));
    }
    else
    {
        emitREX(concat, opBits, reg1, reg0);
        emitSpecCPUOp(concat, CPUOp::CMP, opBits);
        concat.addU8(getModRM(ModRMMode::Register, reg1, encodeReg(reg0)));
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
        emitCPUOp(concat, 0xF8, reg);
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
        SWAG_ASSERT(memReg < CPUReg::R8);
        concat.addU8(0x0F);
        concat.addU8(0x2E);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (opBits == OpBits::F64)
    {
        SWAG_ASSERT(memReg < CPUReg::R8);
        concat.addU8(0x66);
        concat.addU8(0x0F);
        concat.addU8(0x2F);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else
    {
        SWAG_ASSERT(memReg < CPUReg::R8);
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

void SCBE_X64::emitOpUnary(CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits)
{
    if (op == CPUOp::NOT)
    {
        SWAG_ASSERT(memReg == CPUReg::RSP);
        emitREX(concat, opBits);
        emitSpecCPUOp(concat, 0xF7, opBits);
        emitModRM(concat, memOffset, MODRM_REG_2, memReg);
    }
    else if (op == CPUOp::NEG)
    {
        SWAG_ASSERT(memReg == CPUReg::RSP);
        emitREX(concat, opBits);
        emitSpecCPUOp(concat, 0xF7, opBits);
        emitModRM(concat, memOffset, MODRM_REG_3, memReg);
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
        emitSpecCPUOp(concat, op, opBits);
        emitCPUOp(concat, 0xD0, reg);
    }
    else if (op == CPUOp::NEG)
    {
        if (isFloat(opBits))
        {
            SWAG_ASSERT(reg == CPUReg::XMM0);
            emitStore(CPUReg::RSP, cpuFct->getStackOffsetFLT(), opBits == OpBits::F32 ? 0x80000000 : 0x80000000'00000000, OpBits::B64);
            emitLoad(CPUReg::XMM1, CPUReg::RSP, cpuFct->getStackOffsetFLT(), opBits);
            emitOpBinary(CPUReg::XMM0, CPUReg::XMM1, CPUOp::FXOR, opBits);
        }
        else
        {
            SWAG_ASSERT(reg < CPUReg::R8);
            SWAG_ASSERT(opBits == OpBits::B32 || opBits == OpBits::B64);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0xF7);
            emitCPUOp(concat, 0xD8, reg);
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

void SCBE_X64::emitOpBinary(CPUReg regDst, CPUReg memReg, uint64_t memOffset, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    SWAG_ASSERT(regDst == CPUReg::RAX);
    if (op == CPUOp::ADD)
    {
        emitREX(concat, opBits, regDst, memReg);
        emitCPUOp(concat, 0x03);
        emitModRM(concat, memOffset, regDst, memReg);
    }
    else
    {
        emitLoad(CPUReg::RCX, memReg, memOffset, opBits);
        emitOpBinary(regDst, CPUReg::RCX, op, opBits);
    }
}

void SCBE_X64::emitOpBinary(CPUReg regDst, CPUReg regSrc, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    if (op == CPUOp::DIV || op == CPUOp::MOD || op == CPUOp::IDIV || op == CPUOp::IMOD)
    {
        SWAG_ASSERT(regDst == CPUReg::RAX);
        if (opBits == OpBits::B8)
        {
            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                emitLoadSignedExtend(regDst, regDst, OpBits::B32, OpBits::B8);
            else
                emitLoadZeroExtend(regDst, regDst, OpBits::B32, OpBits::B8);
        }
        else if (op == CPUOp::IDIV || op == CPUOp::IMOD)
        {
            emitREX(concat, opBits);
            emitCPUOp(concat, CPUOp::CDQ);
        }
        else
        {
            emitClear(CPUReg::RDX, opBits);
        }
    }

    if (opBits == OpBits::F32)
    {
        if (op != CPUOp::FSQRT &&
            op != CPUOp::FAND &&
            op != CPUOp::UCOMIF &&
            op != CPUOp::FXOR)
        {
            concat.addU8(0xF3);
            emitREX(concat, emitFlags.has(EMITF_B64) ? OpBits::B64 : OpBits::B32, regSrc, regDst);
        }

        concat.addU8(0x0F);
        emitCPUOp(concat, op);
        concat.addU8(static_cast<uint8_t>(0xC0 | encodeReg(regSrc) | encodeReg(regDst) << 3));
    }
    else if (opBits == OpBits::F64)
    {
        if (op != CPUOp::FSQRT &&
            op != CPUOp::FAND &&
            op != CPUOp::UCOMIF &&
            op != CPUOp::FXOR)
        {
            concat.addU8(0xF2);
            emitREX(concat, emitFlags.has(EMITF_B64) ? OpBits::B64 : OpBits::B32, regSrc, regDst);
        }
        else
        {
            concat.addU8(0x66);
        }

        concat.addU8(0x0F);
        emitCPUOp(concat, op);
        concat.addU8(static_cast<uint8_t>(0xC0 | encodeReg(regSrc) | encodeReg(regDst) << 3));
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
        emitSpecCPUOp(concat, op, opBits);
        concat.addU8(getModRM(ModRMMode::Register, regSrc, encodeReg(regDst)));
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
        emitSpecCPUOp(concat, op, opBits);
        emitCPUOp(concat, 0xD0, regSrc);
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
        concat.addU8(getModRM(ModRMMode::Register, regDst, encodeReg(regSrc)));
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
    if (isFloat(opBits))
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
        SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
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
        SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
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
    else if (op == CPUOp::SAR ||
             op == CPUOp::SHR ||
             op == CPUOp::SHL)
    {
        SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
        SWAG_ASSERT(reg == CPUReg::RCX);
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
        emitSpecCPUOp(concat, op, opBits);
        emitModRM(concat, memOffset, reg, memReg);
    }
}

namespace
{
    bool decomposeMul(uint32_t value, uint32_t& factor1, uint32_t& factor2)
    {
        // [3, 5, 9] * [3, 5, 9]
        for (uint32_t i = 3; i <= value; i += 2)
        {
            if ((i == 3 || i == 5 || i == 9) && value % i == 0)
            {
                const uint32_t otherFactor = value / i;
                if ((otherFactor == 3 || otherFactor == 5 || otherFactor == 9))
                {
                    factor1 = i;
                    factor2 = otherFactor;
                    if (factor1 * factor2 == value)
                        return true;
                }
            }
        }

        // powerOf2 * [3, 5, 9]
        const uint32_t pow2        = 1 << std::countr_zero(value);
        const uint32_t otherFactor = value / pow2;
        if ((otherFactor == 3) || (otherFactor == 5) || (otherFactor == 9))
        {
            factor1 = pow2;
            factor2 = otherFactor;
            if (factor1 * factor2 == value)
                return true;
        }

        return false;
    }
}

void SCBE_X64::emitOpBinary(CPUReg reg, uint64_t value, CPUOp op, OpBits opBits, CPUEmitFlags emitFlags)
{
    if (isNoOp(value, op, opBits, emitFlags))
        return;

    ///////////////////////////////////////////

    if (op == CPUOp::DIV || op == CPUOp::MOD || op == CPUOp::IDIV || op == CPUOp::IMOD)
    {
        SWAG_ASSERT(reg == CPUReg::RAX);
        if (opBits == OpBits::B8)
        {
            if (op == CPUOp::IDIV || op == CPUOp::IMOD)
                emitLoadSignedExtend(CPUReg::RAX, CPUReg::RAX, OpBits::B32, OpBits::B8);
            else
                emitLoadZeroExtend(CPUReg::RAX, CPUReg::RAX, OpBits::B32, OpBits::B8);
        }
        else if (op == CPUOp::IDIV || op == CPUOp::IMOD)
        {
            emitREX(concat, opBits);
            emitCPUOp(concat, CPUOp::CDQ);
        }
        else
        {
            emitClear(CPUReg::RDX, opBits);
        }
    }

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
        if (value == 1 && !emitFlags.has(EMITF_Overflow) && optLevel >= BuildCfgBackendOptim::O1)
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RSP);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xFF, opBits);
            concat.addU8(0xC0 | encodeReg(reg)); // inc rax
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
                concat.addU8(0xC0 | encodeReg(reg));
            }
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RSP);
            emitREX(concat, opBits);
            concat.addU8(0x83);
            concat.addU8(0xC0 | encodeReg(reg));
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
                concat.addU8(0xC0 | encodeReg(reg));
            }
            emitValue(concat, value, opBits == OpBits::B16 ? OpBits::B16 : OpBits::B32);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::SUB)
    {
        if (value == 1 && !emitFlags.has(EMITF_Overflow) && optLevel >= BuildCfgBackendOptim::O1)
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RSP);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xFF, opBits);
            concat.addU8(0xC8 | encodeReg(reg)); // dec rax
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
                concat.addU8(0xE8 | encodeReg(reg));
            }
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::RSP);
            emitREX(concat, opBits);
            concat.addU8(0x83);
            concat.addU8(0xE8 | encodeReg(reg));
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
                concat.addU8(0xE8 | encodeReg(reg));
            }
            emitValue(concat, value, opBits == OpBits::B16 ? OpBits::B16 : OpBits::B32);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::MOD || op == CPUOp::IMOD)
    {
        if (Math::isPowerOfTwo(value) && optLevel >= BuildCfgBackendOptim::O1)
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
        if (Math::isPowerOfTwo(value) && optLevel >= BuildCfgBackendOptim::O1)
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
    else if (op == CPUOp::IDIV)
    {
        SWAG_ASSERT(reg == CPUReg::RAX);
        emitLoad(CPUReg::RCX, value, opBits);
        emitOpBinary(reg, CPUReg::RCX, op, opBits, emitFlags);
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::MUL || op == CPUOp::IMUL)
    {
        uint32_t   factor1, factor2;
        const bool canFactorize = (opBits == OpBits::B32 || opBits == OpBits::B64) && optLevel >= BuildCfgBackendOptim::O1 && !emitFlags.has(EMITF_Overflow);
        if (value == 0 && optLevel >= BuildCfgBackendOptim::O1)
        {
            emitClear(reg, opBits);
        }
        else if (value == 3 && canFactorize)
        {
            emitLoadAddress(reg, reg, reg, 2, opBits);
        }
        else if (value == 5 && canFactorize)
        {
            emitLoadAddress(reg, reg, reg, 4, opBits);
        }
        else if (value == 9 && canFactorize)
        {
            emitLoadAddress(reg, reg, reg, 8, opBits);
        }
        else if (Math::isPowerOfTwo(value) && optLevel >= BuildCfgBackendOptim::O1)
        {
            emitOpBinary(reg, static_cast<uint32_t>(log2(value)), CPUOp::SHL, opBits, emitFlags);
        }
        else if (canFactorize && decomposeMul(static_cast<uint32_t>(value), factor1, factor2))
        {
            if (factor1 != 1)
                emitOpBinary(reg, factor1, CPUOp::MUL, opBits, emitFlags);
            if (factor2 != 1)
                emitOpBinary(reg, factor2, CPUOp::MUL, opBits, emitFlags);
        }
        else if (op == CPUOp::IMUL && opBits == OpBits::B8)
        {
            SWAG_ASSERT(reg == CPUReg::RAX);
            emitLoad(CPUReg::RCX, value, opBits);
            emitOpBinary(reg, CPUReg::RCX, op, opBits, emitFlags);
        }
        else if (op == CPUOp::IMUL && value <= 0x7F)
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX);
            emitREX(concat, opBits);
            concat.addU8(0x6B);
            concat.addU8(reg == CPUReg::RAX ? 0xC0 : 0xC9);
            emitValue(concat, value, OpBits::B8);
        }
        else if (op == CPUOp::IMUL)
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX);
            emitREX(concat, opBits);
            concat.addU8(0x69);
            concat.addU8(reg == CPUReg::RAX ? 0xC0 : 0xC9);
            emitValue(concat, value, OpBits::B32);
        }
        else
        {
            SWAG_ASSERT(reg == CPUReg::RAX);
            emitLoad(CPUReg::RCX, value, opBits);
            emitOpBinary(reg, CPUReg::RCX, op, opBits, emitFlags);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::SAR || op == CPUOp::SHR || op == CPUOp::SHL)
    {
        if (op == CPUOp::SHL && value == 1 && optLevel >= BuildCfgBackendOptim::O1)
        {
            emitOpBinary(reg, reg, CPUOp::ADD, opBits);
        }
        else if (value == 1)
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::R9);
            emitREX(concat, opBits, reg, reg);
            emitSpecB8(concat, 0xD1, opBits);
            emitCPUOp(concat, op, reg);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(reg == CPUReg::RAX || reg == CPUReg::RCX || reg == CPUReg::R9);
            emitREX(concat, opBits, reg, reg);
            value = std::min(static_cast<uint32_t>(value), getNumBits(opBits) - 1);
            emitSpecB8(concat, 0xC1, opBits);
            emitCPUOp(concat, op, reg);
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
        SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
        emitLoad(CPUReg::RCX, value, opBits);
        emitOpBinary(memReg, memOffset, CPUReg::RCX, op, opBits, emitFlags);
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::IMOD || op == CPUOp::MOD)
    {
        if (Math::isPowerOfTwo(value) && optLevel >= BuildCfgBackendOptim::O1)
        {
            emitOpBinary(memReg, memOffset, value - 1, CPUOp::AND, opBits, emitFlags);
        }
        else
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
            if (memReg == CPUReg::RAX)
            {
                emitLoad(CPUReg::R8, memReg, OpBits::B64);
                memReg = CPUReg::R8;
            }
            emitLoad(CPUReg::RAX, memReg, memOffset, opBits);
            emitOpBinary(CPUReg::RAX, value, op, opBits, emitFlags);
            emitStore(memReg, memOffset, CPUReg::RAX, opBits);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::DIV)
    {
        if (Math::isPowerOfTwo(value) && optLevel >= BuildCfgBackendOptim::O1)
        {
            emitOpBinary(memReg, memOffset, static_cast<uint32_t>(log2(value)), op == CPUOp::IDIV ? CPUOp::SAR : CPUOp::SHR, opBits, emitFlags);
        }
        else
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
            if (memReg == CPUReg::RAX)
            {
                emitLoad(CPUReg::R8, memReg, OpBits::B64);
                memReg = CPUReg::R8;
            }
            emitLoad(CPUReg::RAX, memReg, memOffset, opBits);
            emitOpBinary(CPUReg::RAX, value, op, opBits, emitFlags);
            emitStore(memReg, memOffset, CPUReg::RAX, opBits);
        }
    }

    else if (op == CPUOp::IDIV)
    {
        SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
        if (memReg == CPUReg::RAX)
        {
            emitLoad(CPUReg::R8, memReg, OpBits::B64);
            memReg = CPUReg::R8;
        }
        emitLoad(CPUReg::RAX, memReg, memOffset, opBits);
        emitOpBinary(CPUReg::RAX, value, op, opBits, emitFlags);
        emitStore(memReg, memOffset, CPUReg::RAX, opBits);
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::IMUL || op == CPUOp::MUL)
    {
        SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
        if (Math::isPowerOfTwo(value) && optLevel >= BuildCfgBackendOptim::O1)
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
            emitLoad(CPUReg::RAX, memReg, memOffset, opBits);
            emitOpBinary(CPUReg::RAX, value, op, opBits, emitFlags);
            emitStore(memReg, memOffset, CPUReg::RAX, opBits);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::SAR || op == CPUOp::SHR || op == CPUOp::SHL)
    {
        if (value == 1)
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xD1, opBits);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, 1 + static_cast<uint8_t>(op) & 0x3F);
        }
        else
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
            value = std::min(static_cast<uint32_t>(value), getNumBits(opBits) - 1);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xC1, opBits);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, 1 + static_cast<uint8_t>(op) & 0x3F);
            emitValue(concat, value, OpBits::B8);
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::ADD)
    {
        if (value == 1 && !emitFlags.has(EMITF_Overflow) && optLevel >= BuildCfgBackendOptim::O1)
        {
            SWAG_ASSERT(memReg < CPUReg::R8);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xFF, opBits);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg);
        }
        else if (opBits == OpBits::B8)
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::SUB)
    {
        if (value == 1 && !emitFlags.has(EMITF_Overflow) && optLevel >= BuildCfgBackendOptim::O1)
        {
            SWAG_ASSERT(memReg < CPUReg::R8);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xFF, opBits);
            emitModRM(concat, memOffset, MODRM_REG_1, memReg);
        }
        else if (opBits == OpBits::B8)
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
    }

    ///////////////////////////////////////////

    else if (op == CPUOp::OR || op == CPUOp::AND || op == CPUOp::XOR)
    {
        if (opBits == OpBits::B8)
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RSP);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
    }

    ///////////////////////////////////////////

    else
    {
        SWAG_ASSERT(false);
    }
}

/////////////////////////////////////////////////////////////////////
void SCBE_X64::emitJumpTable(CPUReg table, CPUReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries)
{
    SWAG_ASSERT(table == CPUReg::RCX && offset == CPUReg::RAX);

    uint8_t*   addrConstant        = nullptr;
    const auto offsetTableConstant = buildParams.module->constantSegment.reserve(numEntries * sizeof(uint32_t), &addrConstant);
    emitSymbolRelocationAddress(table, symCSIndex, offsetTableConstant); // rcx = jump table

    emitREX(concat, OpBits::B64);

    // movsxd rcx, dword ptr [rcx + rax*4]
    emitCPUOp(concat, 0x63);
    emitModRM(concat, ModRMMode::Memory, CPUReg::RCX, MODRM_RM_SID);
    concat.addU8(getSid(2, CPUReg::RAX, CPUReg::RCX));

    // + 7 for this instruction
    // + 5 for the two following instructions
    SWAG_IF_ASSERT(const auto startIdx = concat.totalCount());
    emitSymbolRelocationAddress(CPUReg::RAX, cpuFct->symbolIndex, concat.totalCount() - cpuFct->startAddress + 5 + 7);
    emitOpBinary(CPUReg::RAX, CPUReg::RCX, CPUOp::ADD, OpBits::B64);
    emitJump(CPUReg::RAX);
    SWAG_IF_ASSERT(const auto endIdx = concat.totalCount());
    SWAG_ASSERT(endIdx - startIdx == 12);

    const auto tableCompiler = reinterpret_cast<int32_t*>(buildParams.module->compilerSegment.address(offsetTable));
    const auto currentOffset = static_cast<int32_t>(concat.totalCount());

    CPULabelToSolve label;
    for (uint32_t idx = 0; idx < numEntries; idx++)
    {
        label.ipDest      = tableCompiler[idx] + currentIp + 1;
        label.jump.opBits = OpBits::B32;
        label.jump.offset = currentOffset;
        label.jump.addr   = addrConstant + idx * sizeof(uint32_t);
        cpuFct->labelsToSolve.push_back(label);
    }
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
        case JP:
            concat.addU8(0x0F);
            concat.addU8(0x8A);
            break;
        case JNP:
            concat.addU8(0x0F);
            concat.addU8(0x8B);
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

    concat.addU32(0);
    CPUJump jump;
    jump.addr   = concat.getSeekPtr() - sizeof(uint32_t);
    jump.offset = concat.totalCount();
    jump.opBits = opBits;
    return jump;
}

void SCBE_X64::emitPatchJump(const CPUJump& jump, uint64_t offsetDestination)
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

void SCBE_X64::emitPatchJump(const CPUJump& jump)
{
    const int32_t offset = static_cast<int32_t>(concat.totalCount() - jump.offset);
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

void SCBE_X64::emitJump(CPUReg reg)
{
    SWAG_ASSERT(reg == CPUReg::RAX);
    emitREX(concat, OpBits::Zero, REX_REG_NONE, reg);
    emitCPUOp(concat, 0xFF);
    emitModRM(concat, ModRMMode::Register, MODRM_REG_4, encodeReg(reg));
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitCopy(CPUReg regDst, CPUReg regSrc, uint32_t count)
{
    if (!count)
        return;

    SWAG_ASSERT(regDst == CPUReg::RCX);
    SWAG_ASSERT(regSrc == CPUReg::RDX);
    uint32_t offset = 0;

    // SSE 16 octets
    if (count >= 16)
    {
        while (count >= 16)
        {
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x10); // movups xmm0, [rdx+??]

            if (offset <= 0x7F)
            {
                concat.addU8(0x40 | encodeReg(regSrc));
                emitValue(concat, offset, OpBits::B8);
            }
            else
            {
                concat.addU8(0x80 | encodeReg(regSrc));
                emitValue(concat, offset, OpBits::B32);
            }

            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x11); // movups [rcx+??], xmm0

            if (offset <= 0x7F)
            {
                concat.addU8(0x40 | encodeReg(regDst));
                emitValue(concat, offset, OpBits::B8);
            }
            else
            {
                concat.addU8(0x80 | encodeReg(regDst));
                emitValue(concat, offset, OpBits::B32);
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
    SWAG_ASSERT(memReg == CPUReg::RAX || memReg == CPUReg::RCX || memReg == CPUReg::RSP);
    SWAG_ASSERT(memOffset <= 0x7FFFFFFF);

    // SSE 16 octets
    if (count >= 16)
    {
        // xorps xmm0, xmm0
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0x57);
        concat.addU8(0xC0);

        while (count >= 16)
        {
            // movups [memReg+??], xmm0
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x11);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg);
            count -= 16;
            memOffset += 16;
        }
    }

    while (count >= 8)
    {
        emitStore(memReg, memOffset, 0, OpBits::B64);
        count -= 8;
        memOffset += 8;
    }

    while (count >= 4)
    {
        emitStore(memReg, memOffset, 0, OpBits::B32);
        count -= 4;
        memOffset += 4;
    }

    while (count >= 2)
    {
        emitStore(memReg, memOffset, 0, OpBits::B16);
        count -= 2;
        memOffset += 2;
    }

    while (count >= 1)
    {
        emitStore(memReg, memOffset, 0, OpBits::B8);
        count -= 1;
        memOffset += 1;
    }
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitCallExtern(const Utf8& symbolName)
{
    emitCPUOp(concat, 0xFF);
    emitModRM(concat, ModRMMode::Memory, MODRM_REG_2, MODRM_RM_RIP);

    const auto callSym = getOrAddSymbol(symbolName, CPUSymbolKind::Extern);
    addSymbolRelocation(concat.totalCount() - textSectionOffset, callSym->index, IMAGE_REL_AMD64_REL32);
    concat.addU32(0);
}

void SCBE_X64::emitCallLocal(const Utf8& symbolName)
{
    emitCPUOp(concat, 0xE8);

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
    emitREX(concat, OpBits::Zero, REX_REG_NONE, reg);
    emitCPUOp(concat, 0xFF);
    emitModRM(concat, ModRMMode::Register, MODRM_REG_2, encodeReg(reg));
}

/////////////////////////////////////////////////////////////////////

void SCBE_X64::emitNop()
{
    emitCPUOp(concat, 0x90);
}

// a*b+c
void SCBE_X64::emitMulAdd(CPUReg regDst, CPUReg regMul, CPUReg regAdd, OpBits opBits)
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

uint64_t SCBE_X64::getInstructionInfo(SCBE_MicroOp* inst)
{
    return 0;
}
