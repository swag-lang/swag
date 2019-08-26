#include "pch.h"
#include "TypeManager.h"
#include "Diagnostic.h"
#include "Global.h"
#include "TypeInfo.h"
#include "SourceFile.h"

bool TypeManager::castError(ErrorContext* errorContext, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags, bool explicitIsValid)
{
    if (!(castFlags & CASTFLAG_NOERROR))
    {
        assert(nodeToCast);
        if (explicitIsValid)
            errorContext->report({errorContext->sourceFile, nodeToCast->token, format("can't implicitly cast from '%s' to '%s'", fromType->name.c_str(), toType->name.c_str()).c_str()});
        else
            errorContext->report({errorContext->sourceFile, nodeToCast->token, format("can't cast from '%s' to '%s'", fromType->name.c_str(), toType->name.c_str()).c_str()});
    }

    return false;
}

bool TypeManager::castToNativeBool(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (fromType == g_TypeMgr.typeInfoBool)
        return true;
    return castError(errorContext, g_TypeMgr.typeInfoBool, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU8(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeType::Char:
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
        case NativeType::U8:
        case NativeType::U16:
        case NativeType::U32:
        case NativeType::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU8;
                }
            }
            return true;

        case NativeType::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.u8 = static_cast<uint8_t>(nodeToCast->computedValue.reg.f32);
                    nodeToCast->typeInfo             = g_TypeMgr.typeInfoU8;
                }
            }
            return true;

        case NativeType::F64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.u8 = static_cast<uint8_t>(nodeToCast->computedValue.reg.f64);
                    nodeToCast->typeInfo             = g_TypeMgr.typeInfoU8;
                }
            }
            return true;
        }
    }
    else
    {
        switch (fromType->nativeType)
        {
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u8'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }
            }

        case NativeType::U16:
        case NativeType::U32:
        case NativeType::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.u64 > UINT8_MAX)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%I64u' is not in the range of 'u8'", nodeToCast->computedValue.reg.u64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU8;
                }
                return true;
            }
            break;
        }
    }

    return castError(errorContext, g_TypeMgr.typeInfoU8, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU16(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeType::Char:
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
        case NativeType::U8:
        case NativeType::U16:
        case NativeType::U32:
        case NativeType::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU16;
                }
            }
            return true;

        case NativeType::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.u16 = static_cast<uint16_t>(nodeToCast->computedValue.reg.f32);
                    nodeToCast->typeInfo              = g_TypeMgr.typeInfoU16;
                }
            }
            return true;

        case NativeType::F64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.u16 = static_cast<uint16_t>(nodeToCast->computedValue.reg.f64);
                    nodeToCast->typeInfo              = g_TypeMgr.typeInfoU16;
                }
            }
            return true;
        }
    }
    else
    {
        switch (fromType->nativeType)
        {
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u16'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }
            }

        case NativeType::U8:
        case NativeType::U32:
        case NativeType::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.u64 > UINT16_MAX)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%I64u' is not in the range of 'u16'", nodeToCast->computedValue.reg.u64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU16;
                }
                return true;
            }
            break;
        }
    }

    return castError(errorContext, g_TypeMgr.typeInfoU16, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU32(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeType::Char:
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
        case NativeType::U8:
        case NativeType::U16:
        case NativeType::U32:
        case NativeType::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU32;
                }
            }
            return true;

        case NativeType::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.u32 = static_cast<uint32_t>(nodeToCast->computedValue.reg.f32);
                    nodeToCast->typeInfo              = g_TypeMgr.typeInfoU32;
                }
            }
            return true;

        case NativeType::F64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.u32 = static_cast<uint32_t>(nodeToCast->computedValue.reg.f64);
                    nodeToCast->typeInfo              = g_TypeMgr.typeInfoU32;
                }
            }
            return true;
        }
    }
    else
    {
        switch (fromType->nativeType)
        {
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u32'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }
            }

        case NativeType::U8:
        case NativeType::U16:
        case NativeType::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.u64 > UINT32_MAX)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%I64u' is not in the range of 'u32'", nodeToCast->computedValue.reg.u64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU32;
                }
                return true;
            }
            break;
        }
    }

    return castError(errorContext, g_TypeMgr.typeInfoU32, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU64(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeType::Char:
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
        case NativeType::U8:
        case NativeType::U16:
        case NativeType::U32:
        case NativeType::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU64;
                }
            }
            return true;

        case NativeType::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.u64 = static_cast<uint64_t>(nodeToCast->computedValue.reg.f32);
                    nodeToCast->typeInfo              = g_TypeMgr.typeInfoU64;
                }
            }
            return true;

        case NativeType::F64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.u64 = static_cast<uint64_t>(nodeToCast->computedValue.reg.f64);
                    nodeToCast->typeInfo              = g_TypeMgr.typeInfoU64;
                }
            }
            return true;
        }
    }
    else
    {
        switch (fromType->nativeType)
        {
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u64'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }
            }

        case NativeType::U8:
        case NativeType::U16:
        case NativeType::U32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU64;
                }
            }
            return true;
        }
    }

    return castError(errorContext, g_TypeMgr.typeInfoU64, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS8(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeType::Char:
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
        case NativeType::U8:
        case NativeType::U16:
        case NativeType::U32:
        case NativeType::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS8;
                }
            }
            return true;

        case NativeType::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.s8 = static_cast<int8_t>(nodeToCast->computedValue.reg.f32);
                    nodeToCast->typeInfo             = g_TypeMgr.typeInfoS8;
                }
            }
            return true;

        case NativeType::F64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.s8 = static_cast<int8_t>(nodeToCast->computedValue.reg.f64);
                    nodeToCast->typeInfo             = g_TypeMgr.typeInfoS8;
                }
            }
            return true;
        }
    }
    else
    {
        switch (fromType->nativeType)
        {
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < INT8_MIN || nodeToCast->computedValue.reg.s64 > INT8_MAX)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's8'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS8;
                }
                return true;
            }
            break;
        }
    }

    return castError(errorContext, g_TypeMgr.typeInfoS8, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS16(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeType::Char:
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
        case NativeType::U8:
        case NativeType::U16:
        case NativeType::U32:
        case NativeType::U64:
            if (nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS16;
                }
            }
            return true;

        case NativeType::F32:
            if (nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.s16 = static_cast<int16_t>(nodeToCast->computedValue.reg.f32);
                    nodeToCast->typeInfo              = g_TypeMgr.typeInfoS16;
                }
            }
            return true;

        case NativeType::F64:
            if (nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                nodeToCast->computedValue.reg.s16 = static_cast<int16_t>(nodeToCast->computedValue.reg.f64);
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS16;
                }
            }
            return true;
        }
    }
    else
    {
        switch (fromType->nativeType)
        {
        case NativeType::S8:
        case NativeType::S32:
        case NativeType::S64:
            if (nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < INT16_MIN || nodeToCast->computedValue.reg.s64 > INT16_MAX)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's16'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS16;
                }
                return true;
            }
            break;
        }
    }

    return castError(errorContext, g_TypeMgr.typeInfoS16, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS32(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeType::Char:
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
        case NativeType::U8:
        case NativeType::U16:
        case NativeType::U32:
        case NativeType::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS32;
                }
            }
            return true;

        case NativeType::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                nodeToCast->computedValue.reg.s32 = static_cast<int32_t>(nodeToCast->computedValue.reg.f32);
                nodeToCast->typeInfo              = g_TypeMgr.typeInfoS32;
            }
            return true;

        case NativeType::F64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                nodeToCast->computedValue.reg.s32 = static_cast<int32_t>(nodeToCast->computedValue.reg.f64);
                nodeToCast->typeInfo              = g_TypeMgr.typeInfoS32;
            }
            return true;
        }
    }
    else
    {
        switch (fromType->nativeType)
        {
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < INT32_MIN || nodeToCast->computedValue.reg.s64 > INT32_MAX)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's32'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS32;
                }
                return true;
            }
            break;
        }
    }

    return castError(errorContext, g_TypeMgr.typeInfoS32, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS64(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeType::Char:
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
        case NativeType::U8:
        case NativeType::U16:
        case NativeType::U32:
        case NativeType::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS64;
                }
            }
            return true;

        case NativeType::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                nodeToCast->computedValue.reg.s64 = static_cast<int64_t>(nodeToCast->computedValue.reg.f32);
                nodeToCast->typeInfo              = g_TypeMgr.typeInfoS64;
            }
            return true;

        case NativeType::F64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                nodeToCast->computedValue.reg.s64 = static_cast<int64_t>(nodeToCast->computedValue.reg.f64);
                nodeToCast->typeInfo              = g_TypeMgr.typeInfoS64;
            }
            return true;
        }
    }
    else
    {
        switch (fromType->nativeType)
        {
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < INT64_MIN || nodeToCast->computedValue.reg.s64 > INT64_MAX)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's64'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS64;
                }
                return true;
            }
            break;
        }
    }

    return castError(errorContext, g_TypeMgr.typeInfoS64, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeF32(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (fromType->nativeType)
    {
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
    {
        if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            float   tmpF = static_cast<float>(nodeToCast->computedValue.reg.s64);
            int64_t tmpI = static_cast<int64_t>(tmpF);
            if (tmpI != nodeToCast->computedValue.reg.s64)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%I64d' is truncated in 'f32'", nodeToCast->computedValue.reg.s64)});
                return false;
            }

            nodeToCast->computedValue.reg.f32 = static_cast<float>(nodeToCast->computedValue.reg.s64);
            nodeToCast->typeInfo              = g_TypeMgr.typeInfoF32;
            return true;
        }
        else if (castFlags & CASTFLAG_FORCE)
            return true;
        break;
    }

    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    {
        if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            float    tmpF = static_cast<float>(nodeToCast->computedValue.reg.u64);
            uint64_t tmpI = static_cast<uint64_t>(tmpF);
            if (tmpI != nodeToCast->computedValue.reg.u64)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%I64u' is truncated in 'f64'", nodeToCast->computedValue.reg.u64)});
                return false;
            }

            nodeToCast->computedValue.reg.f32 = static_cast<float>(nodeToCast->computedValue.reg.u64);
            nodeToCast->typeInfo              = g_TypeMgr.typeInfoF32;
            return true;
        }
        else if (castFlags & CASTFLAG_FORCE)
            return true;
        break;
    }

    case NativeType::F64:
    {
        if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            float  tmpF = static_cast<float>(nodeToCast->computedValue.reg.f64);
            double tmpD = static_cast<double>(tmpF);
            if (tmpD != nodeToCast->computedValue.reg.f64)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%Lf' is truncated in 'f32'", nodeToCast->computedValue.reg.f64)});
                return false;
            }

            nodeToCast->computedValue.reg.f32 = static_cast<float>(nodeToCast->computedValue.reg.f64);
            nodeToCast->typeInfo              = g_TypeMgr.typeInfoF32;
            return true;
        }
        else if (castFlags & CASTFLAG_FORCE)
            return true;
        break;
    }
    }

    return castError(errorContext, g_TypeMgr.typeInfoF32, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeF64(ErrorContext* errorContext, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (fromType->nativeType)
    {
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
    {
        if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            double  tmpF = static_cast<double>(nodeToCast->computedValue.reg.s64);
            int64_t tmpI = static_cast<int64_t>(tmpF);
            if (tmpI != nodeToCast->computedValue.reg.s64)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%I64d' is truncated in 'f64'", nodeToCast->computedValue.reg.s64)});
                return false;
            }

            nodeToCast->computedValue.reg.f64 = static_cast<double>(nodeToCast->computedValue.reg.s64);
            nodeToCast->typeInfo              = g_TypeMgr.typeInfoF64;
            return true;
        }
        else if (castFlags & CASTFLAG_FORCE)
            return true;
        break;
    }

    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    {
        if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            double   tmpF = static_cast<double>(nodeToCast->computedValue.reg.u64);
            uint64_t tmpI = static_cast<uint64_t>(tmpF);
            if (tmpI != nodeToCast->computedValue.reg.u64)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    errorContext->report({errorContext->sourceFile, nodeToCast->token, format("value '%I64u' is truncated in 'f64'", nodeToCast->computedValue.reg.u64)});
                return false;
            }

            nodeToCast->computedValue.reg.f64 = static_cast<double>(nodeToCast->computedValue.reg.u64);
            nodeToCast->typeInfo              = g_TypeMgr.typeInfoF64;
            return true;
        }
        else if (castFlags & CASTFLAG_FORCE)
            return true;
        break;
    }

    case NativeType::F32:
        if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            nodeToCast->computedValue.reg.f64 = static_cast<double>(nodeToCast->computedValue.reg.f32);
            nodeToCast->typeInfo              = g_TypeMgr.typeInfoF64;
            return true;
        }
        else if (castFlags & CASTFLAG_FORCE)
            return true;
        break;
    }

    return castError(errorContext, g_TypeMgr.typeInfoF64, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNative(ErrorContext* errorContext, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (toType->nativeType)
    {
    case NativeType::Bool:
        return castToNativeBool(errorContext, fromType, nodeToCast, castFlags);
    case NativeType::U8:
        return castToNativeU8(errorContext, fromType, nodeToCast, castFlags);
    case NativeType::U16:
        return castToNativeU16(errorContext, fromType, nodeToCast, castFlags);
    case NativeType::U32:
    case NativeType::Char:
        return castToNativeU32(errorContext, fromType, nodeToCast, castFlags);
    case NativeType::U64:
        return castToNativeU64(errorContext, fromType, nodeToCast, castFlags);
    case NativeType::S8:
        return castToNativeS8(errorContext, fromType, nodeToCast, castFlags);
    case NativeType::S16:
        return castToNativeS16(errorContext, fromType, nodeToCast, castFlags);
    case NativeType::S32:
        return castToNativeS32(errorContext, fromType, nodeToCast, castFlags);
    case NativeType::S64:
        return castToNativeS64(errorContext, fromType, nodeToCast, castFlags);
    case NativeType::F32:
        return castToNativeF32(errorContext, fromType, nodeToCast, castFlags);
    case NativeType::F64:
        return castToNativeF64(errorContext, fromType, nodeToCast, castFlags);
    }

    return castError(errorContext, toType, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToArray(ErrorContext* errorContext, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    TypeInfoArray* toTypeArray = CastTypeInfo<TypeInfoArray>(toType, TypeInfoKind::Array);
    if (fromType->kind == TypeInfoKind::TypeList)
    {
        TypeInfoList* fromTypeList = CastTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeList);
        if (fromTypeList->listKind == TypeInfoListKind::Array)
        {
            auto fromSize = fromTypeList->childs.size();
            if (toTypeArray->count != fromSize)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                {
                    if (toTypeArray->count > fromTypeList->childs.size())
                        errorContext->report({errorContext->sourceFile, nodeToCast->token, format("can't cast, not enough initializers ('%d' provided, '%d' requested)", fromTypeList->childs.size(), toTypeArray->count)});
                    else
                        errorContext->report({errorContext->sourceFile, nodeToCast->token, format("can't cast, too many initializers ('%d' provided, '%d' requested)", fromTypeList->childs.size(), toTypeArray->count)});
                }

                return false;
            }

            SWAG_ASSERT(!nodeToCast || fromSize == nodeToCast->childs.size());
            for (int i = 0; i < fromSize; i++)
            {
                SWAG_CHECK(TypeManager::makeCompatibles(errorContext, toTypeArray->pointedType, fromTypeList->childs[i], nodeToCast ? nodeToCast->childs[i] : nullptr, castFlags));
            }

            return true;
        }
    }

    return castError(errorContext, toType, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToTuple(ErrorContext* errorContext, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    TypeInfoList* toTypeList = CastTypeInfo<TypeInfoList>(toType, TypeInfoKind::TypeList);
    if (fromType->kind == TypeInfoKind::TypeList)
    {
        TypeInfoList* fromTypeList = CastTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeList);
        if (fromTypeList->listKind == TypeInfoListKind::Tuple)
        {
            auto fromSize = fromTypeList->childs.size();
            if (toTypeList->childs.size() != fromSize)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                {
                    if (toTypeList->childs.size() > fromTypeList->childs.size())
                        errorContext->report({errorContext->sourceFile, nodeToCast->token, format("can't cast, not enough initializers ('%d' provided, '%d' requested)", fromTypeList->childs.size(), toTypeList->childs.size())});
                    else
                        errorContext->report({errorContext->sourceFile, nodeToCast->token, format("can't cast, too many initializers ('%d' provided, '%d' requested)", fromTypeList->childs.size(), toTypeList->childs.size())});
                }

                return false;
            }

            SWAG_ASSERT(!nodeToCast || fromSize == nodeToCast->childs.size());
            for (int i = 0; i < fromSize; i++)
            {
                SWAG_CHECK(TypeManager::makeCompatibles(errorContext, toTypeList->childs[i], fromTypeList->childs[i], nodeToCast ? nodeToCast->childs[i] : nullptr, castFlags));
            }

            if (nodeToCast && !(castFlags & CASTFLAG_JUST_CHECK))
                nodeToCast->typeInfo = toTypeList;

            return true;
        }
    }

    return castError(errorContext, toType, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToSlice(ErrorContext* errorContext, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    TypeInfoSlice* toTypeSlice     = CastTypeInfo<TypeInfoSlice>(toType, TypeInfoKind::Slice);
    bool           explicitIsValid = false;

    if (fromType->kind == TypeInfoKind::TypeList)
    {
        TypeInfoList* fromTypeList = CastTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeList);

        // Special case when typelist is one pointer and one int
        if (fromTypeList->childs.size() == 2)
        {
            bool forcedInit = true;

            // Must start with a pointer of the same type as the slice
            auto typeFront = fromTypeList->childs.front();
            if (typeFront->kind != TypeInfoKind::Pointer)
                forcedInit = false;
            else
            {
                auto typePointer = static_cast<TypeInfoPointer*>(fromTypeList->childs.front());
                if (!TypeManager::makeCompatibles(errorContext, toTypeSlice->pointedType, typePointer->pointedType, nullptr, castFlags | CASTFLAG_JUST_CHECK | CASTFLAG_NOERROR))
                    forcedInit = false;
            }

            // And must and with an U32, which is the slice count
            if (forcedInit && !TypeManager::makeCompatibles(errorContext, g_TypeMgr.typeInfoU32, fromTypeList->childs.back(), nodeToCast ? nodeToCast->childs.back() : nullptr, castFlags | CASTFLAG_JUST_CHECK | CASTFLAG_NOERROR))
                forcedInit = false;
            if (forcedInit)
            {
                if (nodeToCast)
                    nodeToCast->flags |= AST_SLICE_INIT_EXPRESSION;
                return true;
            }
        }

        auto fromSize = fromTypeList->childs.size();
        for (int i = 0; i < fromSize; i++)
        {
            SWAG_CHECK(TypeManager::makeCompatibles(errorContext, toTypeSlice->pointedType, fromTypeList->childs[i], nodeToCast ? nodeToCast->childs[i] : nullptr, castFlags | CASTFLAG_JUST_CHECK));
        }

        return true;
    }
    else if (fromType->kind == TypeInfoKind::Array)
    {
        TypeInfoArray* fromTypeArray = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
        if (toTypeSlice->pointedType->isSameForCast(fromTypeArray->pointedType))
        {
            if (castFlags & CASTFLAG_FORCE)
                return true;
            else
                explicitIsValid = true;
        }
    }
    else if (fromType->isNative(NativeType::String))
    {
        if (toTypeSlice->pointedType->isNative(NativeType::U8))
            return true;
    }
    else if (fromType == g_TypeMgr.typeInfoNull)
    {
        return true;
    }
    else if (fromType->kind == TypeInfoKind::Slice)
    {
        auto fromTypeSlice = CastTypeInfo<TypeInfoSlice>(fromType, TypeInfoKind::Slice);
        if (fromTypeSlice->pointedType->kind == TypeInfoKind::Native)
        {
            int s = toTypeSlice->pointedType->sizeOf;
            int d = fromTypeSlice->pointedType->sizeOf;
            if ((d / s) * s == d)
            {
                if (castFlags & CASTFLAG_FORCE)
                    return true;
                else
                    explicitIsValid = true;
            }
        }
    }

    return castError(errorContext, toType, fromType, nodeToCast, castFlags, explicitIsValid);
}

bool TypeManager::makeCompatibles(ErrorContext* errorContext, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if ((castFlags & CASTFLAG_CONCRETE_ENUM) || (castFlags & CASTFLAG_FORCE))
    {
        toType   = TypeManager::concreteType(toType, MakeConcrete::FlagEnum);
        fromType = TypeManager::concreteType(fromType, MakeConcrete::FlagEnum);
    }

    SWAG_ASSERT(toType && fromType);

    if (toType->kind == TypeInfoKind::FuncAttr)
        toType = TypeManager::concreteType(toType, MakeConcrete::FlagFunc);
    if (fromType->kind == TypeInfoKind::FuncAttr)
        fromType = TypeManager::concreteType(fromType, MakeConcrete::FlagFunc);
    if (toType->kind != TypeInfoKind::Lambda && fromType->kind == TypeInfoKind::Lambda)
        fromType = TypeManager::concreteType(fromType, MakeConcrete::FlagFunc);

    if (toType->kind == TypeInfoKind::Alias)
        toType = TypeManager::concreteType(toType, MakeConcrete::FlagAlias);
    if (fromType->kind == TypeInfoKind::Alias)
        fromType = TypeManager::concreteType(fromType, MakeConcrete::FlagAlias);

    if (fromType->kind == TypeInfoKind::VariadicValue)
        return true;
    if (fromType == toType)
        return true;
    if (fromType->isSameForCast(toType))
        return true;

    // To a generic type
    if (fromType->kind != TypeInfoKind::Generic && toType->kind == TypeInfoKind::Generic)
        return true;

    // Pointer to pointer, with a user cast
    if (toType->kind == TypeInfoKind::Pointer && fromType->kind == TypeInfoKind::Pointer && (castFlags & CASTFLAG_FORCE))
        return true;
    if (toType->kind == TypeInfoKind::Pointer && fromType->kind == TypeInfoKind::Pointer && (toType == g_TypeMgr.typeInfoNull || fromType == g_TypeMgr.typeInfoNull))
        return true;
    if (toType->isNative(NativeType::String) && fromType == g_TypeMgr.typeInfoNull)
        return true;
    if (toType == g_TypeMgr.typeInfoNull && fromType->isNative(NativeType::String))
        return true;

    // Const
    if (!toType->isConst() && fromType->isConst())
    {
        if (toType->kind != TypeInfoKind::Array)
            return castError(errorContext, toType, fromType, nodeToCast, castFlags);
    }

    // Cast to native type
    if (toType->kind == TypeInfoKind::Native)
        return castToNative(errorContext, toType, fromType, nodeToCast, castFlags);

    // Cast to array
    if (toType->kind == TypeInfoKind::Array)
        return castToArray(errorContext, toType, fromType, nodeToCast, castFlags);

    // Cast to tuple
    if (toType->kind == TypeInfoKind::TypeList)
        return castToTuple(errorContext, toType, fromType, nodeToCast, castFlags);

    // Cast to slice
    if (toType->kind == TypeInfoKind::Slice)
        return castToSlice(errorContext, toType, fromType, nodeToCast, castFlags);

    // Cast to slice
    if (toType->kind == TypeInfoKind::Lambda)
    {
        if (toType->isSameForCast(fromType))
            return true;
    }

    return castError(errorContext, toType, fromType, nodeToCast, castFlags);
}

bool TypeManager::makeCompatibles(ErrorContext* errorContext, TypeInfo* toType, AstNode* nodeToCast, uint32_t castFlags)
{
    return makeCompatibles(errorContext, toType, nodeToCast->typeInfo, nodeToCast, castFlags);
}

bool TypeManager::makeCompatibles(ErrorContext* errorContext, AstNode* leftNode, AstNode* rightNode, uint32_t castFlags)
{
    return makeCompatibles(errorContext, leftNode->typeInfo, rightNode, castFlags);
}

void TypeManager::promote(AstNode* left, AstNode* right)
{
    promoteOne(left, right);
    promoteOne(right, left);
}

void TypeManager::promoteOne(AstNode* left, AstNode* right)
{
    TypeInfo* leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    TypeInfo* rightTypeInfo = TypeManager::concreteType(right->typeInfo);
    if ((leftTypeInfo->kind != TypeInfoKind::Native) || (rightTypeInfo->kind != TypeInfoKind::Native))
        return;

    // This types do not have a promotion
    switch (leftTypeInfo->nativeType)
    {
    case NativeType::Bool:
    case NativeType::Char:
    case NativeType::String:
        return;
    }

    TypeInfo* newLeftTypeInfo = (TypeInfo*) g_TypeMgr.promoteMatrix[(int) leftTypeInfo->nativeType][(int) rightTypeInfo->nativeType];
    if (newLeftTypeInfo == nullptr)
        newLeftTypeInfo = leftTypeInfo;

    if (newLeftTypeInfo == leftTypeInfo)
        return;

    if (!(left->flags & AST_VALUE_COMPUTED))
    {
		left->typeInfo = newLeftTypeInfo;
        left->castedTypeInfo = leftTypeInfo;
        return;
    }

    left->typeInfo = newLeftTypeInfo;
    auto newLeft   = newLeftTypeInfo->nativeType;
    switch (leftTypeInfo->nativeType)
    {
    case NativeType::U8:
        if (newLeft == NativeType::U32)
            left->computedValue.reg.u32 = left->computedValue.reg.u8;
        else if (newLeft == NativeType::U64)
            left->computedValue.reg.u64 = left->computedValue.reg.u8;
        else if (newLeft == NativeType::F32)
            left->computedValue.reg.f32 = static_cast<float>(left->computedValue.reg.u8);
        else if (newLeft == NativeType::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.u8);
        break;
    case NativeType::U16:
        if (newLeft == NativeType::U32)
            left->computedValue.reg.u32 = left->computedValue.reg.u16;
        else if (newLeft == NativeType::U64)
            left->computedValue.reg.u64 = left->computedValue.reg.u16;
        else if (newLeft == NativeType::F32)
            left->computedValue.reg.f32 = static_cast<float>(left->computedValue.reg.u16);
        else if (newLeft == NativeType::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.u16);
        break;
    case NativeType::U32:
        if (newLeft == NativeType::U64)
            left->computedValue.reg.u64 = left->computedValue.reg.u32;
        else if (newLeft == NativeType::F32)
            left->computedValue.reg.f32 = static_cast<float>(left->computedValue.reg.u32);
        else if (newLeft == NativeType::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.u32);
        break;
    case NativeType::U64:
        if (newLeft == NativeType::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.u64);
        break;
    case NativeType::S8:
        if (newLeft == NativeType::S32)
            left->computedValue.reg.s32 = left->computedValue.reg.s8;
        else if (newLeft == NativeType::S64)
            left->computedValue.reg.s64 = left->computedValue.reg.s8;
        else if (newLeft == NativeType::F32)
            left->computedValue.reg.f32 = static_cast<float>(left->computedValue.reg.s8);
        else if (newLeft == NativeType::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.s8);
        break;
    case NativeType::S16:
        if (newLeft == NativeType::S32)
            left->computedValue.reg.s32 = left->computedValue.reg.s16;
        else if (newLeft == NativeType::S64)
            left->computedValue.reg.s64 = left->computedValue.reg.s16;
        else if (newLeft == NativeType::F32)
            left->computedValue.reg.f32 = static_cast<float>(left->computedValue.reg.s16);
        else if (newLeft == NativeType::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.s16);
        break;
    case NativeType::S32:
        if (newLeft == NativeType::S64)
            left->computedValue.reg.s64 = left->computedValue.reg.s32;
        else if (newLeft == NativeType::F32)
            left->computedValue.reg.f32 = static_cast<float>(left->computedValue.reg.s32);
        else if (newLeft == NativeType::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.s32);
        break;
    case NativeType::S64:
        if (newLeft == NativeType::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.s64);
        break;
    case NativeType::F32:
        if (newLeft == NativeType::F64)
            left->computedValue.reg.f64 = left->computedValue.reg.f32;
        break;
    }
}
