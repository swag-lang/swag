// ReSharper disable CommentTypo
#include "pch.h"
#include "Backend/SCBE/Encoder/X64/ScbeX64.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Report/Report.h"
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

constexpr auto REX_REG_NONE = static_cast<CpuReg>(255);
constexpr auto MODRM_REG_0  = static_cast<CpuReg>(254);
constexpr auto MODRM_REG_1  = static_cast<CpuReg>(253);
constexpr auto MODRM_REG_2  = static_cast<CpuReg>(252);
constexpr auto MODRM_REG_3  = static_cast<CpuReg>(251);
constexpr auto MODRM_REG_4  = static_cast<CpuReg>(250);
constexpr auto MODRM_REG_5  = static_cast<CpuReg>(249);
constexpr auto MODRM_REG_6  = static_cast<CpuReg>(248);
constexpr auto MODRM_REG_7  = static_cast<CpuReg>(247);

constexpr uint8_t MODRM_RM_SIB = 0b100;
constexpr uint8_t MODRM_RM_RIP = 0b101;

enum class X64Reg : uint8_t
{
    Rax  = 0b000000,
    Rbx  = 0b000011,
    Rcx  = 0b000001,
    Rdx  = 0b000010,
    Rsp  = 0b000100,
    Rbp  = 0b000101,
    Rsi  = 0b000110,
    Rdi  = 0b000111,
    R8   = 0b001000,
    R9   = 0b001001,
    R10  = 0b001010,
    R11  = 0b001011,
    R12  = 0b001100,
    R13  = 0b001101,
    R14  = 0b001110,
    R15  = 0b001111,
    Xmm0 = 0b100000,
    Xmm1 = 0b100001,
    Xmm2 = 0b100010,
    Xmm3 = 0b100011,
    Rip  = 0b110000
};

namespace
{
    CpuReg x64Reg2CpuReg(X64Reg reg)
    {
        switch (reg)
        {
            case X64Reg::Rax:
                return CpuReg::Rax;
            case X64Reg::Rbx:
                return CpuReg::Rbx;
            case X64Reg::Rcx:
                return CpuReg::Rcx;
            case X64Reg::Rdx:
                return CpuReg::Rdx;
            case X64Reg::Rsp:
                return CpuReg::Rsp;
            case X64Reg::Rbp:
                return CpuReg::Rbp;
            case X64Reg::Rsi:
                return CpuReg::Rsi;
            case X64Reg::Rdi:
                return CpuReg::Rdi;
            case X64Reg::R8:
                return CpuReg::R8;
            case X64Reg::R9:
                return CpuReg::R9;
            case X64Reg::R10:
                return CpuReg::R10;
            case X64Reg::R11:
                return CpuReg::R11;
            case X64Reg::R12:
                return CpuReg::R12;
            case X64Reg::R13:
                return CpuReg::R13;
            case X64Reg::R14:
                return CpuReg::R14;
            case X64Reg::R15:
                return CpuReg::R15;
            case X64Reg::Xmm0:
                return CpuReg::Xmm0;
            case X64Reg::Xmm1:
                return CpuReg::Xmm1;
            case X64Reg::Xmm2:
                return CpuReg::Xmm2;
            case X64Reg::Xmm3:
                return CpuReg::Xmm3;
            case X64Reg::Rip:
                return CpuReg::Rip;

            default:
                SWAG_ASSERT(false);
                return CpuReg::Rax;
        }
    }

    X64Reg cpuRegToX64Reg(CpuReg reg)
    {
        switch (reg)
        {
            case CpuReg::Rax:
                return X64Reg::Rax;
            case CpuReg::Rbx:
                return X64Reg::Rbx;
            case CpuReg::Rcx:
                return X64Reg::Rcx;
            case CpuReg::Rdx:
                return X64Reg::Rdx;
            case CpuReg::Rsp:
                return X64Reg::Rsp;
            case CpuReg::Rbp:
                return X64Reg::Rbp;
            case CpuReg::Rsi:
                return X64Reg::Rsi;
            case CpuReg::Rdi:
                return X64Reg::Rdi;
            case CpuReg::R8:
                return X64Reg::R8;
            case CpuReg::R9:
                return X64Reg::R9;
            case CpuReg::R10:
                return X64Reg::R10;
            case CpuReg::R11:
                return X64Reg::R11;
            case CpuReg::R12:
                return X64Reg::R12;
            case CpuReg::R13:
                return X64Reg::R13;
            case CpuReg::R14:
                return X64Reg::R14;
            case CpuReg::R15:
                return X64Reg::R15;
            case CpuReg::Xmm0:
                return X64Reg::Xmm0;
            case CpuReg::Xmm1:
                return X64Reg::Xmm1;
            case CpuReg::Xmm2:
                return X64Reg::Xmm2;
            case CpuReg::Xmm3:
                return X64Reg::Xmm3;
            case CpuReg::Rip:
                return X64Reg::Rip;
            default:
                SWAG_ASSERT(false);
                return X64Reg::Rax;
        }
    }

    uint8_t encodeReg(CpuReg reg)
    {
        switch (reg)
        {
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
            case MODRM_REG_5:
                return 5;
            case MODRM_REG_6:
                return 6;
            case MODRM_REG_7:
                return 7;
            default:
                return static_cast<uint8_t>(cpuRegToX64Reg(reg)) & 0b11111;
        }
    }

    bool canEncode8(uint64_t value, OpBits opBits)
    {
        return value <= 0x7F ||
               (opBits == OpBits::B16 && value >= 0xFF80) ||
               (opBits == OpBits::B32 && value >= 0xFFFFFF80) ||
               (opBits == OpBits::B64 && value >= 0xFFFFFFFFFFFFFF80);
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
    uint8_t getModRM(ModRMMode mod, uint8_t reg, uint8_t rm)
    {
        const auto result = static_cast<uint32_t>(mod) << 6 | ((reg & 0b111) << 3) | (rm & 0b111);
        return static_cast<uint8_t>(result);
    }

    uint8_t getModRM(ModRMMode mod, CpuReg reg, uint8_t rm)
    {
        return getModRM(mod, encodeReg(reg), rm);
    }

    void emitPrefixF64(Concat& concat, OpBits opBits)
    {
        if (opBits == OpBits::B64)
            concat.addU8(0x66);
    }

    void emitSIB(Concat& concat, uint8_t scale, uint8_t index, uint8_t base)
    {
        const uint8_t value = (scale << 6) | (index << 3) | base;
        concat.addU8(value);
    }

    // Scaled index addressing
    void emitSIB(Concat& concat, uint8_t scale, CpuReg regIndex, CpuReg regBase)
    {
        emitSIB(concat, scale, encodeReg(regIndex) & 0b111, encodeReg(regBase) & 0b111);
    }

    void emitREX(Concat& concat, OpBits opBits, CpuReg reg0 = REX_REG_NONE, CpuReg reg1 = REX_REG_NONE)
    {
        if (opBits == OpBits::B16)
            concat.addU8(0x66);

        const bool b1 = (reg0 >= CpuReg::R8 && reg0 <= CpuReg::R15);
        const bool b2 = (reg1 >= CpuReg::R8 && reg1 <= CpuReg::R15);
        if (opBits == OpBits::B64 ||
            b1 || b2 ||
            reg0 == CpuReg::Rsi || reg1 == CpuReg::Rsi ||
            reg0 == CpuReg::Rdi || reg1 == CpuReg::Rdi)
        {
            concat.addU8(getREX(opBits == OpBits::B64, b1, false, b2));
        }
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

    void emitModRM(Concat& concat, ModRMMode mod, CpuReg reg, uint8_t rm)
    {
        const auto value = getModRM(mod, reg, rm);
        concat.addU8(value);
    }

    void emitModRM(Concat& concat, CpuReg reg, CpuReg memReg)
    {
        emitModRM(concat, ModRMMode::Register, reg, encodeReg(memReg));
    }

    void emitModRM(Concat& concat, uint64_t memOffset, CpuReg reg, CpuReg memReg)
    {
        if (memOffset == 0 && memReg != CpuReg::R13)
        {
            if (memReg == CpuReg::Rsp || memReg == CpuReg::R12)
            {
                const auto modRM = getModRM(ModRMMode::Memory, reg, MODRM_RM_SIB);
                concat.addU8(modRM);
                emitSIB(concat, 0, MODRM_RM_SIB, encodeReg(memReg) & 0b111);
            }
            else
            {
                const auto modRM = getModRM(ModRMMode::Memory, reg, encodeReg(memReg));
                concat.addU8(modRM);
            }
        }
        else if (memOffset <= 0x7F)
        {
            if (memReg == CpuReg::Rsp || memReg == CpuReg::R12)
            {
                const auto modRM = getModRM(ModRMMode::Displacement8, reg, MODRM_RM_SIB);
                concat.addU8(modRM);
                emitSIB(concat, 0, MODRM_RM_SIB, encodeReg(memReg) & 0b111);
            }
            else
            {
                const auto modRM = getModRM(ModRMMode::Displacement8, reg, encodeReg(memReg));
                concat.addU8(modRM);
            }

            emitValue(concat, memOffset, OpBits::B8);
        }
        else
        {
            if (memReg == CpuReg::Rsp || memReg == CpuReg::R12)
            {
                const auto modRM = getModRM(ModRMMode::Displacement32, reg, MODRM_RM_SIB);
                concat.addU8(modRM);
                emitSIB(concat, 0, MODRM_RM_SIB, encodeReg(memReg) & 0b111);
            }
            else
            {
                const auto modRM = getModRM(ModRMMode::Displacement32, reg, encodeReg(memReg));
                concat.addU8(modRM);
            }

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
        if (opBits == OpBits::B64)
            concat.addU8(value & ~1);
        else if (opBits == OpBits::B32)
            concat.addU8(value);
    }

    void emitCPUOp(Concat& concat, CpuOp op)
    {
        concat.addU8(static_cast<uint8_t>(op));
    }

    void emitCPUOp(Concat& concat, uint8_t op)
    {
        concat.addU8(op);
    }

    void emitCPUOp(Concat& concat, uint8_t op, CpuReg reg)
    {
        concat.addU8(op | (encodeReg(reg) & 0b111));
    }

    void emitSpecCPUOp(Concat& concat, CpuOp op, OpBits opBits)
    {
        emitSpecB8(concat, static_cast<uint8_t>(op), opBits);
    }

    void emitSpecCPUOp(Concat& concat, uint8_t op, OpBits opBits)
    {
        emitSpecB8(concat, op, opBits);
    }

    void emitAddMul(Concat& concat, uint8_t op, CpuReg regDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, OpBits opBits, CpuEmitFlags emitFlags)
    {
        SWAG_ASSERT(opBits == OpBits::B32 || opBits == OpBits::B64);

        // lea regDst, [regSrc1 + regSrc2 * mulValue]
        const bool b0 = (regDst >= CpuReg::R8 && regDst <= CpuReg::R15);
        const bool b1 = (regSrc2 >= CpuReg::R8 && regSrc2 <= CpuReg::R15);
        const bool b2 = (regSrc1 >= CpuReg::R8 && regSrc1 <= CpuReg::R15);
        if (opBits == OpBits::B32)
            emitCPUOp(concat, 0x67);
        if (opBits == OpBits::B64 || b0 || b1 || b2)
            concat.addU8(getREX(opBits == OpBits::B64, b0, b1, b2));

        emitCPUOp(concat, op);

        if (regSrc1 == CpuReg::R13)
        {
            const auto modRM = getModRM(ModRMMode::Displacement8, regDst, MODRM_RM_SIB);
            concat.addU8(modRM);
        }
        else
        {
            emitModRM(concat, ModRMMode::Memory, regDst, MODRM_RM_SIB);
        }

        SWAG_ASSERT(mulValue == 1 || mulValue == 2 || mulValue == 4 || mulValue == 8);
        emitSIB(concat, static_cast<uint8_t>(log2(mulValue)), encodeReg(regSrc2) & 0b111, encodeReg(regSrc1) & 0b111);

        if (regSrc1 == CpuReg::R13)
            emitValue(concat, 0, OpBits::B8);
    }
}

/////////////////////////////////////////////////////////////////////

CpuEncodeResult ScbeX64::encodeSymbolRelocationRef(const Utf8& name, CpuEmitFlags emitFlags)
{
    const auto callSym = getOrAddSymbol(name, CpuSymbolKind::Extern);
    if (callSym->kind == CpuSymbolKind::Function)
    {
        concat.addS32(static_cast<int32_t>(callSym->value + textSectionOffset - (concat.totalCount() + 4)));
    }
    else
    {
        addSymbolRelocation(concat.totalCount() - textSectionOffset, callSym->index, IMAGE_REL_AMD64_REL32);
        concat.addU32(0);
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeSymbolRelocationAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags)
{
    emitREX(concat, OpBits::B64, reg);
    emitCPUOp(concat, 0x8D); // LEA
    emitModRM(concat, ModRMMode::Memory, reg, MODRM_RM_RIP);
    addSymbolRelocation(concat.totalCount() - textSectionOffset, symbolIndex, IMAGE_REL_AMD64_REL32);
    concat.addU32(offset);
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeSymbolRelocationValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset, CpuEmitFlags emitFlags)
{
    emitREX(concat, OpBits::B64, reg);
    emitCPUOp(concat, 0x8B); // MOV
    emitModRM(concat, ModRMMode::Memory, reg, MODRM_RM_RIP);
    addSymbolRelocation(concat.totalCount() - textSectionOffset, symbolIndex, IMAGE_REL_AMD64_REL32);
    concat.addU32(offset);
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeSymbolGlobalString(CpuReg reg, const Utf8& str, CpuEmitFlags emitFlags)
{
    emitLoadRegImm64(reg, 0);
    const auto sym = getOrCreateGlobalString(str);
    addSymbolRelocation(concat.totalCount() - 8 - textSectionOffset, sym->index, IMAGE_REL_AMD64_ADDR64);
    return CpuEncodeResult::Zero;
}

/////////////////////////////////////////////////////////////////////

CpuEncodeResult ScbeX64::encodePush(CpuReg reg, CpuEmitFlags emitFlags)
{
    emitREX(concat, OpBits::Zero, REX_REG_NONE, reg);
    emitCPUOp(concat, 0x50, reg);
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodePop(CpuReg reg, CpuEmitFlags emitFlags)
{
    emitREX(concat, OpBits::Zero, REX_REG_NONE, reg);
    emitCPUOp(concat, 0x58, reg);
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeRet(CpuEmitFlags emitFlags)
{
    emitCPUOp(concat, 0xC3);
    return CpuEncodeResult::Zero;
}

/////////////////////////////////////////////////////////////////////

CpuEncodeResult ScbeX64::encodeLoadRegReg(CpuReg regDst, CpuReg regSrc, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (isFloat(regDst) && isFloat(regSrc))
    {
        emitSpecF64(concat, 0xF3, opBits);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0x10);
        emitModRM(concat, regDst, regSrc);
    }
    else if (isFloat(regDst))
    {
        emitPrefixF64(concat, OpBits::B64);
        emitREX(concat, opBits, regDst, regSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0x6E);
        emitModRM(concat, regDst, regSrc);
    }
    else if (isFloat(regSrc))
    {
        emitPrefixF64(concat, OpBits::B64);
        emitREX(concat, opBits, regSrc, regDst);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0x7E);
        emitModRM(concat, regSrc, regDst);
    }
    else
    {
        emitREX(concat, opBits, regSrc, regDst);
        emitSpecCPUOp(concat, 0x89, opBits);
        emitModRM(concat, regSrc, regDst);
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeLoadRegImm64(CpuReg reg, uint64_t value, CpuEmitFlags emitFlags)
{
    emitREX(concat, OpBits::B64, REX_REG_NONE, reg);
    emitCPUOp(concat, 0xB8, reg);
    concat.addU64(value);
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeLoadRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (isFloat(reg))
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Right2Reg;
        Report::internalError(module, "emitLoadRegImm, cannot encode");
    }

    if (emitFlags.has(EMIT_CanEncode))
        return CpuEncodeResult::Zero;

    if (opBits == OpBits::B8)
    {
        emitREX(concat, opBits, REX_REG_NONE, reg);
        emitCPUOp(concat, 0xB0, reg);
        emitValue(concat, value, opBits);
    }
    else
    {
        emitREX(concat, opBits, REX_REG_NONE, reg);
        emitCPUOp(concat, 0xB8, reg);
        emitValue(concat, value, opBits);
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeLoadRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (isFloat(reg))
    {
        emitSpecF64(concat, 0xF3, opBits);
        emitREX(concat, OpBits::Zero, reg, memReg);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0x10);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else
    {
        emitREX(concat, opBits, reg, memReg);
        emitSpecCPUOp(concat, 0x8B, opBits);
        emitModRM(concat, memOffset, reg, memReg);
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeLoadZeroExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    if (numBitsSrc == OpBits::B8 && (numBitsDst == OpBits::B32 || numBitsDst == OpBits::B64))
    {
        emitREX(concat, numBitsDst, reg, memReg);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0xB6);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (numBitsSrc == OpBits::B16 && (numBitsDst == OpBits::B32 || numBitsDst == OpBits::B64))
    {
        emitREX(concat, numBitsDst, reg, memReg);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0xB7);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (numBitsSrc == OpBits::B32 && numBitsDst == OpBits::B64)
    {
        emitLoadRegMem(reg, memReg, memOffset, numBitsSrc);
    }
    else
    {
        Report::internalError(module, "encodeLoadZeroExtendRegMem, cannot encode");
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeLoadZeroExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    if (numBitsSrc == OpBits::B8 && (numBitsDst == OpBits::B32 || numBitsDst == OpBits::B64))
    {
        emitREX(concat, numBitsDst, regDst, regSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0xB6);
        emitModRM(concat, regDst, regSrc);
    }
    else if (numBitsSrc == OpBits::B16 && (numBitsDst == OpBits::B32 || numBitsDst == OpBits::B64))
    {
        emitREX(concat, OpBits::B64, regDst, regSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0xB7);
        emitModRM(concat, regDst, regSrc);
    }
    else if (numBitsSrc == OpBits::B32 && numBitsDst == OpBits::B64)
    {
        emitLoadRegReg(regDst, regSrc, numBitsSrc);
    }
    else
    {
        Report::internalError(module, "encodeLoadZeroExtendRegReg, cannot encode");
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeLoadSignedExtendRegMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    if (numBitsSrc == OpBits::B8)
    {
        emitREX(concat, numBitsDst, reg, memReg);
        concat.addU8(0x0F);
        concat.addU8(0xBE);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else if (numBitsSrc == OpBits::B16)
    {
        emitREX(concat, numBitsDst, reg, memReg);
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
        Report::internalError(module, "encodeLoadSignedExtendRegMem, cannot encode");
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeLoadSignedExtendRegReg(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc, CpuEmitFlags emitFlags)
{
    if (numBitsSrc == OpBits::B8)
    {
        emitREX(concat, numBitsDst, regDst, regSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0xBE);
        emitModRM(concat, regDst, regSrc);
    }
    else if (numBitsSrc == OpBits::B16)
    {
        emitREX(concat, numBitsDst, regDst, regSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0xBF);
        emitModRM(concat, regDst, regSrc);
    }
    else if (numBitsSrc == OpBits::B32 && numBitsDst == OpBits::B64)
    {
        emitREX(concat, numBitsDst, regDst, regSrc);
        emitCPUOp(concat, 0x63);
        emitModRM(concat, regDst, regSrc);
    }
    else
    {
        Report::internalError(module, "encodeLoadSignedExtendRegReg, cannot encode");
    }

    return CpuEncodeResult::Zero;
}

/////////////////////////////////////////////////////////////////////

CpuEncodeResult ScbeX64::encodeLoadAddressMem(CpuReg reg, CpuReg memReg, uint64_t memOffset, CpuEmitFlags emitFlags)
{
    if (memReg == CpuReg::Rip)
    {
        SWAG_ASSERT(memOffset == 0);
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, OpBits::B64, reg, memReg);
        emitCPUOp(concat, 0x8D);
        emitModRM(concat, ModRMMode::Memory, reg, MODRM_RM_RIP);
    }
    else if (memOffset == 0)
    {
        emitLoadRegReg(reg, memReg, OpBits::B64);
    }
    else
    {
        if (memOffset > 0x7FFFFFFF)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::NotSupported;
            Report::internalError(module, "encodeLoadAddressMem, cannot encode");
        }

        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, OpBits::B64, reg, memReg);
        emitCPUOp(concat, 0x8D);
        emitModRM(concat, memOffset, reg, memReg);
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeLoadAddressAddMul(CpuReg regDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, OpBits opBits, CpuEmitFlags emitFlags)
{
    SWAG_ASSERT(opBits == OpBits::B32 || opBits == OpBits::B64);
    emitAddMul(concat, 0x8D, regDst, regSrc1, regSrc2, mulValue, opBits, emitFlags);
    return CpuEncodeResult::Zero;
}

/////////////////////////////////////////////////////////////////////

CpuEncodeResult ScbeX64::encodeLoadMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (memOffset > 0x7FFFFFFF)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::NotSupported;
        Report::internalError(module, "encodeLoadMemReg, cannot encode");
    }

    if (emitFlags.has(EMIT_CanEncode))
        return CpuEncodeResult::Zero;

    if (isFloat(reg))
    {
        emitSpecF64(concat, 0xF3, opBits);
        emitREX(concat, OpBits::Zero, reg, memReg);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0x11);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else
    {
        emitREX(concat, opBits, reg, memReg);
        emitSpecCPUOp(concat, 0x89, opBits);
        emitModRM(concat, memOffset, reg, memReg);
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeLoadMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (opBits == OpBits::B64 && value > 0x7FFFFFFF && value >> 32 != 0xFFFFFFFF)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Right2Reg;
        Report::internalError(module, "encodeLoadMemImm, cannot encode");
    }

    if (memOffset > 0x7FFFFFFF)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::NotSupported;
        Report::internalError(module, "encodeLoadMemImm, cannot encode");
    }

    if (emitFlags.has(EMIT_CanEncode))
        return CpuEncodeResult::Zero;

    emitREX(concat, opBits, REX_REG_NONE, memReg);
    emitSpecB8(concat, 0xC7, opBits);
    emitModRM(concat, memOffset, MODRM_REG_0, memReg);
    emitValue(concat, value, std::min(opBits, OpBits::B32));
    return CpuEncodeResult::Zero;
}

/////////////////////////////////////////////////////////////////////

CpuEncodeResult ScbeX64::encodeClearReg(CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (isFloat(reg))
    {
        emitPrefixF64(concat, opBits);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, CpuOp::FXOR);
        emitModRM(concat, reg, reg);
    }
    else
    {
        emitREX(concat, opBits, reg, reg);
        emitSpecCPUOp(concat, CpuOp::XOR, opBits);
        emitModRM(concat, reg, reg);
    }

    return CpuEncodeResult::Zero;
}

/////////////////////////////////////////////////////////////////////

CpuEncodeResult ScbeX64::encodeSetCond(CpuReg reg, CpuCondFlag setType, CpuEmitFlags emitFlags)
{
    emitREX(concat, OpBits::B8, REX_REG_NONE, reg);
    emitCPUOp(concat, 0x0F);

    switch (setType)
    {
        case CpuCondFlag::A:
            emitCPUOp(concat, 0x97);
            break;
        case CpuCondFlag::O:
            emitCPUOp(concat, 0x90);
            break;
        case CpuCondFlag::AE:
            emitCPUOp(concat, 0x93);
            break;
        case CpuCondFlag::G:
            emitCPUOp(concat, 0x9F);
            break;
        case CpuCondFlag::NE:
            emitCPUOp(concat, 0x95);
            break;
        case CpuCondFlag::NA:
            emitCPUOp(concat, 0x96);
            break;
        case CpuCondFlag::B:
            emitCPUOp(concat, 0x92);
            break;
        case CpuCondFlag::BE:
            emitCPUOp(concat, 0x96);
            break;
        case CpuCondFlag::E:
            emitCPUOp(concat, 0x94);
            break;
        case CpuCondFlag::GE:
            emitCPUOp(concat, 0x9D);
            break;
        case CpuCondFlag::L:
            emitCPUOp(concat, 0x9C);
            break;
        case CpuCondFlag::LE:
            emitCPUOp(concat, 0x9E);
            break;
        case CpuCondFlag::P:
            emitCPUOp(concat, 0x9A);
            break;
        case CpuCondFlag::NP:
            emitCPUOp(concat, 0x9B);
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }

    emitModRM(concat, MODRM_REG_0, reg);
    return CpuEncodeResult::Zero;
}

/////////////////////////////////////////////////////////////////////

CpuEncodeResult ScbeX64::encodeCmpRegReg(CpuReg reg0, CpuReg reg1, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (isFloat(reg0))
    {
        emitPrefixF64(concat, opBits);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0x2F);
        emitModRM(concat, reg0, reg1);
    }
    else
    {
        emitREX(concat, opBits, reg1, reg0);
        emitSpecCPUOp(concat, 0x39, opBits);
        emitModRM(concat, reg1, reg0);
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeCmpRegImm(CpuReg reg, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (emitFlags.has(EMIT_CanEncode))
    {
        if (isFloat(reg))
            return CpuEncodeResult::NotSupported;
        return CpuEncodeResult::Zero;
    }

    if (opBits == OpBits::B8)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, opBits, REX_REG_NONE, reg);
        emitCPUOp(concat, 0x80);
        emitModRM(concat, MODRM_REG_7, reg);
        emitValue(concat, value, OpBits::B8);
    }
    else if (canEncode8(value, opBits))
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, opBits, REX_REG_NONE, reg);
        emitCPUOp(concat, 0x83);
        emitModRM(concat, MODRM_REG_7, reg);
        emitValue(concat, value, OpBits::B8);
    }
    else if ((opBits != OpBits::B64 || value <= 0x7FFFFFFF))
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, opBits, REX_REG_NONE, reg);
        emitCPUOp(concat, 0x81);
        emitModRM(concat, MODRM_REG_7, reg);
        emitValue(concat, value, std::min(opBits, OpBits::B32));
    }
    else
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Right2Reg;
        Report::internalError(module, "encodeCmpRegImm, cannot encode");
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeCmpMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (memOffset > 0x7FFFFFFF)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::NotSupported;
        Report::internalError(module, "encodeCmpMemReg, cannot encode");
    }

    if (isFloat(reg))
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Left2Reg;
        Report::internalError(module, "encodeCmpMemReg, cannot encode");
    }
    else
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, opBits, reg, memReg);
        emitSpecCPUOp(concat, 0x39, opBits);
        emitModRM(concat, memOffset, reg, memReg);
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeCmpMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (memOffset > 0x7FFFFFFF)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::NotSupported;
        Report::internalError(module, "encodeCmpMemImm, cannot encode");
    }

    if (opBits == OpBits::B8)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, opBits, REX_REG_NONE, memReg);
        emitCPUOp(concat, 0x80);
        emitModRM(concat, memOffset, MODRM_REG_7, memReg);
        emitValue(concat, value, OpBits::B8);
    }
    else if (canEncode8(value, opBits))
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, opBits, REX_REG_NONE, memReg);
        emitCPUOp(concat, 0x83);
        emitModRM(concat, memOffset, MODRM_REG_7, memReg);
        emitValue(concat, value, OpBits::B8);
    }
    else if (value <= 0x7FFFFFFF)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, opBits, REX_REG_NONE, memReg);
        emitCPUOp(concat, 0x81);
        emitModRM(concat, memOffset, MODRM_REG_7, memReg);
        emitValue(concat, value, opBits == OpBits::B16 ? opBits : OpBits::B32);
    }
    else
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Right2Reg;
        Report::internalError(module, "encodeCmpRegImm, cannot encode");
    }

    return CpuEncodeResult::Zero;
}

/////////////////////////////////////////////////////////////////////

CpuEncodeResult ScbeX64::encodeOpUnaryMem(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    ///////////////////////////////////////////

    if (op == CpuOp::NOT)
    {
        SWAG_ASSERT(memReg == CpuReg::Rsp);
        emitREX(concat, opBits);
        emitSpecCPUOp(concat, 0xF7, opBits);
        emitModRM(concat, memOffset, MODRM_REG_2, memReg);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::NEG)
    {
        SWAG_ASSERT(memReg == CpuReg::Rsp);
        emitREX(concat, opBits);
        emitSpecCPUOp(concat, 0xF7, opBits);
        emitModRM(concat, memOffset, MODRM_REG_3, memReg);
    }

    ///////////////////////////////////////////

    else
    {
        Report::internalError(module, "encodeOpUnaryMem, cannot encode");
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeOpUnaryReg(CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (emitFlags.has(EMIT_CanEncode))
    {
        if (isFloat(reg))
            return CpuEncodeResult::NotSupported;
        return CpuEncodeResult::Zero;
    }

    ///////////////////////////////////////////

    if (op == CpuOp::NOT)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, opBits, REX_REG_NONE, reg);
        emitSpecCPUOp(concat, 0xF7, opBits);
        emitModRM(concat, MODRM_REG_2, reg);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::NEG)
    {
        if (isFloat(reg))
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::NotSupported;
            Report::internalError(module, "encodeOpUnaryReg, cannot encode");
        }
        else
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitSpecCPUOp(concat, 0xF7, opBits);
            emitModRM(concat, MODRM_REG_3, reg);
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::BSWAP)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        if (opBits == OpBits::B16)
        {
            // rol ax, 0x8
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0xC1);
            emitCPUOp(concat, 0xC0);
            emitValue(concat, 0x08, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(opBits == OpBits::B16 || opBits == OpBits::B32 || opBits == OpBits::B64);
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0xC8, reg);
        }
    }

    ///////////////////////////////////////////

    else
    {
        Report::internalError(module, "encodeOpUnaryReg, cannot encode");
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeOpBinaryRegMem(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    ///////////////////////////////////////////

    if (op == CpuOp::ADD)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, opBits, regDst, memReg);
        emitSpecCPUOp(concat, 0x03, opBits);
        emitModRM(concat, memOffset, regDst, memReg);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::SUB)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, opBits, regDst, memReg);
        emitSpecCPUOp(concat, 0x2B, opBits);
        emitModRM(concat, memOffset, regDst, memReg);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::AND)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, opBits, regDst, memReg);
        emitSpecCPUOp(concat, 0x23, opBits);
        emitModRM(concat, memOffset, regDst, memReg);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::OR)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, opBits, regDst, memReg);
        emitSpecCPUOp(concat, 0x0B, opBits);
        emitModRM(concat, memOffset, regDst, memReg);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::XOR)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, opBits, regDst, memReg);
        emitSpecCPUOp(concat, 0x33, opBits);
        emitModRM(concat, memOffset, regDst, memReg);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::IMUL)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        if (opBits == OpBits::B8)
            emitLoadSignedExtendRegReg(regDst, regDst, OpBits::B32, opBits);
        emitREX(concat, opBits, regDst, memReg);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0xAF);
        emitModRM(concat, memOffset, regDst, memReg);
    }

    ///////////////////////////////////////////

    else
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Right2Reg;
        Report::internalError(module, "encodeOpBinaryRegMem, cannot encode");
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeOpBinaryRegReg(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    ///////////////////////////////////////////

    if (op == CpuOp::CVTU2F64)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::NotSupported;
        Report::internalError(module, "encodeOpBinaryRegReg, cannot encode");
    }

    ///////////////////////////////////////////

    else if (isFloat(regDst) || isFloat(regSrc))
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;

        if (op != CpuOp::FSQRT &&
            op != CpuOp::FAND &&
            op != CpuOp::FXOR)
        {
            emitSpecF64(concat, 0xF3, opBits);
            emitREX(concat, emitFlags.has(EMIT_B64) ? OpBits::B64 : OpBits::B32, regDst, regSrc);
        }
        else
        {
            emitPrefixF64(concat, opBits);
        }

        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, op);
        emitModRM(concat, regDst, regSrc);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::DIV ||
             op == CpuOp::IDIV ||
             op == CpuOp::MOD ||
             op == CpuOp::IMOD)
    {
        const auto rax = x64Reg2CpuReg(X64Reg::Rax);
        if (emitFlags.has(EMIT_CanEncode))
        {
            if (cpuRegToX64Reg(regDst) != X64Reg::Rax)
                return CpuEncodeResult::Left2Rax;
            if (cpuRegToX64Reg(regSrc) == X64Reg::Rax)
                return CpuEncodeResult::NotSupported;
            if (cpuRegToX64Reg(regSrc) == X64Reg::Rdx)
                return CpuEncodeResult::NotSupported;

            return CpuEncodeResult::Zero;
        }

        if ((op == CpuOp::IDIV || op == CpuOp::IMOD) && opBits == OpBits::B8)
            emitLoadSignedExtendRegReg(rax, rax, OpBits::B32, OpBits::B8);
        else if (opBits == OpBits::B8)
            emitLoadZeroExtendRegReg(rax, rax, OpBits::B32, OpBits::B8);
        else if (op == CpuOp::DIV || op == CpuOp::MOD)
            emitClearReg(x64Reg2CpuReg(X64Reg::Rdx), opBits);
        else
        {
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x99); // cdq
        }

        emitREX(concat, opBits, rax, regSrc);
        emitSpecCPUOp(concat, 0xF7, opBits);
        if (op == CpuOp::DIV || op == CpuOp::MOD)
            emitModRM(concat, MODRM_REG_6, regSrc);
        else if (op == CpuOp::IDIV || op == CpuOp::IMOD)
            emitModRM(concat, MODRM_REG_7, regSrc);

        if ((op == CpuOp::MOD || op == CpuOp::IMOD) && opBits == OpBits::B8)
            emitOpBinaryRegImm(rax, 8, CpuOp::SHR, OpBits::B32, emitFlags); // AH => AL
        else if (op == CpuOp::MOD || op == CpuOp::IMOD)
            emitLoadRegReg(rax, x64Reg2CpuReg(X64Reg::Rdx), opBits);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::MUL)
    {
        const auto rax = x64Reg2CpuReg(X64Reg::Rax);
        if (emitFlags.has(EMIT_CanEncode))
        {
            if (cpuRegToX64Reg(regDst) != X64Reg::Rax)
                return CpuEncodeResult::Left2Rax;
            if (cpuRegToX64Reg(regSrc) == X64Reg::Rax)
                return CpuEncodeResult::NotSupported;
            if (cpuRegToX64Reg(regSrc) == X64Reg::Rdx)
                return CpuEncodeResult::NotSupported;

            return CpuEncodeResult::Zero;
        }

        emitREX(concat, opBits, rax, regSrc);
        emitSpecCPUOp(concat, 0xF7, opBits);
        emitModRM(concat, MODRM_REG_4, regSrc);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::IMUL)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;

        if (opBits == OpBits::B8)
        {
            emitLoadSignedExtendRegReg(regDst, regDst, OpBits::B32, opBits);
            emitLoadSignedExtendRegReg(regSrc, regSrc, OpBits::B32, opBits);
        }

        emitREX(concat, opBits, regDst, regSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0xAF);
        emitModRM(concat, regDst, regSrc);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::ROL ||
             op == CpuOp::ROR ||
             op == CpuOp::SAL ||
             op == CpuOp::SAR ||
             op == CpuOp::SHL ||
             op == CpuOp::SHR)
    {
        if (emitFlags.has(EMIT_CanEncode))
        {
            if (cpuRegToX64Reg(regSrc) != X64Reg::Rcx)
            {
                SWAG_ASSERT(x64Reg2CpuReg(X64Reg::Rcx) == CpuReg::Rcx);
                return CpuEncodeResult::Right2Rcx;
            }

            return CpuEncodeResult::Zero;
        }

        SWAG_ASSERT(cpuRegToX64Reg(regSrc) == X64Reg::Rcx);
        emitREX(concat, opBits, REX_REG_NONE, regDst);
        emitSpecCPUOp(concat, 0xD3, opBits);
        if (op == CpuOp::ROL)
            emitModRM(concat, MODRM_REG_0, regDst);
        else if (op == CpuOp::ROR)
            emitModRM(concat, MODRM_REG_1, regDst);
        else if (op == CpuOp::SAL || op == CpuOp::SHL)
            emitModRM(concat, MODRM_REG_4, regDst);
        else if (op == CpuOp::SAR)
            emitModRM(concat, MODRM_REG_7, regDst);
        else if (op == CpuOp::SHR)
            emitModRM(concat, MODRM_REG_5, regDst);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::ADD ||
             op == CpuOp::SUB ||
             op == CpuOp::XOR ||
             op == CpuOp::AND ||
             op == CpuOp::OR)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, opBits, regSrc, regDst);
        emitSpecCPUOp(concat, op, opBits);
        emitModRM(concat, regSrc, regDst);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::CMOVB ||
             op == CpuOp::CMOVE ||
             op == CpuOp::CMOVG ||
             op == CpuOp::CMOVL ||
             op == CpuOp::CMOVBE ||
             op == CpuOp::CMOVGE)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        opBits = std::max(opBits, OpBits::B32);
        emitREX(concat, opBits, regDst, regSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, op);
        emitModRM(concat, regDst, regSrc);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::BSF ||
             op == CpuOp::BSR)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitREX(concat, opBits, regDst, regSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, op == CpuOp::BSF ? 0xBC : 0xBD);
        emitModRM(concat, regDst, regSrc);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::POPCNT)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        emitCPUOp(concat, 0xF3);
        emitREX(concat, opBits, regDst, regSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0xB8);
        emitModRM(concat, regDst, regSrc);
    }

    ///////////////////////////////////////////

    else
    {
        Report::internalError(module, "encodeOpBinaryRegReg, cannot encode");
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeOpBinaryMemReg(CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (memOffset > 0x7FFFFFFF)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::NotSupported;
        Report::internalError(module, "encodeOpBinaryMemReg, cannot encode");
    }

    ///////////////////////////////////////////

    if (isFloat(reg))
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Left2Reg;
        Report::internalError(module, "encodeOpBinaryMemReg, cannot encode");
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::DIV ||
             op == CpuOp::IDIV ||
             op == CpuOp::MOD ||
             op == CpuOp::IMOD ||
             op == CpuOp::IMUL ||
             op == CpuOp::MUL)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Left2Reg;
        Report::internalError(module, "encodeOpBinaryMemReg, cannot encode");
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::SAR ||
             op == CpuOp::SHR ||
             op == CpuOp::SHL)
    {
        if (emitFlags.has(EMIT_CanEncode))
        {
            if (cpuRegToX64Reg(reg) != X64Reg::Rcx)
            {
                SWAG_ASSERT(x64Reg2CpuReg(X64Reg::Rcx) == CpuReg::Rcx);
                return CpuEncodeResult::Right2Rcx;
            }

            return CpuEncodeResult::Zero;
        }

        if (emitFlags.has(EMIT_Lock))
            concat.addU8(0xF0);
        emitREX(concat, opBits, REX_REG_NONE, memReg);
        emitSpecCPUOp(concat, 0xD3, opBits);
        if (op == CpuOp::SHL)
            emitModRM(concat, memOffset, MODRM_REG_4, memReg);
        else if (op == CpuOp::SAR)
            emitModRM(concat, memOffset, MODRM_REG_7, memReg);
        else if (op == CpuOp::SHR)
            emitModRM(concat, memOffset, MODRM_REG_5, memReg);
    }

    ///////////////////////////////////////////

    else
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;
        if (emitFlags.has(EMIT_Lock))
            concat.addU8(0xF0);
        emitREX(concat, opBits, reg, memReg);
        emitSpecCPUOp(concat, op, opBits);
        emitModRM(concat, memOffset, reg, memReg);
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeOpBinaryRegImm(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    ///////////////////////////////////////////

    if (op == CpuOp::XOR)
    {
        if (opBits == OpBits::B8)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, MODRM_REG_6, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (canEncode8(value, opBits))
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, MODRM_REG_6, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7FFFFFFF)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, MODRM_REG_6, reg);
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
        else
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Right2Reg;
            Report::internalError(module, "encodeOpBinaryRegImm, cannot encode");
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::OR)
    {
        if (opBits == OpBits::B8)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, MODRM_REG_1, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (canEncode8(value, opBits))
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, MODRM_REG_1, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7FFFFFFF)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, MODRM_REG_1, reg);
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
        else
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Right2Reg;
            Report::internalError(module, "encodeOpBinaryRegImm, cannot encode");
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::AND)
    {
        if (opBits == OpBits::B8)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, MODRM_REG_4, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (canEncode8(value, opBits))
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, MODRM_REG_4, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7FFFFFFF)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, MODRM_REG_4, reg);
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
        else
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Right2Reg;
            Report::internalError(module, "encodeOpBinaryRegImm, cannot encode");
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::ADD)
    {
        if (value == 1 && !emitFlags.has(EMIT_Overflow) && optLevel >= BuildCfgBackendOptim::O1)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitSpecCPUOp(concat, 0xFF, opBits);
            emitModRM(concat, MODRM_REG_0, reg);
        }
        else if (opBits == OpBits::B8)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, MODRM_REG_0, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (canEncode8(value, opBits))
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, MODRM_REG_0, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7FFFFFFF)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, MODRM_REG_0, reg);
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
        else
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Right2Reg;
            Report::internalError(module, "encodeOpBinaryRegImm, cannot encode");
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::SUB)
    {
        if (value == 1 && !emitFlags.has(EMIT_Overflow) && optLevel >= BuildCfgBackendOptim::O1)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitSpecCPUOp(concat, 0xFF, opBits);
            emitModRM(concat, MODRM_REG_1, reg);
        }
        else if (opBits == OpBits::B8)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, MODRM_REG_5, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (canEncode8(value, opBits))
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, MODRM_REG_5, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7FFFFFFF)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, MODRM_REG_5, reg);
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
        else
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Right2Reg;
            Report::internalError(module, "encodeOpBinaryRegImm, cannot encode");
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::MOD ||
             op == CpuOp::IMOD ||
             op == CpuOp::DIV ||
             op == CpuOp::IDIV ||
             op == CpuOp::MUL)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Right2Reg;
        Report::internalError(module, "encodeOpBinaryRegImm, cannot encode");
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::IMUL)
    {
        if (canEncode8(value, opBits))
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            if (opBits == OpBits::B8)
                emitLoadSignedExtendRegReg(reg, reg, OpBits::B32, opBits);
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x6B);
            emitModRM(concat, reg, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7FFFFFFF)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            if (opBits == OpBits::B8 || opBits == OpBits::B16)
                emitLoadSignedExtendRegReg(reg, reg, OpBits::B32, opBits);
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x69);
            emitModRM(concat, reg, reg);
            emitValue(concat, value, OpBits::B32);
        }
        else
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Right2Reg;
            Report::internalError(module, "encodeOpBinaryRegImm, cannot encode");
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::SHL)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;

        SWAG_ASSERT(value <= 0x7F);
        if (value == 1)
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitSpecCPUOp(concat, 0xD1, opBits);
            emitModRM(concat, MODRM_REG_4, reg);
        }
        else
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitSpecCPUOp(concat, 0xC1, opBits);
            emitModRM(concat, MODRM_REG_4, reg);
            emitValue(concat, std::min(static_cast<uint32_t>(value), getNumBits(opBits) - 1), OpBits::B8);
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::SHR)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;

        SWAG_ASSERT(value <= 0x7F);
        if (value == 1)
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitSpecCPUOp(concat, 0xD1, opBits);
            emitModRM(concat, MODRM_REG_5, reg);
        }
        else
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitSpecCPUOp(concat, 0xC1, opBits);
            emitModRM(concat, MODRM_REG_5, reg);
            emitValue(concat, std::min(static_cast<uint32_t>(value), getNumBits(opBits) - 1), OpBits::B8);
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::SAR)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;

        SWAG_ASSERT(value <= 0x7F);
        if (value == 1)
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitSpecCPUOp(concat, 0xD1, opBits);
            emitModRM(concat, MODRM_REG_7, reg);
        }
        else
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitSpecCPUOp(concat, 0xC1, opBits);
            emitModRM(concat, MODRM_REG_7, reg);
            emitValue(concat, std::min(static_cast<uint32_t>(value), getNumBits(opBits) - 1), OpBits::B8);
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::BT)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;

        SWAG_ASSERT(value <= 0x7F);
        emitREX(concat, opBits, REX_REG_NONE, reg);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, op);
        emitModRM(concat, MODRM_REG_4, reg);
        emitValue(concat, value, OpBits::B8);
    }

    ///////////////////////////////////////////

    else
    {
        Report::internalError(module, "encodeOpBinaryRegImm, cannot encode");
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeOpBinaryMemImm(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (memOffset > 0x7FFFFFFF)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::NotSupported;
        Report::internalError(module, "encodeOpBinaryMemImm, cannot encode");
    }

    ///////////////////////////////////////////

    if (op == CpuOp::IMOD ||
        op == CpuOp::MOD ||
        op == CpuOp::DIV ||
        op == CpuOp::IDIV ||
        op == CpuOp::IMUL ||
        op == CpuOp::MUL)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Right2Reg;
        Report::internalError(module, "encodeOpBinaryMemImm, cannot encode");
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::SAR)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;

        if (value == 1)
        {
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitSpecCPUOp(concat, 0xD1, opBits);
            emitModRM(concat, memOffset, MODRM_REG_7, memReg);
        }
        else
        {
            SWAG_ASSERT(value <= 0x7F);
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitSpecCPUOp(concat, 0xC1, opBits);
            emitModRM(concat, memOffset, MODRM_REG_7, memReg);
            emitValue(concat, std::min(static_cast<uint32_t>(value), getNumBits(opBits) - 1), OpBits::B8);
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::SHR)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;

        if (value == 1)
        {
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitSpecCPUOp(concat, 0xD1, opBits);
            emitModRM(concat, memOffset, MODRM_REG_5, memReg);
        }
        else
        {
            SWAG_ASSERT(value <= 0x7F);
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitSpecCPUOp(concat, 0xC1, opBits);
            emitModRM(concat, memOffset, MODRM_REG_5, memReg);
            emitValue(concat, std::min(static_cast<uint32_t>(value), getNumBits(opBits) - 1), OpBits::B8);
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::SHL)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;

        if (value == 1)
        {
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitSpecCPUOp(concat, 0xD1, opBits);
            emitModRM(concat, memOffset, MODRM_REG_4, memReg);
        }
        else
        {
            SWAG_ASSERT(value <= 0x7F);
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitSpecCPUOp(concat, 0xC1, opBits);
            emitModRM(concat, memOffset, MODRM_REG_4, memReg);
            emitValue(concat, std::min(static_cast<uint32_t>(value), getNumBits(opBits) - 1), OpBits::B8);
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::ADD)
    {
        if (value == 1 && !emitFlags.has(EMIT_Overflow) && optLevel >= BuildCfgBackendOptim::O1)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitSpecCPUOp(concat, 0xFF, opBits);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg);
        }
        else if (opBits == OpBits::B8)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (canEncode8(value, opBits))
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7FFFFFFF)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg);
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
        else
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Right2Reg;
            Report::internalError(module, "encodeOpBinaryMemImm, cannot encode");
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::SUB)
    {
        if (value == 1 && !emitFlags.has(EMIT_Overflow) && optLevel >= BuildCfgBackendOptim::O1)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitSpecCPUOp(concat, 0xFF, opBits);
            emitModRM(concat, memOffset, MODRM_REG_1, memReg);
        }
        else if (opBits == OpBits::B8)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, memOffset, MODRM_REG_5, memReg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (canEncode8(value, opBits))
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, memOffset, MODRM_REG_5, memReg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7FFFFFFF)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, memOffset, MODRM_REG_5, memReg);
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
        else
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Right2Reg;
            Report::internalError(module, "encodeOpBinaryMemImm, cannot encode");
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::OR)
    {
        if (opBits == OpBits::B8)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, memOffset, MODRM_REG_1, memReg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (canEncode8(value, opBits))
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, memOffset, MODRM_REG_1, memReg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7FFFFFFF)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, memOffset, MODRM_REG_1, memReg);
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
        else
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Right2Reg;
            Report::internalError(module, "encodeOpBinaryMemImm, cannot encode");
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::AND)
    {
        if (opBits == OpBits::B8)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, memOffset, MODRM_REG_4, memReg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (canEncode8(value, opBits))
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, memOffset, MODRM_REG_4, memReg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7FFFFFFF)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, memOffset, MODRM_REG_4, memReg);
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
        else
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Right2Reg;
            Report::internalError(module, "encodeOpBinaryMemImm, cannot encode");
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::XOR)
    {
        if (opBits == OpBits::B8)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, memOffset, MODRM_REG_6, memReg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (canEncode8(value, opBits))
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, memOffset, MODRM_REG_6, memReg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7FFFFFFF)
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Zero;
            emitREX(concat, opBits, REX_REG_NONE, memReg);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, memOffset, MODRM_REG_6, memReg);
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
        else
        {
            if (emitFlags.has(EMIT_CanEncode))
                return CpuEncodeResult::Right2Reg;
            Report::internalError(module, "encodeOpBinaryMemImm, cannot encode");
        }
    }

    ///////////////////////////////////////////

    else
    {
        Report::internalError(module, "encodeOpBinaryMemImm, cannot encode");
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeOpTernaryRegRegReg(CpuReg reg0, CpuReg reg1, CpuReg reg2, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    ///////////////////////////////////////////

    if (op == CpuOp::MULADD)
    {
        if (emitFlags.has(EMIT_CanEncode))
            return CpuEncodeResult::Zero;

        SWAG_ASSERT(isFloat(reg0) && isFloat(reg1) && isFloat(reg2));
        emitSpecF64(concat, 0xF3, opBits);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, CpuOp::FMUL);
        emitModRM(concat, reg0, reg1);

        emitSpecF64(concat, 0xF3, opBits);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, CpuOp::FADD);
        emitModRM(concat, reg0, reg2);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::CMPXCHG)
    {
        const auto rax = x64Reg2CpuReg(X64Reg::Rax);
        if (emitFlags.has(EMIT_CanEncode))
        {
            if (cpuRegToX64Reg(reg0) != X64Reg::Rax)
            {
                SWAG_ASSERT(rax == CpuReg::Rax);
                return CpuEncodeResult::Left2Rax;
            }

            return CpuEncodeResult::Zero;
        }

        SWAG_ASSERT(cpuRegToX64Reg(reg0) == X64Reg::Rax);

        if (emitFlags.has(EMIT_Lock))
            emitCPUOp(concat, 0xF0);
        emitREX(concat, opBits, reg2, reg1);
        emitCPUOp(concat, 0x0F);
        emitSpecCPUOp(concat, 0xB1, opBits);
        emitModRM(concat, 0, reg2, reg1);
    }

    ///////////////////////////////////////////

    else
    {
        Report::internalError(module, "encodeOpTernaryRegRegReg, cannot encode");
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries, CpuEmitFlags emitFlags)
{
    uint8_t*   addrConstant        = nullptr;
    const auto offsetTableConstant = buildParams.module->constantSegment.reserve(numEntries * sizeof(uint32_t), &addrConstant);
    emitSymbolRelocationAddress(table, symCSIndex, offsetTableConstant);

    // movsxd table, dword ptr [table + offset*4]
    emitAddMul(concat, 0x63, table, table, offset, 4, OpBits::B64, emitFlags);

    const auto startIdx = concat.totalCount();
    emitSymbolRelocationAddress(offset, cpuFct->symbolIndex, concat.totalCount() - cpuFct->startAddress);
    const auto patchPtr = reinterpret_cast<uint32_t*>(concat.currentSP) - 1;
    emitOpBinaryRegReg(offset, table, CpuOp::ADD, OpBits::B64, emitFlags);
    emitJumpReg(offset);
    const auto endIdx = concat.totalCount();
    *patchPtr += endIdx - startIdx;

    const auto tableCompiler = reinterpret_cast<int32_t*>(buildParams.module->compilerSegment.address(offsetTable));
    const auto currentOffset = static_cast<int32_t>(concat.totalCount());

    CpuLabelToSolve label;
    for (uint32_t idx = 0; idx < numEntries; idx++)
    {
        label.ipDest      = tableCompiler[idx] + currentIp + 1;
        label.jump.opBits = OpBits::B32;
        label.jump.offset = currentOffset;
        label.jump.addr   = addrConstant + idx * sizeof(uint32_t);
        cpuFct->labelsToSolve.push_back(label);
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeJump(CpuJump& jump, CpuCondJump jumpType, OpBits opBits, CpuEmitFlags emitFlags)
{
    SWAG_ASSERT(opBits == OpBits::B8 || opBits == OpBits::B32);

    if (opBits == OpBits::B8)
    {
        switch (jumpType)
        {
            case CpuCondJump::JNO:
                emitCPUOp(concat, 0x71);
                break;
            case CpuCondJump::JB:
                emitCPUOp(concat, 0x72);
                break;
            case CpuCondJump::JAE:
                emitCPUOp(concat, 0x73);
                break;
            case CpuCondJump::JZ:
                emitCPUOp(concat, 0x74);
                break;
            case CpuCondJump::JNZ:
                emitCPUOp(concat, 0x75);
                break;
            case CpuCondJump::JBE:
                emitCPUOp(concat, 0x76);
                break;
            case CpuCondJump::JA:
                concat.addU8(0x77);
                break;
            case CpuCondJump::JS:
                emitCPUOp(concat, 0x78);
                break;
            case CpuCondJump::JP:
                emitCPUOp(concat, 0x7A);
                break;
            case CpuCondJump::JNP:
                emitCPUOp(concat, 0x7B);
                break;
            case CpuCondJump::JL:
                emitCPUOp(concat, 0x7C);
                break;
            case CpuCondJump::JGE:
                emitCPUOp(concat, 0x7D);
                break;
            case CpuCondJump::JLE:
                emitCPUOp(concat, 0x7E);
                break;
            case CpuCondJump::JG:
                emitCPUOp(concat, 0x7F);
                break;
            case CpuCondJump::JUMP:
                emitCPUOp(concat, 0xEB);
                break;
            default:
                SWAG_ASSERT(false);
                break;
        }

        concat.addU8(0);

        jump.addr   = concat.getSeekPtr() - 1;
        jump.offset = concat.totalCount();
        jump.opBits = opBits;
        return CpuEncodeResult::Zero;
    }

    switch (jumpType)
    {
        case CpuCondJump::JNO:
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x81);
            break;
        case CpuCondJump::JB:
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x82);
            break;
        case CpuCondJump::JAE:
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x83);
            break;
        case CpuCondJump::JZ:
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x84);
            break;
        case CpuCondJump::JNZ:
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x85);
            break;
        case CpuCondJump::JBE:
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x86);
            break;
        case CpuCondJump::JA:
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x87);
            break;
        case CpuCondJump::JP:
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x8A);
            break;
        case CpuCondJump::JS:
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x88);
            break;
        case CpuCondJump::JNP:
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x8B);
            break;
        case CpuCondJump::JL:
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x8C);
            break;
        case CpuCondJump::JGE:
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x8D);
            break;
        case CpuCondJump::JLE:
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x8E);
            break;
        case CpuCondJump::JG:
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x8F);
            break;
        case CpuCondJump::JUMP:
            emitCPUOp(concat, 0xE9);
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }

    concat.addU32(0);

    jump.addr   = concat.getSeekPtr() - sizeof(uint32_t);
    jump.offset = concat.totalCount();
    jump.opBits = opBits;
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodePatchJump(const CpuJump& jump, uint64_t offsetDestination, CpuEmitFlags emitFlags)
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

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodePatchJump(const CpuJump& jump, CpuEmitFlags emitFlags)
{
    return encodePatchJump(jump, concat.totalCount(), emitFlags);
}

CpuEncodeResult ScbeX64::encodeJumpReg(CpuReg reg, CpuEmitFlags emitFlags)
{
    emitREX(concat, OpBits::Zero, REX_REG_NONE, reg);
    emitCPUOp(concat, 0xFF);
    emitModRM(concat, ModRMMode::Register, MODRM_REG_4, encodeReg(reg));
    return CpuEncodeResult::Zero;
}

/////////////////////////////////////////////////////////////////////

CpuEncodeResult ScbeX64::encodeCallExtern(const Utf8& symbolName, CpuEmitFlags emitFlags)
{
    emitCPUOp(concat, 0xFF);
    emitModRM(concat, ModRMMode::Memory, MODRM_REG_2, MODRM_RM_RIP);

    const auto callSym = getOrAddSymbol(symbolName, CpuSymbolKind::Extern);
    addSymbolRelocation(concat.totalCount() - textSectionOffset, callSym->index, IMAGE_REL_AMD64_REL32);
    concat.addU32(0);
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeCallLocal(const Utf8& symbolName, CpuEmitFlags emitFlags)
{
    emitCPUOp(concat, 0xE8);

    const auto callSym = getOrAddSymbol(symbolName, CpuSymbolKind::Extern);
    if (callSym->kind == CpuSymbolKind::Function)
    {
        concat.addS32(static_cast<int32_t>(callSym->value + textSectionOffset - (concat.totalCount() + 4)));
    }
    else
    {
        addSymbolRelocation(concat.totalCount() - textSectionOffset, callSym->index, IMAGE_REL_AMD64_REL32);
        concat.addU32(0);
    }

    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeCallReg(CpuReg reg, CpuEmitFlags emitFlags)
{
    emitREX(concat, OpBits::Zero, REX_REG_NONE, reg);
    emitCPUOp(concat, 0xFF);
    emitModRM(concat, MODRM_REG_2, reg);
    return CpuEncodeResult::Zero;
}

CpuEncodeResult ScbeX64::encodeNop(CpuEmitFlags emitFlags)
{
    emitCPUOp(concat, 0x90);
    return CpuEncodeResult::Zero;
}

RegisterSet ScbeX64::getReadRegisters(ScbeMicroInstruction* inst)
{
    auto result = ScbeCpu::getReadRegisters(inst);

    if (inst->op == ScbeMicroOp::OpBinaryRI ||
        inst->op == ScbeMicroOp::OpBinaryRR ||
        inst->op == ScbeMicroOp::OpBinaryMI ||
        inst->op == ScbeMicroOp::OpBinaryRM ||
        inst->op == ScbeMicroOp::OpBinaryMR)
    {
        if (inst->cpuOp == CpuOp::ROL ||
            inst->cpuOp == CpuOp::ROR ||
            inst->cpuOp == CpuOp::SAL ||
            inst->cpuOp == CpuOp::SAR ||
            inst->cpuOp == CpuOp::SHL ||
            inst->cpuOp == CpuOp::SHR)
        {
            result.add(x64Reg2CpuReg(X64Reg::Rcx));
        }
        else if (inst->cpuOp == CpuOp::MUL ||
                 inst->cpuOp == CpuOp::DIV ||
                 inst->cpuOp == CpuOp::MOD ||
                 inst->cpuOp == CpuOp::IDIV ||
                 inst->cpuOp == CpuOp::IMOD)
        {
            result.add(x64Reg2CpuReg(X64Reg::Rdx));
        }
    }

    return result;
}

RegisterSet ScbeX64::getWriteRegisters(ScbeMicroInstruction* inst)
{
    auto result = ScbeCpu::getWriteRegisters(inst);

    if (inst->op == ScbeMicroOp::OpBinaryRI ||
        inst->op == ScbeMicroOp::OpBinaryRR ||
        inst->op == ScbeMicroOp::OpBinaryMI ||
        inst->op == ScbeMicroOp::OpBinaryRM ||
        inst->op == ScbeMicroOp::OpBinaryMR)
    {
        if (inst->cpuOp == CpuOp::MUL ||
            inst->cpuOp == CpuOp::DIV ||
            inst->cpuOp == CpuOp::MOD ||
            inst->cpuOp == CpuOp::IDIV ||
            inst->cpuOp == CpuOp::IMOD)
        {
            result.add(x64Reg2CpuReg(X64Reg::Rdx));
        }
    }

    return result;
}
