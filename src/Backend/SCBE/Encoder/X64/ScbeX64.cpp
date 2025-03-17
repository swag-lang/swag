// ReSharper disable CommentTypo
#include "pch.h"
#include "Backend/SCBE/Encoder/X64/ScbeX64.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicro.h"
#include "Backend/SCBE/Encoder/Micro/ScbeMicroInstruction.h"
#include "Core/Math.h"
#include "Semantic/Type/TypeManager.h"
#include "Wmf/Module.h"
#pragma optimize("", off)
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

constexpr uint8_t MODRM_RM_SID = 0b100;
constexpr uint8_t MODRM_RM_RIP = 0b101;

enum class X64Reg : uint8_t
{
    Rax  = 0b0000,
    Rbx  = 0b0011,
    Rcx  = 0b0001,
    Rdx  = 0b0010,
    Rsp  = 0b0100,
    Rbp  = 0b0101,
    Rsi  = 0b0110,
    Rdi  = 0b0111,
    R8   = 0b1000,
    R9   = 0b1001,
    R10  = 0b1010,
    R11  = 0b1011,
    R12  = 0b1100,
    R13  = 0b1101,
    R14  = 0b1110,
    R15  = 0b1111,
    Xmm0 = 0b0000,
    Xmm1 = 0b0001,
    Xmm2 = 0b0010,
    Xmm3 = 0b0011,
    Rip  = 0b10000
};

namespace
{
    X64Reg getReg(CpuReg reg)
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
                return static_cast<uint8_t>(getReg(reg));
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
    uint8_t getModRM(ModRMMode mod, uint8_t reg, uint8_t rm)
    {
        const auto result = static_cast<uint32_t>(mod) << 6 | ((reg & 0b111) << 3) | (rm & 0b111);
        return static_cast<uint8_t>(result);
    }

    uint8_t getModRM(ModRMMode mod, CpuReg reg, uint8_t rm)
    {
        return getModRM(mod, encodeReg(reg), rm);
    }

    // Scaled index addressing
    uint8_t getSid(uint8_t scale, CpuReg regIndex, CpuReg regBase)
    {
        const auto result = static_cast<uint32_t>(scale) << 6 | ((encodeReg(regIndex) & 0b111) << 3) | (encodeReg(regBase) & 0b111);
        return static_cast<uint8_t>(result);
    }

    void emitREX(Concat& concat, OpBits opBits, CpuReg reg0 = REX_REG_NONE, CpuReg reg1 = REX_REG_NONE)
    {
        if (opBits == OpBits::B16 || opBits == OpBits::F64)
            concat.addU8(0x66);
        const bool b1 = reg0 >= CpuReg::R8 && reg0 <= CpuReg::R15;
        const bool b2 = reg1 >= CpuReg::R8 && reg1 <= CpuReg::R15;
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

    void emitModRM(Concat& concat, ModRMMode mod, CpuReg reg, uint8_t rm)
    {
        concat.addU8(getModRM(mod, reg, rm));
    }

    void emitModRM(Concat& concat, CpuReg reg, CpuReg memReg)
    {
        emitModRM(concat, ModRMMode::Register, reg, encodeReg(memReg));
    }

    void emitModRM(Concat& concat, uint64_t memOffset, CpuReg reg, CpuReg memReg, uint8_t op = 1)
    {
        if (memOffset == 0)
        {
            // mov al, byte ptr [rdi]
            concat.addU8(getModRM(ModRMMode::Memory, reg, encodeReg(memReg)) | op - 1);
            if (memReg == CpuReg::Rsp)
                concat.addU8(0x24);
        }
        else if (memOffset <= 0x7F)
        {
            // mov al, byte ptr [rdi + ??]
            concat.addU8(getModRM(ModRMMode::Displacement8, reg, encodeReg(memReg)) | op - 1);
            if (memReg == CpuReg::Rsp)
                concat.addU8(0x24);
            emitValue(concat, memOffset, OpBits::B8);
        }
        else
        {
            // mov al, byte ptr [rdi + ????????]
            concat.addU8(getModRM(ModRMMode::Displacement32, reg, encodeReg(memReg)) | op - 1);
            if (memReg == CpuReg::Rsp)
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

    void emitCPUOp(Concat& concat, CpuOp op, CpuReg reg)
    {
        concat.addU8(static_cast<uint8_t>(op) | (encodeReg(reg) & 0b111));
    }

    void emitSpecCPUOp(Concat& concat, CpuOp op, OpBits opBits)
    {
        emitSpecB8(concat, static_cast<uint8_t>(op), opBits);
    }

    void emitSpecCPUOp(Concat& concat, uint8_t op, OpBits opBits)
    {
        emitSpecB8(concat, op, opBits);
    }
}

/////////////////////////////////////////////////////////////////////

void ScbeX64::emitSymbolRelocationRef(const Utf8& name)
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
}

void ScbeX64::emitSymbolRelocationAddress(CpuReg reg, uint32_t symbolIndex, uint32_t offset)
{
    emitREX(concat, OpBits::B64, reg);
    emitCPUOp(concat, 0x8D); // LEA
    emitModRM(concat, ModRMMode::Memory, reg, MODRM_RM_RIP);
    addSymbolRelocation(concat.totalCount() - textSectionOffset, symbolIndex, IMAGE_REL_AMD64_REL32);
    concat.addU32(offset);
}

void ScbeX64::emitSymbolRelocationValue(CpuReg reg, uint32_t symbolIndex, uint32_t offset)
{
    emitREX(concat, OpBits::B64, reg);
    emitCPUOp(concat, 0x8B); // MOV
    emitModRM(concat, ModRMMode::Memory, reg, MODRM_RM_RIP);
    addSymbolRelocation(concat.totalCount() - textSectionOffset, symbolIndex, IMAGE_REL_AMD64_REL32);
    concat.addU32(offset);
}

void ScbeX64::emitSymbolGlobalString(CpuReg reg, const Utf8& str)
{
    emitLoadRI64(reg, 0);
    const auto sym = getOrCreateGlobalString(str);
    addSymbolRelocation(concat.totalCount() - 8 - textSectionOffset, sym->index, IMAGE_REL_AMD64_ADDR64);
}

/////////////////////////////////////////////////////////////////////

void ScbeX64::emitPush(CpuReg reg)
{
    emitREX(concat, OpBits::Zero, REX_REG_NONE, reg);
    emitCPUOp(concat, 0x50, reg);
}

void ScbeX64::emitPop(CpuReg reg)
{
    emitREX(concat, OpBits::Zero, REX_REG_NONE, reg);
    emitCPUOp(concat, 0x58, reg);
}

void ScbeX64::emitRet()
{
    emitCPUOp(concat, 0xC3);
}

/////////////////////////////////////////////////////////////////////

void ScbeX64::emitLoadRR(CpuReg regDst, CpuReg regSrc, OpBits opBits)
{
    if (isFloat(opBits) && isFloat(regSrc))
    {
        emitSpecF64(concat, 0xF3, opBits);
        concat.addU8(0x0F);
        concat.addU8(0x10);
        emitModRM(concat, regDst, regSrc);
    }
    else if (isFloat(opBits))
    {
        emitREX(concat, OpBits::F64);
        emitREX(concat, opBits == OpBits::F64 ? OpBits::B64 : OpBits::B32);
        concat.addU8(0x0F);
        emitCPUOp(concat, CpuOp::MOVD);
        emitModRM(concat, regDst, regSrc);
    }
    else
    {
        emitREX(concat, opBits, regSrc, regDst);
        emitSpecCPUOp(concat, CpuOp::MOV, opBits);
        emitModRM(concat, regSrc, regDst);
    }
}

void ScbeX64::emitLoadRI64(CpuReg reg, uint64_t value)
{
    emitREX(concat, OpBits::B64, REX_REG_NONE, reg);
    emitCPUOp(concat, 0xB8, reg);
    concat.addU64(value);
}

void ScbeX64::emitLoadRI(CpuReg reg, uint64_t value, OpBits opBits)
{
    if (value == 0)
    {
        emitClearR(reg, opBits);
    }
    else if (opBits == OpBits::F32)
    {
        emitLoadRI(cc->computeRegI0, value, OpBits::B32);
        emitLoadRR(reg, cc->computeRegI0, OpBits::F32);
    }
    else if (opBits == OpBits::F64)
    {
        emitLoadRI(cc->computeRegI0, value, OpBits::B64);
        emitLoadRR(reg, cc->computeRegI0, OpBits::F64);
    }
    else if (opBits == OpBits::B64)
    {
        if (value <= 0x7FFFFFFF && reg < CpuReg::R8)
        {
            emitCPUOp(concat, 0xB8, reg);
            emitValue(concat, value, OpBits::B32);
        }
        else if (value <= 0x7FFFFFFF)
        {
            emitREX(concat, OpBits::B64, REX_REG_NONE, reg);
            emitCPUOp(concat, 0xC7);
            emitCPUOp(concat, 0xC0, reg);
            emitValue(concat, value, OpBits::B32);
        }
        else
        {
            emitREX(concat, OpBits::B64, REX_REG_NONE, reg);
            emitCPUOp(concat, 0xB8, reg);
            emitValue(concat, value, OpBits::B64);
        }
    }
    else if (opBits == OpBits::B8)
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
}

void ScbeX64::emitLoadRM(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits opBits)
{
    if (memOffset > 0x7FFFFFFF)
    {
        SWAG_ASSERT(memReg != cc->computeRegI1);
        emitLoadRI(cc->computeRegI1, memOffset, OpBits::B64);
        emitOpBinaryRR(memReg, cc->computeRegI1, CpuOp::ADD, OpBits::B64);
        memOffset = 0;
    }

    if (isFloat(opBits))
    {
        emitSpecF64(concat, 0xF3, opBits);
        emitREX(concat, OpBits::Zero, reg, memReg);
        concat.addU8(0x0F);
        concat.addU8(0x10);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else
    {
        emitREX(concat, opBits, reg, memReg);
        emitSpecCPUOp(concat, 0x8B, opBits);
        emitModRM(concat, memOffset, reg, memReg);
    }
}

void ScbeX64::emitLoadSignedExtendRM(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    if (numBitsSrc == numBitsDst)
    {
        emitLoadRM(reg, memReg, memOffset, numBitsSrc);
        return;
    }

    if (memOffset > 0x7FFFFFFF)
    {
        SWAG_ASSERT(memReg != cc->computeRegI1);
        emitLoadRI(cc->computeRegI1, memOffset, OpBits::B64);
        emitOpBinaryRR(memReg, cc->computeRegI1, CpuOp::ADD, OpBits::B64);
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

void ScbeX64::emitLoadZeroExtendRM(CpuReg reg, CpuReg memReg, uint64_t memOffset, OpBits numBitsDst, OpBits numBitsSrc)
{
    if (numBitsSrc == numBitsDst)
    {
        emitLoadRM(reg, memReg, memOffset, numBitsSrc);
        return;
    }

    if (memOffset > 0x7FFFFFFF)
    {
        SWAG_ASSERT(memReg != cc->computeRegI1);
        emitLoadRI(cc->computeRegI1, memOffset, OpBits::B64);
        emitOpBinaryRR(memReg, cc->computeRegI1, CpuOp::ADD, OpBits::B64);
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
        emitLoadRM(reg, memReg, memOffset, numBitsSrc);
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void ScbeX64::emitLoadZeroExtendRR(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    if (numBitsSrc == OpBits::B8 && (numBitsDst == OpBits::B32 || numBitsDst == OpBits::B64))
    {
        emitREX(concat, numBitsDst, regDst, regSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0xB6);
        emitModRM(concat, regDst, regSrc);
    }
    else if (numBitsSrc == OpBits::B16 && numBitsDst == OpBits::B64)
    {
        emitREX(concat, OpBits::B64, regDst, regSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0xB7);
        emitModRM(concat, regDst, regSrc);
    }
    else if (numBitsSrc == OpBits::B32 && numBitsDst == OpBits::B64)
    {
        emitLoadRR(regDst, regSrc, numBitsSrc);
    }
    else if (numBitsSrc == OpBits::B64 && numBitsDst == OpBits::F64)
    {
        SWAG_ASSERT(regSrc == cc->computeRegI0 && regDst == cc->computeRegF0);
        emitLoadRR(cc->computeRegF1, cc->computeRegI0, OpBits::F64);
        emitSymbolRelocationAddress(cc->computeRegI1, symCst_U64F64, 0);

        // punpckldq xmm1, xmmword ptr [rcx]
        emitREX(concat, OpBits::F64, cc->computeRegF1, cc->computeRegI1);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0x62);
        emitModRM(concat, 0, cc->computeRegF1, cc->computeRegI1);

        // subpd xmm1, xmmword ptr [rcx + 16]
        emitREX(concat, OpBits::F64, cc->computeRegF1, cc->computeRegI1);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0x5C);
        emitModRM(concat, 16, cc->computeRegF1, cc->computeRegI1);

        // movapd xmm0, xmm1
        emitREX(concat, OpBits::F64, cc->computeRegF0, cc->computeRegF1);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0x28);
        emitModRM(concat, cc->computeRegF0, cc->computeRegF1);

        // unpckhpd xmm0, xmm1
        emitREX(concat, OpBits::F64, cc->computeRegF0, cc->computeRegF1);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0x15);
        emitModRM(concat, cc->computeRegF0, cc->computeRegF1);

        emitOpBinaryRR(cc->computeRegF0, cc->computeRegF1, CpuOp::FADD, OpBits::F64);
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void ScbeX64::emitLoadSignedExtendRR(CpuReg regDst, CpuReg regSrc, OpBits numBitsDst, OpBits numBitsSrc)
{
    if (numBitsDst == OpBits::B32 && numBitsSrc == OpBits::B8)
    {
        emitREX(concat, OpBits::Zero, regDst, regSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0xBE);
        emitModRM(concat, regDst, regSrc);
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

/////////////////////////////////////////////////////////////////////

void ScbeX64::emitLoadAddressM(CpuReg reg, CpuReg memReg, uint64_t memOffset)
{
    if (memReg == CpuReg::Rip)
    {
        SWAG_ASSERT(memOffset == 0);
        emitREX(concat, OpBits::B64, reg, memReg);
        emitCPUOp(concat, 0x8D);
        emitModRM(concat, ModRMMode::Memory, reg, MODRM_RM_RIP);
    }
    else if (memOffset == 0)
    {
        emitLoadRR(reg, memReg, OpBits::B64);
    }
    else
    {
        emitREX(concat, OpBits::B64, reg, memReg);
        emitCPUOp(concat, 0x8D);
        emitModRM(concat, memOffset, reg, memReg);
    }
}

void ScbeX64::emitLoadAddressAddMul(CpuReg regDst, CpuReg regSrc1, CpuReg regSrc2, uint64_t mulValue, OpBits opBits)
{
    SWAG_ASSERT(regDst == cc->computeRegI0);
    SWAG_ASSERT(regSrc1 == cc->computeRegI0);
    SWAG_ASSERT(regSrc2 == cc->computeRegI0);
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

void ScbeX64::emitStoreMR(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits)
{
    if (isFloat(opBits))
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
}

void ScbeX64::emitStoreMI(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    if (opBits == OpBits::B64 && value > 0x7FFFFFFF && value >> 32 != 0xFFFFFFFF)
    {
        SWAG_ASSERT(memReg != cc->computeRegI1);
        emitLoadRI(cc->computeRegI1, value, OpBits::B64);
        emitStoreMR(memReg, memOffset, cc->computeRegI1, OpBits::B64);
    }
    else
    {
        emitREX(concat, opBits, REX_REG_NONE, memReg);
        emitSpecB8(concat, 0xC7, opBits);
        emitModRM(concat, memOffset, MODRM_REG_0, memReg);
        emitValue(concat, value, std::min(opBits, OpBits::B32));
    }
}

/////////////////////////////////////////////////////////////////////

void ScbeX64::emitClearR(CpuReg reg, OpBits opBits)
{
    if (isFloat(opBits))
    {
        SWAG_ASSERT(reg == cc->computeRegF0 || reg == cc->computeRegF1);
        emitREX(concat, opBits);
        concat.addU8(0x0F);
        emitCPUOp(concat, CpuOp::FXOR);
        emitModRM(concat, reg, reg);
    }
    else
    {
        emitREX(concat, opBits, reg, reg);
        emitSpecCPUOp(concat, CpuOp::XOR, opBits);
        emitModRM(concat, reg, reg);
    }
}

/////////////////////////////////////////////////////////////////////

void ScbeX64::emitSetCC(CpuReg reg, CpuCondFlag setType)
{
    switch (setType)
    {
        case CpuCondFlag::A:
            emitREX(concat, OpBits::B8, REX_REG_NONE, reg);
            concat.addU8(0x0F);
            concat.addU8(0x97);
            emitCPUOp(concat, 0xC0, reg);
            break;

        case CpuCondFlag::O:
            emitREX(concat, OpBits::B8, REX_REG_NONE, reg);
            concat.addU8(0x0F);
            concat.addU8(0x90);
            emitCPUOp(concat, 0xC0, reg);
            break;

        case CpuCondFlag::AE:
            emitREX(concat, OpBits::B8, REX_REG_NONE, reg);
            concat.addU8(0x0F);
            concat.addU8(0x93);
            emitCPUOp(concat, 0xC0, reg);
            break;

        case CpuCondFlag::G:
            emitREX(concat, OpBits::B8, REX_REG_NONE, reg);
            concat.addU8(0x0F);
            concat.addU8(0x9F);
            emitCPUOp(concat, 0xC0, reg);
            break;

        case CpuCondFlag::NE:
            SWAG_ASSERT(reg == cc->computeRegI0);
            concat.addU8(0x0F);
            concat.addU8(0x95);
            concat.addU8(0xC0);
            break;

        case CpuCondFlag::NA:
            SWAG_ASSERT(reg == cc->computeRegI0);
            concat.addU8(0x0F);
            concat.addU8(0x96);
            concat.addU8(0xC0);
            break;

        case CpuCondFlag::B:
            SWAG_ASSERT(reg == cc->computeRegI0);
            concat.addU8(0x0F);
            concat.addU8(0x92);
            concat.addU8(0xC0);
            break;

        case CpuCondFlag::BE:
            SWAG_ASSERT(reg == cc->computeRegI0);
            concat.addU8(0x0F);
            concat.addU8(0x96);
            concat.addU8(0xC0);
            break;

        case CpuCondFlag::E:
            SWAG_ASSERT(reg == cc->computeRegI0);
            concat.addU8(0x0F);
            concat.addU8(0x94);
            concat.addU8(0xC0);
            break;

        case CpuCondFlag::GE:
            SWAG_ASSERT(reg == cc->computeRegI0);
            concat.addU8(0x0F);
            concat.addU8(0x9D);
            concat.addU8(0xC0);
            break;

        case CpuCondFlag::L:
            SWAG_ASSERT(reg == cc->computeRegI0);
            concat.addU8(0x0F);
            concat.addU8(0x9C);
            concat.addU8(0xC0);
            break;

        case CpuCondFlag::LE:
            SWAG_ASSERT(reg == cc->computeRegI0);
            concat.addU8(0x0F);
            concat.addU8(0x9E);
            concat.addU8(0xC0);
            break;

        case CpuCondFlag::EP:
            SWAG_ASSERT(reg == cc->computeRegI0);

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

        case CpuCondFlag::NEP:
            SWAG_ASSERT(reg == cc->computeRegI0);

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

        default:
            SWAG_ASSERT(false);
            break;
    }
}

/////////////////////////////////////////////////////////////////////

void ScbeX64::emitCmpRR(CpuReg reg0, CpuReg reg1, OpBits opBits)
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
        emitSpecCPUOp(concat, CpuOp::CMP, opBits);
        concat.addU8(getModRM(ModRMMode::Register, reg1, encodeReg(reg0)));
    }
}

void ScbeX64::emitCmpRI(CpuReg reg, uint64_t value, OpBits opBits)
{
    SWAG_ASSERT(ScbeCpu::isInt(opBits));
    maskValue(value, opBits);

    if (opBits == OpBits::B8)
    {
        if (getReg(reg) == X64Reg::Rax)
            emitCPUOp(concat, 0x3C);
        else
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, MODRM_REG_7, reg);
        }
        emitValue(concat, value, OpBits::B8);
    }
    else if (value <= 0x7F ||
             (opBits == OpBits::B16 && value >= 0xFF80) ||
             (opBits == OpBits::B32 && value >= 0xFFFFFF80) ||
             (opBits == OpBits::B64 && value >= 0xFFFFFFFFFFFFFF80))
    {
        emitREX(concat, opBits, REX_REG_NONE, reg);
        emitCPUOp(concat, 0x83);
        emitModRM(concat, MODRM_REG_7, reg);
        emitValue(concat, value, OpBits::B8);
    }
    else if ((opBits == OpBits::B16 && value <= 0x7FFF) ||
             (opBits == OpBits::B32 && value <= 0x7FFFFFFF))
    {
        if (getReg(reg) == X64Reg::Rax)
            emitCPUOp(concat, 0x3d);
        else
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, MODRM_REG_7, reg);
        }
        emitValue(concat, value, opBits);
    }
    else
    {
        emitLoadRI(cc->computeRegI1, value, opBits);
        emitCmpRR(reg, cc->computeRegI1, opBits);
    }
}

void ScbeX64::emitCmpMR(CpuReg memReg, uint64_t memOffset, CpuReg reg, OpBits opBits)
{
    if (isFloat(opBits))
    {
        emitREX(concat, opBits);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0x2F);
        emitModRM(concat, memOffset, reg, memReg);
    }
    else
    {
        emitREX(concat, opBits, reg, memReg);
        emitSpecCPUOp(concat, 0x3B, opBits);
        emitModRM(concat, memOffset, reg, memReg);
    }
}

void ScbeX64::emitCmpMI(CpuReg memReg, uint64_t memOffset, uint64_t value, OpBits opBits)
{
    SWAG_ASSERT(ScbeCpu::isInt(opBits));
    maskValue(value, opBits);

    if (opBits == OpBits::B8)
    {
        emitREX(concat, opBits, REX_REG_NONE, memReg);
        emitCPUOp(concat, 0x80);
        emitModRM(concat, memOffset, MODRM_REG_7, memReg);
        emitValue(concat, value, OpBits::B8);
    }
    else if (value <= 0x7F ||
             (opBits == OpBits::B16 && value >= 0xFF80) ||
             (opBits == OpBits::B32 && value >= 0xFFFFFF80) ||
             (opBits == OpBits::B64 && value >= 0xFFFFFFFFFFFFFF80))
    {
        emitREX(concat, opBits, REX_REG_NONE, memReg);
        emitCPUOp(concat, 0x83);
        emitModRM(concat, memOffset, MODRM_REG_7, memReg);
        emitValue(concat, value, OpBits::B8);
    }
    else if (value <= 0x7FFFFFFF)
    {
        emitREX(concat, opBits, REX_REG_NONE, memReg);
        emitCPUOp(concat, 0x81);
        emitModRM(concat, memOffset, MODRM_REG_7, memReg);
        emitValue(concat, value, opBits == OpBits::B16 ? opBits : OpBits::B32);
    }
    else
    {
        emitLoadRM(cc->computeRegI0, memReg, memOffset, opBits);
        emitCmpRI(cc->computeRegI0, value, opBits);
    }
}

/////////////////////////////////////////////////////////////////////

void ScbeX64::emitOpUnaryM(CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits)
{
    if (op == CpuOp::NOT)
    {
        SWAG_ASSERT(memReg == CpuReg::Rsp);
        emitREX(concat, opBits);
        emitSpecCPUOp(concat, 0xF7, opBits);
        emitModRM(concat, memOffset, MODRM_REG_2, memReg);
    }
    else if (op == CpuOp::NEG)
    {
        SWAG_ASSERT(memReg == CpuReg::Rsp);
        emitREX(concat, opBits);
        emitSpecCPUOp(concat, 0xF7, opBits);
        emitModRM(concat, memOffset, MODRM_REG_3, memReg);
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void ScbeX64::emitOpUnaryR(CpuReg reg, CpuOp op, OpBits opBits)
{
    if (op == CpuOp::NOT)
    {
        emitREX(concat, opBits, REX_REG_NONE, reg);
        emitSpecCPUOp(concat, 0xF7, opBits);
        emitModRM(concat, MODRM_REG_2, reg);
    }
    else if (op == CpuOp::NEG)
    {
        if (isFloat(opBits))
        {
            SWAG_ASSERT(reg == cc->computeRegF0);
            emitStoreMI(CpuReg::Rsp, cpuFct->getStackOffsetFLT(), opBits == OpBits::F32 ? 0x80000000 : 0x80000000'00000000, OpBits::B64);
            emitLoadRM(cc->computeRegF1, CpuReg::Rsp, cpuFct->getStackOffsetFLT(), opBits);
            emitOpBinaryRR(cc->computeRegF0, cc->computeRegF1, CpuOp::FXOR, opBits);
        }
        else
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitSpecCPUOp(concat, 0xF7, opBits);
            emitModRM(concat, MODRM_REG_3, reg);
        }
    }
    else if (op == CpuOp::BSWAP)
    {
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
    else
    {
        SWAG_ASSERT(false);
    }
}

void ScbeX64::emitOpBinaryRM(CpuReg regDst, CpuReg memReg, uint64_t memOffset, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    SWAG_ASSERT(regDst == cc->computeRegI0);
    if (op == CpuOp::ADD)
    {
        emitREX(concat, opBits, regDst, memReg);
        emitCPUOp(concat, 0x03);
        emitModRM(concat, memOffset, regDst, memReg);
    }
    else
    {
        emitLoadRM(cc->computeRegI1, memReg, memOffset, opBits);
        emitOpBinaryRR(regDst, cc->computeRegI1, op, opBits);
    }
}

void ScbeX64::emitOpBinaryRR(CpuReg regDst, CpuReg regSrc, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (opBits == OpBits::F32)
    {
        if (op != CpuOp::FSQRT &&
            op != CpuOp::FAND &&
            op != CpuOp::UCOMIF &&
            op != CpuOp::FXOR)
        {
            emitCPUOp(concat, 0xF3);
            emitREX(concat, emitFlags.has(EMITF_B64) ? OpBits::B64 : OpBits::B32, regSrc, regDst);
        }

        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, op);
        concat.addU8(static_cast<uint8_t>(0xC0 | encodeReg(regSrc) | encodeReg(regDst) << 3));
    }
    else if (opBits == OpBits::F64)
    {
        if (op != CpuOp::FSQRT &&
            op != CpuOp::FAND &&
            op != CpuOp::UCOMIF &&
            op != CpuOp::FXOR)
        {
            emitCPUOp(concat, 0xF2);
            emitREX(concat, emitFlags.has(EMITF_B64) ? OpBits::B64 : OpBits::B32, regSrc, regDst);
        }
        else
        {
            concat.addU8(0x66);
        }

        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, op);
        concat.addU8(static_cast<uint8_t>(0xC0 | encodeReg(regSrc) | encodeReg(regDst) << 3));
    }
    else if (op == CpuOp::DIV ||
             op == CpuOp::IDIV ||
             op == CpuOp::MOD ||
             op == CpuOp::IMOD)
    {
        SWAG_ASSERT(regDst == cc->computeRegI0);
        SWAG_ASSERT(regSrc == cc->computeRegI1);
        SWAG_ASSERT(getReg(regDst) == X64Reg::Rax);
        SWAG_ASSERT(getReg(regSrc) != X64Reg::Rdx);
        if (opBits == OpBits::B8 && (op == CpuOp::IDIV || op == CpuOp::IMOD))
            emitLoadSignedExtendRR(regDst, regDst, OpBits::B32, OpBits::B8);
        else if (opBits == OpBits::B8)
            emitLoadZeroExtendRR(regDst, regDst, OpBits::B32, OpBits::B8);
        else if (op == CpuOp::IDIV || op == CpuOp::IMOD)
        {
            // cdq
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x99);
        }
        else
        {
            emitClearR(CpuReg::Rdx, opBits);
        }

        emitREX(concat, opBits, regDst, regSrc);
        emitSpecCPUOp(concat, 0xF7, opBits);
        if (op == CpuOp::DIV || op == CpuOp::MOD)
            emitModRM(concat, MODRM_REG_6, regSrc);
        else if (op == CpuOp::IDIV || op == CpuOp::IMOD)
            emitModRM(concat, MODRM_REG_7, regSrc);
        else
            SWAG_ASSERT(false);
        if ((op == CpuOp::MOD || op == CpuOp::IMOD) && opBits == OpBits::B8)
            emitOpBinaryRI(regDst, 8, CpuOp::SHR, OpBits::B32); // AH => AL
        else if (op == CpuOp::MOD || op == CpuOp::IMOD)
            emitLoadRR(regDst, CpuReg::Rdx, opBits);
    }
    else if (op == CpuOp::MUL ||
             op == CpuOp::IMUL)
    {
        SWAG_ASSERT(getReg(regDst) == X64Reg::Rax);
        emitREX(concat, opBits, regDst, regSrc);
        emitSpecCPUOp(concat, 0xF7, opBits);
        if (op == CpuOp::MUL)
            emitModRM(concat, MODRM_REG_4, regSrc);
        else if (op == CpuOp::IMUL)
            emitModRM(concat, MODRM_REG_5, regSrc);
        else
            SWAG_ASSERT(false);
    }
    else if (op == CpuOp::ROL ||
             op == CpuOp::ROR ||
             op == CpuOp::SAL ||
             op == CpuOp::SAR ||
             op == CpuOp::SHL ||
             op == CpuOp::SHR)
    {
        if (regSrc != CpuReg::Rcx)
            emitLoadRR(CpuReg::Rcx, regSrc, opBits);
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
        else
            SWAG_ASSERT(false);
    }
    else if (op == CpuOp::ADD ||
             op == CpuOp::SUB ||
             op == CpuOp::XOR ||
             op == CpuOp::AND ||
             op == CpuOp::OR)
    {
        emitREX(concat, opBits, regSrc, regDst);
        emitSpecCPUOp(concat, op, opBits);
        emitModRM(concat, regSrc, regDst);
    }
    else if (op == CpuOp::CMOVB ||
             op == CpuOp::CMOVE ||
             op == CpuOp::CMOVG ||
             op == CpuOp::CMOVL ||
             op == CpuOp::CMOVBE ||
             op == CpuOp::CMOVGE)
    {
        opBits = std::max(opBits, OpBits::B32);
        emitREX(concat, opBits, regDst, regSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, op);
        emitModRM(concat, regDst, regSrc);
    }
    else if (op == CpuOp::BSF ||
             op == CpuOp::BSR)
    {
        emitREX(concat, opBits, regDst, regSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, op == CpuOp::BSF ? 0xBC : 0xBD);
        emitModRM(concat, regDst, regSrc);
    }
    else if (op == CpuOp::POPCNT)
    {
        concat.addU8(0xF3);
        emitREX(concat, opBits, regDst, regSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, op);
        emitModRM(concat, regDst, regSrc);
    }
    else if (op == CpuOp::CMPXCHG)
    {
        // SWAG_ASSERT(regDst == cc->computeRegI1 && regSrc == CpuReg::Rdx);
        //  lock CMPXCHG [rcx], dl
        if (opBits == OpBits::B16)
            emitREX(concat, opBits);
        concat.addU8(0xF0);
        emitREX(concat, opBits, regSrc, regDst);
        emitCPUOp(concat, 0x0F);
        emitSpecCPUOp(concat, 0xB1, opBits);
        emitModRM(concat, 0, regSrc, regDst);
    }
    else
    {
        SWAG_ASSERT(false);
    }
}

void ScbeX64::emitOpBinaryMR(CpuReg memReg, uint64_t memOffset, CpuReg reg, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (isFloat(opBits))
    {
        SWAG_ASSERT(reg == cc->computeRegF1);
        emitLoadRM(cc->computeRegF0, memReg, memOffset, opBits);
        emitOpBinaryRR(cc->computeRegF0, reg, op, opBits, emitFlags);
        emitStoreMR(memReg, memOffset, cc->computeRegF0, opBits);
    }
    else if (op == CpuOp::IMUL ||
             op == CpuOp::MUL)
    {
        SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
        SWAG_ASSERT(reg == cc->computeRegI1);
        if (memReg == cc->computeRegI0)
        {
            emitLoadRR(cc->computeRegI2, memReg, OpBits::B64);
            memReg = cc->computeRegI2;
        }
        emitLoadRM(cc->computeRegI0, memReg, memOffset, opBits);
        emitOpBinaryRR(cc->computeRegI0, reg, op, opBits, emitFlags);
        emitStoreMR(memReg, memOffset, cc->computeRegI0, opBits);
    }
    else if (op == CpuOp::DIV ||
             op == CpuOp::IDIV ||
             op == CpuOp::MOD ||
             op == CpuOp::IMOD)
    {
        SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
        SWAG_ASSERT(reg == cc->computeRegI1);
        if (memReg == cc->computeRegI0)
        {
            emitLoadRR(cc->computeRegI2, memReg, OpBits::B64);
            memReg = cc->computeRegI2;
        }
        emitLoadRM(cc->computeRegI0, memReg, memOffset, opBits);
        emitOpBinaryRR(cc->computeRegI0, reg, op, opBits, emitFlags);
        emitStoreMR(memReg, memOffset, cc->computeRegI0, opBits);
    }
    else if (op == CpuOp::SAR ||
             op == CpuOp::SHR ||
             op == CpuOp::SHL)
    {
        if (reg != CpuReg::Rcx)
            emitLoadRR(CpuReg::Rcx, reg, opBits);
        if (emitFlags.has(EMITF_Lock))
            concat.addU8(0xF0);
        emitREX(concat, opBits, REX_REG_NONE, memReg);
        emitSpecCPUOp(concat, 0xD3, opBits);
        if (op == CpuOp::SHL)
            emitModRM(concat, memOffset, MODRM_REG_4, memReg);
        else if (op == CpuOp::SAR)
            emitModRM(concat, memOffset, MODRM_REG_7, memReg);
        else if (op == CpuOp::SHR)
            emitModRM(concat, memOffset, MODRM_REG_5, memReg);
        else
            SWAG_ASSERT(false);
    }
    else
    {
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

void ScbeX64::emitOpBinaryRI(CpuReg reg, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    if (isNoOp(value, op, opBits, emitFlags))
        return;

    ///////////////////////////////////////////

    if (value > 0x7FFFFFFF)
    {
        SWAG_ASSERT(reg == cc->computeRegI0);
        emitLoadRI(cc->computeRegI1, value, OpBits::B64);
        emitOpBinaryRR(reg, cc->computeRegI1, op, opBits, emitFlags);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::XOR)
    {
        if (opBits == OpBits::B8)
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, MODRM_REG_6, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, MODRM_REG_6, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, MODRM_REG_6, reg);
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::OR)
    {
        if (opBits == OpBits::B8)
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, MODRM_REG_1, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, MODRM_REG_1, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, MODRM_REG_1, reg);
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::AND)
    {
        if (opBits == OpBits::B8)
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, MODRM_REG_4, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, MODRM_REG_4, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, MODRM_REG_4, reg);
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::ADD)
    {
        if (value == 1 && !emitFlags.has(EMITF_Overflow) && optLevel >= BuildCfgBackendOptim::O1)
        {
            // inc
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitSpecCPUOp(concat, 0xFF, opBits);
            emitModRM(concat, MODRM_REG_0, reg);
        }
        else if (opBits == OpBits::B8)
        {
            SWAG_ASSERT(reg == cc->computeRegI0 || reg == cc->computeRegI1 || reg == CpuReg::Rsp);
            emitREX(concat, opBits, REX_REG_NONE, reg);
            if (getReg(reg) == X64Reg::Rax)
                concat.addU8(0x04);
            else
            {
                concat.addU8(0x80);
                emitCPUOp(concat, 0xC0, reg);
            }
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            concat.addU8(0x83);
            emitCPUOp(concat, 0xC0, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            if (getReg(reg) == X64Reg::Rax)
                concat.addU8(0x05);
            else
            {
                concat.addU8(0x81);
                emitCPUOp(concat, 0xC0, reg);
            }
            emitValue(concat, value, opBits == OpBits::B16 ? OpBits::B16 : OpBits::B32);
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::SUB)
    {
        if (value == 1 && !emitFlags.has(EMITF_Overflow) && optLevel >= BuildCfgBackendOptim::O1)
        {
            // dec
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitSpecCPUOp(concat, 0xFF, opBits);
            emitModRM(concat, MODRM_REG_1, reg);
        }
        else if (opBits == OpBits::B8)
        {
            SWAG_ASSERT(reg == cc->computeRegI0 || reg == cc->computeRegI1 || reg == CpuReg::Rsp);
            emitREX(concat, opBits);
            if (reg == cc->computeRegI0)
                concat.addU8(0x2C);
            else
            {
                concat.addU8(0x80);
                emitCPUOp(concat, 0xE8, reg);
            }
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(reg == cc->computeRegI0 || reg == cc->computeRegI1 || reg == CpuReg::Rsp);
            emitREX(concat, opBits);
            concat.addU8(0x83);
            emitCPUOp(concat, 0xE8, reg);
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(reg == cc->computeRegI0 || reg == cc->computeRegI1 || reg == CpuReg::Rsp);
            emitREX(concat, opBits);
            if (reg == cc->computeRegI0)
                concat.addU8(0x2D);
            else
            {
                concat.addU8(0x81);
                emitCPUOp(concat, 0xE8, reg);
            }
            emitValue(concat, value, opBits == OpBits::B16 ? OpBits::B16 : OpBits::B32);
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::MOD || op == CpuOp::IMOD)
    {
        if (Math::isPowerOfTwo(value) && optLevel >= BuildCfgBackendOptim::O1)
        {
            emitOpBinaryRI(reg, value - 1, CpuOp::AND, opBits, emitFlags);
        }
        else
        {
            SWAG_ASSERT(reg == cc->computeRegI0);
            emitLoadRI(cc->computeRegI1, value, opBits);
            emitOpBinaryRR(reg, cc->computeRegI1, op, opBits, emitFlags);
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::DIV || op == CpuOp::IDIV)
    {
        SWAG_ASSERT(getReg(reg) == X64Reg::Rax);
        if (opBits == OpBits::B8 && op == CpuOp::IDIV)
            emitLoadSignedExtendRR(reg, reg, OpBits::B32, OpBits::B8);
        else if (opBits == OpBits::B8)
            emitLoadZeroExtendRR(reg, reg, OpBits::B32, OpBits::B8);
        else if (op == CpuOp::IDIV)
        {
            // cdq
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x99);
        }
        else
        {
            emitClearR(CpuReg::Rdx, opBits);
        }

        if (op == CpuOp::DIV && Math::isPowerOfTwo(value) && optLevel >= BuildCfgBackendOptim::O1)
        {
            emitOpBinaryRI(reg, static_cast<uint32_t>(log2(value)), CpuOp::SHR, opBits, emitFlags);
        }
        else
        {
            SWAG_ASSERT(reg == cc->computeRegI0);
            SWAG_ASSERT(getReg(cc->computeRegI1) != X64Reg::Rdx);
            emitLoadRI(cc->computeRegI1, value, opBits);
            emitOpBinaryRR(reg, cc->computeRegI1, op, opBits, emitFlags);
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::MUL || op == CpuOp::IMUL)
    {
        uint32_t   factor1, factor2;
        const bool canFactorize = (opBits == OpBits::B32 || opBits == OpBits::B64) && optLevel >= BuildCfgBackendOptim::O1 && !emitFlags.has(EMITF_Overflow);
        if (value == 0 && optLevel >= BuildCfgBackendOptim::O1)
        {
            emitClearR(reg, opBits);
        }
        else if (value == 3 && canFactorize)
        {
            emitLoadAddressAddMul(reg, reg, reg, 2, opBits);
        }
        else if (value == 5 && canFactorize)
        {
            emitLoadAddressAddMul(reg, reg, reg, 4, opBits);
        }
        else if (value == 9 && canFactorize)
        {
            emitLoadAddressAddMul(reg, reg, reg, 8, opBits);
        }
        else if (Math::isPowerOfTwo(value) && optLevel >= BuildCfgBackendOptim::O1)
        {
            emitOpBinaryRI(reg, static_cast<uint32_t>(log2(value)), CpuOp::SHL, opBits, emitFlags);
        }
        else if (canFactorize && decomposeMul(static_cast<uint32_t>(value), factor1, factor2))
        {
            if (factor1 != 1)
                emitOpBinaryRI(reg, factor1, CpuOp::MUL, opBits, emitFlags);
            if (factor2 != 1)
                emitOpBinaryRI(reg, factor2, CpuOp::MUL, opBits, emitFlags);
        }
        else if (op == CpuOp::IMUL && opBits == OpBits::B8)
        {
            SWAG_ASSERT(reg == cc->computeRegI0);
            emitLoadRI(cc->computeRegI1, value, opBits);
            emitOpBinaryRR(reg, cc->computeRegI1, op, opBits, emitFlags);
        }
        else if (op == CpuOp::IMUL && value <= 0x7F)
        {
            SWAG_ASSERT(reg == cc->computeRegI0 || reg == cc->computeRegI1);
            emitREX(concat, opBits);
            concat.addU8(0x6B);
            concat.addU8(reg == cc->computeRegI0 ? 0xC0 : 0xC9);
            emitValue(concat, value, OpBits::B8);
        }
        else if (op == CpuOp::IMUL)
        {
            SWAG_ASSERT(reg == cc->computeRegI0 || reg == cc->computeRegI1);
            emitREX(concat, opBits);
            concat.addU8(0x69);
            concat.addU8(reg == cc->computeRegI0 ? 0xC0 : 0xC9);
            emitValue(concat, value, OpBits::B32);
        }
        else
        {
            SWAG_ASSERT(reg == cc->computeRegI0);
            emitLoadRI(cc->computeRegI1, value, opBits);
            emitOpBinaryRR(reg, cc->computeRegI1, op, opBits, emitFlags);
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::SAR || op == CpuOp::SHR || op == CpuOp::SHL)
    {
        if (op == CpuOp::SHL && value == 1 && optLevel >= BuildCfgBackendOptim::O1)
        {
            emitOpBinaryRR(reg, reg, CpuOp::ADD, opBits);
        }
        else if (value == 1)
        {
            emitREX(concat, opBits, reg, reg);
            emitSpecB8(concat, 0xD1, opBits);
            emitCPUOp(concat, op, reg);
        }
        else if (value <= 0x7F)
        {
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

    else if (op == CpuOp::BT)
    {
        if (value <= 0x7F)
        {
            emitREX(concat, opBits, REX_REG_NONE, reg);
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, op);
            emitModRM(concat, ModRMMode::Register, MODRM_REG_4, encodeReg(reg));
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

void ScbeX64::emitOpBinaryMI(CpuReg memReg, uint64_t memOffset, uint64_t value, CpuOp op, OpBits opBits, CpuEmitFlags emitFlags)
{
    SWAG_ASSERT(ScbeCpu::isInt(opBits));
    if (isNoOp(value, op, opBits, emitFlags))
        return;
    maskValue(value, opBits);

    ///////////////////////////////////////////

    if (value > 0x7FFFFFFF)
    {
        SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
        emitLoadRI(cc->computeRegI1, value, opBits);
        emitOpBinaryMR(memReg, memOffset, cc->computeRegI1, op, opBits, emitFlags);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::IMOD || op == CpuOp::MOD)
    {
        if (Math::isPowerOfTwo(value) && optLevel >= BuildCfgBackendOptim::O1)
        {
            emitOpBinaryMI(memReg, memOffset, value - 1, CpuOp::AND, opBits, emitFlags);
        }
        else
        {
            SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
            if (memReg == cc->computeRegI0)
            {
                emitLoadRR(cc->computeRegI2, memReg, OpBits::B64);
                memReg = cc->computeRegI2;
            }
            emitLoadRM(cc->computeRegI0, memReg, memOffset, opBits);
            emitOpBinaryRI(cc->computeRegI0, value, op, opBits, emitFlags);
            emitStoreMR(memReg, memOffset, cc->computeRegI0, opBits);
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::DIV)
    {
        if (Math::isPowerOfTwo(value) && optLevel >= BuildCfgBackendOptim::O1)
        {
            emitOpBinaryMI(memReg, memOffset, static_cast<uint32_t>(log2(value)), op == CpuOp::IDIV ? CpuOp::SAR : CpuOp::SHR, opBits, emitFlags);
        }
        else
        {
            SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
            if (memReg == cc->computeRegI0)
            {
                emitLoadRR(cc->computeRegI2, memReg, OpBits::B64);
                memReg = cc->computeRegI2;
            }
            emitLoadRM(cc->computeRegI0, memReg, memOffset, opBits);
            emitOpBinaryRI(cc->computeRegI0, value, op, opBits, emitFlags);
            emitStoreMR(memReg, memOffset, cc->computeRegI0, opBits);
        }
    }

    else if (op == CpuOp::IDIV)
    {
        SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
        if (memReg == cc->computeRegI0)
        {
            emitLoadRR(cc->computeRegI2, memReg, OpBits::B64);
            memReg = cc->computeRegI2;
        }
        emitLoadRM(cc->computeRegI0, memReg, memOffset, opBits);
        emitOpBinaryRI(cc->computeRegI0, value, op, opBits, emitFlags);
        emitStoreMR(memReg, memOffset, cc->computeRegI0, opBits);
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::IMUL || op == CpuOp::MUL)
    {
        SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
        if (Math::isPowerOfTwo(value) && optLevel >= BuildCfgBackendOptim::O1)
        {
            emitOpBinaryMI(memReg, memOffset, static_cast<uint32_t>(log2(value)), CpuOp::SHL, opBits, emitFlags);
        }
        else
        {
            if (memReg == cc->computeRegI0)
            {
                emitLoadRR(cc->computeRegI2, memReg, OpBits::B64);
                memReg = cc->computeRegI2;
            }
            emitLoadRM(cc->computeRegI0, memReg, memOffset, opBits);
            emitOpBinaryRI(cc->computeRegI0, value, op, opBits, emitFlags);
            emitStoreMR(memReg, memOffset, cc->computeRegI0, opBits);
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::SAR || op == CpuOp::SHR || op == CpuOp::SHL)
    {
        if (value == 1)
        {
            SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xD1, opBits);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, 1 + static_cast<uint8_t>(op) & 0x3F);
        }
        else
        {
            SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
            value = std::min(static_cast<uint32_t>(value), getNumBits(opBits) - 1);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xC1, opBits);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, 1 + static_cast<uint8_t>(op) & 0x3F);
            emitValue(concat, value, OpBits::B8);
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::ADD)
    {
        if (value == 1 && !emitFlags.has(EMITF_Overflow) && optLevel >= BuildCfgBackendOptim::O1)
        {
            SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xFF, opBits);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg);
        }
        else if (opBits == OpBits::B8)
        {
            SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::SUB)
    {
        if (value == 1 && !emitFlags.has(EMITF_Overflow) && optLevel >= BuildCfgBackendOptim::O1)
        {
            SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
            emitREX(concat, opBits);
            emitSpecB8(concat, 0xFF, opBits);
            emitModRM(concat, memOffset, MODRM_REG_1, memReg);
        }
        else if (opBits == OpBits::B8)
        {
            SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x81);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, std::min(opBits, OpBits::B32));
        }
    }

    ///////////////////////////////////////////

    else if (op == CpuOp::OR || op == CpuOp::AND || op == CpuOp::XOR)
    {
        if (opBits == OpBits::B8)
        {
            SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x80);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else if (value <= 0x7F)
        {
            SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
            emitREX(concat, opBits);
            emitCPUOp(concat, 0x83);
            emitModRM(concat, memOffset, MODRM_REG_0, memReg, static_cast<uint8_t>(op));
            emitValue(concat, value, OpBits::B8);
        }
        else
        {
            SWAG_ASSERT(memReg == cc->computeRegI0 || memReg == CpuReg::Rsp);
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
void ScbeX64::emitJumpTable(CpuReg table, CpuReg offset, int32_t currentIp, uint32_t offsetTable, uint32_t numEntries)
{
    SWAG_ASSERT(table == cc->computeRegI1 && offset == cc->computeRegI0);

    uint8_t*   addrConstant        = nullptr;
    const auto offsetTableConstant = buildParams.module->constantSegment.reserve(numEntries * sizeof(uint32_t), &addrConstant);
    emitSymbolRelocationAddress(table, symCSIndex, offsetTableConstant);

    // movsxd computeRegI1, dword ptr [computeRegI1 + computeRegI0*4]
    emitREX(concat, OpBits::B64, cc->computeRegI1, cc->computeRegI1);
    emitCPUOp(concat, 0x63);
    emitModRM(concat, ModRMMode::Memory, cc->computeRegI1, MODRM_RM_SID);
    concat.addU8(getSid(2, cc->computeRegI0, cc->computeRegI1));

    const auto startIdx = concat.totalCount();
    emitSymbolRelocationAddress(cc->computeRegI0, cpuFct->symbolIndex, concat.totalCount() - cpuFct->startAddress);
    const auto patchPtr = reinterpret_cast<uint32_t*>(concat.currentSP) - 1;
    emitOpBinaryRR(cc->computeRegI0, cc->computeRegI1, CpuOp::ADD, OpBits::B64);
    emitJumpM(cc->computeRegI0);
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
}

CpuJump ScbeX64::emitJump(CpuCondJump jumpType, OpBits opBits)
{
    SWAG_ASSERT(opBits == OpBits::B8 || opBits == OpBits::B32);

    if (opBits == OpBits::B8)
    {
        switch (jumpType)
        {
            case CpuCondJump::JNO:
                concat.addU8(0x71);
                break;
            case CpuCondJump::JB:
                concat.addU8(0x72);
                break;
            case CpuCondJump::JAE:
                concat.addU8(0x73);
                break;
            case CpuCondJump::JZ:
                concat.addU8(0x74);
                break;
            case CpuCondJump::JNZ:
                concat.addU8(0x75);
                break;
            case CpuCondJump::JBE:
                concat.addU8(0x76);
                break;
            case CpuCondJump::JA:
                concat.addU8(0x77);
                break;
            case CpuCondJump::JP:
                concat.addU8(0x7A);
                break;
            case CpuCondJump::JNP:
                concat.addU8(0x7B);
                break;
            case CpuCondJump::JL:
                concat.addU8(0x7C);
                break;
            case CpuCondJump::JGE:
                concat.addU8(0x7D);
                break;
            case CpuCondJump::JLE:
                concat.addU8(0x7E);
                break;
            case CpuCondJump::JG:
                concat.addU8(0x7F);
                break;
            case CpuCondJump::JUMP:
                concat.addU8(0xEB);
                break;
            default:
                SWAG_ASSERT(false);
                break;
        }

        concat.addU8(0);

        CpuJump jump;
        jump.addr   = concat.getSeekPtr() - 1;
        jump.offset = concat.totalCount();
        jump.opBits = opBits;
        return jump;
    }

    switch (jumpType)
    {
        case CpuCondJump::JNO:
            concat.addU8(0x0F);
            concat.addU8(0x81);
            break;
        case CpuCondJump::JB:
            concat.addU8(0x0F);
            concat.addU8(0x82);
            break;
        case CpuCondJump::JAE:
            concat.addU8(0x0F);
            concat.addU8(0x83);
            break;
        case CpuCondJump::JZ:
            concat.addU8(0x0F);
            concat.addU8(0x84);
            break;
        case CpuCondJump::JNZ:
            concat.addU8(0x0F);
            concat.addU8(0x85);
            break;
        case CpuCondJump::JBE:
            concat.addU8(0x0F);
            concat.addU8(0x86);
            break;
        case CpuCondJump::JA:
            concat.addU8(0x0F);
            concat.addU8(0x87);
            break;
        case CpuCondJump::JP:
            concat.addU8(0x0F);
            concat.addU8(0x8A);
            break;
        case CpuCondJump::JNP:
            concat.addU8(0x0F);
            concat.addU8(0x8B);
            break;
        case CpuCondJump::JL:
            concat.addU8(0x0F);
            concat.addU8(0x8C);
            break;
        case CpuCondJump::JGE:
            concat.addU8(0x0F);
            concat.addU8(0x8D);
            break;
        case CpuCondJump::JLE:
            concat.addU8(0x0F);
            concat.addU8(0x8E);
            break;
        case CpuCondJump::JG:
            concat.addU8(0x0F);
            concat.addU8(0x8F);
            break;
        case CpuCondJump::JUMP:
            concat.addU8(0xE9);
            break;
        default:
            SWAG_ASSERT(false);
            break;
    }

    concat.addU32(0);
    CpuJump jump;
    jump.addr   = concat.getSeekPtr() - sizeof(uint32_t);
    jump.offset = concat.totalCount();
    jump.opBits = opBits;
    return jump;
}

void ScbeX64::emitPatchJump(const CpuJump& jump, uint64_t offsetDestination)
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

void ScbeX64::emitPatchJump(const CpuJump& jump)
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

void ScbeX64::emitJumpM(CpuReg reg)
{
    SWAG_ASSERT(reg == cc->computeRegI0);
    emitREX(concat, OpBits::Zero, REX_REG_NONE, reg);
    emitCPUOp(concat, 0xFF);
    emitModRM(concat, ModRMMode::Register, MODRM_REG_4, encodeReg(reg));
}

/////////////////////////////////////////////////////////////////////

void ScbeX64::emitCopy(CpuReg memRegDst, CpuReg memRegSrc, uint32_t count)
{
    if (!count)
        return;

    uint32_t offset = 0;
    SWAG_ASSERT(memRegDst == cc->computeRegI0);
    SWAG_ASSERT(memRegSrc == cc->computeRegI1);

    // SSE 16 octets
    while (count >= 16)
    {
        // movups xmm0, [memRegSrc+??]
        emitREX(concat, OpBits::Zero, REX_REG_NONE, memRegSrc);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0x10);
        emitModRM(concat, offset, cc->computeRegF0, memRegSrc);

        // movups [memRegDst+??], xmm0
        emitREX(concat, OpBits::Zero, REX_REG_NONE, memRegDst);
        emitCPUOp(concat, 0x0F);
        emitCPUOp(concat, 0x11);
        emitModRM(concat, offset, cc->computeRegF0, memRegDst);

        count -= 16;
        offset += 16;
    }

    while (count >= 8)
    {
        emitLoadRM(cc->computeRegI2, memRegSrc, offset, OpBits::B64);
        emitStoreMR(memRegDst, offset, cc->computeRegI2, OpBits::B64);
        count -= 8;
        offset += 8;
    }

    while (count >= 4)
    {
        emitLoadRM(cc->computeRegI2, memRegSrc, offset, OpBits::B32);
        emitStoreMR(memRegDst, offset, cc->computeRegI2, OpBits::B32);
        count -= 4;
        offset += 4;
    }

    while (count >= 2)
    {
        emitLoadRM(cc->computeRegI2, memRegSrc, offset, OpBits::B16);
        emitStoreMR(memRegDst, offset, cc->computeRegI2, OpBits::B16);
        count -= 2;
        offset += 2;
    }

    while (count >= 1)
    {
        emitLoadRM(cc->computeRegI2, memRegSrc, offset, OpBits::B8);
        emitStoreMR(memRegDst, offset, cc->computeRegI2, OpBits::B8);
        count -= 1;
        offset += 1;
    }
}

void ScbeX64::emitClearM(CpuReg memReg, uint64_t memOffset, uint32_t count)
{
    if (!count)
        return;
    SWAG_ASSERT(memOffset <= 0x7FFFFFFF);

    // SSE 16 octets
    if (count >= 16)
    {
        emitClearR(cc->computeRegF0, OpBits::F32);
        while (count >= 16)
        {
            // movups [memReg+??], xmm0
            emitCPUOp(concat, 0x0F);
            emitCPUOp(concat, 0x11);
            emitModRM(concat, memOffset, cc->computeRegF0, memReg);
            count -= 16;
            memOffset += 16;
        }
    }

    while (count >= 8)
    {
        emitStoreMI(memReg, memOffset, 0, OpBits::B64);
        count -= 8;
        memOffset += 8;
    }

    while (count >= 4)
    {
        emitStoreMI(memReg, memOffset, 0, OpBits::B32);
        count -= 4;
        memOffset += 4;
    }

    while (count >= 2)
    {
        emitStoreMI(memReg, memOffset, 0, OpBits::B16);
        count -= 2;
        memOffset += 2;
    }

    while (count >= 1)
    {
        emitStoreMI(memReg, memOffset, 0, OpBits::B8);
        count -= 1;
        memOffset += 1;
    }
}

/////////////////////////////////////////////////////////////////////

void ScbeX64::emitCallExtern(const Utf8& symbolName)
{
    emitCPUOp(concat, 0xFF);
    emitModRM(concat, ModRMMode::Memory, MODRM_REG_2, MODRM_RM_RIP);

    const auto callSym = getOrAddSymbol(symbolName, CpuSymbolKind::Extern);
    addSymbolRelocation(concat.totalCount() - textSectionOffset, callSym->index, IMAGE_REL_AMD64_REL32);
    concat.addU32(0);
}

void ScbeX64::emitCallLocal(const Utf8& symbolName)
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
}

void ScbeX64::emitCallIndirect(CpuReg reg)
{
    emitREX(concat, OpBits::Zero, REX_REG_NONE, reg);
    emitCPUOp(concat, 0xFF);
    emitModRM(concat, ModRMMode::Register, MODRM_REG_2, encodeReg(reg));
}

/////////////////////////////////////////////////////////////////////

void ScbeX64::emitNop()
{
    emitCPUOp(concat, 0x90);
}

// a*b+c
void ScbeX64::emitMulAdd(CpuReg regDst, CpuReg regMul, CpuReg regAdd, OpBits opBits)
{
    SWAG_ASSERT(regDst == cc->computeRegF0);
    SWAG_ASSERT(regMul == cc->computeRegF1);
    SWAG_ASSERT(regAdd == cc->computeRegF2);

    emitSpecF64(concat, 0xF3, opBits);
    concat.addU8(0x0F);
    emitCPUOp(concat, CpuOp::FMUL);
    concat.addU8(0xC1);

    emitSpecF64(concat, 0xF3, opBits);
    concat.addU8(0x0F);
    emitCPUOp(concat, CpuOp::FADD);
    concat.addU8(0xC2);
}

ScbeMicroOpDetails ScbeX64::getInstructionDetails(ScbeMicroInstruction* inst) const
{
    ScbeMicroOpDetails result = MOD_ZERO;

    switch (inst->op)
    {
        case ScbeMicroOp::Nop:
        case ScbeMicroOp::Ignore:
        case ScbeMicroOp::Label:
        case ScbeMicroOp::Debug:
        case ScbeMicroOp::Push:
        case ScbeMicroOp::Pop:
        case ScbeMicroOp::Ret:
            return MOD_ZERO;

        case ScbeMicroOp::SetCC:
            result.add(1ULL << static_cast<uint32_t>(inst->regA));
            return result;

        case ScbeMicroOp::ClearM:
            return MOD_ZERO;
        case ScbeMicroOp::ClearR:
            result.add(1ULL << static_cast<uint32_t>(inst->regA));
            return result;

        case ScbeMicroOp::StoreMR:
            return MOD_ZERO;
        case ScbeMicroOp::StoreMI:
            if (inst->opBitsA == OpBits::B64 && inst->valueB > 0x7FFFFFFF && inst->valueB >> 32 != 0xFFFFFFFF)
                result.add(1ULL << static_cast<uint32_t>(cc->computeRegI1));
            return result;

        case ScbeMicroOp::LoadRI:
            result.add(1ULL << static_cast<uint32_t>(inst->regA));
            if (isFloat(inst->opBitsA))
                result.add(1ULL << static_cast<uint32_t>(cc->computeRegI0));
            return result;
        case ScbeMicroOp::LoadRR:
            result.add(1ULL << static_cast<uint32_t>(inst->regA));
            return result;

        case ScbeMicroOp::LoadRM:
        case ScbeMicroOp::LoadZeroExtendRM:
        case ScbeMicroOp::LoadSignedExtendRM:
            result.add(1ULL << static_cast<uint32_t>(inst->regA));
            if (inst->valueA > 0x7FFFFFFF)
                result.add(1ULL << static_cast<uint32_t>(cc->computeRegI1));
            return result;

        case ScbeMicroOp::CmpMR:
        case ScbeMicroOp::CmpRR:
            return MOD_ZERO;
        case ScbeMicroOp::CmpMI:
            if (inst->valueA > 0x7FFFFFFF)
                result.add(1ULL << static_cast<uint32_t>(cc->computeRegI0));
            return result;

        case ScbeMicroOp::OpBinaryRI:
            result.add(1ULL << static_cast<uint32_t>(inst->regA));
            if (inst->valueA > 0x7FFFFFFF)
                result.add(1ULL << static_cast<uint32_t>(cc->computeRegI1));
            if (inst->cpuOp == CpuOp::DIV || inst->cpuOp == CpuOp::MOD || inst->cpuOp == CpuOp::IDIV || inst->cpuOp == CpuOp::IMOD)
                result.add(1ULL << static_cast<uint32_t>(cc->computeRegI1));
            if (inst->cpuOp == CpuOp::DIV || inst->cpuOp == CpuOp::MOD || inst->cpuOp == CpuOp::IDIV || inst->cpuOp == CpuOp::IMOD)
                result.add(1ULL << static_cast<uint32_t>(CpuReg::Rdx));
            return result;
    }

    return MOD_REG_ALL;
}
