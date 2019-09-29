#include "pch.h"
#include "TypeManager.h"
#include "Diagnostic.h"
#include "Global.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "TypeTable.h"
#include "Module.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"

bool TypeManager::castError(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags, bool explicitIsValid)
{
    if (!(castFlags & CASTFLAG_NOERROR))
    {
        assert(nodeToCast);
        if (explicitIsValid)
            context->errorContext.report({context->sourceFile, nodeToCast->token, format("cannot cast from '%s' to '%s' (an explicit cast exists)", fromType->name.c_str(), toType->name.c_str()).c_str()});
        else
            context->errorContext.report({context->sourceFile, nodeToCast->token, format("cannot cast from '%s' to '%s'", fromType->name.c_str(), toType->name.c_str()).c_str()});
    }

    return false;
}

bool TypeManager::castToNativeBool(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (fromType == g_TypeMgr.typeInfoBool)
        return true;

    // Automatic cast to a bool is done only if requested, on specific nodes (like if or while expressions)
    if (!(castFlags & CASTFLAG_AUTO_BOOL))
        return castError(context, g_TypeMgr.typeInfoBool, fromType, nodeToCast, castFlags);

    if (fromType->kind == TypeInfoKind::Pointer)
    {
        if (!(castFlags & CASTFLAG_JUST_CHECK))
        {
            nodeToCast->typeInfo       = g_TypeMgr.typeInfoBool;
            nodeToCast->castedTypeInfo = fromType;
            return true;
        }
    }

    if (fromType->kind == TypeInfoKind::Native)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            if (nodeToCast)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (nodeToCast->flags & AST_VALUE_COMPUTED)
                    {
                        nodeToCast->computedValue.reg.b = nodeToCast->computedValue.reg.u8;
                        nodeToCast->typeInfo            = g_TypeMgr.typeInfoBool;
                    }
                    else
                    {
                        nodeToCast->typeInfo       = g_TypeMgr.typeInfoBool;
                        nodeToCast->castedTypeInfo = fromType;
                    }
                }
            }
            return true;

        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            if (nodeToCast)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (nodeToCast->flags & AST_VALUE_COMPUTED)
                    {
                        nodeToCast->computedValue.reg.b = nodeToCast->computedValue.reg.u16;
                        nodeToCast->typeInfo            = g_TypeMgr.typeInfoBool;
                    }
                    else
                    {
                        nodeToCast->typeInfo       = g_TypeMgr.typeInfoBool;
                        nodeToCast->castedTypeInfo = fromType;
                    }
                }
            }
            return true;

        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            if (nodeToCast)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (nodeToCast->flags & AST_VALUE_COMPUTED)
                    {
                        nodeToCast->computedValue.reg.b = nodeToCast->computedValue.reg.u32;
                        nodeToCast->typeInfo            = g_TypeMgr.typeInfoBool;
                    }
                    else
                    {
                        nodeToCast->typeInfo       = g_TypeMgr.typeInfoBool;
                        nodeToCast->castedTypeInfo = fromType;
                    }
                }
            }
            return true;

        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            if (nodeToCast)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (nodeToCast->flags & AST_VALUE_COMPUTED)
                    {
                        nodeToCast->computedValue.reg.b = nodeToCast->computedValue.reg.u64;
                        nodeToCast->typeInfo            = g_TypeMgr.typeInfoBool;
                    }
                    else
                    {
                        nodeToCast->typeInfo       = g_TypeMgr.typeInfoBool;
                        nodeToCast->castedTypeInfo = fromType;
                    }
                }
            }
            return true;
        }
    }

    return castError(context, g_TypeMgr.typeInfoBool, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeChar(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Char:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.u32 = static_cast<uint32_t>(nodeToCast->computedValue.reg.f32);
                    nodeToCast->typeInfo              = g_TypeMgr.typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::F64:
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

    return castError(context, g_TypeMgr.typeInfoU32, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU8(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Char:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU8;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.u8 = static_cast<uint8_t>(nodeToCast->computedValue.reg.f32);
                    nodeToCast->typeInfo             = g_TypeMgr.typeInfoU8;
                }
            }
            return true;

        case NativeTypeKind::F64:
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
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u8'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                SWAG_ASSERT(!nodeToCast);
                auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                if (value < 0)
                    return false;
            }

        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.u64 > UINT8_MAX)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%I64u' is not in the range of 'u8'", nodeToCast->computedValue.reg.u64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU8;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                SWAG_ASSERT(!nodeToCast);
                auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                if (value > UINT8_MAX)
                    return false;
                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoU8, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU16(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Char:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU16;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.u16 = static_cast<uint16_t>(nodeToCast->computedValue.reg.f32);
                    nodeToCast->typeInfo              = g_TypeMgr.typeInfoU16;
                }
            }
            return true;

        case NativeTypeKind::F64:
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
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u16'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                SWAG_ASSERT(!nodeToCast);
                auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                if (value < 0)
                    return false;
            }

        case NativeTypeKind::U8:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.u64 > UINT16_MAX)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%I64u' is not in the range of 'u16'", nodeToCast->computedValue.reg.u64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU16;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                SWAG_ASSERT(!nodeToCast);
                auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                if (value > UINT16_MAX)
                    return false;
                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoU16, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU32(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Char:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.u32 = static_cast<uint32_t>(nodeToCast->computedValue.reg.f32);
                    nodeToCast->typeInfo              = g_TypeMgr.typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::F64:
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
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u32'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                SWAG_ASSERT(!nodeToCast);
                auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                if (value < 0)
                    return false;
            }

        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.u64 > UINT32_MAX)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%I64u' is not in the range of 'u32'", nodeToCast->computedValue.reg.u64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU32;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                SWAG_ASSERT(!nodeToCast);
                auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                if (value > UINT32_MAX)
                    return false;
                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoU32, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeU64(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Char:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU64;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.u64 = static_cast<uint64_t>(nodeToCast->computedValue.reg.f32);
                    nodeToCast->typeInfo              = g_TypeMgr.typeInfoU64;
                }
            }
            return true;

        case NativeTypeKind::F64:
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
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%I64d' is negative and not in the range of 'u64'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                SWAG_ASSERT(!nodeToCast);
                auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                if (value < 0)
                    return false;
            }

        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoU64;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoU64, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS8(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Char:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS8;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.s8 = static_cast<int8_t>(nodeToCast->computedValue.reg.f32);
                    nodeToCast->typeInfo             = g_TypeMgr.typeInfoS8;
                }
            }
            return true;

        case NativeTypeKind::F64:
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
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < INT8_MIN || nodeToCast->computedValue.reg.s64 > INT8_MAX)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's8'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS8;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                SWAG_ASSERT(!nodeToCast);
                auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                if (value < INT8_MIN || value > INT8_MAX)
                    return false;
                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoS8, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS16(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Char:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS16;
            }
            return true;

        case NativeTypeKind::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    nodeToCast->computedValue.reg.s16 = static_cast<int16_t>(nodeToCast->computedValue.reg.f32);
                    nodeToCast->typeInfo              = g_TypeMgr.typeInfoS16;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                nodeToCast->computedValue.reg.s16 = static_cast<int16_t>(nodeToCast->computedValue.reg.f64);
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS16;
            }
            return true;
        }
    }
    else
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < INT16_MIN || nodeToCast->computedValue.reg.s64 > INT16_MAX)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's16'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS16;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                SWAG_ASSERT(!nodeToCast);
                auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                if (value < INT16_MIN || value > INT16_MAX)
                    return false;
                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoS16, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS32(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Char:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS32;
            }
            return true;

        case NativeTypeKind::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                nodeToCast->computedValue.reg.s32 = static_cast<int32_t>(nodeToCast->computedValue.reg.f32);
                nodeToCast->typeInfo              = g_TypeMgr.typeInfoS32;
            }
            return true;

        case NativeTypeKind::F64:
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
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < INT32_MIN || nodeToCast->computedValue.reg.s64 > INT32_MAX)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's32'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS32;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                SWAG_ASSERT(!nodeToCast);
                auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                if (value < INT32_MIN || value > INT32_MAX)
                    return false;
                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoS32, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeS64(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    if (castFlags & CASTFLAG_FORCE)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Char:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS64;
            }
            return true;

        case NativeTypeKind::F32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                nodeToCast->computedValue.reg.s64 = static_cast<int64_t>(nodeToCast->computedValue.reg.f32);
                nodeToCast->typeInfo              = g_TypeMgr.typeInfoS64;
            }
            return true;

        case NativeTypeKind::F64:
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
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
            if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
            {
                if (nodeToCast->computedValue.reg.s64 < INT64_MIN || nodeToCast->computedValue.reg.s64 > INT64_MAX)
                {
                    if (!(castFlags & CASTFLAG_NOERROR))
                        context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%I64d' is not in the range of 's64'", nodeToCast->computedValue.reg.s64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    nodeToCast->typeInfo = g_TypeMgr.typeInfoS64;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                SWAG_ASSERT(!nodeToCast);
                auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                if (value < INT64_MIN || value > INT64_MAX)
                    return false;
                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoS64, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeF32(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (fromType->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    {
        if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            float   tmpF = static_cast<float>(nodeToCast->computedValue.reg.s64);
            int64_t tmpI = static_cast<int64_t>(tmpF);
            if (tmpI != nodeToCast->computedValue.reg.s64)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%I64d' is truncated in 'f32'", nodeToCast->computedValue.reg.s64)});
                return false;
            }

            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                nodeToCast->computedValue.reg.f32 = static_cast<float>(nodeToCast->computedValue.reg.s64);
                nodeToCast->typeInfo              = g_TypeMgr.typeInfoF32;
            }
            return true;
        }
        else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
        {
            SWAG_ASSERT(!nodeToCast);
            auto    value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
            float   tmpF  = static_cast<float>(value);
            int64_t tmpI  = static_cast<int64_t>(tmpF);
            if (tmpI != value)
                return false;
            return true;
        }
        else if (castFlags & CASTFLAG_FORCE)
            return true;
        break;
    }

    case NativeTypeKind::U8:
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    {
        if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            float    tmpF = static_cast<float>(nodeToCast->computedValue.reg.u64);
            uint64_t tmpI = static_cast<uint64_t>(tmpF);
            if (tmpI != nodeToCast->computedValue.reg.u64)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%I64u' is truncated in 'f64'", nodeToCast->computedValue.reg.u64)});
                return false;
            }

            if (nodeToCast && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                nodeToCast->computedValue.reg.f32 = static_cast<float>(nodeToCast->computedValue.reg.u64);
                nodeToCast->typeInfo              = g_TypeMgr.typeInfoF32;
            }

            return true;
        }
        else if (castFlags & CASTFLAG_FORCE)
            return true;
        break;
    }

    case NativeTypeKind::F64:
    {
        if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            float  tmpF = static_cast<float>(nodeToCast->computedValue.reg.f64);
            double tmpD = static_cast<double>(tmpF);
            if (tmpD != nodeToCast->computedValue.reg.f64)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%Lf' is truncated in 'f32'", nodeToCast->computedValue.reg.f64)});
                return false;
            }

            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                nodeToCast->computedValue.reg.f32 = static_cast<float>(nodeToCast->computedValue.reg.f64);
                nodeToCast->typeInfo              = g_TypeMgr.typeInfoF32;
            }

            return true;
        }
        else if (castFlags & CASTFLAG_FORCE)
            return true;
        break;
    }
    }

    return castError(context, g_TypeMgr.typeInfoF32, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNativeF64(SemanticContext* context, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (fromType->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    {
        if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            double  tmpF = static_cast<double>(nodeToCast->computedValue.reg.s64);
            int64_t tmpI = static_cast<int64_t>(tmpF);
            if (tmpI != nodeToCast->computedValue.reg.s64)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%I64d' is truncated in 'f64'", nodeToCast->computedValue.reg.s64)});
                return false;
            }

            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                nodeToCast->computedValue.reg.f64 = static_cast<double>(nodeToCast->computedValue.reg.s64);
                nodeToCast->typeInfo              = g_TypeMgr.typeInfoF64;
            }
            return true;
        }
        else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
        {
            SWAG_ASSERT(!nodeToCast);
            auto    value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
            double  tmpF  = static_cast<double>(value);
            int64_t tmpI  = static_cast<int64_t>(tmpF);
            if (tmpI != value)
                return false;
            return true;
        }
        else if (castFlags & CASTFLAG_FORCE)
            return true;
        break;
    }

    case NativeTypeKind::U8:
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    {
        if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            double   tmpF = static_cast<double>(nodeToCast->computedValue.reg.u64);
            uint64_t tmpI = static_cast<uint64_t>(tmpF);
            if (tmpI != nodeToCast->computedValue.reg.u64)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                    context->errorContext.report({context->errorContext.sourceFile, nodeToCast->token, format("value '%I64u' is truncated in 'f64'", nodeToCast->computedValue.reg.u64)});
                return false;
            }

            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                nodeToCast->computedValue.reg.f64 = static_cast<double>(nodeToCast->computedValue.reg.u64);
                nodeToCast->typeInfo              = g_TypeMgr.typeInfoF64;
            }

            return true;
        }
        else if (castFlags & CASTFLAG_FORCE)
            return true;
        break;
    }

    case NativeTypeKind::F32:
        if (nodeToCast && nodeToCast->flags & AST_VALUE_COMPUTED)
        {
            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                nodeToCast->computedValue.reg.f64 = static_cast<double>(nodeToCast->computedValue.reg.f32);
                nodeToCast->typeInfo              = g_TypeMgr.typeInfoF64;
            }

            return true;
        }
        else if (fromType->flags & TYPEINFO_UNTYPED_FLOAT)
            return true;
        else if (castFlags & CASTFLAG_FORCE)
            return true;
        break;
    }

    return castError(context, g_TypeMgr.typeInfoF64, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    switch (toType->nativeType)
    {
    case NativeTypeKind::Bool:
        return castToNativeBool(context, fromType, nodeToCast, castFlags);
    case NativeTypeKind::U8:
        return castToNativeU8(context, fromType, nodeToCast, castFlags);
    case NativeTypeKind::U16:
        return castToNativeU16(context, fromType, nodeToCast, castFlags);
    case NativeTypeKind::U32:
        return castToNativeU32(context, fromType, nodeToCast, castFlags);
    case NativeTypeKind::Char:
        return castToNativeChar(context, fromType, nodeToCast, castFlags);
    case NativeTypeKind::U64:
        return castToNativeU64(context, fromType, nodeToCast, castFlags);
    case NativeTypeKind::S8:
        return castToNativeS8(context, fromType, nodeToCast, castFlags);
    case NativeTypeKind::S16:
        return castToNativeS16(context, fromType, nodeToCast, castFlags);
    case NativeTypeKind::S32:
        return castToNativeS32(context, fromType, nodeToCast, castFlags);
    case NativeTypeKind::S64:
        return castToNativeS64(context, fromType, nodeToCast, castFlags);
    case NativeTypeKind::F32:
        return castToNativeF32(context, fromType, nodeToCast, castFlags);
    case NativeTypeKind::F64:
        return castToNativeF64(context, fromType, nodeToCast, castFlags);
    }

    return castError(context, toType, fromType, nodeToCast, castFlags);
}

bool TypeManager::castExpressionList(SemanticContext* context, TypeInfoList* fromTypeList, TypeInfo* toType, AstNode* nodeToCast, uint32_t castFlags)
{
    auto fromSize = fromTypeList->childs.size();
    while (nodeToCast && nodeToCast->kind != AstNodeKind::ExpressionList)
        nodeToCast = nodeToCast->childs.front();
    SWAG_ASSERT(!nodeToCast || fromSize == nodeToCast->childs.size());

    // Need to recompute total size, as the size of each element can have been changed by the cast
    if (nodeToCast)
    {
        fromTypeList->sizeOf = 0;
        nodeToCast->flags |= AST_CONST_EXPR;
    }

    TypeInfoList* toTypeList = nullptr;
    if (toType->kind == TypeInfoKind::TypeList)
        toTypeList = CastTypeInfo<TypeInfoList>(toType, TypeInfoKind::TypeList);

    for (int i = 0; i < fromSize; i++)
    {
        auto child = nodeToCast ? nodeToCast->childs[i] : nullptr;
        SWAG_CHECK(TypeManager::makeCompatibles(context, toTypeList ? toTypeList->childs[i] : toType, fromTypeList->childs[i], child, castFlags));
        if (child)
        {
            fromTypeList->sizeOf += child->typeInfo->sizeOf;
            if (!(child->flags & AST_CONST_EXPR))
                nodeToCast->flags &= ~AST_CONST_EXPR;
        }
    }

    return true;
}

bool TypeManager::castToArray(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    TypeInfoArray* toTypeArray = CastTypeInfo<TypeInfoArray>(toType, TypeInfoKind::Array);
    if (fromType->kind == TypeInfoKind::TypeList)
    {
        TypeInfoList* fromTypeList = CastTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeList);
        if (fromTypeList->listKind == TypeInfoListKind::Bracket)
        {
            auto fromSize = fromTypeList->childs.size();
            if (toTypeArray->count != fromSize)
            {
                if (!(castFlags & CASTFLAG_NOERROR))
                {
                    if (toTypeArray->count > fromTypeList->childs.size())
                        context->errorContext.report({context->errorContext.sourceFile, nodeToCast, format("cannot cast, not enough initializers ('%d' provided, '%d' requested)", fromTypeList->childs.size(), toTypeArray->count)});
                    else
                        context->errorContext.report({context->errorContext.sourceFile, nodeToCast, format("cannot cast, too many initializers ('%d' provided, '%d' requested)", fromTypeList->childs.size(), toTypeArray->count)});
                }

                return false;
            }

            SWAG_CHECK(castExpressionList(context, fromTypeList, toTypeArray->pointedType, nodeToCast, castFlags));
            return true;
        }
    }

    return castError(context, toType, fromType, nodeToCast, castFlags);
}

bool TypeManager::castToSlice(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    TypeInfoSlice* toTypeSlice     = CastTypeInfo<TypeInfoSlice>(toType, TypeInfoKind::Slice);
    bool           explicitIsValid = false;

    if (fromType->kind == TypeInfoKind::TypeList)
    {
        TypeInfoList* fromTypeList = CastTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeList);

        // Can only cast array to slice
        if (fromTypeList->listKind != TypeInfoListKind::Bracket)
            return castError(context, toType, fromType, nodeToCast, castFlags, explicitIsValid);

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
                if (!TypeManager::makeCompatibles(context, toTypeSlice->pointedType, typePointer->pointedType, nullptr, castFlags | CASTFLAG_JUST_CHECK | CASTFLAG_NOERROR))
                    forcedInit = false;
            }

            // And must and with an U32, which is the slice count
            if (forcedInit && !TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoU32, fromTypeList->childs.back(), nodeToCast ? nodeToCast->childs.back() : nullptr, castFlags | CASTFLAG_JUST_CHECK | CASTFLAG_NOERROR))
                forcedInit = false;
            if (forcedInit)
            {
                if (nodeToCast)
                    nodeToCast->flags |= AST_SLICE_INIT_EXPRESSION;
                return true;
            }
        }

        SWAG_CHECK(castExpressionList(context, fromTypeList, toTypeSlice->pointedType, nodeToCast, castFlags));
        return true;
    }
    else if (fromType->kind == TypeInfoKind::Array)
    {
        TypeInfoArray* fromTypeArray = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
        if (toTypeSlice->pointedType->isSame(fromTypeArray->pointedType, ISSAME_CAST))
        {
            if (nodeToCast && !(castFlags & CASTFLAG_JUST_CHECK) && !(castFlags & CASTFLAG_FORCE))
            {
                nodeToCast->castedTypeInfo = nodeToCast->typeInfo;
                nodeToCast->typeInfo       = toTypeSlice;
            }

            return true;
        }
    }
    else if (fromType->isNative(NativeTypeKind::String))
    {
        if (toTypeSlice->pointedType->isNative(NativeTypeKind::U8))
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

    return castError(context, toType, fromType, nodeToCast, castFlags, explicitIsValid);
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
    case NativeTypeKind::Bool:
    case NativeTypeKind::Char:
    case NativeTypeKind::String:
        return;
    }

    TypeInfo* newLeftTypeInfo = (TypeInfo*) g_TypeMgr.promoteMatrix[(int) leftTypeInfo->nativeType][(int) rightTypeInfo->nativeType];
    if (newLeftTypeInfo == nullptr)
        newLeftTypeInfo = leftTypeInfo;

    if (newLeftTypeInfo == leftTypeInfo)
        return;

    if (!(left->flags & AST_VALUE_COMPUTED))
    {
        left->typeInfo       = newLeftTypeInfo;
        left->castedTypeInfo = leftTypeInfo;
        return;
    }

    left->typeInfo = newLeftTypeInfo;
    auto newLeft   = newLeftTypeInfo->nativeType;
    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::U8:
        if (newLeft == NativeTypeKind::U32)
            left->computedValue.reg.u32 = left->computedValue.reg.u8;
        else if (newLeft == NativeTypeKind::U64)
            left->computedValue.reg.u64 = left->computedValue.reg.u8;
        else if (newLeft == NativeTypeKind::F32)
            left->computedValue.reg.f32 = static_cast<float>(left->computedValue.reg.u8);
        else if (newLeft == NativeTypeKind::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.u8);
        break;
    case NativeTypeKind::U16:
        if (newLeft == NativeTypeKind::U32)
            left->computedValue.reg.u32 = left->computedValue.reg.u16;
        else if (newLeft == NativeTypeKind::U64)
            left->computedValue.reg.u64 = left->computedValue.reg.u16;
        else if (newLeft == NativeTypeKind::F32)
            left->computedValue.reg.f32 = static_cast<float>(left->computedValue.reg.u16);
        else if (newLeft == NativeTypeKind::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.u16);
        break;
    case NativeTypeKind::U32:
        if (newLeft == NativeTypeKind::U64)
            left->computedValue.reg.u64 = left->computedValue.reg.u32;
        else if (newLeft == NativeTypeKind::F32)
            left->computedValue.reg.f32 = static_cast<float>(left->computedValue.reg.u32);
        else if (newLeft == NativeTypeKind::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.u32);
        break;
    case NativeTypeKind::U64:
        if (newLeft == NativeTypeKind::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.u64);
        break;
    case NativeTypeKind::S8:
        if (newLeft == NativeTypeKind::S32)
            left->computedValue.reg.s32 = left->computedValue.reg.s8;
        else if (newLeft == NativeTypeKind::S64)
            left->computedValue.reg.s64 = left->computedValue.reg.s8;
        else if (newLeft == NativeTypeKind::F32)
            left->computedValue.reg.f32 = static_cast<float>(left->computedValue.reg.s8);
        else if (newLeft == NativeTypeKind::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.s8);
        break;
    case NativeTypeKind::S16:
        if (newLeft == NativeTypeKind::S32)
            left->computedValue.reg.s32 = left->computedValue.reg.s16;
        else if (newLeft == NativeTypeKind::S64)
            left->computedValue.reg.s64 = left->computedValue.reg.s16;
        else if (newLeft == NativeTypeKind::F32)
            left->computedValue.reg.f32 = static_cast<float>(left->computedValue.reg.s16);
        else if (newLeft == NativeTypeKind::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.s16);
        break;
    case NativeTypeKind::S32:
        if (newLeft == NativeTypeKind::S64)
            left->computedValue.reg.s64 = left->computedValue.reg.s32;
        else if (newLeft == NativeTypeKind::F32)
            left->computedValue.reg.f32 = static_cast<float>(left->computedValue.reg.s32);
        else if (newLeft == NativeTypeKind::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.s32);
        break;
    case NativeTypeKind::S64:
        if (newLeft == NativeTypeKind::F64)
            left->computedValue.reg.f64 = static_cast<double>(left->computedValue.reg.s64);
        break;
    case NativeTypeKind::F32:
        if (newLeft == NativeTypeKind::F64)
            left->computedValue.reg.f64 = left->computedValue.reg.f32;
        break;
    }
}

bool TypeManager::makeCompatibles(SemanticContext* context, AstNode* leftNode, AstNode* rightNode, uint32_t castFlags)
{
    SWAG_CHECK(makeCompatibles(context, leftNode->typeInfo, rightNode, castFlags));
    return true;
}

bool TypeManager::convertExpressionListToVarDecl(SemanticContext* context, TypeInfo* toType, AstNode* nodeToCast)
{
    auto sourceFile = context->sourceFile;
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(toType, TypeInfoKind::Struct);

    if (nodeToCast->kind == AstNodeKind::FuncCallParam)
    {
        nodeToCast->setPassThrough();
        nodeToCast = nodeToCast->childs.front();
        if (nodeToCast->kind != AstNodeKind::ExpressionList)
            return true;
    }

    // Declare a variable
    auto varNode  = Ast::newVarDecl(sourceFile, format("__tmp_%d", g_Global.uniqueID.fetch_add(1)), nodeToCast->parent);
    auto typeNode = Ast::newTypeExpression(sourceFile, varNode);
    typeNode->flags |= AST_HAS_STRUCT_PARAMETERS;
    varNode->type        = typeNode;
    typeNode->identifier = Ast::newIdentifierRef(sourceFile, typeStruct->structNode->name, typeNode);
	typeNode->identifier->flags |= AST_GENERATED;
    auto back            = typeNode->identifier->childs.back();
    back->flags &= ~AST_NO_BYTECODE;
    back->flags |= AST_IN_TYPE_VAR_DECLARATION;

    // And make a reference to that variable
    auto parent = nodeToCast;
    if (nodeToCast->parent->kind == AstNodeKind::FuncCallParam)
        parent = nodeToCast->parent;
    auto identifierRef = Ast::newIdentifierRef(sourceFile, varNode->name, parent);
    identifierRef->flags |= AST_R_VALUE | AST_TRANSIENT | AST_DONT_COLLECT;

    // Make parameters
    auto identifier            = CastAst<AstIdentifier>(typeNode->identifier->childs.back(), AstNodeKind::Identifier);
    identifier->callParameters = Ast::newFuncCallParams(sourceFile, identifier);
    int countParams            = (int) nodeToCast->childs.size();
    if (parent == nodeToCast)
        countParams--;
    for (int i = 0; i < countParams; i++)
    {
        auto         oneChild = nodeToCast->childs[i];
        auto         oneParam = Ast::newFuncCallParam(sourceFile, identifier->callParameters);
        CloneContext cloneContext;
        cloneContext.parent = oneParam;
        oneChild->clone(cloneContext);
        oneChild->flags |= AST_NO_BYTECODE | AST_DISABLED;
    }

    // For a tuple initialization, every parameters must be covered
    if (typeStruct->flags & TYPEINFO_STRUCT_IS_TUPLE)
    {
        if (countParams != typeStruct->childs.size())
        {
            return context->errorContext.report({sourceFile, identifier, format("not enough parameters in tuple initialization ('%d' expected, '%d' provided)", typeStruct->childs.size(), countParams)});
        }
    }

    // Add the 2 nodes to the semantic
    auto b = context->job->nodes.back();
    context->job->nodes.pop_back();
    context->job->nodes.push_back(identifierRef);
    context->job->nodes.push_back(varNode);
    context->job->nodes.push_back(b);

    nodeToCast->typeInfo = toType;
    nodeToCast->setPassThrough();
    return true;
}

bool TypeManager::makeCompatibles(SemanticContext* context, TypeInfo* toType, AstNode* nodeToCast, uint32_t castFlags)
{
    // convert {...} expression list to a structure : this will create a variable, with parameters
    auto fromType = concreteType(nodeToCast->typeInfo, MakeConcrete::FlagAlias);
    if (fromType->kind == TypeInfoKind::TypeList && toType->kind == TypeInfoKind::Struct)
    {
        TypeInfoList* typeList = CastTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeList);
        if (typeList->listKind == TypeInfoListKind::Curly)
        {
            SWAG_CHECK(convertExpressionListToVarDecl(context, toType, nodeToCast));
            return true;
        }
    }

    SWAG_CHECK(makeCompatibles(context, toType, nodeToCast->typeInfo, nodeToCast, castFlags));
    if (!nodeToCast)
        return true;

    if ((nodeToCast->typeInfo->flags & TYPEINFO_AUTO_CAST) && !nodeToCast->castedTypeInfo)
    {
        if (!(castFlags & CASTFLAG_JUST_CHECK))
        {
            nodeToCast->castedTypeInfo = nodeToCast->typeInfo;
            nodeToCast->typeInfo       = toType;
        }
    }

    if (nodeToCast->typeInfo->kind == TypeInfoKind::TypeList && !(nodeToCast->flags & AST_SLICE_INIT_EXPRESSION))
    {
        TypeInfoList* typeList = CastTypeInfo<TypeInfoList>(nodeToCast->typeInfo, TypeInfoKind::TypeList);
        auto          fromSize = typeList->childs.size();

        while (nodeToCast && nodeToCast->kind != AstNodeKind::ExpressionList)
            nodeToCast = nodeToCast->childs.empty() ? nullptr : nodeToCast->childs.front();

        if (nodeToCast && (nodeToCast->flags & AST_CONST_EXPR))
        {
            SWAG_ASSERT(fromSize == nodeToCast->childs.size());
            auto module   = context->sourceFile->module;
            auto exprList = CastAst<AstExpressionList>(nodeToCast, AstNodeKind::ExpressionList);
            if (exprList && exprList->storageOffsetSegment == UINT32_MAX)
                SWAG_CHECK(SemanticJob::reserveAndStoreToSegment(context, exprList->storageOffsetSegment, &module->constantSegment, nullptr, nodeToCast->typeInfo, exprList));
        }
    }

    return true;
}

bool TypeManager::makeCompatibles(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* nodeToCast, uint32_t castFlags)
{
    // convert {...} expression list to a structure : this will create a variable, with parameters
    auto realFromType = concreteType(fromType, MakeConcrete::FlagAlias);
    auto realToType   = concreteType(toType, MakeConcrete::FlagAlias);
    if (realFromType->kind == TypeInfoKind::TypeList && realToType->kind == TypeInfoKind::Struct)
    {
        TypeInfoList* typeList = CastTypeInfo<TypeInfoList>(realFromType, TypeInfoKind::TypeList);
        if (typeList->listKind == TypeInfoListKind::Curly)
        {
            return true;
        }
    }

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

    if (fromType == toType)
        return true;

    // Everything can be casted to type 'any'
    if (toType->isNative(NativeTypeKind::Any))
    {
        if (nodeToCast && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            nodeToCast->castedTypeInfo = nodeToCast->typeInfo;
            nodeToCast->typeInfo       = toType;
            auto& typeTable            = context->sourceFile->module->typeTable;
            SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, nodeToCast->castedTypeInfo, &nodeToCast->concreteTypeInfo, &nodeToCast->concreteTypeInfoStorage));
        }

        return true;
    }

    if (fromType->kind == TypeInfoKind::TypedVariadic)
        fromType = ((TypeInfoVariadic*) fromType)->rawType;
    if (toType->kind == TypeInfoKind::TypedVariadic)
        toType = ((TypeInfoVariadic*) toType)->rawType;

    // Const mismatch
    if (!toType->isConst() && fromType->isConst())
    {
        if (!(castFlags & CASTFLAG_UNCONST))
            return castError(context, toType, fromType, nodeToCast, castFlags);

        // We can affect a const to an unconst if type is by copy, and we are in an affectation
        if (!(fromType->flags & TYPEINFO_RETURN_BY_COPY) && !(toType->flags & TYPEINFO_RETURN_BY_COPY))
            return castError(context, toType, fromType, nodeToCast, castFlags);
    }

    if (fromType->isSame(toType, ISSAME_CAST))
        return true;

    if (fromType->flags & TYPEINFO_AUTO_CAST)
        castFlags |= CASTFLAG_FORCE;

    // Always match against a generic
    if (toType->kind == TypeInfoKind::Generic)
        return true;

    // Pointer to pointer
    if (toType->kind == TypeInfoKind::Pointer && fromType->kind == TypeInfoKind::Pointer)
    {
        if (castFlags & CASTFLAG_FORCE)
            return true;
    }

    // Struct to pointer
    if (toType->kind == TypeInfoKind::Pointer && fromType->kind == TypeInfoKind::Struct)
    {
        auto typePtr = static_cast<TypeInfoPointer*>(toType);
        if (typePtr->ptrCount == 1 && typePtr->pointedType->isSame(fromType, ISSAME_CAST))
        {
            if (nodeToCast && (castFlags & CASTFLAG_JUST_CHECK))
            {
                nodeToCast->castedTypeInfo = nodeToCast->typeInfo;
                nodeToCast->typeInfo       = toType;
            }

            return true;
        }
    }

    // String <=> null
    if (toType->isNative(NativeTypeKind::String) && fromType == g_TypeMgr.typeInfoNull)
    {
        if (nodeToCast && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            nodeToCast->typeInfo       = toType;
            nodeToCast->castedTypeInfo = g_TypeMgr.typeInfoNull;
        }

        return true;
    }

    if (toType == g_TypeMgr.typeInfoNull && fromType->isNative(NativeTypeKind::String))
        return true;

    // Cast to native type
    if (toType->kind == TypeInfoKind::Native)
        return castToNative(context, toType, fromType, nodeToCast, castFlags);

    // Cast to array
    if (toType->kind == TypeInfoKind::Array)
        return castToArray(context, toType, fromType, nodeToCast, castFlags);

    // Cast to slice
    if (toType->kind == TypeInfoKind::Slice)
        return castToSlice(context, toType, fromType, nodeToCast, castFlags);

    // Cast to lambda
    if (toType->kind == TypeInfoKind::Lambda)
    {
        if (toType->isSame(fromType, ISSAME_CAST))
            return true;
    }

    return castError(context, toType, fromType, nodeToCast, castFlags);
}
