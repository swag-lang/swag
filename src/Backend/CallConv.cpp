#include "pch.h"
#include "Backend/CallConv.h"

CallConv g_CallConv[static_cast<int>(CallConvKind::Max)];

void initCallConvKinds()
{
    auto& ccX64 = g_CallConv[static_cast<int>(CallConvKind::X86_64)];

    ccX64.paramByRegisterCount = 4;
    ccX64.paramByRegisterInteger.push_back(CpuReg::Rcx);
    ccX64.paramByRegisterInteger.push_back(CpuReg::Rdx);
    ccX64.paramByRegisterInteger.push_back(CpuReg::R8);
    ccX64.paramByRegisterInteger.push_back(CpuReg::R9);

    ccX64.paramByRegisterFloat.push_back(CpuReg::Xmm0);
    ccX64.paramByRegisterFloat.push_back(CpuReg::Xmm1);
    ccX64.paramByRegisterFloat.push_back(CpuReg::Xmm2);
    ccX64.paramByRegisterFloat.push_back(CpuReg::Xmm3);

    ccX64.ffiBaseRegister         = CpuReg::Rdi;
    ccX64.returnByRegisterInteger = CpuReg::Rax;
    ccX64.returnByRegisterFloat   = CpuReg::Xmm0;
    ccX64.computeRegI0            = CpuReg::Rax;
    ccX64.computeRegI1            = CpuReg::Rcx;
    ccX64.computeRegF0            = CpuReg::Xmm0;
    ccX64.computeRegF1            = CpuReg::Xmm1;
    ccX64.computeRegF2            = CpuReg::Xmm2;

    ccX64.volatileRegisters.push_back(CpuReg::Rax);
    ccX64.volatileRegisters.push_back(CpuReg::Rcx);
    ccX64.volatileRegisters.push_back(CpuReg::Rdx);
    ccX64.volatileRegisters.push_back(CpuReg::R8);
    ccX64.volatileRegisters.push_back(CpuReg::R9);
    ccX64.volatileRegisters.push_back(CpuReg::R10);
    ccX64.volatileRegisters.push_back(CpuReg::R11);
    ccX64.volatileRegisters.push_back(CpuReg::Xmm0);
    ccX64.volatileRegisters.push_back(CpuReg::Xmm1);
    ccX64.volatileRegisters.push_back(CpuReg::Xmm2);
    ccX64.volatileRegisters.push_back(CpuReg::Xmm3);

    ccX64.nonVolatileRegisters.push_back(CpuReg::Rbx);
    ccX64.nonVolatileRegisters.push_back(CpuReg::Rdi);
    ccX64.nonVolatileRegisters.push_back(CpuReg::Rsi);

    ccX64.useRegisterFloat       = true;
    ccX64.structParamByRegister  = true;
    ccX64.structReturnByRegister = true;

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
        if (r == ccCaller.computeRegI0 && flags.has(VF_EXCLUDE_COMPUTE_I0))
            continue;
        if (r == ccCaller.computeRegI1 && flags.has(VF_EXCLUDE_COMPUTE_I1))
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
