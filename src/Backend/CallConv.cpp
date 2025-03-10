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
    ccSwag.nonVolatileRegisters.push_back(CpuReg::RBP);
    ccSwag.nonVolatileRegisters.push_back(CpuReg::RSP);

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
