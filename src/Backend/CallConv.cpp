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

    g_CallConv[static_cast<int>(CallConvKind::Swag)]     = g_CallConv[static_cast<int>(CallConvKind::X86_64)];
    g_CallConv[static_cast<int>(CallConvKind::ByteCode)] = g_CallConv[static_cast<int>(CallConvKind::X86_64)];
}

bool CallConv::structParamByValue(const TypeInfo* typeParam) const
{
    return structParamByRegister && typeParam->isStruct() && typeParam->sizeOf <= sizeof(void*);
}

bool CallConv::returnByAddress(const TypeInfoFuncAttr* typeFunc)
{
    if (!typeFunc->returnType || typeFunc->returnType->isVoid())
        return false;

    const auto type = typeFunc->concreteReturnType();
    if (type->isSlice() ||
        type->isInterface() ||
        type->isAny() ||
        type->isString())
    {
        return true;
    }

    return returnByStackAddress(typeFunc);
}

bool CallConv::returnNeedsStack(const TypeInfoFuncAttr* typeFunc)
{
    if (!typeFunc->returnType || typeFunc->returnType->isVoid())
        return false;
    const auto type = typeFunc->concreteReturnType();
    if (type->isStruct())
        return true;
    return returnByStackAddress(typeFunc);
}

bool CallConv::returnByValue(const TypeInfoFuncAttr* typeFunc)
{
    if (!typeFunc->returnType || typeFunc->returnType->isVoid())
        return false;

    return !returnByAddress(typeFunc);
}

bool CallConv::returnStructByValue(const TypeInfoFuncAttr* typeFunc)
{
    if (!typeFunc->returnType || typeFunc->returnType->isVoid())
        return false;
    if (!typeFunc->declNode)
        return false;
    if (typeFunc->hasFlag(TYPEINFO_CAN_THROW))
        return false;

    const auto type = typeFunc->concreteReturnType();
    if (!type->isStruct())
        return false;

    const auto typeStruct = castTypeInfo<TypeInfoStruct>(type, TypeInfoKind::Struct);
    if (!typeStruct->isPlainOldData())
        return false;

    const auto& cc = typeFunc->getCallConv();
    if (cc.structReturnByRegister && type->isStruct() && type->sizeOf <= sizeof(void*))
        return true;

    return false;
}

bool CallConv::returnByStackAddress(const TypeInfoFuncAttr* typeFunc)
{
    if (!typeFunc->returnType || typeFunc->returnType->isVoid())
        return false;

    const auto type = typeFunc->concreteReturnType();
    if (type->isArray() || type->isClosure())
        return true;

    if (!type->isStruct())
        return false;
    if (returnStructByValue(typeFunc))
        return false;

    return true;
}
