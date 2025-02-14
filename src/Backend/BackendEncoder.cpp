#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/SCBE/Encoder/SCBE_CPU.h"
#include "Semantic/Type/TypeInfo.h"
#include "Semantic/Type/TypeManager.h"

uint32_t BackendEncoder::getBitsCount(CPUBits numBits)
{
    switch (numBits)
    {
        case CPUBits::B8:
            return 8;
        case CPUBits::B16:
            return 16;
        case CPUBits::B32:
        case CPUBits::F32:
            return 32;
        case CPUBits::B64:
        case CPUBits::F64:
            return 64;
    }

    SWAG_ASSERT(false);
    return 0;
}

CPUBits BackendEncoder::getCPUBits(ByteCodeOp op)
{
    const auto flags = ByteCode::opFlags(op);
    if (flags.has(OPF_8))
        return CPUBits::B8;
    if (flags.has(OPF_16))
        return CPUBits::B16;
    if (flags.has(OPF_32) && flags.has(OPF_FLOAT))
        return CPUBits::F32;
    if (flags.has(OPF_32))
        return CPUBits::B32;
    if (flags.has(OPF_64) && flags.has(OPF_FLOAT))
        return CPUBits::F64;
    if (flags.has(OPF_64))
        return CPUBits::B64;
    SWAG_ASSERT(false);
    return CPUBits::B32;
}

TypeInfo* BackendEncoder::getCPUType(ByteCodeOp op)
{
    const auto flags = ByteCode::opFlags(op);
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
