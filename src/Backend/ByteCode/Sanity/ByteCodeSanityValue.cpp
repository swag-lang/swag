#include "pch.h"
#include "Backend/ByteCode/Sanity/ByteCodeSanityValue.h"
#include "Backend/ByteCode/ByteCode.h"

bool SanityValue::isNull() const
{
    return kind == SanityValueKind::Constant && !reg.pointer;
}

void SanityValue::setConstant(uint64_t val)
{
    kind    = SanityValueKind::Constant;
    reg.u64 = val;
}

void SanityValue::setUnknown()
{
    kind = SanityValueKind::Unknown;
}

void SanityValue::updateIp(ByteCodeInstruction* ip)
{
    ips.push_back(ip);
}

void SanityValue::setIp(ByteCodeInstruction* ip)
{
    ips.clear();
    ips.push_back(ip);
}
