#include "pch.h"
#include "AstNode.h"
#include "TypeManager.h"
#include "Diagnostic.h"
#include "Global.h"

castResult TypeManager::castToNativeU8(SourceFile* sourceFile, AstNode* fromNode, uint32_t castFlags, TypeInfo** result)
{
    switch (fromNode->typeInfo->nativeType)
    {
    case NativeType::SX:
        if (fromNode->token.literalValue.s64 < 0)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, fromNode->token, format("value '%I64d' is negative and not in the range of 'u8'", fromNode->token.literalValue.s64)});
            return castResult::FailWithoutError;
        }

    case NativeType::UX:
        if (fromNode->token.literalValue.u64 > UINT8_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, fromNode->token, format("value '%I64u' is not in the range of 'u8'", fromNode->token.literalValue.u64)});
            return castResult::FailWithoutError;
        }

        *result = &g_TypeInfoU8;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeU16(SourceFile* sourceFile, AstNode* fromNode, uint32_t castFlags, TypeInfo** result)
{
    switch (fromNode->typeInfo->nativeType)
    {
    case NativeType::SX:
        if (fromNode->token.literalValue.s64 < 0)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, fromNode->token, format("value '%I64d' is negative and not in the range of 'u16'", fromNode->token.literalValue.s64)});
            return castResult::FailWithoutError;
        }

    case NativeType::UX:
        if (fromNode->token.literalValue.u64 > UINT16_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, fromNode->token, format("value '%I64u' is not in the range of 'u16'", fromNode->token.literalValue.u64)});
            return castResult::FailWithoutError;
        }

        *result = &g_TypeInfoU16;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeU32(SourceFile* sourceFile, AstNode* fromNode, uint32_t castFlags, TypeInfo** result)
{
    switch (fromNode->typeInfo->nativeType)
    {
    case NativeType::SX:
        if (fromNode->token.literalValue.s64 < 0)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, fromNode->token, format("value '%I64d' is negative and not in the range of 'u32'", fromNode->token.literalValue.s64)});
            return castResult::FailWithoutError;
        }

    case NativeType::UX:
        if (fromNode->token.literalValue.u64 > UINT32_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, fromNode->token, format("value '%I64u' is not in the range of 'u32'", fromNode->token.literalValue.u64)});
            return castResult::FailWithoutError;
        }

        *result = &g_TypeInfoU32;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeU64(SourceFile* sourceFile, AstNode* fromNode, uint32_t castFlags, TypeInfo** result)
{
    switch (fromNode->typeInfo->nativeType)
    {
    case NativeType::SX:
        if (fromNode->token.literalValue.s64 < 0)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, fromNode->token, format("value '%I64d' is negative and not in the range of 'u64'", fromNode->token.literalValue.s64)});
            return castResult::FailWithoutError;
        }

    case NativeType::UX:
        *result = &g_TypeInfoU64;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeS8(SourceFile* sourceFile, AstNode* fromNode, uint32_t castFlags, TypeInfo** result)
{
    switch (fromNode->typeInfo->nativeType)
    {
    case NativeType::SX:
    case NativeType::UX:
        if (fromNode->token.literalValue.s64 < INT8_MIN || fromNode->token.literalValue.s64 > INT8_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, fromNode->token, format("value '%I64d' is not in the range of 's8'", fromNode->token.literalValue.s64)});
            return castResult::FailWithoutError;
        }

        *result = &g_TypeInfoS8;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeS16(SourceFile* sourceFile, AstNode* fromNode, uint32_t castFlags, TypeInfo** result)
{
    switch (fromNode->typeInfo->nativeType)
    {
    case NativeType::SX:
    case NativeType::UX:
        if (fromNode->token.literalValue.s64 < INT16_MIN || fromNode->token.literalValue.s64 > INT16_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, fromNode->token, format("value '%I64d' is not in the range of 's16'", fromNode->token.literalValue.s64)});
            return castResult::FailWithoutError;
        }

        *result = &g_TypeInfoS16;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeS32(SourceFile* sourceFile, AstNode* fromNode, uint32_t castFlags, TypeInfo** result)
{
    switch (fromNode->typeInfo->nativeType)
    {
    case NativeType::SX:
    case NativeType::UX:
        if (fromNode->token.literalValue.s64 < INT32_MIN || fromNode->token.literalValue.s64 > INT32_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, fromNode->token, format("value '%I64d' is not in the range of 's32'", fromNode->token.literalValue.s64)});
            return castResult::FailWithoutError;
        }

        *result = &g_TypeInfoS32;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeS64(SourceFile* sourceFile, AstNode* fromNode, uint32_t castFlags, TypeInfo** result)
{
    switch (fromNode->typeInfo->nativeType)
    {
    case NativeType::SX:
    case NativeType::UX:
        if (fromNode->token.literalValue.s64 < INT64_MIN || fromNode->token.literalValue.s64 > INT64_MAX)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, fromNode->token, format("value '%I64d' is not in the range of 's64'", fromNode->token.literalValue.s64)});
            return castResult::FailWithoutError;
        }

        *result = &g_TypeInfoS64;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeF32(SourceFile* sourceFile, AstNode* fromNode, uint32_t castFlags, TypeInfo** result)
{
    switch (fromNode->typeInfo->nativeType)
    {
    case NativeType::SX:
    {
        float   tmpF = static_cast<float>(fromNode->token.literalValue.s64);
        int64_t tmpI = static_cast<int64_t>(tmpF);
        if (tmpI != fromNode->token.literalValue.s64)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, fromNode->token, format("value '%I64d' is truncated in 'f32'", fromNode->token.literalValue.s64)});
            return castResult::FailWithoutError;
        }
        *result = &g_TypeInfoF32;
        return castResult::Ok;
    }

    case NativeType::UX:
    {
        float    tmpF = static_cast<float>(fromNode->token.literalValue.u64);
        uint64_t tmpI = static_cast<uint64_t>(tmpF);
        if (tmpI != fromNode->token.literalValue.u64)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, fromNode->token, format("value '%I64u' is truncated in 'f64'", fromNode->token.literalValue.u64)});
            return castResult::FailWithoutError;
        }
        *result = &g_TypeInfoF32;
        return castResult::Ok;
    }
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNativeF64(SourceFile* sourceFile, AstNode* fromNode, uint32_t castFlags, TypeInfo** result)
{
    switch (fromNode->typeInfo->nativeType)
    {
    case NativeType::SX:
    {
        double  tmpF = static_cast<double>(fromNode->token.literalValue.s64);
        int64_t tmpI = static_cast<int64_t>(tmpF);
        if (tmpI != fromNode->token.literalValue.s64)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, fromNode->token, format("value '%I64d' is truncated in 'f64'", fromNode->token.literalValue.s64)});
            return castResult::FailWithoutError;
        }

        *result = &g_TypeInfoF64;
        return castResult::Ok;
    }

    case NativeType::UX:
    {
        double   tmpF = static_cast<double>(fromNode->token.literalValue.u64);
        uint64_t tmpI = static_cast<uint64_t>(tmpF);
        if (tmpI != fromNode->token.literalValue.u64)
        {
            if (!(castFlags & CASTFLAG_NOERROR))
                sourceFile->report({sourceFile, fromNode->token, format("value '%I64u' is truncated in 'f64'", fromNode->token.literalValue.u64)});
            return castResult::FailWithoutError;
        }

        *result = &g_TypeInfoF64;
        return castResult::Ok;
    }
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNative(SourceFile* sourceFile, AstNode* requestedTypeNode, AstNode* fromNode, uint32_t castFlags, TypeInfo** result)
{
    auto fromNodeInfo = fromNode->typeInfo;

    // Cast from native only for now
    if (!(fromNodeInfo->flags & TYPEINFO_NATIVE))
        return castResult::FailWithError;
    if (!(fromNode->flags & AST_VALUE_COMPUTED))
        return castResult::FailWithError;

    switch (requestedTypeNode->typeInfo->nativeType)
    {
    case NativeType::U8:
        return castToNativeU8(sourceFile, fromNode, castFlags, result);
    case NativeType::U16:
        return castToNativeU16(sourceFile, fromNode, castFlags, result);
    case NativeType::U32:
        return castToNativeU32(sourceFile, fromNode, castFlags, result);
    case NativeType::U64:
        return castToNativeU64(sourceFile, fromNode, castFlags, result);
    case NativeType::S8:
        return castToNativeS8(sourceFile, fromNode, castFlags, result);
    case NativeType::S16:
        return castToNativeS16(sourceFile, fromNode, castFlags, result);
    case NativeType::S32:
        return castToNativeS32(sourceFile, fromNode, castFlags, result);
    case NativeType::S64:
        return castToNativeS64(sourceFile, fromNode, castFlags, result);
    case NativeType::F32:
        return castToNativeF32(sourceFile, fromNode, castFlags, result);
    }

    return castResult::FailWithError;
}

TypeInfo* TypeManager::makeCompatibles(SourceFile* sourceFile, AstNode* requestedTypeNode, AstNode* fromNode, uint32_t castFlags)
{
    if (!requestedTypeNode)
        return fromNode->typeInfo;
    if (!fromNode)
        return requestedTypeNode->typeInfo;
    if (requestedTypeNode->typeInfo == fromNode->typeInfo)
        return requestedTypeNode->typeInfo;

    TypeInfo*  resultType = nullptr;
    castResult ok         = castResult::FailWithError;
    if (requestedTypeNode->typeInfo->flags & TYPEINFO_NATIVE)
    {
        ok = castToNative(sourceFile, requestedTypeNode, fromNode, castFlags, &resultType);
    }

    if (ok == castResult::Ok)
    {
        assert(resultType);
        return resultType;
    }

    if (ok == castResult::FailWithError)
    {
        if (!(castFlags & CASTFLAG_NOERROR))
            sourceFile->report({sourceFile, requestedTypeNode->token, "incompatible types"});
    }

    return false;
}

string TypeManager::nativeTypeName(NativeType nativeType)
{
    switch (nativeType)
    {
    case NativeType::Bool:
        return "bool";
    case NativeType::Char:
        return "char";
    case NativeType::String:
        return "string";
    case NativeType::F32:
        return "f32";
    case NativeType::F64:
        return "f64";
    case NativeType::S8:
        return "s8";
    case NativeType::S16:
        return "s16";
    case NativeType::S32:
        return "s32";
    case NativeType::S64:
        return "s64";
    case NativeType::U8:
        return "u8";
    case NativeType::U16:
        return "u16";
    case NativeType::U32:
        return "u32";
    case NativeType::U64:
        return "u64";
    }

	return "???";
}