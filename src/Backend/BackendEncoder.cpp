#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Encoder/ScbeCpu.h"
#include "Semantic/Type/TypeInfo.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/AstNode.h"
#include "Wmf/Module.h"

void BackendEncoder::init(const BuildParameters& buildParameters)
{
    SWAG_ASSERT(buildParameters.module);
    buildParams = buildParameters;
}

OpBits BackendEncoder::getOpBitsByBytes(uint32_t numBytes)
{
    switch (numBytes)
    {
        case 1:
            return OpBits::B8;
        case 2:
            return OpBits::B16;
        case 3:
        case 4:
            return OpBits::B32;
        case 5:
        case 6:
        case 7:
        case 8:
        default:
            return OpBits::B64;
    }
}

uint32_t BackendEncoder::getNumBits(OpBits opBits)
{
    switch (opBits)
    {
        case OpBits::B8:
            return 8;
        case OpBits::B16:
            return 16;
        case OpBits::B32:
            return 32;
        case OpBits::B64:
            return 64;
        case OpBits::B128:
            return 128;
    }

    return 0;
}

bool BackendEncoder::isFloat(ByteCodeOp op)
{
    const auto flags = ByteCode::opFlags(op);
    return flags.has(OPF_FLOAT);
}

bool BackendEncoder::isInt(ByteCodeOp op)
{
    return !isFloat(op);
}

OpBits BackendEncoder::getOpBits(ByteCodeOp op)
{
    const auto flags = ByteCode::opFlags(op);
    if (flags.has(OPF_8))
        return OpBits::B8;
    if (flags.has(OPF_16))
        return OpBits::B16;
    if (flags.has(OPF_32))
        return OpBits::B32;
    if (flags.has(OPF_64))
        return OpBits::B64;
    SWAG_ASSERT(false);
    return OpBits::B32;
}

TypeInfo* BackendEncoder::getOpType(ByteCodeOp op)
{
    const auto flags = ByteCode::opFlags(op);

    if (flags.has(OPF_FLOAT))
    {
        if (flags.has(OPF_32))
            return g_TypeMgr->typeInfoF32;
        return g_TypeMgr->typeInfoF64;
    }

    if (flags.has(OPF_SIGNED))
    {
        if (flags.has(OPF_8))
            return g_TypeMgr->typeInfoS8;
        if (flags.has(OPF_16))
            return g_TypeMgr->typeInfoS16;
        if (flags.has(OPF_32))
            return g_TypeMgr->typeInfoS32;
        if (flags.has(OPF_64))
            return g_TypeMgr->typeInfoS64;
        SWAG_ASSERT(false);
        return nullptr;
    }

    if (flags.has(OPF_8))
        return g_TypeMgr->typeInfoU8;
    if (flags.has(OPF_16))
        return g_TypeMgr->typeInfoU16;
    if (flags.has(OPF_32))
        return g_TypeMgr->typeInfoU32;
    if (flags.has(OPF_64))
        return g_TypeMgr->typeInfoU64;
    SWAG_ASSERT(false);
    return nullptr;
}

bool BackendEncoder::mustCheckOverflow(const Module* module, const ByteCodeInstruction* ip)
{
    const bool nw = !ip->node->hasAttribute(ATTRIBUTE_CAN_OVERFLOW_ON) && !ip->hasFlag(BCI_CAN_OVERFLOW);
    return nw && module->mustEmitSafetyOverflow(ip->node) && !ip->hasFlag(BCI_CANT_OVERFLOW);
}

void BackendEncoder::maskValue(uint64_t& value, OpBits opBits)
{
    switch (opBits)
    {
        case OpBits::B8:
            value &= 0xFF;
            break;
        case OpBits::B16:
            value &= 0xFFFF;
            break;
        case OpBits::B32:
            value &= 0xFFFFFFFF;
            break;
    }
}

bool BackendEncoder::isFuncReturnRegister(CpuReg reg) const
{
    if (cpuFct->typeFunc->returnStructByValue() && reg == cpuFct->cc->returnByRegisterInteger)
        return true;
    if (cpuFct->typeFunc->returnByValue() && cpuFct->typeFunc->returnType->isNativeFloat() && reg == cpuFct->cc->returnByRegisterFloat)
        return true;
    if (cpuFct->typeFunc->returnByValue() && !cpuFct->typeFunc->returnType->isNativeFloat() && reg == cpuFct->cc->returnByRegisterInteger)
        return true;
    return false;
}

bool BackendEncoder::isFuncParameterRegister(CpuReg reg) const
{
    if (cpuFct->typeFunc->numParamsRegisters() == 0)
        return false;
    if (!cpuFct->cc->paramsRegistersIntegerSet.contains(reg) && !cpuFct->cc->paramsRegistersFloatSet.contains(reg))
        return false;

    for (uint32_t i = 0; i < cpuFct->typeFunc->numParamsRegisters(); i++)
    {
        if (reg == cpuFct->cc->paramsRegistersInteger[i] || reg == cpuFct->cc->paramsRegistersFloat[i])
            return true;
    }

    if (cpuFct->typeFunc->returnByAddress() &&
        cpuFct->typeFunc->numParamsRegisters() < cpuFct->cc->paramsRegistersInteger.size() &&
        reg == cpuFct->cc->paramsRegistersInteger[cpuFct->typeFunc->numParamsRegisters()])
    {
        return true;
    }

    return false;
}
