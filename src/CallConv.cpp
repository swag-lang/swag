#include "pch.h"
#include "CallConv.h"
#include "Ast.h"
#include "TypeInfo.h"

CallConv g_CallConv[CallConvKind::Max];

void initCallConvKinds()
{
    auto& ccSwag                = g_CallConv[CallConvKind::Swag];
    ccSwag.paramByRegisterCount = 4;
    ccSwag.paramByRegisterInteger.push_back(RCX);
    ccSwag.paramByRegisterInteger.push_back(RDX);
    ccSwag.paramByRegisterInteger.push_back(R8);
    ccSwag.paramByRegisterInteger.push_back(R9);
    ccSwag.paramByRegisterFloat.push_back(XMM0);
    ccSwag.paramByRegisterFloat.push_back(XMM1);
    ccSwag.paramByRegisterFloat.push_back(XMM2);
    ccSwag.paramByRegisterFloat.push_back(XMM3);
    ccSwag.returnByRegisterInteger = CPURegister::RAX;
    ccSwag.returnByRegisterFloat   = CPURegister::XMM0;
    ccSwag.useRegisterFloat        = true;
    ccSwag.structParamByRegister   = true;
    ccSwag.structReturnByRegister  = true;
}

bool CallConv::structParamByValue(TypeInfoFuncAttr* typeFunc, TypeInfo* typeParam)
{
    const auto& cc = typeFunc->getCallConv();
    return cc.structParamByRegister && typeParam->isStruct() && typeParam->sizeOf <= sizeof(void*);
}

bool CallConv::returnByAddress(TypeInfoFuncAttr* typeFunc)
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

    return CallConv::returnByStackAddress(typeFunc);
}

bool CallConv::returnByStackAddress(TypeInfoFuncAttr* typeFunc)
{
    if (!typeFunc->returnType || typeFunc->returnType->isVoid())
        return false;

    const auto type = typeFunc->concreteReturnType();
    if (type->isArray() ||
        type->isClosure())
    {
        return true;
    }

    if (returnStructByValue(typeFunc))
        return false;
    else if (type->isStruct())
        return true;

    return false;
}

bool CallConv::returnNeedsStack(TypeInfoFuncAttr* typeFunc)
{
    if (!typeFunc->returnType || typeFunc->returnType->isVoid())
        return false;
    const auto type = typeFunc->concreteReturnType();
    if (type->isStruct())
        return true;
    return returnByStackAddress(typeFunc);
}

bool CallConv::returnByValue(TypeInfoFuncAttr* typeFunc)
{
    if (!typeFunc->returnType || typeFunc->returnType->isVoid())
        return false;

    return !CallConv::returnByAddress(typeFunc);
}

bool CallConv::returnStructByValue(TypeInfoFuncAttr* typeFunc)
{
    if (!typeFunc->returnType || typeFunc->returnType->isVoid())
        return false;
    if (!typeFunc->declNode)
        return false;
    if (typeFunc->flags & TYPEINFO_CAN_THROW)
        return false;

    if (typeFunc->declNode->kind == AstNodeKind::FuncDecl)
    {
        const auto fctNode = CastAst<AstFuncDecl>(typeFunc->declNode, AstNodeKind::FuncDecl);
        if (fctNode->mustInline())
            return false;
    }

    const auto type = typeFunc->concreteReturnType();
    if (!type->isStruct())
        return false;

    const auto typeStruct = CastTypeInfo<TypeInfoStruct>(type, TypeInfoKind::Struct);
    if (!typeStruct->isPlainOldData())
        return false;

    const auto& cc = typeFunc->getCallConv();
    if (cc.structReturnByRegister && type->isStruct() && type->sizeOf <= sizeof(void*))
        return true;

    return false;
}
