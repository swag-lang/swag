#include "pch.h"
#include "Backend/CallConv.h"

CallConv g_CallConv[static_cast<int>(CallConvKind::Max)];

void initCallConvKinds()
{
    auto& ccSwag = g_CallConv[static_cast<int>(CallConvKind::X86_64)];

    ccSwag.paramByRegisterCount = 4;
    ccSwag.paramByRegisterInteger.push_back(CpuReg::RCX);
    ccSwag.paramByRegisterInteger.push_back(CpuReg::RDX);
    ccSwag.paramByRegisterInteger.push_back(CpuReg::R8);
    ccSwag.paramByRegisterInteger.push_back(CpuReg::R9);

    ccSwag.paramByRegisterFloat.push_back(CpuReg::XMM0);
    ccSwag.paramByRegisterFloat.push_back(CpuReg::XMM1);
    ccSwag.paramByRegisterFloat.push_back(CpuReg::XMM2);
    ccSwag.paramByRegisterFloat.push_back(CpuReg::XMM3);

    ccSwag.returnByRegisterInteger = CpuReg::RAX;
    ccSwag.returnByRegisterFloat   = CpuReg::XMM0;

    ccSwag.volatileRegisters.push_back(CpuReg::RAX);
    ccSwag.volatileRegisters.push_back(CpuReg::RCX);
    ccSwag.volatileRegisters.push_back(CpuReg::RDX);
    ccSwag.volatileRegisters.push_back(CpuReg::R8);
    ccSwag.volatileRegisters.push_back(CpuReg::R9);
    ccSwag.volatileRegisters.push_back(CpuReg::R10);
    ccSwag.volatileRegisters.push_back(CpuReg::R11);
    ccSwag.volatileRegisters.push_back(CpuReg::XMM0);
    ccSwag.volatileRegisters.push_back(CpuReg::XMM1);
    ccSwag.volatileRegisters.push_back(CpuReg::XMM2);
    ccSwag.volatileRegisters.push_back(CpuReg::XMM3);

    ccSwag.nonVolatileRegisters.push_back(CpuReg::RBX);
    ccSwag.nonVolatileRegisters.push_back(CpuReg::RDI);
    ccSwag.nonVolatileRegisters.push_back(CpuReg::RSI);

    ccSwag.useRegisterFloat       = true;
    ccSwag.structParamByRegister  = true;
    ccSwag.structReturnByRegister = true;

    g_CallConv[static_cast<int>(CallConvKind::Swag)] = g_CallConv[static_cast<int>(CallConvKind::X86_64)];

#ifdef _M_X64
    g_CallConv[static_cast<int>(CallConvKind::ByteCode)] = g_CallConv[static_cast<int>(CallConvKind::X86_64)];
#else
    static_assert(false, "unsupported architecture")
#endif
}

const CallConv* CallConv::get(CallConvKind kind)
{
    return &g_CallConv[static_cast<int>(kind)];
}

CpuReg CallConv::getVolatileRegister(const CallConv& ccCaller, const CallConv& ccCallee, VolatileFlags flags)
{
    // We need a working volatile register which is not used as a call parameter
    auto regRes = CpuReg::Max;
    for (const auto& r : ccCaller.volatileRegisters)
    {
        if (r == ccCaller.cpuReg0 && flags.has(VF_EXCLUDE_COMPUTE_I0))
            continue;
        if (r == ccCaller.cpuReg1 && flags.has(VF_EXCLUDE_COMPUTE_I1))
            continue;
        if (r == ccCallee.returnByRegisterInteger && flags.has(VF_EXCLUDE_RETURN))
            continue;
        if (r == ccCallee.returnByRegisterFloat && flags.has(VF_EXCLUDE_RETURN))
            continue;

        if (!ccCallee.paramByRegisterInteger.empty() && r == ccCallee.paramByRegisterInteger[0] && flags.has(VF_EXCLUDE_PARAM0))
            continue;
        if (ccCallee.paramByRegisterInteger.size() >= 2 && r == ccCallee.paramByRegisterInteger[1] && flags.has(VF_EXCLUDE_PARAM1))
            continue;
        if (ccCallee.paramByRegisterInteger.size() >= 3 && r == ccCallee.paramByRegisterInteger[2] && flags.has(VF_EXCLUDE_PARAM2))
            continue;
        if (ccCallee.paramByRegisterInteger.size() >= 4 && r == ccCallee.paramByRegisterInteger[3] && flags.has(VF_EXCLUDE_PARAM3))
            continue;

        regRes = r;
        break;
    }

    SWAG_ASSERT(regRes != CpuReg::Max);
    return regRes;
}
