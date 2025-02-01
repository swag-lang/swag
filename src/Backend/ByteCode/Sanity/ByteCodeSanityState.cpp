#include "pch.h"
#include "Backend/ByteCode/Sanity/ByteCodeSanityState.h"
#include "Backend/ByteCode/ByteCodeInstruction.h"
#include "Backend/ByteCode/Sanity/ByteCodeSanity.h"
#include "Report/ErrorIds.h"

bool ByteCodeSanityState::getImmediateA(SanityValue& result, const ByteCodeInstruction* ipn)
{
    if (!ipn)
        ipn = ip;
    if (ipn->hasFlag(BCI_IMM_A))
    {
        result.kind = SanityValueKind::Constant;
        result.reg  = ipn->b;
        return true;
    }

    SanityValue* ra = nullptr;
    SWAG_CHECK(getRegister(ra, ipn->a.u32));
    result = *ra;
    return true;
}

bool ByteCodeSanityState::getImmediateB(SanityValue& result, const ByteCodeInstruction* ipn)
{
    if (!ipn)
        ipn = ip;
    if (ipn->hasFlag(BCI_IMM_B))
    {
        result.kind = SanityValueKind::Constant;
        result.reg  = ipn->b;
        return true;
    }

    SanityValue* rb = nullptr;
    SWAG_CHECK(getRegister(rb, ipn->b.u32));
    result = *rb;
    return true;
}

bool ByteCodeSanityState::getImmediateC(SanityValue& result, const ByteCodeInstruction* ipn)
{
    if (!ipn)
        ipn = ip;
    if (ipn->hasFlag(BCI_IMM_C))
    {
        result.kind = SanityValueKind::Constant;
        result.reg  = ipn->c;
        return true;
    }

    SanityValue* rc = nullptr;
    SWAG_CHECK(getRegister(rc, ipn->c.u32));
    result = *rc;
    return true;
}

bool ByteCodeSanityState::getImmediateD(SanityValue& result, const ByteCodeInstruction* ipn)
{
    if (!ipn)
        ipn = ip;
    if (ipn->hasFlag(BCI_IMM_D))
    {
        result.kind = SanityValueKind::Constant;
        result.reg  = ipn->d;
        return true;
    }

    SanityValue* rd = nullptr;
    SWAG_CHECK(getRegister(rd, ipn->d.u32));
    result = *rd;
    return true;
}

bool ByteCodeSanityState::getRegister(SanityValue*& result, uint32_t reg)
{
    SWAG_ASSERT(reg < regs.size());
    result = &regs[reg];
    return true;
}

bool ByteCodeSanityState::checkStackOffset(uint64_t stackOffset, uint32_t sizeOf, const SanityValue* locValue) const
{
    if (stackOffset + sizeOf <= static_cast<size_t>(stack.size()))
        return true;
    const auto err = ByteCodeSanity::raiseError(ip, formErr(San0007, stackOffset + sizeOf, stack.size()), locValue);
    if (err)
        return san->context.report(*err);
    return true;
}

bool ByteCodeSanityState::checkStackInitialized(void* addr, uint32_t sizeOf, const SanityValue* locValue) const
{
    SanityValue memValue;
    SWAG_CHECK(getStackKind(&memValue, addr, sizeOf));
    if (memValue.kind != SanityValueKind::Invalid)
        return true;

    const auto err = ByteCodeSanity::raiseError(ip, toErr(San0008), locValue);
    if (err)
        return san->context.report(*err);
    return true;
}

bool ByteCodeSanityState::getStackAddress(uint8_t*& result, uint64_t stackOffset, uint32_t sizeOf, const SanityValue* locValue)
{
    SWAG_CHECK(checkStackOffset(stackOffset, sizeOf, locValue));
    result = stack.data() + stackOffset;
    return true;
}

bool ByteCodeSanityState::getStackKind(SanityValue* result, void* stackAddr, uint32_t sizeOf) const
{
    const auto offset = static_cast<uint8_t*>(stackAddr) - stack.data();
    SWAG_ASSERT(offset + sizeOf <= stackKind.size());
    auto addrValue = stackKind.data() + offset;

    *result = *addrValue;
    addrValue++;

    for (uint32_t i = 1; i < sizeOf; i++)
    {
        if (addrValue->kind != result->kind)
        {
            result->kind = SanityValueKind::Unknown;
            return true;
        }

        addrValue++;
    }

    return true;
}

void ByteCodeSanityState::setStackKind(void* stackAddr, uint32_t sizeOf, SanityValueKind kind, SanityValueFlags flags)
{
    const auto offset = static_cast<uint32_t>(static_cast<uint8_t*>(stackAddr) - stack.data());
    SWAG_ASSERT(offset + sizeOf <= stackKind.size());
    for (uint32_t i = offset; i < offset + sizeOf; i++)
    {
        auto& val = stackKind[i];
        val.kind  = kind;
        val.flags = flags;
    }
}

void ByteCodeSanityState::setStackIps(void* addr, uint32_t sizeOf, bool clear)
{
    const auto offset = static_cast<uint32_t>(static_cast<uint8_t*>(addr) - stack.data());
    SWAG_ASSERT(offset + sizeOf <= stackKind.size());
    for (uint32_t i = offset; i < offset + sizeOf; i++)
    {
        auto& val = stackKind[i];
        if (clear)
            val.ips.clear();
        val.ips.push_back(ip);
    }
}

void ByteCodeSanityState ::updateStackIps(void* addr, uint32_t sizeOf, const SanityValue* from)
{
    const auto offset = static_cast<uint32_t>(static_cast<uint8_t*>(addr) - stack.data());
    SWAG_ASSERT(offset + sizeOf <= stackKind.size());
    for (uint32_t i = offset; i < offset + sizeOf; i++)
    {
        auto& val = stackKind[i];
        val.ips   = from->ips;
        val.ips.push_back(ip);
    }
}

void ByteCodeSanityState::invalidateStack()
{
    setStackKind(stack.data(), stack.size(), SanityValueKind::Unknown);
}
