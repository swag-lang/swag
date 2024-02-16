#include "pch.h"
#include "Ast.h"
#include "TypeManager.h"

TypeInfo* TypeManager::promoteUntyped(TypeInfo* typeInfo)
{
	if (typeInfo->isUntypedInteger())
		return g_TypeMgr->typeInfoS32;
	if (typeInfo->isUntypedFloat())
		return g_TypeMgr->typeInfoF32;
	if (typeInfo->isUntypedBinHex())
		return g_TypeMgr->typeInfoU32;
	return typeInfo;
}

void TypeManager::promoteUntypedInteger(AstNode* left, const AstNode* right)
{
	TypeInfo* leftTypeInfo = concreteType(left->typeInfo);
	SWAG_ASSERT(leftTypeInfo->isUntypedInteger());

	TypeInfo* rightTypeInfo = concreteType(right->typeInfo);
	if (!rightTypeInfo->isNativeInteger())
		return;

	const auto leftNative = castTypeInfo<TypeInfoNative>(leftTypeInfo, TypeInfoKind::Native);
	if (leftNative->valueInteger == 0)
	{
		left->typeInfo = rightTypeInfo;
	}
	else if ((leftNative->valueInteger > 0) && rightTypeInfo->hasFlag(TYPEINFO_UNSIGNED))
	{
		if (leftNative->valueInteger <= UINT8_MAX)
			left->typeInfo = g_TypeMgr->typeInfoU8;
		else if (leftNative->valueInteger <= UINT16_MAX)
			left->typeInfo = g_TypeMgr->typeInfoU16;
		else
			left->typeInfo = g_TypeMgr->typeInfoU32;
	}
	else
	{
		if (leftNative->valueInteger >= INT8_MIN && leftNative->valueInteger <= INT8_MAX)
			left->typeInfo = g_TypeMgr->typeInfoS8;
		else if (leftNative->valueInteger >= INT16_MIN && leftNative->valueInteger <= INT16_MAX)
			left->typeInfo = g_TypeMgr->typeInfoS16;
		else if (leftNative->valueInteger >= INT32_MIN && leftNative->valueInteger <= INT32_MAX)
			left->typeInfo = g_TypeMgr->typeInfoS32;
		else
			left->typeInfo = g_TypeMgr->typeInfoS64;
	}
}

bool TypeManager::promote32(SemanticContext* context, AstNode* left)
{
	const TypeInfo* typeInfo = concreteType(left->typeInfo);
	if (!typeInfo->isNative())
		return true;

	switch (typeInfo->nativeType)
	{
	case NativeTypeKind::S8:
		if (left->hasComputedValue())
			left->computedValue->reg.s64 = left->computedValue->reg.s8;
		SWAG_CHECK(makeCompatibles(context, g_TypeMgr->typeInfoS32, nullptr, left, CAST_FLAG_TRY_COERCE));
		break;
	case NativeTypeKind::S16:
		if (left->hasComputedValue())
			left->computedValue->reg.s64 = left->computedValue->reg.s16;
		SWAG_CHECK(makeCompatibles(context, g_TypeMgr->typeInfoS32, nullptr, left, CAST_FLAG_TRY_COERCE));
		break;
	case NativeTypeKind::U8:
		if (left->hasComputedValue())
			left->computedValue->reg.u64 = left->computedValue->reg.u8;
		SWAG_CHECK(makeCompatibles(context, g_TypeMgr->typeInfoU32, nullptr, left, CAST_FLAG_TRY_COERCE));
		break;
	case NativeTypeKind::U16:
		if (left->hasComputedValue())
			left->computedValue->reg.u64 = left->computedValue->reg.u32;
		SWAG_CHECK(makeCompatibles(context, g_TypeMgr->typeInfoU32, nullptr, left, CAST_FLAG_TRY_COERCE));
		break;
	default:
		break;
	}

	return true;
}

bool TypeManager::promote(SemanticContext* context, AstNode* left, AstNode* right)
{
	SWAG_CHECK(promoteLeft(context, left, right));
	SWAG_CHECK(promoteLeft(context, right, left));
	return true;
}

bool TypeManager::promoteLeft(SemanticContext* context, AstNode* left, AstNode* right)
{
	TypeInfo* leftTypeInfo  = concreteType(left->typeInfo);
	TypeInfo* rightTypeInfo = concreteType(right->typeInfo);

	// Promotion only for native types
	if (!leftTypeInfo->isNativeInteger() && !leftTypeInfo->isNativeFloat())
		return true;
	if (!rightTypeInfo->isNativeInteger() && !rightTypeInfo->isNativeFloat())
		return true;

	if (leftTypeInfo->isUntypedInteger() && !rightTypeInfo->isUntypedInteger())
	{
		promoteUntypedInteger(left, right);
		leftTypeInfo = left->typeInfo;
	}

	if (!leftTypeInfo->isUntypedInteger() && rightTypeInfo->isUntypedInteger())
	{
		promoteUntypedInteger(right, left);
		rightTypeInfo = right->typeInfo;
	}

	const auto newLeftTypeInfo = g_TypeMgr->promoteMatrix[static_cast<int>(leftTypeInfo->nativeType)][static_cast<int>(rightTypeInfo->nativeType)];
	SWAG_ASSERT(newLeftTypeInfo);
	if (newLeftTypeInfo == leftTypeInfo)
		return true;

	left->typeInfo = newLeftTypeInfo;
	if (!left->hasComputedValue())
	{
		left->castedTypeInfo = leftTypeInfo;
		return true;
	}

	const auto newLeft = newLeftTypeInfo->nativeType;
	switch (leftTypeInfo->nativeType)
	{
	case NativeTypeKind::U8:
		if (newLeft == NativeTypeKind::U32)
			left->computedValue->reg.u64 = left->computedValue->reg.u8;
		else if (newLeft == NativeTypeKind::U64)
			left->computedValue->reg.u64 = left->computedValue->reg.u8;
		else if (newLeft == NativeTypeKind::F32)
			left->computedValue->reg.f32 = static_cast<float>(left->computedValue->reg.u8);
		else if (newLeft == NativeTypeKind::F64)
			left->computedValue->reg.f64 = static_cast<double>(left->computedValue->reg.u8);
		break;
	case NativeTypeKind::U16:
		if (newLeft == NativeTypeKind::U32)
			left->computedValue->reg.u64 = left->computedValue->reg.u16;
		else if (newLeft == NativeTypeKind::U64)
			left->computedValue->reg.u64 = left->computedValue->reg.u16;
		else if (newLeft == NativeTypeKind::F32)
			left->computedValue->reg.f32 = static_cast<float>(left->computedValue->reg.u16);
		else if (newLeft == NativeTypeKind::F64)
			left->computedValue->reg.f64 = static_cast<double>(left->computedValue->reg.u16);
		break;
	case NativeTypeKind::U32:
		if (newLeft == NativeTypeKind::U64)
			left->computedValue->reg.u64 = left->computedValue->reg.u32;
		else if (newLeft == NativeTypeKind::F32)
			left->computedValue->reg.f32 = static_cast<float>(left->computedValue->reg.u32);
		else if (newLeft == NativeTypeKind::F64)
			left->computedValue->reg.f64 = static_cast<double>(left->computedValue->reg.u32);
		break;
	case NativeTypeKind::U64:
		if (newLeft == NativeTypeKind::F64)
			left->computedValue->reg.f64 = static_cast<double>(left->computedValue->reg.u64);
		break;
	case NativeTypeKind::S8:
		if (newLeft == NativeTypeKind::S32)
			left->computedValue->reg.s64 = left->computedValue->reg.s8;
		else if (newLeft == NativeTypeKind::S64)
			left->computedValue->reg.s64 = left->computedValue->reg.s8;
		else if (newLeft == NativeTypeKind::F32)
			left->computedValue->reg.f32 = static_cast<float>(left->computedValue->reg.s8);
		else if (newLeft == NativeTypeKind::F64)
			left->computedValue->reg.f64 = static_cast<double>(left->computedValue->reg.s8);
		break;
	case NativeTypeKind::S16:
		if (newLeft == NativeTypeKind::S32)
			left->computedValue->reg.s64 = left->computedValue->reg.s16;
		else if (newLeft == NativeTypeKind::S64)
			left->computedValue->reg.s64 = left->computedValue->reg.s16;
		else if (newLeft == NativeTypeKind::F32)
			left->computedValue->reg.f32 = static_cast<float>(left->computedValue->reg.s16);
		else if (newLeft == NativeTypeKind::F64)
			left->computedValue->reg.f64 = static_cast<double>(left->computedValue->reg.s16);
		break;
	case NativeTypeKind::S32:
		if (newLeft == NativeTypeKind::S64)
			left->computedValue->reg.s64 = left->computedValue->reg.s32;
		else if (newLeft == NativeTypeKind::S64)
			left->computedValue->reg.s64 = left->computedValue->reg.s32;
		else if (newLeft == NativeTypeKind::F32)
			left->computedValue->reg.f32 = static_cast<float>(left->computedValue->reg.s32);
		else if (newLeft == NativeTypeKind::F64)
			left->computedValue->reg.f64 = static_cast<double>(left->computedValue->reg.s32);
		break;
	case NativeTypeKind::S64:
		if (newLeft == NativeTypeKind::F64)
			left->computedValue->reg.f64 = static_cast<double>(left->computedValue->reg.s64);
		break;
	case NativeTypeKind::F32:
		if (newLeft == NativeTypeKind::F64)
			left->computedValue->reg.f64 = left->computedValue->reg.f32;
		break;
	default:
		break;
	}

	return true;
}
