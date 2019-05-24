#include "pch.h"
#include "AstNode.h"
#include "TypeManager.h"
#include "Diagnostic.h"
#include "Global.h"

bool TypeManager::castError(SourceFile* sourceFile, TypeInfo* requestedType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (!(castFlags & CASTFLAG_NOERROR))
    {
        sourceFile->report({sourceFile,
                            nodeToCast->token,
                            format("can't cast from '%s' to '%s'", TypeManager::nativeTypeName(nodeToCast->typeInfo).c_str(), TypeManager::nativeTypeName(requestedType).c_str()).c_str()});
    }

    return false;
}

bool TypeManager::castToNativeBool(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    if (nodeToCast->typeInfo == &g_TypeInfoBool)
        return true;
    return castError(sourceFile, &g_TypeInfoBool, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU8(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
        if (nodeToCast->computedValue.variant.s64 < 0)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u8'", nodeToCast->computedValue.variant.s64)});
            return false;
        }

    case NativeType::UX:
        if (nodeToCast->computedValue.variant.u64 > UINT8_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64u' is not in the range of 'u8'", nodeToCast->computedValue.variant.u64)});
            return false;
        }

        nodeToCast->typeInfo = &g_TypeInfoU8;
        return true;
    }

    return castError(sourceFile, &g_TypeInfoU8, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU16(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
        if (nodeToCast->computedValue.variant.s64 < 0)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u16'", nodeToCast->computedValue.variant.s64)});
            return false;
        }

    case NativeType::UX:
        if (nodeToCast->computedValue.variant.u64 > UINT16_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64u' is not in the range of 'u16'", nodeToCast->computedValue.variant.u64)});
            return false;
        }

        nodeToCast->typeInfo = &g_TypeInfoU16;
        return true;
    }

    return castError(sourceFile, &g_TypeInfoU16, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU32(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
        if (nodeToCast->computedValue.variant.s64 < 0)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u32'", nodeToCast->computedValue.variant.s64)});
            return false;
        }

    case NativeType::UX:
        if (nodeToCast->computedValue.variant.u64 > UINT32_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64u' is not in the range of 'u32'", nodeToCast->computedValue.variant.u64)});
            return false;
        }

        nodeToCast->typeInfo = &g_TypeInfoU32;
        return true;
    }

    return castError(sourceFile, &g_TypeInfoU32, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU64(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
        if (nodeToCast->computedValue.variant.s64 < 0)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u64'", nodeToCast->computedValue.variant.s64)});
            return false;
        }

    case NativeType::UX:
        nodeToCast->typeInfo = &g_TypeInfoU64;
        return true;
    }

    return castError(sourceFile, &g_TypeInfoU64, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS8(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
    case NativeType::UX:
        if (nodeToCast->computedValue.variant.s64 < INT8_MIN || nodeToCast->computedValue.variant.s64 > INT8_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's8'", nodeToCast->computedValue.variant.s64)});
            return false;
        }

        nodeToCast->typeInfo = &g_TypeInfoS8;
        return true;
    }

    return castError(sourceFile, &g_TypeInfoS8, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS16(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
    case NativeType::UX:
        if (nodeToCast->computedValue.variant.s64 < INT16_MIN || nodeToCast->computedValue.variant.s64 > INT16_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's16'", nodeToCast->computedValue.variant.s64)});
            return false;
        }

        nodeToCast->typeInfo = &g_TypeInfoS16;
        return true;
    }

    return castError(sourceFile, &g_TypeInfoS16, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS32(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
    case NativeType::UX:
        if (nodeToCast->computedValue.variant.s64 < INT32_MIN || nodeToCast->computedValue.variant.s64 > INT32_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's32'", nodeToCast->computedValue.variant.s64)});
            return false;
        }

        nodeToCast->typeInfo = &g_TypeInfoS32;
        return true;
    }

    return castError(sourceFile, &g_TypeInfoS32, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS64(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
    case NativeType::UX:
        if (nodeToCast->computedValue.variant.s64 < INT64_MIN || nodeToCast->computedValue.variant.s64 > INT64_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's64'", nodeToCast->computedValue.variant.s64)});
            return false;
        }

        nodeToCast->typeInfo = &g_TypeInfoS64;
        return true;
    }

    return castError(sourceFile, &g_TypeInfoS64, nodeToCast, castFlags);
}

bool TypeManager::castToNativeF32(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::SX:
    {
        float   tmpF = static_cast<float>(nodeToCast->computedValue.variant.s64);
        int64_t tmpI = static_cast<int64_t>(tmpF);
        if (tmpI != nodeToCast->computedValue.variant.s64)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is truncated in 'f32'", nodeToCast->computedValue.variant.s64)});
            return false;
        }

        nodeToCast->typeInfo = &g_TypeInfoF32;
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
            nodeToCast->computedValue.variant.f64 = static_cast<float>(nodeToCast->computedValue.variant.s64);
        return true;
    }

    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    case NativeType::UX:
    {
        float    tmpF = static_cast<float>(nodeToCast->computedValue.variant.u64);
        uint64_t tmpI = static_cast<uint64_t>(tmpF);
        if (tmpI != nodeToCast->computedValue.variant.u64)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64u' is truncated in 'f64'", nodeToCast->computedValue.variant.u64)});
            return false;
        }

        nodeToCast->typeInfo = &g_TypeInfoF32;
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
            nodeToCast->computedValue.variant.f64 = static_cast<float>(nodeToCast->computedValue.variant.u64);
        return true;
    }

    case NativeType::FX:
    case NativeType::F64:
    {
        float  tmpF = static_cast<float>(nodeToCast->computedValue.variant.f64);
        double tmpD = static_cast<double>(tmpF);
        if (tmpD != nodeToCast->computedValue.variant.f64)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%Lf' is truncated in 'f32'", nodeToCast->computedValue.variant.f64)});
            return false;
        }
        nodeToCast->typeInfo = &g_TypeInfoF32;
        return true;
    }
    }

    return castError(sourceFile, &g_TypeInfoF32, nodeToCast, castFlags);
}

bool TypeManager::castToNativeF64(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::SX:
    {
        double  tmpF = static_cast<double>(nodeToCast->computedValue.variant.s64);
        int64_t tmpI = static_cast<int64_t>(tmpF);
        if (tmpI != nodeToCast->computedValue.variant.s64)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is truncated in 'f64'", nodeToCast->computedValue.variant.s64)});
            return false;
        }

        nodeToCast->typeInfo = &g_TypeInfoF64;
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
            nodeToCast->computedValue.variant.f64 = static_cast<double>(nodeToCast->computedValue.variant.s64);
        return true;
    }

    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    case NativeType::UX:
    {
        double   tmpF = static_cast<double>(nodeToCast->computedValue.variant.u64);
        uint64_t tmpI = static_cast<uint64_t>(tmpF);
        if (tmpI != nodeToCast->computedValue.variant.u64)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64u' is truncated in 'f64'", nodeToCast->computedValue.variant.u64)});
            return false;
        }

        nodeToCast->typeInfo = &g_TypeInfoF64;
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
            nodeToCast->computedValue.variant.f64 = static_cast<double>(nodeToCast->computedValue.variant.u64);
        return true;
    }

    case NativeType::FX:
	case NativeType::F32:
        nodeToCast->typeInfo = &g_TypeInfoF64;
        return true;
    }

    return castError(sourceFile, &g_TypeInfoF64, nodeToCast, castFlags);
}

bool TypeManager::castToNative(SourceFile* sourceFile, TypeInfo* toType, AstNode* nodeToCast, uint32_t castFlags)
{
    // Cast from native only for now
    if (!(toType->flags & TYPEINFO_NATIVE))
        return false;
    if (!(nodeToCast->flags & AST_VALUE_COMPUTED))
        return false;

    switch (toType->nativeType)
    {
    case NativeType::Bool:
        return castToNativeBool(sourceFile, nodeToCast, castFlags);
    case NativeType::U8:
        return castToNativeU8(sourceFile, nodeToCast, castFlags);
    case NativeType::U16:
        return castToNativeU16(sourceFile, nodeToCast, castFlags);
    case NativeType::U32:
        return castToNativeU32(sourceFile, nodeToCast, castFlags);
    case NativeType::U64:
        return castToNativeU64(sourceFile, nodeToCast, castFlags);
    case NativeType::S8:
        return castToNativeS8(sourceFile, nodeToCast, castFlags);
    case NativeType::S16:
        return castToNativeS16(sourceFile, nodeToCast, castFlags);
    case NativeType::S32:
        return castToNativeS32(sourceFile, nodeToCast, castFlags);
    case NativeType::S64:
        return castToNativeS64(sourceFile, nodeToCast, castFlags);
    case NativeType::F32:
        return castToNativeF32(sourceFile, nodeToCast, castFlags);
    case NativeType::F64:
        return castToNativeF64(sourceFile, nodeToCast, castFlags);
    }

    return castError(sourceFile, toType, nodeToCast, castFlags);
}

bool TypeManager::makeCompatibles(SourceFile* sourceFile, TypeInfo* toType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (nodeToCast->typeInfo == toType)
        return true;

    if (toType->flags & TYPEINFO_NATIVE)
    {
        return castToNative(sourceFile, toType, nodeToCast, castFlags);
    }

    return castError(sourceFile, toType, nodeToCast, castFlags);
}

bool TypeManager::makeCompatibles(SourceFile* sourceFile, AstNode* leftNode, AstNode* rightNode, uint32_t castFlags)
{
    if (leftNode->typeInfo == rightNode->typeInfo)
        return true;

    auto leftType  = leftNode->typeInfo;
    auto rightType = rightNode->typeInfo;

    if ((leftType->flags & TYPEINFO_INT_SIGNED) && (rightType->flags & TYPEINFO_FLOAT))
        return castToNative(sourceFile, rightType, leftNode, castFlags);

    return castToNative(sourceFile, leftType, rightNode, castFlags);
}

void TypeManager::promote(AstNode* left, AstNode* right)
{
    if (left->typeInfo == right->typeInfo)
        return;
    if (!(left->typeInfo->flags & TYPEINFO_NATIVE) || !(right->typeInfo->flags & TYPEINFO_NATIVE))
        return;

    promoteInteger(left);
    promoteInteger(right);

    if (left->typeInfo->nativeType == NativeType::S64 && right->typeInfo->nativeType == NativeType::S32)
    {
        right->typeInfo = &g_TypeInfoS64;
        return;
    }

    if (left->typeInfo->nativeType == NativeType::S32 && right->typeInfo->nativeType == NativeType::S64)
    {
        left->typeInfo = &g_TypeInfoS64;
        return;
    }

    if (left->typeInfo->nativeType == NativeType::U64 && right->typeInfo->nativeType == NativeType::U32)
    {
        right->typeInfo = &g_TypeInfoU64;
        return;
    }

    if (left->typeInfo->nativeType == NativeType::U32 && right->typeInfo->nativeType == NativeType::U64)
    {
        left->typeInfo = &g_TypeInfoU64;
        return;
    }

    if (left->typeInfo->nativeType == NativeType::F64 && right->typeInfo->nativeType == NativeType::F32)
    {
        right->typeInfo = &g_TypeInfoF64;
        return;
    }

    if (left->typeInfo->nativeType == NativeType::F32 && right->typeInfo->nativeType == NativeType::F64)
    {
        left->typeInfo = &g_TypeInfoF64;
        return;
    }

    if (left->typeInfo->nativeType == NativeType::FX && right->typeInfo->nativeType == NativeType::F32)
    {
        left->typeInfo = &g_TypeInfoF32;
        return;
    }

	if (left->typeInfo->nativeType == NativeType::FX && right->typeInfo->nativeType == NativeType::F64)
    {
            left->typeInfo = &g_TypeInfoF64;
        return;
    }

	if (left->typeInfo->nativeType == NativeType::F64 && right->typeInfo->nativeType == NativeType::FX)
    {
        right->typeInfo = &g_TypeInfoF64;
        return;
    }
}

void TypeManager::promoteInteger(AstNode* node)
{
    if (!(node->flags & TYPEINFO_NATIVE))
        return;

    if (node->typeInfo == &g_TypeInfoU8 || node->typeInfo == &g_TypeInfoU16)
    {
        node->typeInfo = &g_TypeInfoU32;
        return;
    }

    if (node->typeInfo == &g_TypeInfoS8 || node->typeInfo == &g_TypeInfoS16)
    {
        node->typeInfo = &g_TypeInfoS32;
        return;
    }
}