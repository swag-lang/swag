#include "pch.h"
#include "TypeManager.h"
#include "Diagnostic.h"
#include "Global.h"
#include "TypeInfo.h"
#include "SourceFile.h"

bool TypeManager::castError(SourceFile* sourceFile, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (!(castFlags & CASTFLAG_NOERROR))
    {
        sourceFile->report({sourceFile, nodeToCast->token, format("can't cast from '%s' to '%s'", fromType->name.c_str(), toType->name.c_str()).c_str()});
    }

    return false;
}

bool TypeManager::castToNativeBool(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (nodeToCast->typeInfo == g_TypeMgr.typeInfoBool)
        return true;
    return castError(sourceFile, g_TypeMgr.typeInfoBool, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU8(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (fromType->nativeType)
    {
    case NativeType::Char:
        if (!(castFlags & CASTFLAG_FORCE))
            break;
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
            nodeToCast->typeInfo = g_TypeMgr.typeInfoU8;
        return true;

    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            if (nodeToCast->computedValue.reg.s64 < 0)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u8'", nodeToCast->computedValue.reg.s64)});
                return false;
            }
        }

    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            if (nodeToCast->computedValue.reg.u64 > UINT8_MAX)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64u' is not in the range of 'u8'", nodeToCast->computedValue.reg.u64)});
                return false;
            }

            nodeToCast->typeInfo = g_TypeMgr.typeInfoU8;
        }

        return true;
    }

    return castError(sourceFile, g_TypeMgr.typeInfoU8, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU16(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (fromType->nativeType)
    {
    case NativeType::Char:
        if (!(castFlags & CASTFLAG_FORCE))
            break;
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
            nodeToCast->typeInfo = g_TypeMgr.typeInfoU16;
        return true;

    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            if (nodeToCast->computedValue.reg.s64 < 0)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u16'", nodeToCast->computedValue.reg.s64)});
                return false;
            }
        }

    case NativeType::U8:
    case NativeType::U32:
    case NativeType::U64:
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            if (nodeToCast->computedValue.reg.u64 > UINT16_MAX)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64u' is not in the range of 'u16'", nodeToCast->computedValue.reg.u64)});
                return false;
            }

            nodeToCast->typeInfo = g_TypeMgr.typeInfoU16;
        }

        return true;
    }

    return castError(sourceFile, g_TypeMgr.typeInfoU16, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU32(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (fromType->nativeType)
    {
    case NativeType::Char:
        if (!(castFlags & CASTFLAG_FORCE))
            break;
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
            nodeToCast->typeInfo = g_TypeMgr.typeInfoU32;
        return true;

    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            if (nodeToCast->computedValue.reg.s64 < 0)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u32'", nodeToCast->computedValue.reg.s64)});
                return false;
            }
        }

    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U64:
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            if (nodeToCast->computedValue.reg.u64 > UINT32_MAX)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64u' is not in the range of 'u32'", nodeToCast->computedValue.reg.u64)});
                return false;
            }

            nodeToCast->typeInfo = g_TypeMgr.typeInfoU32;
        }

        return true;
    }

    return castError(sourceFile, g_TypeMgr.typeInfoU32, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU64(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (fromType->nativeType)
    {
    case NativeType::Char:
        if (!(castFlags & CASTFLAG_FORCE))
            break;
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
            nodeToCast->typeInfo = g_TypeMgr.typeInfoU64;
        return true;

    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            if (nodeToCast->computedValue.reg.s64 < 0)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u64'", nodeToCast->computedValue.reg.s64)});
                return false;
            }
        }

    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
            nodeToCast->typeInfo = g_TypeMgr.typeInfoU64;
        return true;
    }

    return castError(sourceFile, g_TypeMgr.typeInfoU64, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS8(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (fromType->nativeType)
    {
    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    case NativeType::Char:
        if (!(castFlags & CASTFLAG_FORCE))
            break;
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
            nodeToCast->typeInfo = g_TypeMgr.typeInfoS8;
        return true;

    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            if (nodeToCast->computedValue.reg.s64 < INT8_MIN || nodeToCast->computedValue.reg.s64 > INT8_MAX)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's8'", nodeToCast->computedValue.reg.s64)});
                return false;
            }

            nodeToCast->typeInfo = g_TypeMgr.typeInfoS8;
        }

        return true;
    }

    return castError(sourceFile, g_TypeMgr.typeInfoS8, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS16(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (fromType->nativeType)
    {
    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    case NativeType::Char:
        if (!(castFlags & CASTFLAG_FORCE))
            break;
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
            nodeToCast->typeInfo = g_TypeMgr.typeInfoS16;
        return true;

    case NativeType::S8:
    case NativeType::S32:
    case NativeType::S64:
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            if (nodeToCast->computedValue.reg.s64 < INT16_MIN || nodeToCast->computedValue.reg.s64 > INT16_MAX)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's16'", nodeToCast->computedValue.reg.s64)});
                return false;
            }

            nodeToCast->typeInfo = g_TypeMgr.typeInfoS16;
        }

        return true;
    }

    return castError(sourceFile, g_TypeMgr.typeInfoS16, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS32(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::Char:
        if (!(castFlags & CASTFLAG_FORCE))
            break;
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
            nodeToCast->typeInfo = g_TypeMgr.typeInfoS32;
        return true;

    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S64:
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            if (nodeToCast->computedValue.reg.s64 < INT32_MIN || nodeToCast->computedValue.reg.s64 > INT32_MAX)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's32'", nodeToCast->computedValue.reg.s64)});
                return false;
            }

            nodeToCast->typeInfo = g_TypeMgr.typeInfoS32;
        }

        return true;
    }

    return castError(sourceFile, g_TypeMgr.typeInfoS32, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS64(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (fromType->nativeType)
    {
    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    case NativeType::Char:
        if (!(castFlags & CASTFLAG_FORCE))
            break;
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
            nodeToCast->typeInfo = g_TypeMgr.typeInfoS64;
        return true;

    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            if (nodeToCast->computedValue.reg.s64 < INT64_MIN || nodeToCast->computedValue.reg.s64 > INT64_MAX)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's64'", nodeToCast->computedValue.reg.s64)});
                return false;
            }

            nodeToCast->typeInfo = g_TypeMgr.typeInfoS64;
        }

        return true;
    }

    return castError(sourceFile, g_TypeMgr.typeInfoS64, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeF32(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (fromType->nativeType)
    {
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
    {
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            float   tmpF = static_cast<float>(nodeToCast->computedValue.reg.s64);
            int64_t tmpI = static_cast<int64_t>(tmpF);
            if (tmpI != nodeToCast->computedValue.reg.s64)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is truncated in 'f32'", nodeToCast->computedValue.reg.s64)});
                return false;
            }

            nodeToCast->computedValue.reg.f32 = static_cast<float>(nodeToCast->computedValue.reg.s64);
            nodeToCast->typeInfo              = g_TypeMgr.typeInfoF32;
        }

        return true;
    }

    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    {
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            float    tmpF = static_cast<float>(nodeToCast->computedValue.reg.u64);
            uint64_t tmpI = static_cast<uint64_t>(tmpF);
            if (tmpI != nodeToCast->computedValue.reg.u64)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64u' is truncated in 'f64'", nodeToCast->computedValue.reg.u64)});
                return false;
            }

            nodeToCast->computedValue.reg.f64 = static_cast<float>(nodeToCast->computedValue.reg.u64);
            nodeToCast->typeInfo              = g_TypeMgr.typeInfoF32;
        }

        return true;
    }

    case NativeType::F64:
    {
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            float  tmpF = static_cast<float>(nodeToCast->computedValue.reg.f64);
            double tmpD = static_cast<double>(tmpF);
            if (tmpD != nodeToCast->computedValue.reg.f64)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%Lf' is truncated in 'f32'", nodeToCast->computedValue.reg.f64)});
                return false;
            }

            nodeToCast->computedValue.reg.f32 = static_cast<float>(nodeToCast->computedValue.reg.f64);
            nodeToCast->typeInfo              = g_TypeMgr.typeInfoF32;
        }

        return true;
    }
    }

    return castError(sourceFile, g_TypeMgr.typeInfoF32, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeF64(SourceFile* sourceFile, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (fromType->nativeType)
    {
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
    {
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            double  tmpF = static_cast<double>(nodeToCast->computedValue.reg.s64);
            int64_t tmpI = static_cast<int64_t>(tmpF);
            if (tmpI != nodeToCast->computedValue.reg.s64)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is truncated in 'f64'", nodeToCast->computedValue.reg.s64)});
                return false;
            }

            nodeToCast->computedValue.reg.f64 = static_cast<double>(nodeToCast->computedValue.reg.s64);
            nodeToCast->typeInfo              = g_TypeMgr.typeInfoF64;
        }

        return true;
    }

    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    {
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            double   tmpF = static_cast<double>(nodeToCast->computedValue.reg.u64);
            uint64_t tmpI = static_cast<uint64_t>(tmpF);
            if (tmpI != nodeToCast->computedValue.reg.u64)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64u' is truncated in 'f64'", nodeToCast->computedValue.reg.u64)});
                return false;
            }

            nodeToCast->computedValue.reg.f64 = static_cast<double>(nodeToCast->computedValue.reg.u64);
            nodeToCast->typeInfo              = g_TypeMgr.typeInfoF64;
        }

        return true;
    }

    case NativeType::F32:
        if (nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            nodeToCast->computedValue.reg.f64 = static_cast<double>(nodeToCast->computedValue.reg.f32);
            nodeToCast->typeInfo              = g_TypeMgr.typeInfoF64;
        }
        return true;
    }

    return castError(sourceFile, g_TypeMgr.typeInfoF64, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNative(SourceFile* sourceFile, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (toType->nativeType)
    {
    case NativeType::Bool:
        return castToNativeBool(sourceFile, fromType, nodeToCast, castFlags);
    case NativeType::U8:
        return castToNativeU8(sourceFile, fromType, nodeToCast, castFlags);
    case NativeType::U16:
        return castToNativeU16(sourceFile, fromType, nodeToCast, castFlags);
    case NativeType::U32:
        return castToNativeU32(sourceFile, fromType, nodeToCast, castFlags);
    case NativeType::U64:
        return castToNativeU64(sourceFile, fromType, nodeToCast, castFlags);
    case NativeType::S8:
        return castToNativeS8(sourceFile, fromType, nodeToCast, castFlags);
    case NativeType::S16:
        return castToNativeS16(sourceFile, fromType, nodeToCast, castFlags);
    case NativeType::S32:
        return castToNativeS32(sourceFile, fromType, nodeToCast, castFlags);
    case NativeType::S64:
        return castToNativeS64(sourceFile, fromType, nodeToCast, castFlags);
    case NativeType::F32:
        return castToNativeF32(sourceFile, fromType, nodeToCast, castFlags);
    case NativeType::F64:
        return castToNativeF64(sourceFile, fromType, nodeToCast, castFlags);
    }

    return castError(sourceFile, toType, fromType, nodeToCast, castFlags);
}

bool TypeManager::makeCompatibles(SourceFile* sourceFile, TypeInfo* toType, AstNode* nodeToCast, uint32_t castFlags)
{
    auto fromType = nodeToCast->typeInfo;

    if (toType->kind == TypeInfoKind::FunctionAttribute)
        toType = CastTypeInfo<TypeInfoFuncAttr>(toType, TypeInfoKind::FunctionAttribute)->returnType;
    if (fromType->kind == TypeInfoKind::FunctionAttribute)
        fromType = CastTypeInfo<TypeInfoFuncAttr>(fromType, TypeInfoKind::FunctionAttribute)->returnType;

    if (fromType == toType)
        return true;
    if (fromType->isSame(toType))
        return true;

    if (toType->kind == TypeInfoKind::Native)
    {
        return castToNative(sourceFile, toType, fromType, nodeToCast, castFlags);
    }

    return castError(sourceFile, toType, fromType, nodeToCast, castFlags);
}

bool TypeManager::makeCompatibles(SourceFile* sourceFile, AstNode* leftNode, AstNode* rightNode, uint32_t castFlags)
{
    auto leftType  = leftNode->typeInfo;
    auto rightType = rightNode->typeInfo;

    if ((leftType->flags & TYPEINFO_INT_SIGNED) && (rightType->flags & TYPEINFO_FLOAT))
        return makeCompatibles(sourceFile, rightType, leftNode, castFlags);

    return makeCompatibles(sourceFile, leftType, rightNode, castFlags);
}

void TypeManager::promote(AstNode* left, AstNode* right)
{
    if (left->typeInfo == right->typeInfo)
        return;
    if ((left->typeInfo->kind != TypeInfoKind::Native) || (right->typeInfo->kind != TypeInfoKind::Native))
        return;

    promoteInteger(left);
    promoteInteger(right);

    if (left->typeInfo->nativeType == NativeType::S32 && right->typeInfo->nativeType == NativeType::S64)
    {
        if (left->flags & AST_VALUE_COMPUTED)
        {
            left->typeInfo = g_TypeMgr.typeInfoS64;
            return;
        }
    }

    if (left->typeInfo->nativeType == NativeType::S64 && right->typeInfo->nativeType == NativeType::S32)
    {
        if (right->flags & AST_VALUE_COMPUTED)
        {
            right->typeInfo = g_TypeMgr.typeInfoS64;
            return;
        }
    }

    if (left->typeInfo->nativeType == NativeType::U32 && right->typeInfo->nativeType == NativeType::U64)
    {
        if (left->flags & AST_VALUE_COMPUTED)
        {
            left->typeInfo = g_TypeMgr.typeInfoU64;
            return;
        }
    }

    if (left->typeInfo->nativeType == NativeType::U64 && right->typeInfo->nativeType == NativeType::U32)
    {
        if (right->flags & AST_VALUE_COMPUTED)
        {
            right->typeInfo = g_TypeMgr.typeInfoU64;
            return;
        }
    }

    if (left->typeInfo->nativeType == NativeType::F32 && right->typeInfo->nativeType == NativeType::F64)
    {
        if (left->flags & AST_VALUE_COMPUTED)
        {
            left->computedValue.reg.f64 = left->computedValue.reg.f32;
            left->typeInfo              = g_TypeMgr.typeInfoF64;
            return;
        }
    }

    if (left->typeInfo->nativeType == NativeType::F64 && right->typeInfo->nativeType == NativeType::F32)
    {
        if (right->flags & AST_VALUE_COMPUTED)
        {
            right->computedValue.reg.f64 = right->computedValue.reg.f32;
            right->typeInfo              = g_TypeMgr.typeInfoF64;
            return;
        }
    }
}

void TypeManager::promoteInteger(AstNode* node)
{
    auto typeInfo = node->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Native)
        return;

    switch (typeInfo->nativeType)
    {
    case NativeType::U8:
        if (node->flags & AST_VALUE_COMPUTED)
        {
            node->computedValue.reg.u32 = node->computedValue.reg.u8;
            node->typeInfo              = g_TypeMgr.typeInfoU32;
        }
        else
            node->castedTypeInfo = g_TypeMgr.typeInfoU32;
        break;
    case NativeType::U16:
        if (node->flags & AST_VALUE_COMPUTED)
        {
            node->computedValue.reg.u32 = node->computedValue.reg.u16;
            node->typeInfo              = g_TypeMgr.typeInfoU32;
        }
        else
            node->castedTypeInfo = g_TypeMgr.typeInfoU32;
        break;
    case NativeType::S8:
        if (node->flags & AST_VALUE_COMPUTED)
        {
            node->computedValue.reg.s32 = node->computedValue.reg.s8;
            node->typeInfo              = g_TypeMgr.typeInfoS32;
        }
        else
            node->castedTypeInfo = g_TypeMgr.typeInfoS32;
        break;
    case NativeType::S16:
        if (node->flags & AST_VALUE_COMPUTED)
        {
            node->computedValue.reg.s32 = node->computedValue.reg.s16;
            node->typeInfo              = g_TypeMgr.typeInfoS32;
        }
        else
            node->castedTypeInfo = g_TypeMgr.typeInfoS32;
        break;
    }
}
