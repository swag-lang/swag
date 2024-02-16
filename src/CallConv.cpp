#include "pch.h"
#include "CallConv.h"
#include "Ast.h"
#include "TypeInfo.h"

CallConv g_CallConv[Max];

void initCallConvKinds()
{
	auto& ccSwag                = g_CallConv[Swag];
	ccSwag.paramByRegisterCount = 4;
	ccSwag.paramByRegisterInteger.push_back(RCX);
	ccSwag.paramByRegisterInteger.push_back(RDX);
	ccSwag.paramByRegisterInteger.push_back(R8);
	ccSwag.paramByRegisterInteger.push_back(R9);
	ccSwag.paramByRegisterFloat.push_back(XMM0);
	ccSwag.paramByRegisterFloat.push_back(XMM1);
	ccSwag.paramByRegisterFloat.push_back(XMM2);
	ccSwag.paramByRegisterFloat.push_back(XMM3);
	ccSwag.returnByRegisterInteger = RAX;
	ccSwag.returnByRegisterFloat   = XMM0;
	ccSwag.useRegisterFloat        = true;
	ccSwag.structParamByRegister   = true;
	ccSwag.structReturnByRegister  = true;
}

bool CallConv::structParamByValue(const TypeInfoFuncAttr* typeFunc, const TypeInfo* typeParam)
{
	const auto& cc = typeFunc->getCallConv();
	return cc.structParamByRegister && typeParam->isStruct() && typeParam->sizeOf <= sizeof(void*);
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

bool CallConv::returnByStackAddress(const TypeInfoFuncAttr* typeFunc)
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
	if (type->isStruct())
		return true;

	return false;
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

	if (typeFunc->declNode->kind == AstNodeKind::FuncDecl)
	{
		const auto fctNode = castAst<AstFuncDecl>(typeFunc->declNode, AstNodeKind::FuncDecl);
		if (fctNode->mustInline())
			return false;
	}

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
