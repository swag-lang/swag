#include "pch.h"
#include "Backend/ByteCode/Sanity/ByteCodeSanityValue.h"
#include "Backend/ByteCode/ByteCode.h"

bool SanityValue::isZero() const
{
    if (kind == SanityValueKind::Constant && !reg.u64)
        return true;
    return false;
}

bool SanityValue::isNotZero() const
{
    if (kind == SanityValueKind::Constant && reg.u64)
        return true;
    if (kind == SanityValueKind::Unknown && flags.has(SANITY_VALUE_FLAG_NOT_ZERO))
        return true;    
    return false;
}

void SanityValue::setConstant(uint8_t val)
{
    kind   = SanityValueKind::Constant;
    reg.u8 = val;
}

void SanityValue::setConstant(uint16_t val)
{
    kind    = SanityValueKind::Constant;
    reg.u16 = val;
}

void SanityValue::setConstant(uint32_t val)
{
    kind    = SanityValueKind::Constant;
    reg.u32 = val;
}

void SanityValue::setConstant(uint64_t val)
{
    kind    = SanityValueKind::Constant;
    reg.u64 = val;
}

void SanityValue::setConstant(int8_t val)
{
    kind   = SanityValueKind::Constant;
    reg.s8 = val;
}

void SanityValue::setConstant(int16_t val)
{
    kind    = SanityValueKind::Constant;
    reg.s16 = val;
}

void SanityValue::setConstant(int32_t val)
{
    kind    = SanityValueKind::Constant;
    reg.s32 = val;
}

void SanityValue::setConstant(int64_t val)
{
    kind    = SanityValueKind::Constant;
    reg.s64 = val;
}

void SanityValue::setConstant(float val)
{
    kind    = SanityValueKind::Constant;
    reg.f32 = val;
}

void SanityValue::setConstant(double val)
{
    kind    = SanityValueKind::Constant;
    reg.f64 = val;
}

void SanityValue::setUnknown(SanityValueFlags fl)
{
    kind = SanityValueKind::Unknown;
    flags = fl;
}

void SanityValue::setStackAddr(uint64_t val)
{
    kind    = SanityValueKind::StackAddr;
    reg.u64 = val;
}

void SanityValue::setKind(SanityValueKind val)
{
    kind = val;
}

namespace
{
    void updateIp(ByteCodeInstruction* ip, SanityValue* value)
    {
        value->ips.push_back(ip);
    }

    void setIp(ByteCodeInstruction* ip, SanityValue* value)
    {
        value->ips.clear();
        value->ips.push_back(ip);
    }
}

void SanityValue::computeIp(ByteCodeInstruction* ip, SanityValue* ra, SanityValue* rb, SanityValue* rc, SanityValue* rd)
{
    if (ByteCode::hasWriteRegInA(ip))
    {
        SWAG_ASSERT(ra);
        if (ByteCode::hasReadRefToRegA(ip, ip->a.u32) ||
            ByteCode::hasReadRefToRegB(ip, ip->a.u32) ||
            ByteCode::hasReadRefToRegC(ip, ip->a.u32) ||
            ByteCode::hasReadRefToRegD(ip, ip->a.u32))
            updateIp(ip, ra);
        else
            setIp(ip, ra);
    }

    if (ByteCode::hasWriteRegInB(ip))
    {
        SWAG_ASSERT(rb);
        if (ByteCode::hasReadRefToRegA(ip, ip->b.u32) ||
            ByteCode::hasReadRefToRegB(ip, ip->b.u32) ||
            ByteCode::hasReadRefToRegC(ip, ip->b.u32) ||
            ByteCode::hasReadRefToRegD(ip, ip->b.u32))
            updateIp(ip, rb);
        else
            setIp(ip, rb);
    }

    if (ByteCode::hasWriteRegInC(ip))
    {
        SWAG_ASSERT(rc);
        if (ByteCode::hasReadRefToRegA(ip, ip->c.u32) ||
            ByteCode::hasReadRefToRegB(ip, ip->c.u32) ||
            ByteCode::hasReadRefToRegC(ip, ip->c.u32) ||
            ByteCode::hasReadRefToRegD(ip, ip->c.u32))
            updateIp(ip, rc);
        else
            setIp(ip, rc);
    }

    if (ByteCode::hasWriteRegInD(ip))
    {
        SWAG_ASSERT(rd);
        if (ByteCode::hasReadRefToRegA(ip, ip->d.u32) ||
            ByteCode::hasReadRefToRegB(ip, ip->d.u32) ||
            ByteCode::hasReadRefToRegC(ip, ip->d.u32) ||
            ByteCode::hasReadRefToRegD(ip, ip->d.u32))
            updateIp(ip, rd);
        else
            setIp(ip, rd);
    }
}
