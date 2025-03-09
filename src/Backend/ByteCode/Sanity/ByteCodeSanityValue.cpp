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
    kind  = SanityValueKind::Unknown;
    flags = fl;
}

void SanityValue::setStackAddr(uint64_t val)
{
    kind    = SanityValueKind::StackAddr;
    reg.u64 = val;
}

void SanityValue::setConstantAddr(uint64_t val)
{
    kind    = SanityValueKind::ConstantAddr;
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

void SanityValue::setIps(ByteCodeInstruction* ip, SanityValue* ra, SanityValue* rb, SanityValue* rc, SanityValue* rd)
{
    if (ip->hasWriteRegInA())
    {
        SWAG_ASSERT(ra);
        if (ip->hasReadRefToRegA(ip->a.u32) ||
            ip->hasReadRefToRegB(ip->a.u32) ||
            ip->hasReadRefToRegC(ip->a.u32) ||
            ip->hasReadRefToRegD(ip->a.u32))
            updateIp(ip, ra);
        else
            setIp(ip, ra);
    }

    if (ip->hasWriteRegInB())
    {
        SWAG_ASSERT(rb);
        if (ip->hasReadRefToRegA(ip->b.u32) ||
            ip->hasReadRefToRegB(ip->b.u32) ||
            ip->hasReadRefToRegC(ip->b.u32) ||
            ip->hasReadRefToRegD(ip->b.u32))
            updateIp(ip, rb);
        else
            setIp(ip, rb);
    }

    if (ip->hasWriteRegInC())
    {
        SWAG_ASSERT(rc);
        if (ip->hasReadRefToRegA(ip->c.u32) ||
            ip->hasReadRefToRegB(ip->c.u32) ||
            ip->hasReadRefToRegC(ip->c.u32) ||
            ip->hasReadRefToRegD(ip->c.u32))
            updateIp(ip, rc);
        else
            setIp(ip, rc);
    }

    if (ip->hasWriteRegInD())
    {
        SWAG_ASSERT(rd);
        if (ip->hasReadRefToRegA(ip->d.u32) ||
            ip->hasReadRefToRegB(ip->d.u32) ||
            ip->hasReadRefToRegC(ip->d.u32) ||
            ip->hasReadRefToRegD(ip->d.u32))
            updateIp(ip, rd);
        else
            setIp(ip, rd);
    }
}
