#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Wmf/Module.h"

bool TypeManager::castToNativeBool(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (fromType == g_TypeMgr->typeInfoBool)
        return true;

    // Automatic cast to a bool is done only if requested, on specific nodes (like if or while expressions)
    if (!castFlags.has(CAST_FLAG_AUTO_BOOL) && !castFlags.has(CAST_FLAG_EXPLICIT))
        return castError(context, g_TypeMgr->typeInfoBool, fromType, fromNode, castFlags);

    fromType = concreteType(fromType);

    if (fromType->isPointer() ||
        fromType->isLambdaClosure() ||
        fromType->isInterface() ||
        fromType->isSlice())
    {
        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
        {
            fromNode->typeInfo     = g_TypeMgr->typeInfoBool;
            fromNode->typeInfoCast = fromType;
            return true;
        }
    }

    if (fromType->isNative())
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::U8:
                if (fromNode)
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                    {
                        if (fromNode->hasFlagComputedValue())
                        {
                            fromNode->computedValue()->reg.b = fromNode->computedValue()->reg.u8;
                            fromNode->typeInfo               = g_TypeMgr->typeInfoBool;
                        }
                        else
                        {
                            fromNode->typeInfo     = g_TypeMgr->typeInfoBool;
                            fromNode->typeInfoCast = fromType;
                        }
                    }
                }
                return true;

            case NativeTypeKind::S16:
            case NativeTypeKind::U16:
                if (fromNode)
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                    {
                        if (fromNode->hasFlagComputedValue())
                        {
                            fromNode->computedValue()->reg.b = fromNode->computedValue()->reg.u16;
                            fromNode->typeInfo               = g_TypeMgr->typeInfoBool;
                        }
                        else
                        {
                            fromNode->typeInfo     = g_TypeMgr->typeInfoBool;
                            fromNode->typeInfoCast = fromType;
                        }
                    }
                }
                return true;

            case NativeTypeKind::S32:
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                if (fromNode)
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                    {
                        if (fromNode->hasFlagComputedValue())
                        {
                            fromNode->computedValue()->reg.b = fromNode->computedValue()->reg.u32;
                            fromNode->typeInfo               = g_TypeMgr->typeInfoBool;
                        }
                        else
                        {
                            fromNode->typeInfo     = g_TypeMgr->typeInfoBool;
                            fromNode->typeInfoCast = fromType;
                        }
                    }
                }
                return true;

            case NativeTypeKind::S64:
            case NativeTypeKind::U64:
                if (fromNode)
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                    {
                        if (fromNode->hasFlagComputedValue())
                        {
                            fromNode->computedValue()->reg.b = fromNode->computedValue()->reg.u64;
                            fromNode->typeInfo               = g_TypeMgr->typeInfoBool;
                        }
                        else
                        {
                            fromNode->typeInfo     = g_TypeMgr->typeInfoBool;
                            fromNode->typeInfoCast = fromType;
                        }
                    }
                }
                return true;

            case NativeTypeKind::F32:
                if (fromNode)
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                    {
                        if (fromNode->hasFlagComputedValue())
                        {
                            fromNode->computedValue()->reg.b = fromNode->computedValue()->reg.f32 != 0;
                            fromNode->typeInfo               = g_TypeMgr->typeInfoBool;
                        }
                        else
                        {
                            fromNode->typeInfo     = g_TypeMgr->typeInfoBool;
                            fromNode->typeInfoCast = fromType;
                        }
                    }
                }
                return true;

            case NativeTypeKind::F64:
                if (fromNode)
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                    {
                        if (fromNode->hasFlagComputedValue())
                        {
                            fromNode->computedValue()->reg.b = fromNode->computedValue()->reg.f64 != 0;
                            fromNode->typeInfo               = g_TypeMgr->typeInfoBool;
                        }
                        else
                        {
                            fromNode->typeInfo     = g_TypeMgr->typeInfoBool;
                            fromNode->typeInfoCast = fromType;
                        }
                    }
                }
                return true;

            case NativeTypeKind::String:
                if (fromNode)
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                    {
                        if (fromNode->hasFlagComputedValue())
                        {
                            fromNode->computedValue()->reg.b = true;
                            fromNode->typeInfo               = g_TypeMgr->typeInfoBool;
                        }
                        else
                        {
                            fromNode->typeInfo     = g_TypeMgr->typeInfoBool;
                            fromNode->typeInfoCast = fromType;
                        }
                    }
                }
                return true;

            default:
                break;
        }
    }

    return castError(context, g_TypeMgr->typeInfoBool, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeRune(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (fromType->nativeType == NativeTypeKind::Rune)
        return true;

    if (castFlags.has(CAST_FLAG_EXPLICIT) && castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::U8:
            case NativeTypeKind::U16:
            case NativeTypeKind::U32:
                break;
            default:
                return castError(context, g_TypeMgr->typeInfoRune, fromType, fromNode, castFlags);
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::Rune:
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
            case NativeTypeKind::Bool:
            case NativeTypeKind::U8:
            case NativeTypeKind::U16:
            case NativeTypeKind::U32:
            case NativeTypeKind::U64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                    {
                        fromNode->typeInfo = g_TypeMgr->typeInfoU32;
                    }
                }
                return true;

            case NativeTypeKind::F32:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                    {
                        fromNode->computedValue()->reg.u64 = static_cast<uint64_t>(fromNode->computedValue()->reg.f32);
                        fromNode->typeInfo                 = g_TypeMgr->typeInfoU32;
                    }
                }
                return true;

            case NativeTypeKind::F64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                    {
                        fromNode->computedValue()->reg.u64 = static_cast<uint64_t>(fromNode->computedValue()->reg.f64);
                        fromNode->typeInfo                 = g_TypeMgr->typeInfoU32;
                    }
                }
                return true;

            case NativeTypeKind::String:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    uint32_t ch;
                    if (fromNode->computedValue()->text.toChar32(ch))
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                        {
                            fromNode->computedValue()->reg.u64 = ch;
                            fromNode->typeInfo                 = g_TypeMgr->typeInfoRune;
                        }

                        return true;
                    }
                }
                break;

            default:
                break;
        }
    }
    else
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S32:
                if (fromType->isCharacter())
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                    {
                        fromNode->typeInfo = g_TypeMgr->typeInfoRune;
                    }

                    return true;
                }

                break;

            case NativeTypeKind::String:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    uint32_t ch;
                    if (fromNode->computedValue()->text.toChar32(ch))
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                        {
                            fromNode->computedValue()->reg.u64 = ch;
                            fromNode->typeInfo                 = g_TypeMgr->typeInfoRune;
                        }

                        return true;
                    }
                }
                break;
            default:
                break;
        }
    }

    return castError(context, g_TypeMgr->typeInfoRune, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU8(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U8)
        return true;

    if (!isOverflowEnabled(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.s64 < 0)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU8, true);
                        return false;
                    }
                }
                else if (fromType->isUntypedInteger())
                {
                    if (!fromNode)
                    {
                        const auto native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                        const auto value  = native->valueInteger;
                        if (value < 0)
                            return false;
                    }
                }
                [[fallthrough]];

            case NativeTypeKind::U16:
            case NativeTypeKind::U32:
            case NativeTypeKind::U64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.u64 > UINT8_MAX)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU8);
                        return false;
                    }
                }
                else if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
                {
                    if (!fromNode)
                    {
                        const auto native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                        const auto value  = static_cast<uint32_t>(native->valueInteger);
                        if (value > UINT8_MAX)
                            return false;
                    }
                }
                break;

            case NativeTypeKind::F32:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f32 <= -SAFETY_ZERO_EPSILON)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU8, true);
                        return false;
                    }

                    if (fromNode->computedValue()->reg.f32 >= static_cast<float>(UINT8_MAX) + 0.5f)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU8);
                        return false;
                    }
                }
                break;

            case NativeTypeKind::F64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f64 <= -SAFETY_ZERO_EPSILON)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU8, true);
                        return false;
                    }

                    if (fromNode->computedValue()->reg.f64 >= static_cast<double>(UINT8_MAX) + 0.5)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU8);
                        return false;
                    }
                }
                break;

            default:
                break;
        }
    }

    if (!castFlags.has(CAST_FLAG_EXPLICIT) || castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
            case NativeTypeKind::U16:
            case NativeTypeKind::U32:
            case NativeTypeKind::U64:
            {
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                        fromNode->typeInfo = g_TypeMgr->typeInfoU8;
                    return true;
                }

                if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
                {
                    return true;
                }
            }
            break;

            default:
                break;
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT) && castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::U8:
                break;
            default:
                return castError(context, g_TypeMgr->typeInfoU8, fromType, fromNode, castFlags);
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT))
    {
        const bool canChange = fromNode && fromNode->hasFlagComputedValue() && !castFlags.has(CAST_FLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoU8;
        switch (fromType->nativeType)
        {
            case NativeTypeKind::Rune:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint8_t>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::S8:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint8_t>(fromNode->computedValue()->reg.s8);
                return true;
            case NativeTypeKind::S16:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint8_t>(fromNode->computedValue()->reg.s16);
                return true;
            case NativeTypeKind::S32:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint8_t>(fromNode->computedValue()->reg.s32);
                return true;
            case NativeTypeKind::S64:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint8_t>(fromNode->computedValue()->reg.s64);
                return true;
            case NativeTypeKind::Bool:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint8_t>(fromNode->computedValue()->reg.b) ? 1 : 0;
                return true;
            case NativeTypeKind::U16:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint8_t>(fromNode->computedValue()->reg.u16);
                return true;
            case NativeTypeKind::U32:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint8_t>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::U64:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint8_t>(fromNode->computedValue()->reg.u64);
                return true;
            case NativeTypeKind::F32:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint8_t>(fromNode->computedValue()->reg.f32);
                return true;
            case NativeTypeKind::F64:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint8_t>(fromNode->computedValue()->reg.f64);
                return true;
            default:
                break;
        }
    }

    return castError(context, g_TypeMgr->typeInfoU8, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU16(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U16)
        return true;

    if (!isOverflowEnabled(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.s64 < 0)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU16, true);
                        return false;
                    }
                }
                else if (fromType->isUntypedInteger())
                {
                    if (!fromNode)
                    {
                        const auto native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                        const auto value  = native->valueInteger;
                        if (value < 0)
                            return false;
                    }
                }
                [[fallthrough]];

            case NativeTypeKind::U8:
            case NativeTypeKind::U32:
            case NativeTypeKind::U64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.u64 > UINT16_MAX)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU16);
                        return false;
                    }
                }
                else if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
                {
                    if (!fromNode)
                    {
                        const auto native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                        const auto value  = static_cast<uint32_t>(native->valueInteger);
                        if (value > UINT16_MAX)
                            return false;
                    }
                }
                break;

            case NativeTypeKind::F32:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f32 <= -SAFETY_ZERO_EPSILON)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU16, true);
                        return false;
                    }
                    if (fromNode->computedValue()->reg.f32 >= static_cast<float>(UINT16_MAX) + 0.5f)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU16);
                        return false;
                    }
                }
                break;

            case NativeTypeKind::F64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f64 <= -SAFETY_ZERO_EPSILON)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU16, true);
                        return false;
                    }
                    if (fromNode->computedValue()->reg.f64 >= static_cast<double>(UINT16_MAX) + 0.5)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU16);
                        return false;
                    }
                }
                break;
            default:
                break;
        }
    }

    if (!castFlags.has(CAST_FLAG_EXPLICIT) || castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
            case NativeTypeKind::U8:
            case NativeTypeKind::U32:
            case NativeTypeKind::U64:
            {
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                        fromNode->typeInfo = g_TypeMgr->typeInfoU16;
                    return true;
                }
                if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
                {
                    return true;
                }
            }
            break;

            default:
                break;
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT) && castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::U8:
                break;
            default:
                return castError(context, g_TypeMgr->typeInfoU16, fromType, fromNode, castFlags);
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT))
    {
        const bool canChange = fromNode && fromNode->hasFlagComputedValue() && !castFlags.has(CAST_FLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoU16;
        switch (fromType->nativeType)
        {
            case NativeTypeKind::Rune:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint16_t>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::S8:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint16_t>(fromNode->computedValue()->reg.s8);
                return true;
            case NativeTypeKind::S16:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint16_t>(fromNode->computedValue()->reg.s16);
                return true;
            case NativeTypeKind::S32:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint16_t>(fromNode->computedValue()->reg.s32);
                return true;
            case NativeTypeKind::S64:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint16_t>(fromNode->computedValue()->reg.s64);
                return true;
            case NativeTypeKind::Bool:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint16_t>(fromNode->computedValue()->reg.b) ? 1 : 0;
                return true;
            case NativeTypeKind::U8:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint16_t>(fromNode->computedValue()->reg.u8);
                return true;
            case NativeTypeKind::U32:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint16_t>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::U64:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint16_t>(fromNode->computedValue()->reg.u64);
                return true;
            case NativeTypeKind::F32:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint16_t>(fromNode->computedValue()->reg.f32);
                return true;
            case NativeTypeKind::F64:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint16_t>(fromNode->computedValue()->reg.f64);
                return true;
            default:
                break;
        }
    }

    return castError(context, g_TypeMgr->typeInfoU16, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U32)
        return true;

    if (!isOverflowEnabled(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.s64 < 0)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU32, true);
                        return false;
                    }
                }
                else if (fromType->isUntypedInteger())
                {
                    if (!fromNode)
                    {
                        const auto native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                        const auto value  = native->valueInteger;
                        if (value < 0)
                            return false;
                    }
                }
                [[fallthrough]];

            case NativeTypeKind::U8:
            case NativeTypeKind::U16:
            case NativeTypeKind::U64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.u64 > UINT32_MAX)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU32);
                        return false;
                    }
                }
                break;

            case NativeTypeKind::F32:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f32 <= -SAFETY_ZERO_EPSILON)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU32, true);
                        return false;
                    }
                    if (fromNode->computedValue()->reg.f32 >= static_cast<float>(UINT32_MAX) + 0.5f)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU32);
                        return false;
                    }
                }
                break;

            case NativeTypeKind::F64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f64 <= -SAFETY_ZERO_EPSILON)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU32, true);
                        return false;
                    }
                    if (fromNode->computedValue()->reg.f64 >= static_cast<double>(UINT32_MAX) + 0.5)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU32);
                        return false;
                    }
                }
                break;

            default:
                break;
        }
    }

    if (!castFlags.has(CAST_FLAG_EXPLICIT) || castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
            case NativeTypeKind::U8:
            case NativeTypeKind::U16:
            case NativeTypeKind::U64:
            {
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                        fromNode->typeInfo = g_TypeMgr->typeInfoU32;
                    return true;
                }
                if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
                {
                    return true;
                }
            }
            break;

            default:
                break;
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT) && castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::U8:
            case NativeTypeKind::U16:
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                break;
            default:
                return castError(context, g_TypeMgr->typeInfoU32, fromType, fromNode, castFlags);
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT))
    {
        const bool canChange = fromNode && fromNode->hasFlagComputedValue() && !castFlags.has(CAST_FLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoU32;
        switch (fromType->nativeType)
        {
            case NativeTypeKind::Rune:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = fromNode->computedValue()->reg.u32;
                return true;
            case NativeTypeKind::S8:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint32_t>(fromNode->computedValue()->reg.s8);
                return true;
            case NativeTypeKind::S16:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint32_t>(fromNode->computedValue()->reg.s16);
                return true;
            case NativeTypeKind::S32:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint32_t>(fromNode->computedValue()->reg.s32);
                return true;
            case NativeTypeKind::S64:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint32_t>(fromNode->computedValue()->reg.s64);
                return true;
            case NativeTypeKind::Bool:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint32_t>(fromNode->computedValue()->reg.b) ? 1 : 0;
                return true;
            case NativeTypeKind::U8:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint32_t>(fromNode->computedValue()->reg.u8);
                return true;
            case NativeTypeKind::U16:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint32_t>(fromNode->computedValue()->reg.u16);
                return true;
            case NativeTypeKind::U64:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint32_t>(fromNode->computedValue()->reg.u64);
                return true;
            case NativeTypeKind::F32:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint32_t>(fromNode->computedValue()->reg.f32);
                return true;
            case NativeTypeKind::F64:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint32_t>(fromNode->computedValue()->reg.f64);
                return true;
            default:
                break;
        }
    }

    return castError(context, g_TypeMgr->typeInfoU32, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U64)
        return true;

    if (fromType->isPointer() || fromType->isLambdaClosure())
    {
        if (castFlags.has(CAST_FLAG_EXPLICIT))
        {
            if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                fromNode->typeInfo = g_TypeMgr->typeInfoU64;
            return true;
        }
    }

    if (!isOverflowEnabled(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.s64 < 0)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU64, true);
                        return false;
                    }
                }
                else if (fromType->isUntypedInteger())
                {
                    if (!fromNode)
                    {
                        const auto native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                        const auto value  = native->valueInteger;
                        if (value < 0)
                            return false;
                    }
                }
                break;

            case NativeTypeKind::F32:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f32 <= -SAFETY_ZERO_EPSILON)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU64, true);
                        return false;
                    }
                }
                break;

            case NativeTypeKind::F64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f64 <= -SAFETY_ZERO_EPSILON)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU64, true);
                        return false;
                    }
                    if (fromNode->computedValue()->reg.f64 >= static_cast<double>(UINT64_MAX) + 0.5)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoU64);
                        return false;
                    }
                }
                break;

            default:
                break;
        }
    }

    if (!castFlags.has(CAST_FLAG_EXPLICIT) || castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
            case NativeTypeKind::U8:
            case NativeTypeKind::U16:
            case NativeTypeKind::U32:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                        fromNode->typeInfo = g_TypeMgr->typeInfoU64;
                    return true;
                }

                if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
                {
                    return true;
                }
                break;

            default:
                break;
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT) && castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
            case NativeTypeKind::U8:
            case NativeTypeKind::U16:
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                break;
            default:
                return castError(context, g_TypeMgr->typeInfoU64, fromType, fromNode, castFlags);
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT))
    {
        const bool canChange = fromNode && fromNode->hasFlagComputedValue() && !castFlags.has(CAST_FLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoU64;
        switch (fromType->nativeType)
        {
            case NativeTypeKind::Rune:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint64_t>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::S8:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint64_t>(fromNode->computedValue()->reg.s8);
                return true;
            case NativeTypeKind::S16:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint64_t>(fromNode->computedValue()->reg.s16);
                return true;
            case NativeTypeKind::S32:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint64_t>(fromNode->computedValue()->reg.s32);
                return true;
            case NativeTypeKind::S64:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint64_t>(fromNode->computedValue()->reg.s64);
                return true;
            case NativeTypeKind::Bool:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint64_t>(fromNode->computedValue()->reg.b) ? 1 : 0;
                return true;
            case NativeTypeKind::U8:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint64_t>(fromNode->computedValue()->reg.u8);
                return true;
            case NativeTypeKind::U16:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint64_t>(fromNode->computedValue()->reg.u16);
                return true;
            case NativeTypeKind::U32:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint64_t>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::F32:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint64_t>(fromNode->computedValue()->reg.f32);
                return true;
            case NativeTypeKind::F64:
                if (canChange)
                    fromNode->computedValue()->reg.u64 = static_cast<uint64_t>(fromNode->computedValue()->reg.f64);
                return true;
            default:
                break;
        }
    }

    return castError(context, g_TypeMgr->typeInfoU64, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS8(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S8)
        return true;

    if (!isOverflowEnabled(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::U8:
            case NativeTypeKind::U16:
            case NativeTypeKind::U32:
            case NativeTypeKind::U64:
                if (fromType->isUntypedBinHex())
                {
                    const auto native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                    const auto value  = native->valueInteger;
                    if (value > UINT8_MAX)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS8);
                        return false;
                    }
                    break;
                }

                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.u64 > INT8_MAX)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS8);
                        return false;
                    }
                }
                [[fallthrough]];

            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.s64 < INT8_MIN || fromNode->computedValue()->reg.s64 > INT8_MAX)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS8);
                        return false;
                    }
                }
                else if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
                {
                    if (!fromNode)
                    {
                        const auto native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                        const auto value  = native->valueInteger;
                        if (value < INT8_MIN || value > INT8_MAX)
                            return false;
                    }
                }
                break;

            case NativeTypeKind::F32:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f32 <= static_cast<float>(INT8_MIN) - 0.5f)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS8);
                        return false;
                    }
                    if (fromNode->computedValue()->reg.f32 >= static_cast<float>(INT8_MAX) + 0.5f)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS8);
                        return false;
                    }
                }
                break;

            case NativeTypeKind::F64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f64 <= static_cast<double>(INT8_MIN) - 0.5)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS8);
                        return false;
                    }
                    if (fromNode->computedValue()->reg.f64 >= static_cast<double>(INT8_MAX) + 0.5)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS8);
                        return false;
                    }
                }
                break;

            default:
                break;
        }
    }

    if (!castFlags.has(CAST_FLAG_EXPLICIT) || castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::U32:
                if (fromType->isUntypedBinHex())
                {
                    const auto native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                    const auto value  = native->valueInteger;
                    if (value <= UINT8_MAX)
                        return true;
                }
                break;

            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                        fromNode->typeInfo = g_TypeMgr->typeInfoS8;
                    return true;
                }

                if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
                {
                    return true;
                }
                break;

            default:
                break;
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT) && castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::U8:
                break;
            default:
                return castError(context, g_TypeMgr->typeInfoS8, fromType, fromNode, castFlags);
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT))
    {
        const bool canChange = fromNode && fromNode->hasFlagComputedValue() && !castFlags.has(CAST_FLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoS8;
        switch (fromType->nativeType)
        {
            case NativeTypeKind::Rune:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int8_t>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::S16:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int8_t>(fromNode->computedValue()->reg.s16);
                return true;
            case NativeTypeKind::S32:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int8_t>(fromNode->computedValue()->reg.s32);
                return true;
            case NativeTypeKind::S64:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int8_t>(fromNode->computedValue()->reg.s64);
                return true;
            case NativeTypeKind::Bool:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int8_t>(fromNode->computedValue()->reg.b) ? 1 : 0;
                return true;
            case NativeTypeKind::U8:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int8_t>(fromNode->computedValue()->reg.u8);
                return true;
            case NativeTypeKind::U16:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int8_t>(fromNode->computedValue()->reg.u16);
                return true;
            case NativeTypeKind::U32:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int8_t>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::U64:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int8_t>(fromNode->computedValue()->reg.u64);
                return true;
            case NativeTypeKind::F32:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int8_t>(fromNode->computedValue()->reg.f32);
                return true;
            case NativeTypeKind::F64:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int8_t>(fromNode->computedValue()->reg.f64);
                return true;
            default:
                break;
        }
    }

    return castError(context, g_TypeMgr->typeInfoS8, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS16(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S16)
        return true;

    if (!isOverflowEnabled(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::U16:
            case NativeTypeKind::U32:
            case NativeTypeKind::U64:
                if (fromType->isUntypedBinHex())
                {
                    const auto native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                    const auto value  = native->valueInteger;
                    if (value > UINT16_MAX)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS16);
                        return false;
                    }
                    break;
                }

                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.u64 > INT16_MAX)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS16);
                        return false;
                    }
                }
                [[fallthrough]];

            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.s64 < INT16_MIN || fromNode->computedValue()->reg.s64 > INT16_MAX)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS16);
                        return false;
                    }
                }
                else if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
                {
                    if (!fromNode)
                    {
                        const auto native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                        const auto value  = native->valueInteger;
                        if (value < INT16_MIN || value > INT16_MAX)
                            return false;
                    }
                }
                break;

            case NativeTypeKind::F32:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f32 <= static_cast<float>(INT16_MIN) - 0.5f)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS16);
                        return false;
                    }
                    if (fromNode->computedValue()->reg.f32 >= static_cast<float>(INT16_MAX) + 0.5f)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS16);
                        return false;
                    }
                }
                break;

            case NativeTypeKind::F64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f64 <= static_cast<double>(INT16_MIN) - 0.5)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS16);
                        return false;
                    }
                    if (fromNode->computedValue()->reg.f64 >= static_cast<double>(INT16_MAX) + 0.5)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS16);
                        return false;
                    }
                }
                break;

            default:
                break;
        }
    }

    if (!castFlags.has(CAST_FLAG_EXPLICIT) || castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::U32:
                if (fromType->isUntypedBinHex())
                {
                    const auto native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                    const auto value  = native->valueInteger;
                    if (value <= UINT16_MAX)
                        return true;
                }
                break;

            case NativeTypeKind::S8:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                        fromNode->typeInfo = g_TypeMgr->typeInfoS16;
                    return true;
                }

                if (fromType->isUntypedInteger())
                {
                    return true;
                }

                break;

            default:
                break;
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT) && castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::U8:
            case NativeTypeKind::U16:
                break;
            default:
                return castError(context, g_TypeMgr->typeInfoS16, fromType, fromNode, castFlags);
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT))
    {
        const bool canChange = fromNode && fromNode->hasFlagComputedValue() && !castFlags.has(CAST_FLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoS16;
        switch (fromType->nativeType)
        {
            case NativeTypeKind::Rune:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int16_t>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::S8:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int16_t>(fromNode->computedValue()->reg.s8);
                return true;
            case NativeTypeKind::S32:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int16_t>(fromNode->computedValue()->reg.s32);
                return true;
            case NativeTypeKind::S64:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int16_t>(fromNode->computedValue()->reg.s64);
                return true;
            case NativeTypeKind::Bool:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int16_t>(fromNode->computedValue()->reg.b) ? 1 : 0;
                return true;
            case NativeTypeKind::U8:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int16_t>(fromNode->computedValue()->reg.u8);
                return true;
            case NativeTypeKind::U16:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int16_t>(fromNode->computedValue()->reg.u16);
                return true;
            case NativeTypeKind::U32:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int16_t>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::U64:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int16_t>(fromNode->computedValue()->reg.u64);
                return true;
            case NativeTypeKind::F32:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int16_t>(fromNode->computedValue()->reg.f32);
                return true;
            case NativeTypeKind::F64:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int16_t>(fromNode->computedValue()->reg.f64);
                return true;
            default:
                break;
        }
    }

    return castError(context, g_TypeMgr->typeInfoS16, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S32)
        return true;

    if (!isOverflowEnabled(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::U32:
            case NativeTypeKind::U64:
                if (fromType->isUntypedBinHex())
                {
                    const auto native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                    const auto value  = native->valueInteger;
                    if (value > INT32_MAX)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS32);
                        return false;
                    }
                    break;
                }

                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.u64 > INT32_MAX)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS32);
                        return false;
                    }
                }
                [[fallthrough]];

            case NativeTypeKind::S64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.s64 < INT32_MIN || fromNode->computedValue()->reg.s64 > INT32_MAX)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS32);
                        return false;
                    }
                }
                else if (fromType->isUntypedInteger() || fromType->isUntypedBinHex())
                {
                    if (!fromNode)
                    {
                        const auto native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
                        const auto value  = native->valueInteger;
                        if (value < INT32_MIN || value > INT32_MAX)
                            return false;
                    }
                }
                break;

            case NativeTypeKind::F32:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f32 <= static_cast<float>(INT32_MIN) - 0.5f)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS32);
                        return false;
                    }

                    if (fromNode->computedValue()->reg.f32 >= static_cast<float>(INT32_MAX) + 0.5f)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS32);
                        return false;
                    }
                }
                break;

            case NativeTypeKind::F64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f64 <= static_cast<double>(INT32_MIN) - 0.5)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS32);
                        return false;
                    }

                    if (fromNode->computedValue()->reg.f64 >= static_cast<double>(INT32_MAX) + 0.5)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS32);
                        return false;
                    }
                }
                break;

            default:
                break;
        }
    }

    if (!castFlags.has(CAST_FLAG_EXPLICIT) || castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::U32:
                if (fromType->isUntypedBinHex())
                    return true;
                [[fallthrough]];

            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                        fromNode->typeInfo = g_TypeMgr->typeInfoS32;
                    return true;
                }

                if (fromType->isUntypedInteger())
                {
                    return true;
                }

                break;
            default:
                break;
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT) && castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::U8:
            case NativeTypeKind::U16:
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                break;
            default:
                return castError(context, g_TypeMgr->typeInfoS32, fromType, fromNode, castFlags);
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT))
    {
        const bool canChange = fromNode && fromNode->hasFlagComputedValue() && !castFlags.has(CAST_FLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoS32;
        switch (fromType->nativeType)
        {
            case NativeTypeKind::Rune:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int32_t>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::S8:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int32_t>(fromNode->computedValue()->reg.s8);
                return true;
            case NativeTypeKind::S16:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int32_t>(fromNode->computedValue()->reg.s16);
                return true;
            case NativeTypeKind::S64:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int32_t>(fromNode->computedValue()->reg.s64);
                return true;
            case NativeTypeKind::Bool:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int32_t>(fromNode->computedValue()->reg.b) ? 1 : 0;
                return true;
            case NativeTypeKind::U8:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int32_t>(fromNode->computedValue()->reg.u8);
                return true;
            case NativeTypeKind::U16:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int32_t>(fromNode->computedValue()->reg.u16);
                return true;
            case NativeTypeKind::U32:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int32_t>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::U64:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int32_t>(fromNode->computedValue()->reg.u64);
                return true;
            case NativeTypeKind::F32:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int32_t>(fromNode->computedValue()->reg.f32);
                return true;
            case NativeTypeKind::F64:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int32_t>(fromNode->computedValue()->reg.f64);
                return true;
            default:
                break;
        }
    }

    return castError(context, g_TypeMgr->typeInfoS32, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S64)
        return true;

    if (!isOverflowEnabled(context, fromNode, castFlags))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::U64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.u64 > INT64_MAX)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS64);
                        return false;
                    }
                }
                break;

            case NativeTypeKind::F32:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f32 <= static_cast<float>(INT64_MIN) - 0.5f)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS64);
                        return false;
                    }
                    if (fromNode->computedValue()->reg.f32 >= static_cast<float>(INT64_MAX) + 0.5f)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS64);
                        return false;
                    }
                }
                break;

            case NativeTypeKind::F64:
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (fromNode->computedValue()->reg.f64 <= static_cast<double>(INT64_MIN) - 0.5)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS64);
                        return false;
                    }
                    if (fromNode->computedValue()->reg.f64 >= static_cast<double>(INT64_MAX) + 0.5)
                    {
                        if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                            safetyErrorOutOfRange(context, fromNode, fromType, g_TypeMgr->typeInfoS64);
                        return false;
                    }
                }
                break;

            default:
                break;
        }
    }

    if (!castFlags.has(CAST_FLAG_EXPLICIT) || castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::U32:
                if (fromType->isUntypedBinHex())
                    return true;
                [[fallthrough]];

            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            {
                if (fromNode && fromNode->hasFlagComputedValue())
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                        fromNode->typeInfo = g_TypeMgr->typeInfoS64;
                    return true;
                }
                if (fromType->isUntypedInteger())
                {
                    return true;
                }
            }

            break;
            default:
                break;
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT) && castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::U8:
            case NativeTypeKind::U16:
            case NativeTypeKind::U32:
            case NativeTypeKind::U64:
                break;
            default:
                return castError(context, g_TypeMgr->typeInfoS64, fromType, fromNode, castFlags);
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT))
    {
        const bool canChange = fromNode && fromNode->hasFlagComputedValue() && !castFlags.has(CAST_FLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoS64;
        switch (fromType->nativeType)
        {
            case NativeTypeKind::Rune:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int64_t>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::S8:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int64_t>(fromNode->computedValue()->reg.s8);
                return true;
            case NativeTypeKind::S16:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int64_t>(fromNode->computedValue()->reg.s16);
                return true;
            case NativeTypeKind::S32:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int64_t>(fromNode->computedValue()->reg.s32);
                return true;
            case NativeTypeKind::Bool:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int64_t>(fromNode->computedValue()->reg.b) ? 1 : 0;
                return true;
            case NativeTypeKind::U8:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int64_t>(fromNode->computedValue()->reg.u8);
                return true;
            case NativeTypeKind::U16:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int64_t>(fromNode->computedValue()->reg.u16);
                return true;
            case NativeTypeKind::U32:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int64_t>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::U64:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int64_t>(fromNode->computedValue()->reg.u64);
                return true;
            case NativeTypeKind::F32:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int64_t>(fromNode->computedValue()->reg.f32);
                return true;
            case NativeTypeKind::F64:
                if (canChange)
                    fromNode->computedValue()->reg.s64 = static_cast<int64_t>(fromNode->computedValue()->reg.f64);
                return true;
            default:
                break;
        }
    }

    return castError(context, g_TypeMgr->typeInfoS64, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeF32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (fromType->nativeType == NativeTypeKind::F32)
        return true;

    if (!isOverflowEnabled(context, fromNode, castFlags))
    {
        if (!fromNode && fromType->isUntypedInteger())
        {
            const auto  native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
            const auto  value  = native->valueInteger;
            const float tmpF   = static_cast<float>(value);
            const auto  tmpI   = static_cast<int64_t>(tmpF);
            if (tmpI != value)
                return false;
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT) && castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::U8:
            case NativeTypeKind::U16:
            case NativeTypeKind::U32:
            case NativeTypeKind::F32:
                break;
            default:
                return castError(context, g_TypeMgr->typeInfoF32, fromType, fromNode, castFlags);
        }
    }

    if (fromType->isUntypedInteger() || fromType->isUntypedFloat())
        castFlags.add(CAST_FLAG_EXPLICIT);

    if (castFlags.has(CAST_FLAG_EXPLICIT))
    {
        const bool canChange = fromNode && fromNode->hasFlagComputedValue() && !castFlags.has(CAST_FLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoF32;
        switch (fromType->nativeType)
        {
            case NativeTypeKind::Rune:
                if (canChange)
                    fromNode->computedValue()->reg.f32 = static_cast<float>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::S8:
                if (canChange)
                    fromNode->computedValue()->reg.f32 = static_cast<float>(fromNode->computedValue()->reg.s8);
                return true;
            case NativeTypeKind::S16:
                if (canChange)
                    fromNode->computedValue()->reg.f32 = static_cast<float>(fromNode->computedValue()->reg.s16);
                return true;
            case NativeTypeKind::S32:
                if (canChange)
                    fromNode->computedValue()->reg.f32 = static_cast<float>(fromNode->computedValue()->reg.s32);
                return true;
            case NativeTypeKind::S64:
                if (canChange)
                    fromNode->computedValue()->reg.f32 = static_cast<float>(fromNode->computedValue()->reg.s64);
                return true;
            case NativeTypeKind::Bool:
                if (canChange)
                    fromNode->computedValue()->reg.f32 = fromNode->computedValue()->reg.b ? 1.0f : 0.0f;
                return true;
            case NativeTypeKind::U8:
                if (canChange)
                    fromNode->computedValue()->reg.f32 = static_cast<float>(fromNode->computedValue()->reg.u8);
                return true;
            case NativeTypeKind::U16:
                if (canChange)
                    fromNode->computedValue()->reg.f32 = static_cast<float>(fromNode->computedValue()->reg.u16);
                return true;
            case NativeTypeKind::U32:
                if (canChange)
                    fromNode->computedValue()->reg.f32 = static_cast<float>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::U64:
                if (canChange)
                    fromNode->computedValue()->reg.f32 = static_cast<float>(fromNode->computedValue()->reg.u64);
                return true;
            case NativeTypeKind::F64:
                if (canChange)
                    fromNode->computedValue()->reg.f32 = static_cast<float>(fromNode->computedValue()->reg.f64);
                return true;
            default:
                break;
        }
    }

    return castError(context, g_TypeMgr->typeInfoF32, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeF64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (fromType->nativeType == NativeTypeKind::F64)
        return true;

    if (!isOverflowEnabled(context, fromNode, castFlags))
    {
        if (!fromNode && fromType->isUntypedInteger())
        {
            const auto    native = castTypeInfo<TypeInfoNative>(fromType, fromType->kind);
            const auto    value  = native->valueInteger;
            const double  tmpF   = value;
            const int64_t tmpI   = static_cast<int64_t>(tmpF);
            if (tmpI != value)
                return false;
        }
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT) && castFlags.has(CAST_FLAG_COERCE))
    {
        switch (fromType->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::S16:
            case NativeTypeKind::S32:
            case NativeTypeKind::S64:
            case NativeTypeKind::U8:
            case NativeTypeKind::U16:
            case NativeTypeKind::U32:
            case NativeTypeKind::U64:
            case NativeTypeKind::F32:
                break;
            default:
                return castError(context, g_TypeMgr->typeInfoF32, fromType, fromNode, castFlags);
        }
    }

    if (fromType->isUntypedInteger() || fromType->isUntypedFloat())
        castFlags.add(CAST_FLAG_EXPLICIT);

    if (castFlags.has(CAST_FLAG_EXPLICIT))
    {
        const bool canChange = fromNode && fromNode->hasFlagComputedValue() && !castFlags.has(CAST_FLAG_JUST_CHECK);
        if (canChange)
            fromNode->typeInfo = g_TypeMgr->typeInfoF64;
        switch (fromType->nativeType)
        {
            case NativeTypeKind::Rune:
                if (canChange)
                    fromNode->computedValue()->reg.f64 = static_cast<double>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::S8:
                if (canChange)
                    fromNode->computedValue()->reg.f64 = static_cast<double>(fromNode->computedValue()->reg.s8);
                return true;
            case NativeTypeKind::S16:
                if (canChange)
                    fromNode->computedValue()->reg.f64 = static_cast<double>(fromNode->computedValue()->reg.s16);
                return true;
            case NativeTypeKind::S32:
                if (canChange)
                    fromNode->computedValue()->reg.f64 = static_cast<double>(fromNode->computedValue()->reg.s32);
                return true;
            case NativeTypeKind::S64:
                if (canChange)
                    fromNode->computedValue()->reg.f64 = static_cast<double>(fromNode->computedValue()->reg.s64);
                return true;
            case NativeTypeKind::Bool:
                if (canChange)
                    fromNode->computedValue()->reg.f64 = fromNode->computedValue()->reg.b ? 1.0 : 0.0;
                return true;
            case NativeTypeKind::U8:
                if (canChange)
                    fromNode->computedValue()->reg.f64 = static_cast<double>(fromNode->computedValue()->reg.u8);
                return true;
            case NativeTypeKind::U16:
                if (canChange)
                    fromNode->computedValue()->reg.f64 = static_cast<double>(fromNode->computedValue()->reg.u16);
                return true;
            case NativeTypeKind::U32:
                if (canChange)
                    fromNode->computedValue()->reg.f64 = static_cast<double>(fromNode->computedValue()->reg.u32);
                return true;
            case NativeTypeKind::U64:
                if (canChange)
                    fromNode->computedValue()->reg.f64 = static_cast<double>(fromNode->computedValue()->reg.u64);
                return true;
            case NativeTypeKind::F32:
                if (canChange)
                    fromNode->computedValue()->reg.f64 = static_cast<double>(fromNode->computedValue()->reg.f32);
                return true;
            default:
                break;
        }
    }

    return castError(context, g_TypeMgr->typeInfoF64, fromType, fromNode, castFlags);
}

bool TypeManager::castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, CastFlags castFlags)
{
    // Pick the best order
    if (castFlags.has(CAST_FLAG_COMMUTATIVE))
    {
        if (toType->isUntypedInteger() && !fromType->isUntypedInteger())
        {
            std::swap(toType, fromType);
            std::swap(toNode, fromNode);
        }
        else if (toType->isUntypedFloat() && !fromType->isUntypedFloat())
        {
            std::swap(toType, fromType);
            std::swap(toNode, fromNode);
        }
    }

    return castToNative(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    // Automatic conversions if coerce mode is on (for expressions)
    if (castFlags.has(CAST_FLAG_TRY_COERCE))
    {
        if (toType->sizeOf >= fromType->sizeOf)
        {
            const auto leftIsInt    = toType->isNativeIntegerOrRune();
            const auto rightIsInt   = fromType->isNativeIntegerOrRune();
            const auto leftIsFloat  = toType->isNativeFloat();
            const auto rightIsFloat = fromType->isNativeFloat();
            if ((leftIsInt && rightIsInt) || (leftIsFloat && rightIsFloat))
                castFlags.add(CAST_FLAG_EXPLICIT | CAST_FLAG_COERCE);
            else if (leftIsFloat && fromType->isNativeInteger())
                castFlags.add(CAST_FLAG_EXPLICIT | CAST_FLAG_COERCE);
        }
        else
        {
            if (toType->isNativeFloat() && fromType->isNativeInteger())
                castFlags.add(CAST_FLAG_EXPLICIT | CAST_FLAG_COERCE);
        }
    }

    // If it matches, then it matches with a conversion
    if (!toType->isPointerNull() && !fromType->isPointerNull())
    {
        if (!toType->isSame(fromType, CAST_FLAG_EXACT))
            context->castFlagsResult.add(CAST_RESULT_COERCE);
    }

    // If it matches, then it matches with a conversion
    if (fromType->isUntypedInteger() || fromType->isUntypedFloat())
        context->castFlagsResult.add(CAST_RESULT_UNTYPED_CONVERT);

    switch (toType->nativeType)
    {
        case NativeTypeKind::Bool:
            SWAG_CHECK(castToNativeBool(context, fromType, fromNode, castFlags));
            break;
        case NativeTypeKind::U8:
            SWAG_CHECK(castToNativeU8(context, fromType, fromNode, castFlags));
            break;
        case NativeTypeKind::U16:
            SWAG_CHECK(castToNativeU16(context, fromType, fromNode, castFlags));
            break;
        case NativeTypeKind::U32:
            SWAG_CHECK(castToNativeU32(context, fromType, fromNode, castFlags));
            break;
        case NativeTypeKind::Rune:
            SWAG_CHECK(castToNativeRune(context, fromType, fromNode, castFlags));
            break;
        case NativeTypeKind::U64:
            SWAG_CHECK(castToNativeU64(context, fromType, fromNode, castFlags));
            break;
        case NativeTypeKind::S8:
            SWAG_CHECK(castToNativeS8(context, fromType, fromNode, castFlags));
            break;
        case NativeTypeKind::S16:
            SWAG_CHECK(castToNativeS16(context, fromType, fromNode, castFlags));
            break;
        case NativeTypeKind::S32:
            SWAG_CHECK(castToNativeS32(context, fromType, fromNode, castFlags));
            break;
        case NativeTypeKind::S64:
            SWAG_CHECK(castToNativeS64(context, fromType, fromNode, castFlags));
            break;
        case NativeTypeKind::F32:
            SWAG_CHECK(castToNativeF32(context, fromType, fromNode, castFlags));
            break;
        case NativeTypeKind::F64:
            SWAG_CHECK(castToNativeF64(context, fromType, fromNode, castFlags));
            break;
        case NativeTypeKind::String:
            SWAG_CHECK(castToString(context, toType, fromType, fromNode, castFlags));
            break;
        default:
            return castError(context, toType, fromType, fromNode, castFlags);
    }

    SWAG_CHECK(safetyComputedValue(context, toType, fromType, fromNode, castFlags));

    // Automatic cast has been done
    if (castFlags.has(CAST_FLAG_COERCE))
    {
        if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK) && !fromNode->hasFlagComputedValue())
        {
            fromNode->typeInfo = toType;
            if (!fromNode->typeInfoCast)
                fromNode->typeInfoCast = fromType;
        }
    }

    return true;
}

bool TypeManager::castToEnum(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* /*toNode*/, AstNode* fromNode, CastFlags castFlags)
{
    auto toEnum = castTypeInfo<TypeInfoEnum>(toType, TypeInfoKind::Enum);

    // Cast from enum to enum, take care of 'using'
    if (fromType->isEnum())
    {
        const auto fromEnum = castTypeInfo<TypeInfoEnum>(fromType, TypeInfoKind::Enum);

        context->castCollectEnum.clear();
        context->castCollectEnumDone.clear();
        context->castCollectEnum.push_back(toEnum);
        context->castCollectEnumDone.insert(toEnum);

        while (!context->castCollectEnum.empty())
        {
            toEnum = context->castCollectEnum.back();
            context->castCollectEnum.pop_back();
            if (fromEnum->isSame(toEnum, castFlags.with(CAST_FLAG_CAST)))
            {
                if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
                {
                    fromNode->typeInfoCast = fromType;
                    fromNode->typeInfo     = toType;
                }

                return true;
            }

            if (!toEnum->hasFlag(TYPEINFO_ENUM_HAS_USING))
                continue;

            for (const auto value : toEnum->values)
            {
                if (value->typeInfo->isEnum())
                {
                    auto toSubEnum = castTypeInfo<TypeInfoEnum>(value->typeInfo, TypeInfoKind::Enum);
                    if (!context->castCollectEnumDone.contains(toSubEnum))
                    {
                        context->castCollectEnum.push_back(toSubEnum);
                        context->castCollectEnumDone.insert(toSubEnum);
                    }
                }
            }
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castSubExpressionList(SemanticContext* context, AstNode* child, TypeInfo* toType, CastFlags castFlags)
{
    const auto exprNode     = castAst<AstExpressionList>(child, AstNodeKind::ExpressionList);
    const auto toTypeStruct = castTypeInfo<TypeInfoStruct>(toType, TypeInfoKind::Struct);
    exprNode->castToStruct  = toTypeStruct;

    bool hasChanged = false;

    // Not enough fields
    if (toTypeStruct->fields.size() > child->childCount())
    {
        exprNode->castToStruct = toTypeStruct;
    }

    // Too many fields
    else if (toTypeStruct->fields.size() < child->childCount())
    {
        const auto       msg = formErr(Err0509, toTypeStruct->fields.size(), toTypeStruct->getDisplayNameC(), child->childCount());
        const Diagnostic err{child->children[toTypeStruct->fields.count], msg};
        return context->report(err);
    }

    SymbolMatchContext symContext;
    symContext.semContext = context;
    for (auto c : child->children)
        symContext.parameters.push_back(c);
    Match::match(toTypeStruct, symContext);

    switch (symContext.result)
    {
        case MatchResult::MissingNamedArgument:
        case MatchResult::DuplicatedNamedArgument:
        case MatchResult::InvalidNamedArgument:
        {
            Vector<const Diagnostic*> result0;
            Vector<const Diagnostic*> result1;
            OneTryMatch               oneTry;
            oneTry.symMatchContext = symContext;
            oneTry.callParameters  = child;
            oneTry.overload        = toTypeStruct->declNode->resolvedSymbolOverload();
            oneTry.type            = toTypeStruct;
            SemanticError::getDiagnosticForMatch(context, oneTry, result0, result1);
            SWAG_ASSERT(!result0.empty());
            return context->report(*result0[0], result1);
        }
    }

    for (uint32_t j = 0; j < child->childCount(); j++)
    {
        const auto           childJ = child->children[j];
        const TypeInfoParam* fieldJ = symContext.solvedCallParameters[j];

        const auto oldType = childJ->typeInfo;
        SWAG_CHECK(TypeManager::makeCompatibles(context, fieldJ->typeInfo, childJ->typeInfo, nullptr, childJ, castFlags.with(CAST_FLAG_TRY_COERCE)));
        if (childJ->typeInfo != oldType)
            hasChanged = true;

        // Collect array to slice : will need special treatment when collecting constants
        if (childJ->typeInfo->isListArray() && fieldJ->typeInfo->isSlice())
            childJ->addExtraPointer(ExtraPointerKind::CollectTypeInfo, fieldJ->typeInfo);

        // We use castOffset to store the offset to the field, in order to collect later at the right position
        // Note that offset is +1 to differentiate it from a "default" 0.
        childJ->allocateExtension(ExtensionKind::Misc);
        const auto newOffset = static_cast<uint32_t>(fieldJ->offset) + 1;
        if (childJ->extMisc()->castOffset != newOffset)
        {
            childJ->extMisc()->castOffset = newOffset;
            hasChanged                    = true;
        }
    }

    if (child->typeInfo->sizeOf != toTypeStruct->sizeOf)
        hasChanged = true;

    if (hasChanged)
    {
        Semantic::computeExpressionListTupleType(context, child);
        SWAG_ASSERT(context->result == ContextResult::Done);
        child->typeInfo->sizeOf = toTypeStruct->sizeOf;
    }

    return true;
}

bool TypeManager::castExpressionList(SemanticContext* context, TypeInfoList* fromTypeList, TypeInfo* toType, AstNode* fromNode, CastFlags castFlags)
{
    const auto fromSize = fromTypeList->subTypes.size();
    while (fromNode && fromNode->isNot(AstNodeKind::ExpressionList))
        fromNode = fromNode->firstChild();
    SWAG_ASSERT(!fromNode || fromSize == fromNode->childCount());

    // Need to recompute total size, as the size of each element can have been changed by the cast
    uint32_t newSizeof = 0;

    // Compute if expression is constexpr
    if (fromNode)
        fromNode->addAstFlag(AST_CONST_EXPR);

    for (uint32_t i = 0; i < fromSize; i++)
    {
        const auto child = fromNode ? fromNode->children[i] : nullptr;

        // Expression list inside another expression list (like a struct inside an array)
        if (child && child->is(AstNodeKind::ExpressionList) && toType->isStruct())
        {
            SWAG_CHECK(castSubExpressionList(context, child, toType, castFlags));
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, toType, fromTypeList->subTypes[i]->typeInfo, nullptr, child, castFlags.with(CAST_FLAG_TRY_COERCE)));

        if (child)
        {
            const auto childType = concreteType(child->typeInfo, CONCRETE_FUNC);
            newSizeof += childType->sizeOf;
            if (!child->hasAstFlag(AST_CONST_EXPR))
                fromNode->removeAstFlag(AST_CONST_EXPR);
        }
    }

    if (fromNode && fromTypeList->sizeOf != newSizeof)
    {
        const auto oldSizeof = fromTypeList->sizeOf;
        fromTypeList         = castTypeInfo<TypeInfoList>(fromTypeList->clone());
        for (uint32_t i = 0; i < fromTypeList->subTypes.size(); i++)
            fromTypeList->subTypes[i]->typeInfo = fromNode->children[i]->typeInfo;
        fromTypeList->sizeOf = newSizeof;
        fromNode->typeInfo   = fromTypeList;

        // :ExprListArrayStorage
        if (!fromNode->hasAstFlag(AST_CONST_EXPR) && fromNode->ownerScope && fromNode->ownerFct)
        {
            SWAG_ASSERT(fromNode->computedValue());
            fromNode->ownerScope->startStackSize -= oldSizeof;
            fromNode->ownerScope->startStackSize += newSizeof;
            Semantic::setOwnerMaxStackSize(fromNode, fromNode->ownerScope->startStackSize);
        }
    }

    return true;
}

bool TypeManager::castToString(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (fromType->isPointerNull())
    {
        if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
        {
            if (fromNode->hasFlagComputedValue())
                fromNode->computedValue()->text.release();
            fromNode->typeInfo     = toType;
            fromNode->typeInfoCast = g_TypeMgr->typeInfoNull;
        }

        return true;
    }

    if (castFlags.has(CAST_FLAG_EXPLICIT))
    {
        // [..] u8 to string, this is possible !
        if (fromType->isSlice())
        {
            const auto fromTypeSlice = castTypeInfo<TypeInfoSlice>(fromType, TypeInfoKind::Slice);
            const auto concreteType  = TypeManager::concreteType(fromTypeSlice->pointedType, CONCRETE_ALIAS);
            if (concreteType->isNative(NativeTypeKind::U8))
            {
                if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
                {
                    fromNode->typeInfoCast = fromNode->typeInfo;
                    fromNode->typeInfo     = g_TypeMgr->typeInfoString;
                }

                return true;
            }
        }

        // [] u8 to string, this is possible !
        if (fromType->isArray())
        {
            const auto fromTypeArray = castTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
            const auto concreteType  = TypeManager::concreteType(fromTypeArray->pointedType, CONCRETE_ALIAS);
            if (concreteType->isNative(NativeTypeKind::U8))
            {
                if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
                {
                    fromNode->typeInfoCast = fromNode->typeInfo;
                    fromNode->typeInfo     = g_TypeMgr->typeInfoString;
                }

                return true;
            }
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToAny(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, CastFlags castFlags)
{
    if (fromType->isPointerNull())
    {
        if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
        {
            fromNode->typeInfo     = toType;
            fromNode->typeInfoCast = fromType;
        }

        return true;
    }

    if (castFlags.has(CAST_FLAG_COMMUTATIVE))
    {
        if (toNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
        {
            // When casting something complex to any, we will copy the value to the stack to be sure
            // that the memory layout is correct, without relying on registers being contiguous, and not being reallocated (by an optimize pass).
            // See ByteCodeGen::emitCastToNativeAny
            if (toNode->ownerFct && toType->numRegisters() > 1)
            {
                toNode->allocateExtension(ExtensionKind::Misc);
                toNode->extMisc()->stackOffset = toNode->ownerScope->startStackSize;
                toNode->ownerScope->startStackSize += toType->numRegisters() * sizeof(Register);
                Semantic::setOwnerMaxStackSize(toNode, toNode->ownerScope->startStackSize);
            }

            toNode->typeInfoCast = toType;
            toNode->typeInfo     = fromType;
            SWAG_CHECK(toNode->addAnyType(context, fromType));
            YIELD();
        }

        return true;
    }

    if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
    {
        // When casting something complex to any, we will copy the value to the stack to be sure
        // that the memory layout is correct, without relying on registers being contiguous, and not being reallocated (by an optimize pass).
        // See ByteCodeGen::emitCastToNativeAny
        if (fromNode->ownerFct && fromType->numRegisters() > 1)
        {
            fromNode->allocateExtension(ExtensionKind::Misc);
            fromNode->extMisc()->stackOffset = fromNode->ownerScope->startStackSize;
            fromNode->ownerScope->startStackSize += fromType->numRegisters() * sizeof(Register);
            Semantic::setOwnerMaxStackSize(fromNode, fromNode->ownerScope->startStackSize);
        }

        fromNode->typeInfoCast = fromType;
        fromNode->typeInfo     = toType;
        SWAG_CHECK(fromNode->addAnyType(context, fromNode->typeInfoCast));
        YIELD();
    }

    return true;
}

bool TypeManager::castFromAny(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, CastFlags castFlags)
{
    const auto toRealType = concretePtrRef(toType);

    if (!castFlags.has(CAST_FLAG_EXPLICIT))
    {
        // Ambiguous. Do we check for a bool, or do we check for null
        if (toRealType->isBool())
            return castError(context, toRealType, fromType, fromNode, castFlags);

        // To convert a simple any to something more complex, need an explicit cast
        if (toRealType->isSlice() ||
            toRealType->isArray() ||
            toRealType->isPointer())
            return castError(context, toRealType, fromType, fromNode, castFlags);
    }

    // From a constant
    if (fromNode && fromNode->hasFlagComputedValue())
    {
        if (toType->isPointerRef())
        {
            if (!castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                fromNode->removeAstFlag(AST_COMPUTED_VALUE | AST_CONST_EXPR | AST_NO_BYTECODE_CHILDREN);
                fromNode->extSemantic()->computedValue = nullptr;
            }
        }
        else
        {
            const SwagAny* any         = static_cast<SwagAny*>(fromNode->computedValue()->getStorageAddr());
            const auto     newTypeInfo = context->sourceFile->module->typeGen.getRealType(fromNode->computedValue()->storageSegment, any->type);

            // need to check the type
            if (newTypeInfo && context->sourceFile->module->mustEmitSafety(fromNode, SAFETY_DYN_CAST, true))
            {
                if (!toRealType->isSame(newTypeInfo, castFlags.with(CAST_FLAG_EXACT)))
                {
                    if (!castFlags.has(CAST_FLAG_JUST_CHECK))
                        return castError(context, toRealType, newTypeInfo, fromNode, castFlags, CastErrorType::SafetyCastAny);
                    return false;
                }
            }

            if (!castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                fromNode->typeInfo     = newTypeInfo;
                fromNode->typeInfoCast = nullptr;
                SWAG_CHECK(Semantic::derefConstantValue(context, fromNode, fromNode->typeInfo, fromNode->computedValue()->storageSegment, static_cast<uint8_t*>(any->value)));
            }

            return true;
        }
    }

    if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
    {
        // When casting something complex to any, we will copy the value to the stack to be sure
        // that the memory layout is correct, without relying on registers being contiguous, and not being reallocated (by an optimize pass).
        // See ByteCodeGen::emitCastToNativeAny
        if (fromNode->ownerFct && fromType->numRegisters() > 1)
        {
            fromNode->allocateExtension(ExtensionKind::Misc);
            fromNode->extMisc()->stackOffset = fromNode->ownerScope->startStackSize;
            fromNode->ownerScope->startStackSize += fromType->numRegisters() * sizeof(Register);
            Semantic::setOwnerMaxStackSize(fromNode, fromNode->ownerScope->startStackSize);
        }

        fromNode->typeInfoCast = fromType;
        fromNode->typeInfo     = toType;
        SWAG_CHECK(fromNode->addAnyType(context, toType));
        YIELD();
    }

    return true;
}

bool TypeManager::castStructToStruct(SemanticContext* context,
                                     TypeInfoStruct*  toStruct,
                                     TypeInfoStruct*  fromStruct,
                                     TypeInfo*        toType,
                                     TypeInfo*        fromType,
                                     AstNode*         fromNode,
                                     CastFlags        castFlags,
                                     bool&            ok)
{
    context->castStructStructFields.clear();
    auto& stack = context->castStructStructFields;

    stack.push_back({fromStruct, 0, nullptr});
    while (!stack.empty())
    {
        const auto it = stack.back();
        stack.pop_back();

        const auto testStruct = it.typeStruct->getStructOrPointedStruct();
        SWAG_ASSERT(testStruct);
        if (testStruct->isSame(toStruct, castFlags.with(CAST_FLAG_CAST)))
        {
            ok = true;
            if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                if (toType->isSelf())
                {
                    if (it.offset)
                    {
                        fromNode->allocateExtension(ExtensionKind::Misc);
                        fromNode->extMisc()->castOffset = it.offset;
                        fromNode->typeInfoCast          = fromNode->typeInfo;
                        fromNode->typeInfo              = toType;
                    }

                    continue;
                }

                // We will have to dereference the pointer to get the real thing
                if (it.field && it.field->typeInfo->isPointer())
                    fromNode->addSemFlag(SEMFLAG_DEREF_USING);
                fromNode->addSemFlag(SEMFLAG_USING);

                fromNode->allocateExtension(ExtensionKind::Misc);
                fromNode->extMisc()->castOffset = it.offset;
                fromNode->typeInfoCast          = fromNode->typeInfo;
                fromNode->typeInfo              = toType;
                continue;
            }
        }

        // No using ! We're done
        if (castFlags.has(CAST_FLAG_FOR_GENERIC) && !it.typeStruct->hasFlag(TYPEINFO_STRUCT_TYPEINFO))
            return true;

        const auto structNode = castAst<AstStruct>(it.typeStruct->declNode, AstNodeKind::StructDecl);
        if (!structNode->hasSpecFlag(AstStruct::SPEC_FLAG_HAS_USING))
            continue;

        Semantic::waitStructOverloadDefined(context->baseJob, it.typeStruct);
        YIELD();
        Semantic::waitOverloadCompleted(context->baseJob, structNode->resolvedSymbolOverload());
        YIELD();

        const TypeInfoParam*  foundField  = nullptr;
        const TypeInfoStruct* foundStruct = nullptr;
        for (const auto field : it.typeStruct->fields)
        {
            if (!field->flags.has(TYPEINFOPARAM_HAS_USING))
                continue;

            TypeInfoStruct* typeStruct = nullptr;
            if (field->typeInfo->isStruct())
            {
                typeStruct = castTypeInfo<TypeInfoStruct>(field->typeInfo, TypeInfoKind::Struct);
                if (typeStruct == it.typeStruct)
                    continue;
            }
            else if (field->typeInfo->isPointerTo(TypeInfoKind::Struct))
            {
                const auto typePointer = castTypeInfo<TypeInfoPointer>(field->typeInfo, TypeInfoKind::Pointer);
                typeStruct             = castTypeInfo<TypeInfoStruct>(typePointer->pointedType, TypeInfoKind::Struct);
            }

            if (typeStruct)
            {
                // Ambiguous ! Two fields with a 'using' on the same struct
                if (foundStruct)
                {
                    const auto foundTypeStruct0 = foundStruct->getStructOrPointedStruct();
                    const auto foundTypeStruct1 = typeStruct->getStructOrPointedStruct();
                    if (foundTypeStruct0 && foundTypeStruct1 && foundTypeStruct0->isSame(foundTypeStruct1, castFlags.with(CAST_FLAG_CAST)))
                    {
                        if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
                        {
                            Diagnostic err{fromNode, formErr(Err0019, fromType->getDisplayNameC(), toType->getDisplayNameC())};
                            err.addNote(formNte(Nte0153, toStruct->getDisplayNameC(), fromStruct->getDisplayNameC()));
                            err.addNote(foundField->declNode, toNte(Nte0179));
                            err.addNote(field->declNode, toNte(Nte0178));
                            return context->report(err);
                        }
                    }
                }

                foundField  = field;
                foundStruct = typeStruct;
                stack.push_back({typeStruct, it.offset + field->offset, field});
            }
        }
    }

    return true;
}

bool TypeManager::collectInterface(SemanticContext* context, TypeInfoStruct* fromTypeStruct, const TypeInfoStruct* toTypeItf, InterfaceRef& ref, bool skipFirst)
{
    const TypeInfoParam* foundField = nullptr;

    context->castCollectInterfaceField.clear();
    auto& stack = context->castCollectInterfaceField;

    stack.push_back({fromTypeStruct, 0, nullptr, nullptr});
    while (!stack.empty())
    {
        const auto it = stack.back();
        stack.pop_back();

        if (!skipFirst)
        {
            const auto foundItf = it.typeStruct->hasInterface(toTypeItf);
            if (foundItf)
            {
                if (foundField)
                {
                    Diagnostic err{context->node, formErr(Err0021, fromTypeStruct->getDisplayNameC(), toTypeItf->name.cstr())};
                    err.addNote(it.field->declNode, formNte(Nte0163, it.field->typeInfo->getDisplayNameC()));
                    err.addNote(foundField->declNode, formNte(Nte0165, foundField->typeInfo->getDisplayNameC()));
                    return context->report(err);
                }

                ref.itf         = foundItf;
                ref.fieldOffset = it.offset;
                ref.fieldRef    = it.fieldAccessName;
                foundField      = it.field;
                continue;
            }
        }

        skipFirst             = false;
        const auto structNode = castAst<AstStruct>(it.typeStruct->declNode, AstNodeKind::StructDecl);
        if (!structNode->hasSpecFlag(AstStruct::SPEC_FLAG_HAS_USING))
            continue;

        Semantic::waitOverloadCompleted(context->baseJob, it.typeStruct->declNode->resolvedSymbolOverload());
        YIELD();

        for (const auto field : it.typeStruct->fields)
        {
            if (!field->flags.has(TYPEINFOPARAM_HAS_USING))
                continue;
            if (!field->typeInfo->isStruct())
                continue;

            const auto typeStruct = castTypeInfo<TypeInfoStruct>(field->typeInfo, TypeInfoKind::Struct);
            if (typeStruct == it.typeStruct)
                continue;

            Semantic::waitAllStructInterfaces(context->baseJob, typeStruct);
            YIELD();

            auto accessName = it.fieldAccessName;
            if (!accessName.empty())
                accessName += ".";
            accessName += field->name;

            stack.push_back({typeStruct, it.offset + field->offset, field, accessName});
        }
    }

    return true;
}

bool TypeManager::castToInterface(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (castFlags.has(CAST_FLAG_FOR_GENERIC))
        return castError(context, toType, fromType, fromNode, castFlags);

    const auto toTypeItf = castTypeInfo<TypeInfoStruct>(toType, TypeInfoKind::Interface);

    if (fromType->isPointerNull())
    {
        if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
        {
            // We will copy the value to the stack to be sure that the memory layout is correct, without relying on
            // registers being contiguous, and not being reallocated (by an optimize pass).
            // This is the same problem when casting to 'any'.
            // See ByteCodeGen::emitCastToNativeAny
            if (fromNode->ownerFct)
            {
                fromNode->allocateExtension(ExtensionKind::Misc);
                fromNode->extMisc()->stackOffset = fromNode->ownerScope->startStackSize;
                fromNode->ownerScope->startStackSize += 2 * sizeof(Register);
                Semantic::setOwnerMaxStackSize(fromNode, fromNode->ownerScope->startStackSize);
            }

            fromNode->typeInfoCast = fromNode->typeInfo;
            fromNode->typeInfo     = toType;
        }

        return true;
    }

    // Struct to interface
    // We need to take care of "using" fields.
    if (fromType->isStruct() || fromType->isPointerTo(TypeInfoKind::Struct))
    {
        Semantic::waitAllStructInterfaces(context->baseJob, fromType);
        if (context->result != ContextResult::Done)
        {
            SWAG_ASSERT(castFlags.has(CAST_FLAG_ACCEPT_PENDING));
            return true;
        }

        auto typeStruct = fromType;
        if (fromType->isPointer())
        {
            const auto typePointer = castTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer);
            typeStruct             = typePointer->pointedType;
        }

        const auto fromTypeStruct = castTypeInfo<TypeInfoStruct>(typeStruct, TypeInfoKind::Struct);

        InterfaceRef itfRef;
        SWAG_CHECK(collectInterface(context, fromTypeStruct, toTypeItf, itfRef));
        if (context->result != ContextResult::Done)
        {
            SWAG_ASSERT(castFlags.has(CAST_FLAG_ACCEPT_PENDING));
            return true;
        }

        if (itfRef.itf)
        {
            if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                fromNode->allocateExtension(ExtensionKind::Misc);
                fromNode->extMisc()->castOffset = itfRef.fieldOffset;
                fromNode->addExtraPointer(ExtraPointerKind::CastItf, itfRef.itf);
                fromNode->typeInfoCast = fromType;
                fromNode->typeInfo     = toTypeItf;
            }

            return true;
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToPointerRef(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    const auto toTypePointer = castTypeInfo<TypeInfoPointer>(toType, TypeInfoKind::Pointer);

    if (fromType->isPointer())
    {
        if (fromType->isPointerNull())
            return castError(context, toType, fromType, fromNode, castFlags);

        // Convert from pointer to ref : only if authorized
        if (!fromType->isPointerRef() && !castFlags.has(CAST_FLAG_EXPLICIT) && !castFlags.has(CAST_FLAG_PTR_REF))
            return castError(context, toType, fromType, fromNode, castFlags);

        // When affecting a ref, const must be the same
        if (fromNode &&
            fromType->isPointerRef() &&
            castFlags.has(CAST_FLAG_FOR_AFFECT) &&
            fromNode->is(AstNodeKind::KeepRef) &&
            toType->isConst() != fromType->isConst())
        {
            return castError(context, toType, fromType, fromNode, castFlags, CastErrorType::Const);
        }

        // Compare pointed types
        const auto fromTypePointer = castTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer);
        if (toTypePointer->pointedType->isSame(fromTypePointer->pointedType, castFlags.with(CAST_FLAG_CAST)))
            return true;
    }

    // Value (with address) to const ref is possible for function parameters
    else if (toType->isConst() && castFlags.has(CAST_FLAG_PARAMS))
    {
        if (toTypePointer->pointedType->isSame(fromType, castFlags.with(CAST_FLAG_CAST)))
            return true;

        if (toTypePointer->pointedType->isSlice() && fromType->isListArray())
            return makeCompatibles(context, toTypePointer->pointedType, fromType, nullptr, fromNode, castFlags);
    }

    // Structure to interface reference
    if (fromType->isStruct() && toTypePointer->pointedType->isInterface())
    {
        const auto toTypeItf      = castTypeInfo<TypeInfoStruct>(toTypePointer->pointedType, TypeInfoKind::Interface);
        const auto fromTypeStruct = castTypeInfo<TypeInfoStruct>(fromType, TypeInfoKind::Struct);
        if (!fromTypeStruct->hasInterface(toTypeItf))
            return castError(context, toType, fromType, fromNode, castFlags);

        if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
        {
            fromNode->typeInfoCast = fromType;
            fromNode->typeInfo     = toTypeItf;
        }

        return true;
    }

    // Struct to automatic const ref struct
    if (fromType->isStruct() && toTypePointer->pointedType->isStruct() && toTypePointer->isAutoConstPointerRef())
    {
        const auto fromStruct = castTypeInfo<TypeInfoStruct>(fromType, TypeInfoKind::Struct);
        const auto toStruct   = castTypeInfo<TypeInfoStruct>(toTypePointer->pointedType, TypeInfoKind::Struct);
        bool       ok         = false;
        SWAG_CHECK(castStructToStruct(context, toStruct, fromStruct, toType, fromType, fromNode, castFlags, ok));
        if (ok || context->result == ContextResult::Pending)
        {
            if (!fromStruct->isSame(toStruct, castFlags.with(CAST_FLAG_CAST)))
                context->castFlagsResult.add(CAST_RESULT_STRUCT_CONVERT);
            return true;
        }
    }

    // Struct to struct pointer
    if (fromType->isPointerTo(TypeInfoKind::Struct) && toTypePointer->pointedType->isStruct())
    {
        const auto fromPtr    = castTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer);
        const auto fromStruct = castTypeInfo<TypeInfoStruct>(fromPtr->pointedType, TypeInfoKind::Struct);
        const auto toStruct   = castTypeInfo<TypeInfoStruct>(toTypePointer->pointedType, TypeInfoKind::Struct);
        bool       ok         = false;
        SWAG_CHECK(castStructToStruct(context, toStruct, fromStruct, toType, fromType, fromNode, castFlags, ok));
        if (ok || context->result == ContextResult::Pending)
        {
            if (!fromStruct->isSame(toStruct, castFlags.with(CAST_FLAG_CAST)))
                context->castFlagsResult.add(CAST_RESULT_STRUCT_CONVERT);
            return true;
        }
    }

    // Struct to moveref can go to there
    if (fromType->isStruct() && toTypePointer->pointedType->isStruct() && toTypePointer->isPointerMoveRef())
    {
        if (fromType->isSame(toTypePointer->pointedType, castFlags.with(CAST_FLAG_CAST)))
        {
            context->castFlagsResult.add(CAST_RESULT_GUESS_MOVE);
            return true;
        }
    }

    // UFCS, accept type to ref type
    if (castFlags.has(CAST_FLAG_UFCS))
    {
        if (toTypePointer->pointedType->isSame(fromType, castFlags.with(CAST_FLAG_CAST)))
        {
            context->castFlagsResult.add(CAST_RESULT_FORCE_REF);
            return true;
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToPointer(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    if (castFlags.has(CAST_FLAG_UFCS))
        fromType = concretePtrRef(fromType);

    // To "cstring"
    if (toType->isCString())
    {
        if (fromType->isString())
        {
            if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                fromNode->typeInfoCast = fromType;
                fromNode->typeInfo     = toType;
            }

            return true;
        }
    }

    const auto toTypePointer = castTypeInfo<TypeInfoPointer>(toType, TypeInfoKind::Pointer);

    // Pointer to struct to pointer to struct. Take care of using
    if (fromType->isPointer() && toTypePointer->pointedType->isStruct())
    {
        const auto fromTypePointer = castTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer);
        if (fromTypePointer->pointedType->isStruct())
        {
            const auto fromStruct = castTypeInfo<TypeInfoStruct>(fromTypePointer->pointedType, TypeInfoKind::Struct);
            const auto toStruct   = castTypeInfo<TypeInfoStruct>(toTypePointer->pointedType, TypeInfoKind::Struct);
            bool       ok         = false;
            SWAG_CHECK(castStructToStruct(context, toStruct, fromStruct, toType, fromType, fromNode, castFlags, ok));
            if (ok || context->result == ContextResult::Pending)
            {
                if (!fromStruct->isSame(toStruct, castFlags.with(CAST_FLAG_CAST)))
                    context->castFlagsResult.add(CAST_RESULT_STRUCT_CONVERT);
                return true;
            }
        }
    }

    // Struct to pointer to struct
    // Accept only if this is UFCS, to simulate calling a method of a base 'class'
    if (castFlags.has(CAST_FLAG_UFCS) && fromType->isStruct() && toTypePointer->pointedType->isStruct())
    {
        const auto fromStruct = castTypeInfo<TypeInfoStruct>(fromType, TypeInfoKind::Struct);
        const auto toStruct   = castTypeInfo<TypeInfoStruct>(toTypePointer->pointedType, TypeInfoKind::Struct);
        bool       ok         = false;
        SWAG_CHECK(castStructToStruct(context, toStruct, fromStruct, toType, fromType, fromNode, castFlags, ok));
        if (ok || context->result == ContextResult::Pending)
        {
            if (!fromStruct->isSame(toStruct, castFlags.with(CAST_FLAG_CAST)))
                context->castFlagsResult.add(CAST_RESULT_STRUCT_CONVERT);
            return true;
        }
    }

    // Lambda to *void
    if (fromType->isLambdaClosure())
    {
        if (castFlags.has(CAST_FLAG_EXPLICIT) && toType->isPointerVoid())
        {
            if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
                fromNode->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid, TYPEINFO_CONST);
            return true;
        }
    }

    // Pointer to pointer
    if (fromType->isPointer())
    {
        if (castFlags.has(CAST_FLAG_EXPLICIT))
            return true;

        // Fine to compare pointers of TypeInfos, even if not of the same type
        if (castFlags.has(CAST_FLAG_FOR_COMPARE) && fromType->isPointerToTypeInfo() && toType->isPointerToTypeInfo())
            return true;

        // Pointer to *void or *void to pointer
        if (toType->isPointerVoid() || fromType->isPointerVoid())
        {
            if (castFlags.has(CAST_FLAG_PARAMS))
            {
                if (toType->hasFlag(TYPEINFO_GENERIC | TYPEINFO_FROM_GENERIC))
                    return castError(context, toType, fromType, fromNode, castFlags);
            }

            if (castFlags.has(CAST_FLAG_FOR_GENERIC))
            {
                return castError(context, toType, fromType, fromNode, castFlags);
            }

            return true;
        }

        // :PointerArithmetic
        // Cannot cast from non arithmetic to arithmetic
        if (toType->isPointerArithmetic() && !fromType->isPointerArithmetic())
        {
            // Fine to compare pointers for examples, but not to affect them.
            if (castFlags.has(CAST_FLAG_FOR_AFFECT))
                return castError(context, toType, fromType, fromNode, castFlags);
            return true;
        }
    }

    // Array to pointer of the same type
    if (fromType->isArray())
    {
        const auto fromTypeArray = castTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
        if ((!castFlags.has(CAST_FLAG_NO_IMPLICIT) && toTypePointer->pointedType->isVoid()) ||
            (!castFlags.has(CAST_FLAG_NO_IMPLICIT) && toTypePointer->pointedType->isSame(fromTypeArray->pointedType, castFlags.with(CAST_FLAG_CAST))) ||
            castFlags.has(CAST_FLAG_EXPLICIT))
        {
            if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                fromNode->typeInfoCast = fromNode->typeInfo;
                fromNode->typeInfo     = toType;
            }

            return true;
        }
    }

    // Slice to pointer of the same type
    if (fromType->isSlice())
    {
        const auto fromTypeSlice = castTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Slice);
        if ((!castFlags.has(CAST_FLAG_NO_IMPLICIT) && toTypePointer->pointedType->isVoid()) ||
            (!castFlags.has(CAST_FLAG_NO_IMPLICIT) && toTypePointer->pointedType->isSame(fromTypeSlice->pointedType, castFlags.with(CAST_FLAG_CAST))) ||
            castFlags.has(CAST_FLAG_EXPLICIT))
        {
            if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                fromNode->typeInfoCast = fromNode->typeInfo;
                fromNode->typeInfo     = toType;
            }

            return true;
        }
    }

    // Struct/Interface to pointer
    if (fromType->isStruct() || fromType->isInterface())
    {
        if (castFlags.has(CAST_FLAG_EXPLICIT) || toTypePointer->isSelf() || toTypePointer->isConst() || castFlags.has(CAST_FLAG_UFCS))
        {
            // to *void or *structure
            if (toTypePointer->pointedType->isVoid() ||
                toTypePointer->pointedType->isSame(fromType, castFlags.with(CAST_FLAG_CAST)))
            {
                if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK) && !toTypePointer->isSelf())
                {
                    fromNode->typeInfoCast = fromNode->typeInfo;
                    fromNode->typeInfo     = toType;
                }

                return true;
            }
        }
    }

    // Interface back to struct pointer
    if (fromType->isInterface() && toTypePointer->isPointerTo(TypeInfoKind::Struct))
    {
        if (castFlags.has(CAST_FLAG_EXPLICIT))
        {
            const auto fromTypeItf  = castTypeInfo<TypeInfoStruct>(fromType, TypeInfoKind::Interface);
            const auto toTypeStruct = castTypeInfo<TypeInfoStruct>(toTypePointer->pointedType, TypeInfoKind::Struct);
            if (!toTypeStruct->hasInterface(fromTypeItf))
                return castError(context, toType, fromType, fromNode, castFlags);

            if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                fromNode->typeInfoCast = fromType;
                fromNode->typeInfo     = toType;
            }

            return true;
        }
    }

    // Struct pointer to interface pointer
    if (fromType->isPointerTo(TypeInfoKind::Struct) && toType->isPointerTo(TypeInfoKind::Interface))
        return castError(context, toType, fromType, fromNode, castFlags);

    // String to const *u8
    if (fromType->isString())
    {
        if (toType->isPointerNull())
            return true;

        if (castFlags.has(CAST_FLAG_EXPLICIT))
        {
            if (toTypePointer->pointedType->isNative(NativeTypeKind::U8) && toTypePointer->isConst())
            {
                if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
                {
                    fromNode->typeInfoCast = fromNode->typeInfo;
                    fromNode->typeInfo     = toType;
                }

                return true;
            }
        }
    }

    // u64 to whatever pointer
    if (fromType->isNative(NativeTypeKind::U64))
    {
        if (castFlags.has(CAST_FLAG_EXPLICIT))
        {
            if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                fromNode->typeInfoCast = fromNode->typeInfo;
                fromNode->typeInfo     = toType;
            }

            return true;
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToArray(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    const TypeInfoArray* toTypeArray = castTypeInfo<TypeInfoArray>(toType, TypeInfoKind::Array);
    if (fromType->isListArray())
    {
        TypeInfoList* fromTypeList = castTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeListArray);
        const auto    fromSize     = fromTypeList->subTypes.size();
        if (toTypeArray->count != fromSize)
        {
            if (!castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                if (toTypeArray->count > fromTypeList->subTypes.size())
                    context->report({fromNode, formErr(Err0479, toTypeArray->count, fromTypeList->subTypes.size())});
                else
                    context->report({fromNode, formErr(Err0510, toTypeArray->count, fromTypeList->subTypes.size())});
            }

            return false;
        }

        SWAG_CHECK(castExpressionList(context, fromTypeList, toTypeArray->pointedType, fromNode, castFlags));

        if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
        {
            fromType->addFlag(TYPEINFO_LIST_ARRAY_ARRAY);
        }

        return true;
    }

    if (castFlags.has(CAST_FLAG_FOR_VAR_INIT))
    {
        if (toTypeArray->finalType->isSame(fromType, castFlags.with(CAST_FLAG_CAST)))
        {
            const auto finalType = concreteType(toTypeArray->finalType, CONCRETE_ENUM | CONCRETE_ALIAS);
            if (finalType->isString() ||
                finalType->isNativeFloat() ||
                finalType->isNativeIntegerOrRune() ||
                finalType->isBool())
            {
                return true;
            }
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToLambda(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    TypeInfoFuncAttr* toTypeLambda = castTypeInfo<TypeInfoFuncAttr>(toType, TypeInfoKind::LambdaClosure);
    if (castFlags.has(CAST_FLAG_EXPLICIT))
    {
        if (fromType->isPointerNull() || fromType->isPointerConstVoid())
        {
            if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                fromNode->typeInfoCast = fromNode->typeInfo;
                fromNode->typeInfo     = toTypeLambda;
            }

            return true;
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToClosure(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    TypeInfoFuncAttr* toTypeLambda = castTypeInfo<TypeInfoFuncAttr>(toType, TypeInfoKind::LambdaClosure);
    if (castFlags.has(CAST_FLAG_EXPLICIT))
    {
        if (fromType->isPointerNull())
        {
            if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                fromNode->typeInfoCast = fromNode->typeInfo;
                fromNode->typeInfo     = toTypeLambda;
            }

            return true;
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToSlice(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, CastFlags castFlags)
{
    auto           castErrorType = CastErrorType::Zero;
    TypeInfoSlice* toTypeSlice   = castTypeInfo<TypeInfoSlice>(toType, TypeInfoKind::Slice);

    if (fromType->isListArray())
    {
        TypeInfoList* fromTypeList = castTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeListArray);
        SWAG_CHECK(castExpressionList(context, fromTypeList, toTypeSlice->pointedType, fromNode, castFlags));
        return true;
    }
    if (fromType->isArray())
    {
        const TypeInfoArray* fromTypeArray = castTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
        if ((!castFlags.has(CAST_FLAG_NO_IMPLICIT) && toTypeSlice->pointedType->isSame(fromTypeArray->pointedType, castFlags.with(CAST_FLAG_CAST))) ||
            castFlags.has(CAST_FLAG_EXPLICIT))
        {
            const auto s = toTypeSlice->pointedType->sizeOf;
            const auto d = fromTypeArray->sizeOf;
            SWAG_ASSERT(s != 0 || toTypeSlice->pointedType->isGeneric());

            const bool match = s == 0 || d / s * s == d;
            if (match)
            {
                if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
                {
                    fromNode->typeInfoCast = fromNode->typeInfo;
                    fromNode->typeInfo     = toTypeSlice;
                }

                return true;
            }

            castErrorType = CastErrorType::SliceArray;
        }
    }
    else if (fromType->isString())
    {
        if (toTypeSlice->pointedType->isNative(NativeTypeKind::U8) && toTypeSlice->isConst())
        {
            if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                fromNode->typeInfoCast = fromNode->typeInfo;
                fromNode->typeInfo     = toTypeSlice;

                // Need to make a slice literal from the string
                if (fromNode->hasFlagComputedValue())
                {
                    const auto storageSegment = Semantic::getConstantSegFromContext(fromNode);
                    uint8_t*   addrString;
                    const auto stringOffset = storageSegment->addString(fromNode->computedValue()->text, &addrString);

                    SwagSlice* slice;
                    fromNode->computedValue()->storageSegment = storageSegment;
                    fromNode->computedValue()->storageOffset  = storageSegment->reserve(sizeof(SwagSlice), reinterpret_cast<uint8_t**>(&slice));
                    slice->buffer                             = addrString;
                    slice->count                              = fromNode->computedValue()->text.length();
                    storageSegment->addInitPtr(fromNode->computedValue()->storageOffset, stringOffset);
                }
            }

            return true;
        }
    }
    else if (fromType->isPointerNull())
    {
        if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
        {
            fromNode->typeInfoCast = fromNode->typeInfo;
            fromNode->typeInfo     = toTypeSlice;
        }

        return true;
    }
    else if (fromType->isPointerTo(g_TypeMgr->typeInfoVoid))
    {
        if (castFlags.has(CAST_FLAG_EXPLICIT))
        {
            if (fromNode && !castFlags.has(CAST_FLAG_JUST_CHECK))
            {
                fromNode->typeInfoCast = fromNode->typeInfo;
                fromNode->typeInfo     = toTypeSlice;
            }

            return true;
        }
    }
    else if (fromType->isSlice())
    {
        const auto fromTypeSlice = castTypeInfo<TypeInfoSlice>(fromType, TypeInfoKind::Slice);
        if (castFlags.has(CAST_FLAG_EXPLICIT) || toTypeSlice->pointedType->isSame(fromTypeSlice->pointedType, castFlags.with(CAST_FLAG_CAST)))
            return true;
    }

    return castError(context, toType, fromType, fromNode, castFlags, castErrorType);
}
