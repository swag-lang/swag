#include "pch.h"
#include "Backend/CallConv.h"
#include "Semantic/Type/TypeInfo.h"
#include "Syntax/Ast.h"

CallConv g_CallConv[static_cast<int>(CallConvKind::Max)];

void initCallConvKinds()
{
    auto& ccSwag                = g_CallConv[static_cast<int>(CallConvKind::X86_64)];
    ccSwag.paramByRegisterCount = 4;
    ccSwag.paramByRegisterInteger.push_back(CPUReg::RCX);
    ccSwag.paramByRegisterInteger.push_back(CPUReg::RDX);
    ccSwag.paramByRegisterInteger.push_back(CPUReg::R8);
    ccSwag.paramByRegisterInteger.push_back(CPUReg::R9);
    ccSwag.paramByRegisterFloat.push_back(CPUReg::XMM0);
    ccSwag.paramByRegisterFloat.push_back(CPUReg::XMM1);
    ccSwag.paramByRegisterFloat.push_back(CPUReg::XMM2);
    ccSwag.paramByRegisterFloat.push_back(CPUReg::XMM3);
    ccSwag.returnByRegisterInteger = CPUReg::RAX;
    ccSwag.returnByRegisterFloat   = CPUReg::XMM0;
    ccSwag.useRegisterFloat        = true;
    ccSwag.structParamByRegister   = true;
    ccSwag.structReturnByRegister  = true;

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
