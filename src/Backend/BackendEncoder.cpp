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
            return OpBits::B64;
        default:
            SWAG_ASSERT(false);
            break;
    }

    return OpBits::Zero;
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

    SWAG_ASSERT(false);
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
