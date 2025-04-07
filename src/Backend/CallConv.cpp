#include "pch.h"
#include "Backend/CallConv.h"

CallConv g_CallConv[static_cast<int>(CallConvKind::Max)];

void initCallConvKinds()
{
    auto& ccX64 = g_CallConv[static_cast<int>(CallConvKind::X86_64)];

    ccX64.paramsRegistersInteger.push_back(CpuReg::Rcx);
    ccX64.paramsRegistersInteger.push_back(CpuReg::Rdx);
    ccX64.paramsRegistersInteger.push_back(CpuReg::R8);
    ccX64.paramsRegistersInteger.push_back(CpuReg::R9);

    ccX64.paramsRegistersFloat.push_back(CpuReg::Xmm0);
    ccX64.paramsRegistersFloat.push_back(CpuReg::Xmm1);
    ccX64.paramsRegistersFloat.push_back(CpuReg::Xmm2);
    ccX64.paramsRegistersFloat.push_back(CpuReg::Xmm3);

    ccX64.ffiBaseRegister         = CpuReg::Rdi;
    ccX64.returnByRegisterInteger = CpuReg::Rax;
    ccX64.returnByRegisterFloat   = CpuReg::Xmm0;

    ccX64.computeRegI0 = CpuReg::Rax;
    ccX64.computeRegI1 = CpuReg::R10;
    ccX64.computeRegI2 = CpuReg::R11;
    ccX64.computeRegF0 = CpuReg::Xmm0;
    ccX64.computeRegF1 = CpuReg::Xmm1;
    ccX64.computeRegF2 = CpuReg::Xmm2;

    ccX64.volatileRegistersInteger.push_back(CpuReg::Rax);
    ccX64.volatileRegistersInteger.push_back(CpuReg::Rcx);
    ccX64.volatileRegistersInteger.push_back(CpuReg::Rdx);
    ccX64.volatileRegistersInteger.push_back(CpuReg::R8);
    ccX64.volatileRegistersInteger.push_back(CpuReg::R9);
    ccX64.volatileRegistersInteger.push_back(CpuReg::R10);
    ccX64.volatileRegistersInteger.push_back(CpuReg::R11);

    ccX64.volatileRegistersFloat.push_back(CpuReg::Xmm0);
    ccX64.volatileRegistersFloat.push_back(CpuReg::Xmm1);
    ccX64.volatileRegistersFloat.push_back(CpuReg::Xmm2);
    ccX64.volatileRegistersFloat.push_back(CpuReg::Xmm3);

    ccX64.nonVolatileRegistersInteger.push_back(CpuReg::Rbx);
    ccX64.nonVolatileRegistersInteger.push_back(CpuReg::Rdi);
    ccX64.nonVolatileRegistersInteger.push_back(CpuReg::Rsi);
    ccX64.nonVolatileRegistersInteger.push_back(CpuReg::R12);
    ccX64.nonVolatileRegistersInteger.push_back(CpuReg::R13);
    ccX64.nonVolatileRegistersInteger.push_back(CpuReg::R14);
    ccX64.nonVolatileRegistersInteger.push_back(CpuReg::R15);

    ccX64.useRegisterFloat       = true;
    ccX64.structParamByRegister  = true;
    ccX64.structReturnByRegister = true;

    ccX64.compute();

    g_CallConv[static_cast<int>(CallConvKind::Swag)] = g_CallConv[static_cast<int>(CallConvKind::X86_64)];

#ifdef _M_X64
    g_CallConv[static_cast<int>(CallConvKind::Compiler)] = g_CallConv[static_cast<int>(CallConvKind::X86_64)];
#else
    static_assert(false, "unsupported architecture")
#endif
}

void CallConv::compute()
{
    for (const auto r : paramsRegistersInteger)
        paramsRegistersIntegerSet.add(r);
    for (const auto r : paramsRegistersFloat)
        paramsRegistersFloatSet.add(r);
    for (const auto r : volatileRegistersInteger)
        volatileRegistersIntegerSet.add(r);
    for (const auto r : volatileRegistersFloat)
        volatileRegistersFloatSet.add(r);
    for (const auto r : nonVolatileRegistersInteger)
        nonVolatileRegistersIntegerSet.add(r);
}

const CallConv* CallConv::get(CallConvKind kind)
{
    return &g_CallConv[static_cast<int>(kind)];
}

CpuReg CallConv::getVolatileRegisterInteger(const CallConv* ccCaller, const CallConv* ccCallee, VolatileFlags flags)
{
    // We need a working volatile register which is not used as a call parameter
    auto regRes = CpuReg::Max;
    for (const auto& r : ccCaller->volatileRegistersInteger)
    {
        if (r == ccCaller->computeRegI0 && flags.has(VF_EXCLUDE_COMPUTE_I0))
            continue;
        if (r == ccCaller->computeRegI1 && flags.has(VF_EXCLUDE_COMPUTE_I1))
            continue;
        if (r == ccCaller->computeRegI2 && flags.has(VF_EXCLUDE_COMPUTE_I2))
            continue;
        if (r == ccCallee->returnByRegisterInteger && flags.has(VF_EXCLUDE_RETURN))
            continue;

        if (!ccCallee->paramsRegistersInteger.empty() && r == ccCallee->paramsRegistersInteger[0] && flags.has(VF_EXCLUDE_PARAM0))
            continue;
        if (ccCallee->paramsRegistersInteger.size() >= 2 && r == ccCallee->paramsRegistersInteger[1] && flags.has(VF_EXCLUDE_PARAM1))
            continue;
        if (ccCallee->paramsRegistersInteger.size() >= 3 && r == ccCallee->paramsRegistersInteger[2] && flags.has(VF_EXCLUDE_PARAM2))
            continue;
        if (ccCallee->paramsRegistersInteger.size() >= 4 && r == ccCallee->paramsRegistersInteger[3] && flags.has(VF_EXCLUDE_PARAM3))
            continue;

        regRes = r;
        break;
    }

    SWAG_ASSERT(regRes != CpuReg::Max);
    return regRes;
}
