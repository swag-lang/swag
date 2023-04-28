#include "pch.h"
#include "CallConv.h"
#include "TypeInfo.h"

CallConv g_CallConv[CallConvKind::Max];

void initCallConvKinds()
{
    auto& ccSwag           = g_CallConv[CallConvKind::Swag];
    ccSwag.byRegisterCount = 4;
    ccSwag.byRegisterInteger.push_back(RCX);
    ccSwag.byRegisterInteger.push_back(RDX);
    ccSwag.byRegisterInteger.push_back(R8);
    ccSwag.byRegisterInteger.push_back(R9);
    ccSwag.byRegisterFloat.push_back(XMM0);
    ccSwag.byRegisterFloat.push_back(XMM1);
    ccSwag.byRegisterFloat.push_back(XMM2);
    ccSwag.byRegisterFloat.push_back(XMM3);
    ccSwag.returnByRegisterInteger = CPURegister::RAX;
    ccSwag.returnByRegisterFloat   = CPURegister::XMM0;
    ccSwag.useRegisterFloat        = true;
    ccSwag.structByRegister        = true;
}

bool CallConv::structParamByValue(TypeInfoFuncAttr* typeFunc, TypeInfo* typeParam)
{
    const auto& cc = typeFunc->getCallConv();
    return cc.structByRegister && typeParam->isStruct() && typeParam->sizeOf <= sizeof(void*);
}

bool CallConv::returnByAddress(TypeInfoFuncAttr* typeFunc)
{
    if (!typeFunc->returnType || typeFunc->returnType->isVoid())
        return false;

    auto type = typeFunc->concreteReturnType();
    if (type->isSlice() ||
        type->isInterface() ||
        type->isAny() ||
        type->isString())
    {
        return true;
    }

    return CallConv::returnByStackAddress(typeFunc);
}

bool CallConv::returnByStackAddress(TypeInfoFuncAttr* typeFunc)
{
    if (!typeFunc->returnType || typeFunc->returnType->isVoid())
        return false;

    auto type = typeFunc->concreteReturnType();
    if (type->isStruct() ||
        type->isArray() ||
        type->isClosure())
    {
        return true;
    }

    return false;
}

bool CallConv::returnByValue(TypeInfoFuncAttr* typeFunc)
{
    if (!typeFunc->returnType || typeFunc->returnType->isVoid())
        return false;

    return !CallConv::returnByAddress(typeFunc);
}
