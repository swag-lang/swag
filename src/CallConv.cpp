#include "pch.h"
#include "CallConv.h"
#include "TypeInfo.h"

CallingConventionDesc g_CallConv[CallingConvention::Max];

void initCallingConventions()
{
    auto& cc = g_CallConv[CallingConvention::Swag];

    cc.byRegisterCount = 4;

    cc.byRegisterInteger.push_back(RCX);
    cc.byRegisterInteger.push_back(RDX);
    cc.byRegisterInteger.push_back(R8);
    cc.byRegisterInteger.push_back(R9);

    cc.byRegisterFloat.push_back(XMM0);
    cc.byRegisterFloat.push_back(XMM1);
    cc.byRegisterFloat.push_back(XMM2);
    cc.byRegisterFloat.push_back(XMM3);
    cc.useRegisterFloat = true;

    cc.returnByRegisterInteger = CPURegister::RAX;
    cc.returnByRegisterFloat   = CPURegister::XMM0;

    cc.structByRegister = true;
}

bool CallingConventionDesc::structByValue(TypeInfo* typeParam) const
{
    return structByRegister && typeParam->isStruct() && typeParam->sizeOf <= sizeof(void*);
}