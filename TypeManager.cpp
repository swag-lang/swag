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
            sourceFile->report({sourceFile, fromNode->token, format("value '%I64d' is negative and not in the range of 'u8'", fromNode->token.literalValue.s64)});
            return castResult::FailWithoutError;
        }

    case NativeType::UX:
        if (fromNode->token.literalValue.u64 > UINT8_MAX)
        {
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
            sourceFile->report({sourceFile, fromNode->token, format("value '%I64d' is negative and not in the range of 'u16'", fromNode->token.literalValue.s64)});
            return castResult::FailWithoutError;
        }

    case NativeType::UX:
        if (fromNode->token.literalValue.u64 > UINT16_MAX)
        {
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
            sourceFile->report({sourceFile, fromNode->token, format("value '%I64d' is negative and not in the range of 'u32'", fromNode->token.literalValue.s64)});
            return castResult::FailWithoutError;
        }

    case NativeType::UX:
        if (fromNode->token.literalValue.u64 > UINT32_MAX)
        {
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
            sourceFile->report({sourceFile, fromNode->token, format("value '%I64d' is negative and not in the range of 'u64'", fromNode->token.literalValue.s64)});
            return castResult::FailWithoutError;
        }

    case NativeType::UX:
        *result = &g_TypeInfoU64;
        return castResult::Ok;
    }

    return castResult::FailWithError;
}

castResult TypeManager::castToNative(SourceFile* sourceFile, AstNode* requestedTypeNode, AstNode* fromNode, uint32_t castFlags, TypeInfo** result)
{
    auto requestedTypeInfo = static_cast<NativeTypeInfo*>(requestedTypeNode->typeInfo);
    auto fromNodeInfo      = fromNode->typeInfo;

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
    else if (ok == castResult::FailWithError)
    {
        sourceFile->report({sourceFile, requestedTypeNode->token, "incompatible types"});
    }

    return false;
}