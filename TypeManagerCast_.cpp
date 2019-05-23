#include "pch.h"
#include "AstNode.h"
#include "TypeManager.h"
#include "Diagnostic.h"
#include "Global.h"

castResult TypeManager::castToNativeBool(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    if (nodeToCast->typeInfo == &g_TypeInfoBool)
        return castResult::Ok;
    return castResult::FailWithoutError;
}

castResult TypeManager::castToNativeU8(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
        if (nodeToCast->computedValue.variant.s64 < 0)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u8'", nodeToCast->computedValue.variant.s64)});
            return castResult::FailWithoutError;
        }

    case NativeType::UX:
        if (nodeToCast->computedValue.variant.u64 > UINT8_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64u' is not in the range of 'u8'", nodeToCast->computedValue.variant.u64)});
            return castResult::FailWithoutError;
        }

        nodeToCast->typeInfo = &g_TypeInfoU8;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeU16(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
        if (nodeToCast->computedValue.variant.s64 < 0)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u16'", nodeToCast->computedValue.variant.s64)});
            return castResult::FailWithoutError;
        }

    case NativeType::UX:
        if (nodeToCast->computedValue.variant.u64 > UINT16_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64u' is not in the range of 'u16'", nodeToCast->computedValue.variant.u64)});
            return castResult::FailWithoutError;
        }

        nodeToCast->typeInfo = &g_TypeInfoU16;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeU32(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
        if (nodeToCast->computedValue.variant.s64 < 0)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u32'", nodeToCast->computedValue.variant.s64)});
            return castResult::FailWithoutError;
        }

    case NativeType::UX:
        if (nodeToCast->computedValue.variant.u64 > UINT32_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64u' is not in the range of 'u32'", nodeToCast->computedValue.variant.u64)});
            return castResult::FailWithoutError;
        }

        nodeToCast->typeInfo = &g_TypeInfoU32;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeU64(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
        if (nodeToCast->computedValue.variant.s64 < 0)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u64'", nodeToCast->computedValue.variant.s64)});
            return castResult::FailWithoutError;
        }

    case NativeType::UX:
        nodeToCast->typeInfo = &g_TypeInfoU64;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeS8(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
    case NativeType::UX:
        if (nodeToCast->computedValue.variant.s64 < INT8_MIN || nodeToCast->computedValue.variant.s64 > INT8_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's8'", nodeToCast->computedValue.variant.s64)});
            return castResult::FailWithoutError;
        }

        nodeToCast->typeInfo = &g_TypeInfoS8;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeS16(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
    case NativeType::UX:
        if (nodeToCast->computedValue.variant.s64 < INT16_MIN || nodeToCast->computedValue.variant.s64 > INT16_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's16'", nodeToCast->computedValue.variant.s64)});
            return castResult::FailWithoutError;
        }

        nodeToCast->typeInfo = &g_TypeInfoS16;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeS32(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
    case NativeType::UX:
        if (nodeToCast->computedValue.variant.s64 < INT32_MIN || nodeToCast->computedValue.variant.s64 > INT32_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's32'", nodeToCast->computedValue.variant.s64)});
            return castResult::FailWithoutError;
        }

        nodeToCast->typeInfo = &g_TypeInfoS32;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeS64(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
    case NativeType::UX:
        if (nodeToCast->computedValue.variant.s64 < INT64_MIN || nodeToCast->computedValue.variant.s64 > INT64_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's64'", nodeToCast->computedValue.variant.s64)});
            return castResult::FailWithoutError;
        }

        nodeToCast->typeInfo = &g_TypeInfoS64;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeF32(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
    {
        float   tmpF = static_cast<float>(nodeToCast->computedValue.variant.s64);
        int64_t tmpI = static_cast<int64_t>(tmpF);
        if (tmpI != nodeToCast->computedValue.variant.s64)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is truncated in 'f32'", nodeToCast->computedValue.variant.s64)});
            return castResult::FailWithoutError;
        }

        nodeToCast->typeInfo = &g_TypeInfoF32;
        return castResult::Ok;
    }

    case NativeType::UX:
    {
        float    tmpF = static_cast<float>(nodeToCast->computedValue.variant.u64);
        uint64_t tmpI = static_cast<uint64_t>(tmpF);
        if (tmpI != nodeToCast->computedValue.variant.u64)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64u' is truncated in 'f64'", nodeToCast->computedValue.variant.u64)});
            return castResult::FailWithoutError;
        }

        nodeToCast->typeInfo = &g_TypeInfoF32;
        return castResult::Ok;
    }
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeF64(SourceFile* sourceFile, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (nodeToCast->typeInfo->nativeType)
    {
    case NativeType::SX:
    {
        double  tmpF = static_cast<double>(nodeToCast->computedValue.variant.s64);
        int64_t tmpI = static_cast<int64_t>(tmpF);
        if (tmpI != nodeToCast->computedValue.variant.s64)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64d' is truncated in 'f64'", nodeToCast->computedValue.variant.s64)});
            return castResult::FailWithoutError;
        }

        nodeToCast->typeInfo = &g_TypeInfoF64;
        return castResult::Ok;
    }

    case NativeType::UX:
    {
        double   tmpF = static_cast<double>(nodeToCast->computedValue.variant.u64);
        uint64_t tmpI = static_cast<uint64_t>(tmpF);
        if (tmpI != nodeToCast->computedValue.variant.u64)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, nodeToCast->token, format("value '%I64u' is truncated in 'f64'", nodeToCast->computedValue.variant.u64)});
            return castResult::FailWithoutError;
        }

        nodeToCast->typeInfo = &g_TypeInfoF64;
        return castResult::Ok;
    }
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNative(SourceFile* sourceFile, TypeInfo* toType, AstNode* nodeToCast, uint32_t castFlags)
{
    // Cast from native only for now
    if (!(toType->flags & TYPEINFO_NATIVE))
        return castResult::FailWithError;
    if (!(nodeToCast->flags & AST_VALUE_COMPUTED))
        return castResult::FailWithError;

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

    return castResult::FailWithError;
}

bool TypeManager::makeCompatibles(SourceFile* sourceFile, TypeInfo* requestedType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (nodeToCast->typeInfo == requestedType)
        return requestedType;

    castResult ok = castResult::FailWithError;
    if (requestedType->flags & TYPEINFO_NATIVE)
    {
        ok = castToNative(sourceFile, requestedType, nodeToCast, castFlags);
    }

    if (ok == castResult::Ok)
        return true;

    if (ok == castResult::FailWithError)
    {
        if (!(castFlags & CASTFLAG_NOERROR))
        {
            sourceFile->report({sourceFile,
                                nodeToCast->token,
                                format("can't cast from '%s' to '%s'", TypeManager::nativeTypeName(nodeToCast->typeInfo).c_str(), TypeManager::nativeTypeName(requestedType).c_str()).c_str()});
        }
    }

    return nullptr;
}

bool TypeManager::makeCompatibles(SourceFile* sourceFile, AstNode* requestedTypeNode, AstNode* nodeToCast, uint32_t castFlags)
{
    assert(requestedTypeNode && nodeToCast);
    return makeCompatibles(sourceFile, requestedTypeNode->typeInfo, nodeToCast, castFlags);
}
