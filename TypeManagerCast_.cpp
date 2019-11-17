#include "pch.h"
#include "TypeManager.h"
#include "Diagnostic.h"
#include "Global.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "TypeTable.h"
#include "Module.h"
#include "Ast.h"
#include "AstNode.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"

bool TypeManager::castError(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    // Last change : opCast, with a structure
    if (fromType->kind == TypeInfoKind::Struct)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(fromType, TypeInfoKind::Struct);
        auto structNode = CastAst<AstStruct>(typeStruct->structNode, AstNodeKind::StructDecl);
        auto symbol     = structNode->scope->symTable->find("opCast");
        if (symbol)
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                auto node         = Ast::newNode(context->sourceFile, AstNodeKind::Cast, fromNode);
                node->semanticFct = SemanticJob::resolveUserCast;

                auto lastNode = context->job->nodes.back();
                context->job->nodes.pop_back();
                context->job->nodes.push_back(node);
                context->job->nodes.push_back(lastNode);

                node->castedTypeInfo = fromType;
                node->typeInfo       = toType;
                fromNode->flags |= AST_USER_CAST;
            }

            return true;
        }
    }

    if (!(castFlags & CASTFLAG_NO_ERROR))
    {
        SWAG_ASSERT(fromNode);
        context->report({fromNode, fromNode->token, format("cannot cast from '%s' to '%s'", fromType->name.c_str(), toType->name.c_str()).c_str()});
    }

    return false;
}

bool TypeManager::castToNativeBool(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType == g_TypeMgr.typeInfoBool)
        return true;

    // Automatic cast to a bool is done only if requested, on specific nodes (like if or while expressions)
    if (!(castFlags & CASTFLAG_AUTO_BOOL) && !(castFlags & CASTFLAG_EXPLICIT))
        return castError(context, g_TypeMgr.typeInfoBool, fromType, fromNode, castFlags);

    if (fromType->kind == TypeInfoKind::Pointer || fromType->kind == TypeInfoKind::Lambda)
    {
        if (!(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->typeInfo       = g_TypeMgr.typeInfoBool;
            fromNode->castedTypeInfo = fromType;
            return true;
        }
    }

    fromType = TypeManager::concreteType(fromType);
    if (fromType->kind == TypeInfoKind::Native)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            if (fromNode)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (fromNode->flags & AST_VALUE_COMPUTED)
                    {
                        fromNode->computedValue.reg.b = fromNode->computedValue.reg.u8;
                        fromNode->typeInfo            = g_TypeMgr.typeInfoBool;
                    }
                    else
                    {
                        fromNode->typeInfo       = g_TypeMgr.typeInfoBool;
                        fromNode->castedTypeInfo = fromType;
                    }
                }
            }
            return true;

        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            if (fromNode)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (fromNode->flags & AST_VALUE_COMPUTED)
                    {
                        fromNode->computedValue.reg.b = fromNode->computedValue.reg.u16;
                        fromNode->typeInfo            = g_TypeMgr.typeInfoBool;
                    }
                    else
                    {
                        fromNode->typeInfo       = g_TypeMgr.typeInfoBool;
                        fromNode->castedTypeInfo = fromType;
                    }
                }
            }
            return true;

        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            if (fromNode)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (fromNode->flags & AST_VALUE_COMPUTED)
                    {
                        fromNode->computedValue.reg.b = fromNode->computedValue.reg.u32;
                        fromNode->typeInfo            = g_TypeMgr.typeInfoBool;
                    }
                    else
                    {
                        fromNode->typeInfo       = g_TypeMgr.typeInfoBool;
                        fromNode->castedTypeInfo = fromType;
                    }
                }
            }
            return true;

        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            if (fromNode)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    if (fromNode->flags & AST_VALUE_COMPUTED)
                    {
                        fromNode->computedValue.reg.b = fromNode->computedValue.reg.u64;
                        fromNode->typeInfo            = g_TypeMgr.typeInfoBool;
                    }
                    else
                    {
                        fromNode->typeInfo       = g_TypeMgr.typeInfoBool;
                        fromNode->castedTypeInfo = fromType;
                    }
                }
            }
            return true;
        }
    }

    return castError(context, g_TypeMgr.typeInfoBool, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeChar(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::Char)
        return true;

    if (castFlags & CASTFLAG_EXPLICIT)
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->typeInfo = g_TypeMgr.typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue.reg.u32 = static_cast<uint32_t>(fromNode->computedValue.reg.f32);
                    fromNode->typeInfo              = g_TypeMgr.typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue.reg.u32 = static_cast<uint32_t>(fromNode->computedValue.reg.f64);
                    fromNode->typeInfo              = g_TypeMgr.typeInfoU32;
                }
            }
            return true;
        }
    }
    else
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::U32:
            return true;
        }

        if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
        {
            if (!fromNode)
            {
                auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                if (value < 0)
                    return false;
            }

            return true;
        }
    }

    return castError(context, g_TypeMgr.typeInfoChar, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU8(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U8)
        return true;

    if (castFlags & CASTFLAG_EXPLICIT)
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->typeInfo = g_TypeMgr.typeInfoU8;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue.reg.u8 = static_cast<uint8_t>(fromNode->computedValue.reg.f32);
                    fromNode->typeInfo             = g_TypeMgr.typeInfoU8;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue.reg.u8 = static_cast<uint8_t>(fromNode->computedValue.reg.f64);
                    fromNode->typeInfo             = g_TypeMgr.typeInfoU8;
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue.reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%I64d' is negative and not in the range of 'u8'", fromNode->computedValue.reg.s64)});
                    return false;
                }
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                    if (value < 0)
                        return false;
                }
            }

        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue.reg.u64 > UINT8_MAX)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%I64u' is not in the range of 'u8'", fromNode->computedValue.reg.u64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr.typeInfoU8;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                    if (value > UINT8_MAX)
                        return false;
                }

                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoU8, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU16(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U16)
        return true;

    if (castFlags & CASTFLAG_EXPLICIT)
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->typeInfo = g_TypeMgr.typeInfoU16;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue.reg.u16 = static_cast<uint16_t>(fromNode->computedValue.reg.f32);
                    fromNode->typeInfo              = g_TypeMgr.typeInfoU16;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue.reg.u16 = static_cast<uint16_t>(fromNode->computedValue.reg.f64);
                    fromNode->typeInfo              = g_TypeMgr.typeInfoU16;
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue.reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%I64d' is negative and not in the range of 'u16'", fromNode->computedValue.reg.s64)});
                    return false;
                }
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                    if (value < 0)
                        return false;
                }
            }

        case NativeTypeKind::U8:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue.reg.u64 > UINT16_MAX)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%I64u' is not in the range of 'u16'", fromNode->computedValue.reg.u64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr.typeInfoU16;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                    if (value > UINT16_MAX)
                        return false;
                }

                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoU16, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U32)
        return true;

    if (castFlags & CASTFLAG_EXPLICIT)
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->typeInfo = g_TypeMgr.typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue.reg.u32 = static_cast<uint32_t>(fromNode->computedValue.reg.f32);
                    fromNode->typeInfo              = g_TypeMgr.typeInfoU32;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue.reg.u32 = static_cast<uint32_t>(fromNode->computedValue.reg.f64);
                    fromNode->typeInfo              = g_TypeMgr.typeInfoU32;
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue.reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%I64d' is negative and not in the range of 'u32'", fromNode->computedValue.reg.s64)});
                    return false;
                }
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                    if (value < 0)
                        return false;
                }
            }

        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue.reg.u64 > UINT32_MAX)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%I64u' is not in the range of 'u32'", fromNode->computedValue.reg.u64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr.typeInfoU32;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                    if (value > UINT32_MAX)
                        return false;
                }

                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoU32, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeU64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::U64)
        return true;

    if (castFlags & CASTFLAG_EXPLICIT)
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->typeInfo = g_TypeMgr.typeInfoU64;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue.reg.u64 = static_cast<uint64_t>(fromNode->computedValue.reg.f32);
                    fromNode->typeInfo              = g_TypeMgr.typeInfoU64;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue.reg.u64 = static_cast<uint64_t>(fromNode->computedValue.reg.f64);
                    fromNode->typeInfo              = g_TypeMgr.typeInfoU64;
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue.reg.s64 < 0)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%I64d' is negative and not in the range of 'u64'", fromNode->computedValue.reg.s64)});
                    return false;
                }
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                    if (value < 0)
                        return false;
                }
            }

        case NativeTypeKind::U8:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr.typeInfoU64;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoU64, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS8(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S8)
        return true;

    if (castFlags & CASTFLAG_EXPLICIT)
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->typeInfo = g_TypeMgr.typeInfoS8;
                }
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue.reg.s8 = static_cast<int8_t>(fromNode->computedValue.reg.f32);
                    fromNode->typeInfo             = g_TypeMgr.typeInfoS8;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue.reg.s8 = static_cast<int8_t>(fromNode->computedValue.reg.f64);
                    fromNode->typeInfo             = g_TypeMgr.typeInfoS8;
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue.reg.s64 < INT8_MIN || fromNode->computedValue.reg.s64 > INT8_MAX)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%I64d' is not in the range of 's8'", fromNode->computedValue.reg.s64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr.typeInfoS8;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                    if (value < INT8_MIN || value > INT8_MAX)
                        return false;
                }

                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoS8, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS16(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S16)
        return true;

    if (castFlags & CASTFLAG_EXPLICIT)
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr.typeInfoS16;
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->computedValue.reg.s16 = static_cast<int16_t>(fromNode->computedValue.reg.f32);
                    fromNode->typeInfo              = g_TypeMgr.typeInfoS16;
                }
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                fromNode->computedValue.reg.s16 = static_cast<int16_t>(fromNode->computedValue.reg.f64);
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr.typeInfoS16;
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue.reg.s64 < INT16_MIN || fromNode->computedValue.reg.s64 > INT16_MAX)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%I64d' is not in the range of 's16'", fromNode->computedValue.reg.s64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr.typeInfoS16;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                    if (value < INT16_MIN || value > INT16_MAX)
                        return false;
                }

                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoS16, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S32)
        return true;

    if (castFlags & CASTFLAG_EXPLICIT)
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr.typeInfoS32;
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                fromNode->computedValue.reg.s32 = static_cast<int32_t>(fromNode->computedValue.reg.f32);
                fromNode->typeInfo              = g_TypeMgr.typeInfoS32;
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                fromNode->computedValue.reg.s32 = static_cast<int32_t>(fromNode->computedValue.reg.f64);
                fromNode->typeInfo              = g_TypeMgr.typeInfoS32;
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue.reg.s64 < INT32_MIN || fromNode->computedValue.reg.s64 > INT32_MAX)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%I64d' is not in the range of 's32'", fromNode->computedValue.reg.s64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr.typeInfoS32;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                    if (value < INT32_MIN || value > INT32_MAX)
                        return false;
                }

                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoS32, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeS64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::S64)
        return true;

    if (castFlags & CASTFLAG_EXPLICIT)
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr.typeInfoS64;
            }
            return true;

        case NativeTypeKind::F32:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                fromNode->computedValue.reg.s64 = static_cast<int64_t>(fromNode->computedValue.reg.f32);
                fromNode->typeInfo              = g_TypeMgr.typeInfoS64;
            }
            return true;

        case NativeTypeKind::F64:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                fromNode->computedValue.reg.s64 = static_cast<int64_t>(fromNode->computedValue.reg.f64);
                fromNode->typeInfo              = g_TypeMgr.typeInfoS64;
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
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                if (fromNode->computedValue.reg.s64 < INT64_MIN || fromNode->computedValue.reg.s64 > INT64_MAX)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%I64d' is not in the range of 's64'", fromNode->computedValue.reg.s64)});
                    return false;
                }

                if (!(castFlags & CASTFLAG_JUST_CHECK))
                    fromNode->typeInfo = g_TypeMgr.typeInfoS64;
                return true;
            }
            else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                if (!fromNode)
                {
                    auto value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                    if (value < INT64_MIN || value > INT64_MAX)
                        return false;
                }

                return true;
            }

            break;
        }
    }

    return castError(context, g_TypeMgr.typeInfoS64, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeF32(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::F32)
        return true;

    switch (fromType->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    {
        if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
        {
            if (!(castFlags & CASTFLAG_EXPLICIT))
            {
                float   tmpF = static_cast<float>(fromNode->computedValue.reg.s64);
                int64_t tmpI = static_cast<int64_t>(tmpF);
                if (tmpI != fromNode->computedValue.reg.s64)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%I64d' is truncated in 'f32'", fromNode->computedValue.reg.s64)});
                    return false;
                }
            }

            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->computedValue.reg.f32 = static_cast<float>(fromNode->computedValue.reg.s64);
                fromNode->typeInfo              = g_TypeMgr.typeInfoF32;
            }
            return true;
        }
        else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
        {
            if (!fromNode)
            {
                auto    value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                float   tmpF  = static_cast<float>(value);
                int64_t tmpI  = static_cast<int64_t>(tmpF);
                if (tmpI != value)
                    return false;
            }

            return true;
        }
        else if (castFlags & CASTFLAG_EXPLICIT)
            return true;
        break;
    }

    case NativeTypeKind::U8:
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    {
        if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
        {
            if (!(castFlags & CASTFLAG_EXPLICIT))
            {
                float    tmpF = static_cast<float>(fromNode->computedValue.reg.u64);
                uint64_t tmpI = static_cast<uint64_t>(tmpF);
                if (tmpI != fromNode->computedValue.reg.u64)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%I64u' is truncated in 'f32'", fromNode->computedValue.reg.u64)});
                    return false;
                }
            }

            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->computedValue.reg.f32 = static_cast<float>(fromNode->computedValue.reg.u64);
                fromNode->typeInfo              = g_TypeMgr.typeInfoF32;
            }

            return true;
        }
        else if (castFlags & CASTFLAG_EXPLICIT)
            return true;
        break;
    }

    case NativeTypeKind::F64:
    {
        if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
        {
            if (!(castFlags & CASTFLAG_EXPLICIT))
            {
                float  tmpF = static_cast<float>(fromNode->computedValue.reg.f64);
                double tmpD = static_cast<double>(tmpF);
                if (tmpD != fromNode->computedValue.reg.f64)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%lf' is truncated in 'f32'", fromNode->computedValue.reg.f64)});
                    return false;
                }
            }

            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->computedValue.reg.f32 = static_cast<float>(fromNode->computedValue.reg.f64);
                fromNode->typeInfo              = g_TypeMgr.typeInfoF32;
            }

            return true;
        }
        else if (castFlags & CASTFLAG_EXPLICIT)
            return true;
        break;
    }
    }

    return castError(context, g_TypeMgr.typeInfoF32, fromType, fromNode, castFlags);
}

bool TypeManager::castToNativeF64(SemanticContext* context, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType->nativeType == NativeTypeKind::F64)
        return true;

    switch (fromType->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    {
        if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
        {
            if (!(castFlags & CASTFLAG_EXPLICIT))
            {
                double  tmpF = static_cast<double>(fromNode->computedValue.reg.s64);
                int64_t tmpI = static_cast<int64_t>(tmpF);
                if (tmpI != fromNode->computedValue.reg.s64)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%I64d' is truncated in 'f64'", fromNode->computedValue.reg.s64)});
                    return false;
                }
            }

            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->computedValue.reg.f64 = static_cast<double>(fromNode->computedValue.reg.s64);
                fromNode->typeInfo              = g_TypeMgr.typeInfoF64;
            }
            return true;
        }
        else if (fromType->flags & TYPEINFO_UNTYPED_INTEGER)
        {
            if (!fromNode)
            {
                auto    value = static_cast<TypeInfoNative*>(fromType)->valueInteger;
                double  tmpF  = static_cast<double>(value);
                int64_t tmpI  = static_cast<int64_t>(tmpF);
                if (tmpI != value)
                    return false;
            }

            return true;
        }
        else if (castFlags & CASTFLAG_EXPLICIT)
            return true;
        break;
    }

    case NativeTypeKind::U8:
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    {
        if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
        {
            if (!(castFlags & CASTFLAG_EXPLICIT))
            {
                double   tmpF = static_cast<double>(fromNode->computedValue.reg.u64);
                uint64_t tmpI = static_cast<uint64_t>(tmpF);
                if (tmpI != fromNode->computedValue.reg.u64)
                {
                    if (!(castFlags & CASTFLAG_NO_ERROR))
                        context->report({fromNode, fromNode->token, format("value '%I64u' is truncated in 'f64'", fromNode->computedValue.reg.u64)});
                    return false;
                }
            }

            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->computedValue.reg.f64 = static_cast<double>(fromNode->computedValue.reg.u64);
                fromNode->typeInfo              = g_TypeMgr.typeInfoF64;
            }

            return true;
        }
        else if (castFlags & CASTFLAG_EXPLICIT)
            return true;
        break;
    }

    case NativeTypeKind::F32:
        if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
        {
            if (!(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->computedValue.reg.f64 = static_cast<double>(fromNode->computedValue.reg.f32);
                fromNode->typeInfo              = g_TypeMgr.typeInfoF64;
            }

            return true;
        }
        else if (fromType->flags & TYPEINFO_UNTYPED_FLOAT)
            return true;
        else if (castFlags & CASTFLAG_EXPLICIT)
            return true;
        break;
    }

    return castError(context, g_TypeMgr.typeInfoF64, fromType, fromNode, castFlags);
}

bool TypeManager::castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    switch (toType->nativeType)
    {
    case NativeTypeKind::Bool:
        return castToNativeBool(context, fromType, fromNode, castFlags);
    case NativeTypeKind::U8:
        return castToNativeU8(context, fromType, fromNode, castFlags);
    case NativeTypeKind::U16:
        return castToNativeU16(context, fromType, fromNode, castFlags);
    case NativeTypeKind::U32:
        return castToNativeU32(context, fromType, fromNode, castFlags);
    case NativeTypeKind::Char:
        return castToNativeChar(context, fromType, fromNode, castFlags);
    case NativeTypeKind::U64:
        return castToNativeU64(context, fromType, fromNode, castFlags);
    case NativeTypeKind::S8:
        return castToNativeS8(context, fromType, fromNode, castFlags);
    case NativeTypeKind::S16:
        return castToNativeS16(context, fromType, fromNode, castFlags);
    case NativeTypeKind::S32:
        return castToNativeS32(context, fromType, fromNode, castFlags);
    case NativeTypeKind::S64:
        return castToNativeS64(context, fromType, fromNode, castFlags);
    case NativeTypeKind::F32:
        return castToNativeF32(context, fromType, fromNode, castFlags);
    case NativeTypeKind::F64:
        return castToNativeF64(context, fromType, fromNode, castFlags);
    case NativeTypeKind::String:
        if (fromType->nativeType == NativeTypeKind::String)
            return true;
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castExpressionList(SemanticContext* context, TypeInfoList* fromTypeList, TypeInfo* toType, AstNode* fromNode, uint32_t castFlags)
{
    auto fromSize = fromTypeList->childs.size();
    while (fromNode && fromNode->kind != AstNodeKind::ExpressionList)
        fromNode = fromNode->childs.front();
    SWAG_ASSERT(!fromNode || fromSize == fromNode->childs.size());

    // Need to recompute total size, as the size of each element can have been changed by the cast
    int newSizeof = 0;

    // Compute if expression is constexpr
    if (fromNode)
        fromNode->flags |= AST_CONST_EXPR;

    TypeInfoList* toTypeList = nullptr;
    if (toType->kind == TypeInfoKind::TypeList)
        toTypeList = CastTypeInfo<TypeInfoList>(toType, TypeInfoKind::TypeList);

    for (int i = 0; i < fromSize; i++)
    {
        auto child = fromNode ? fromNode->childs[i] : nullptr;
        SWAG_CHECK(TypeManager::makeCompatibles(context, toTypeList ? toTypeList->childs[i] : toType, fromTypeList->childs[i], nullptr, child, castFlags));
        if (child)
        {
            newSizeof += child->typeInfo->sizeOf;
            if (!(child->flags & AST_CONST_EXPR))
                fromNode->flags &= ~AST_CONST_EXPR;
        }
    }

    if (fromNode && (fromTypeList->sizeOf != newSizeof))
    {
        SWAG_ASSERT(fromNode->typeInfo == fromTypeList);
        fromTypeList         = (TypeInfoList*) fromTypeList->clone();
        fromTypeList->sizeOf = newSizeof;
        fromNode->typeInfo   = fromTypeList;
    }

    return true;
}

bool TypeManager::castToArray(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
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
                if (!(castFlags & CASTFLAG_NO_ERROR))
                {
                    if (toTypeArray->count > fromTypeList->childs.size())
                        context->report({fromNode, format("cannot cast, not enough initializers ('%d' provided, '%d' requested)", fromTypeList->childs.size(), toTypeArray->count)});
                    else
                        context->report({fromNode, format("cannot cast, too many initializers ('%d' provided, '%d' requested)", fromTypeList->childs.size(), toTypeArray->count)});
                }

                return false;
            }

            SWAG_CHECK(castExpressionList(context, fromTypeList, toTypeArray->pointedType, fromNode, castFlags));
            return true;
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToSlice(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    TypeInfoSlice* toTypeSlice = CastTypeInfo<TypeInfoSlice>(toType, TypeInfoKind::Slice);
    if (fromType->kind == TypeInfoKind::TypeList)
    {
        TypeInfoList* fromTypeList = CastTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeList);

        // Can only cast array to slice
        if (fromTypeList->listKind != TypeInfoListKind::Bracket)
            return castError(context, toType, fromType, fromNode, castFlags);

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
                if (!TypeManager::makeCompatibles(context, toTypeSlice->pointedType, typePointer->finalType, nullptr, nullptr, castFlags | CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
                    forcedInit = false;
            }

            // And must and with an U32, which is the slice count
            if (forcedInit && !TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoU32, fromTypeList->childs.back(), nullptr, fromNode ? fromNode->childs.back() : nullptr, castFlags | CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
                forcedInit = false;
            if (forcedInit)
            {
                if (fromNode)
                    fromNode->flags |= AST_SLICE_INIT_EXPRESSION;
                return true;
            }
        }

        SWAG_CHECK(castExpressionList(context, fromTypeList, toTypeSlice->pointedType, fromNode, castFlags));
        return true;
    }
    else if (fromType->kind == TypeInfoKind::Array)
    {
        TypeInfoArray* fromTypeArray = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
        if (toTypeSlice->pointedType->isSame(fromTypeArray->pointedType, ISSAME_CAST))
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK) && !(castFlags & CASTFLAG_EXPLICIT))
            {
                fromNode->castedTypeInfo = fromNode->typeInfo;
                fromNode->typeInfo       = toTypeSlice;
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
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->castedTypeInfo = fromNode->typeInfo;
            fromNode->typeInfo       = toTypeSlice;
        }

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
                if (castFlags & CASTFLAG_EXPLICIT)
                    return true;
            }
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
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

bool TypeManager::convertExpressionListToVarDecl(SemanticContext* context, TypeInfo* toType, AstNode* fromNode)
{
    auto sourceFile = context->sourceFile;
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(toType, TypeInfoKind::Struct);

    if (fromNode->kind == AstNodeKind::FuncCallParam)
    {
        fromNode->setPassThrough();
        fromNode = fromNode->childs.front();
        if (fromNode->kind != AstNodeKind::ExpressionList)
            return true;
    }

    // Declare a variable
    auto varNode = Ast::newVarDecl(sourceFile, format("__tmp_%d", g_Global.uniqueID.fetch_add(1)), fromNode->parent);
    varNode->inheritTokenLocation(fromNode->token);

    auto typeNode = Ast::newTypeExpression(sourceFile, varNode);
    typeNode->inheritTokenLocation(fromNode->token);
    typeNode->flags |= AST_HAS_STRUCT_PARAMETERS;
    varNode->type = typeNode;

    typeNode->identifier = Ast::newIdentifierRef(sourceFile, typeStruct->structNode->name, typeNode);
    typeNode->identifier->flags |= AST_GENERATED;
    typeNode->identifier->inheritTokenLocation(fromNode->token);

    auto back = typeNode->identifier->childs.back();
    back->flags &= ~AST_NO_BYTECODE;
    back->flags |= AST_IN_TYPE_VAR_DECLARATION;

    // And make a reference to that variable
    auto parent = fromNode;
    if (fromNode->parent->kind == AstNodeKind::FuncCallParam)
        parent = fromNode->parent;
    auto identifierRef = Ast::newIdentifierRef(sourceFile, varNode->name, parent);
    identifierRef->flags |= AST_R_VALUE | AST_TRANSIENT | AST_DONT_COLLECT;

    // Make parameters
    auto identifier = CastAst<AstIdentifier>(typeNode->identifier->childs.back(), AstNodeKind::Identifier);
    identifier->inheritTokenLocation(fromNode->token);
    identifier->callParameters = Ast::newFuncCallParams(sourceFile, identifier);
    int countParams            = (int) fromNode->childs.size();
    if (parent == fromNode)
        countParams--;
    for (int i = 0; i < countParams; i++)
    {
        auto         oneChild = fromNode->childs[i];
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
            return context->report({identifier, format("not enough parameters in tuple initialization ('%d' expected, '%d' provided)", typeStruct->childs.size(), countParams)});
        }
    }

    // Add the 2 nodes to the semantic
    auto b = context->job->nodes.back();
    context->job->nodes.pop_back();
    context->job->nodes.push_back(identifierRef);
    context->job->nodes.push_back(varNode);
    context->job->nodes.push_back(b);

    fromNode->typeInfo = toType;
    fromNode->setPassThrough();
    return true;
}

bool TypeManager::makeCompatibles(SemanticContext* context, AstNode* leftNode, AstNode* rightNode, uint32_t castFlags)
{
    SWAG_CHECK(makeCompatibles(context, leftNode->typeInfo, leftNode, rightNode, castFlags));
    return true;
}

bool TypeManager::makeCompatibles(SemanticContext* context, TypeInfo* toType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags)
{
    // convert {...} expression list to a structure : this will create a variable, with parameters
    auto fromType = concreteType(fromNode->typeInfo, CONCRETE_ALIAS);
    if (fromType->kind == TypeInfoKind::TypeList && toType->kind == TypeInfoKind::Struct)
    {
        TypeInfoList* typeList = CastTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeList);
        if (typeList->listKind == TypeInfoListKind::Curly)
        {
            SWAG_CHECK(convertExpressionListToVarDecl(context, toType, fromNode));
            return true;
        }
    }

    SWAG_CHECK(makeCompatibles(context, toType, fromNode->typeInfo, toNode, fromNode, castFlags));
    if (!fromNode)
        return true;

    if ((fromNode->typeInfo->flags & TYPEINFO_AUTO_CAST) && !fromNode->castedTypeInfo)
    {
        if (!(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->castedTypeInfo = fromNode->typeInfo;
            fromNode->typeInfo       = toType;
        }
    }

    // Store constant expression in the constant segment
    if (!(castFlags & CASTFLAG_NO_COLLECT))
    {
        if (fromNode->typeInfo->kind == TypeInfoKind::TypeList && !(fromNode->flags & AST_SLICE_INIT_EXPRESSION))
        {
            TypeInfoList* typeList = CastTypeInfo<TypeInfoList>(fromNode->typeInfo, TypeInfoKind::TypeList);
            auto          fromSize = typeList->childs.size();

            while (fromNode && fromNode->kind != AstNodeKind::ExpressionList)
                fromNode = fromNode->childs.empty() ? nullptr : fromNode->childs.front();

            if (fromNode && (fromNode->flags & AST_CONST_EXPR))
            {
                SWAG_ASSERT(fromSize == fromNode->childs.size());
                auto module   = context->sourceFile->module;
                auto exprList = CastAst<AstExpressionList>(fromNode, AstNodeKind::ExpressionList);
                if (exprList && exprList->storageOffsetSegment == UINT32_MAX)
                {
                    SWAG_CHECK(SemanticJob::reserveAndStoreToSegment(context, exprList->storageOffsetSegment, &module->constantSegment, nullptr, fromNode->typeInfo, exprList));
                }
            }
        }
    }

    return true;
}

bool TypeManager::makeCompatibles(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags)
{
    // convert {...} expression list to a structure : this will create a variable, with parameters
    auto realFromType = concreteType(fromType, CONCRETE_ALIAS);
    auto realToType   = concreteType(toType, CONCRETE_ALIAS);
    SWAG_ASSERT(realFromType && realToType);
    if (realFromType->kind == TypeInfoKind::TypeList && realToType->kind == TypeInfoKind::Struct)
    {
        TypeInfoList* typeList = CastTypeInfo<TypeInfoList>(realFromType, TypeInfoKind::TypeList);
        if (typeList->listKind == TypeInfoListKind::Curly)
        {
            return true;
        }
    }

    SWAG_ASSERT(toType && fromType);

    if (toType->kind == TypeInfoKind::FuncAttr)
        toType = TypeManager::concreteType(toType, CONCRETE_FUNC);
    if (fromType->kind == TypeInfoKind::FuncAttr)
        fromType = TypeManager::concreteType(fromType, CONCRETE_FUNC);
    if (toType->kind != TypeInfoKind::Lambda && fromType->kind == TypeInfoKind::Lambda)
        fromType = TypeManager::concreteType(fromType, CONCRETE_FUNC);

    if (toType->kind == TypeInfoKind::Alias)
        toType = TypeManager::concreteType(toType, CONCRETE_ALIAS);
    if (fromType->kind == TypeInfoKind::Alias)
        fromType = TypeManager::concreteType(fromType, CONCRETE_ALIAS);

    if ((castFlags & CASTFLAG_CONCRETE_ENUM) || (castFlags & CASTFLAG_EXPLICIT))
    {
        toType   = TypeManager::concreteType(toType, CONCRETE_ENUM);
        fromType = TypeManager::concreteType(fromType, CONCRETE_ENUM);
    }

    if (fromType == toType)
        return true;

    // Everything can be casted to type 'any'
    if (toType->isNative(NativeTypeKind::Any) && (castFlags & CASTFLAG_BIJECTIF))
    {
        if (toNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            if (!(castFlags & CASTFLAG_EXPLICIT))
            {
                toNode->castedTypeInfo = toType;
                toNode->typeInfo       = fromNode->typeInfo;
            }

            auto& typeTable = context->sourceFile->module->typeTable;
            SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, fromType, &toNode->concreteTypeInfo, &toNode->concreteTypeInfoStorage));
        }

        return true;
    }

    if (fromType->isNative(NativeTypeKind::Any))
    {
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            if (!(castFlags & CASTFLAG_EXPLICIT))
            {
                fromNode->castedTypeInfo = fromNode->typeInfo;
                fromNode->typeInfo       = toType;
            }

            auto& typeTable = context->sourceFile->module->typeTable;
            SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, toType, &fromNode->concreteTypeInfo, &fromNode->concreteTypeInfoStorage));
        }

        return true;
    }

    if (toType->isNative(NativeTypeKind::Any))
    {
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->castedTypeInfo = fromNode->typeInfo;
            fromNode->typeInfo       = toType;
            auto& typeTable          = context->sourceFile->module->typeTable;
            SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, fromNode->castedTypeInfo, &fromNode->concreteTypeInfo, &fromNode->concreteTypeInfoStorage));
        }

        return true;
    }

    if (fromType->kind == TypeInfoKind::TypedVariadic)
        fromType = ((TypeInfoVariadic*) fromType)->rawType;
    if (toType->kind == TypeInfoKind::TypedVariadic)
        toType = ((TypeInfoVariadic*) toType)->rawType;

    // Const mismatch
    if (!toType->isConst() && fromType->isConst() && !toType->isNative(NativeTypeKind::String))
    {
        if (!(castFlags & CASTFLAG_UNCONST))
            return castError(context, toType, fromType, fromNode, castFlags);

        // We can affect a const to an unconst if type is by copy, and we are in an affectation
        if (!(fromType->flags & TYPEINFO_RETURN_BY_COPY) && !(toType->flags & TYPEINFO_RETURN_BY_COPY))
            return castError(context, toType, fromType, fromNode, castFlags);
    }

    if (fromType->isSame(toType, ISSAME_CAST))
        return true;

    if (fromType->flags & TYPEINFO_AUTO_CAST)
        castFlags |= CASTFLAG_EXPLICIT;

    // Always match against a generic
    if (toType->kind == TypeInfoKind::Generic)
        return true;

    // Pointer to pointer
    if (toType->kind == TypeInfoKind::Pointer && fromType->kind == TypeInfoKind::Pointer)
    {
        if (castFlags & CASTFLAG_EXPLICIT)
            return true;
    }

    // Struct to pointer
    if (toType->kind == TypeInfoKind::Pointer && fromType->kind == TypeInfoKind::Struct)
    {
        auto typePtr = static_cast<TypeInfoPointer*>(toType);
        if (typePtr->ptrCount == 1 && typePtr->finalType->isSame(fromType, ISSAME_CAST))
        {
            if (fromNode && (castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromNode->typeInfo;
                fromNode->typeInfo       = toType;
            }

            return true;
        }
    }

    // String <=> null
    if (toType->isNative(NativeTypeKind::String) && fromType == g_TypeMgr.typeInfoNull)
    {
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->typeInfo       = toType;
            fromNode->castedTypeInfo = g_TypeMgr.typeInfoNull;
        }

        return true;
    }

    if (toType == g_TypeMgr.typeInfoNull && fromType->isNative(NativeTypeKind::String))
        return true;

    // const [..] u8 to string, this is possible !
    if (toType->isNative(NativeTypeKind::String) && fromType->kind == TypeInfoKind::Slice)
    {
        auto fromTypeSlice = CastTypeInfo<TypeInfoSlice>(fromType, TypeInfoKind::Slice);
        if ((fromTypeSlice->flags & TYPEINFO_CONST) && (fromTypeSlice->pointedType == g_TypeMgr.typeInfoU8))
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromNode->typeInfo;
                fromNode->typeInfo       = g_TypeMgr.typeInfoString;
            }

            return true;
        }
    }

    // Cast to native type
    if (toType->kind == TypeInfoKind::Native)
        return castToNative(context, toType, fromType, fromNode, castFlags);

    // Cast to array
    if (toType->kind == TypeInfoKind::Array)
        return castToArray(context, toType, fromType, fromNode, castFlags);

    // Cast to slice
    if (toType->kind == TypeInfoKind::Slice)
        return castToSlice(context, toType, fromType, fromNode, castFlags);

    // Cast to lambda
    if (toType->kind == TypeInfoKind::Lambda)
    {
        if (toType->isSame(fromType, ISSAME_CAST))
            return true;
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}
