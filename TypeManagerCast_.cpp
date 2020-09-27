#include "pch.h"
#include "TypeManager.h"
#include "SourceFile.h"
#include "Module.h"
#include "Ast.h"
#include "SemanticJob.h"

bool TypeManager::castError(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    // Last minute change : opCast, with a structure
    if (fromType->kind == TypeInfoKind::Struct && (castFlags & CASTFLAG_EXPLICIT))
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(fromType, TypeInfoKind::Struct);
        if (typeStruct->declNode)
        {
            auto structNode = CastAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
            auto symbol     = structNode->scope->symTable.find("opCast");
            if (symbol)
            {
                if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
                {
                    auto node         = Ast::newNode(context->sourceFile, AstNodeKind::Cast, fromNode);
                    node->semanticFct = SemanticJob::resolveUserCast;
                    node->name        = Utf8Crc("opCast");

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
    }

    if (!(castFlags & CASTFLAG_NO_ERROR))
    {
        SWAG_ASSERT(fromNode);

        // Is there an explicit cast possible ?
        if (!(castFlags & CASTFLAG_EXPLICIT))
        {
            if (TypeManager::makeCompatibles(context, toType, fromType, nullptr, nullptr, CASTFLAG_EXPLICIT | CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
            {
                Diagnostic diag{fromNode, fromNode->token, format("cannot cast implicitly from '%s' to '%s'", fromType->name.c_str(), toType->name.c_str()).c_str()};
                diag.codeComment = format("'cast(%s)' can be used in that context", toType->name.c_str());
                context->report(diag);
            }
            else
            {
                context->report({fromNode, fromNode->token, format("cannot cast from '%s' to '%s'", fromType->name.c_str(), toType->name.c_str()).c_str()});
            }
        }
        else
        {
            context->report({fromNode, fromNode->token, format("cannot cast from '%s' to '%s'", fromType->name.c_str(), toType->name.c_str()).c_str()});
        }
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

    if (castFlags & CASTFLAG_STRICT)
    {
        if (fromType->nativeType != NativeTypeKind::Bool)
        {
            return castError(context, g_TypeMgr.typeInfoBool, fromType, fromNode, castFlags);
        }
    }

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
        case NativeTypeKind::Bool:
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

        case NativeTypeKind::String:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                char32_t ch;
                if (fromNode->computedValue.text.toChar32(ch))
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                    {
                        fromNode->computedValue.reg.ch = ch;
                        fromNode->typeInfo             = g_TypeMgr.typeInfoChar;
                    }

                    return true;
                }
            }
            break;
        }
    }
    else
    {
        if (castFlags & CASTFLAG_STRICT)
        {
            if (fromType->nativeType != NativeTypeKind::Char)
            {
                return castError(context, g_TypeMgr.typeInfoChar, fromType, fromNode, castFlags);
            }
        }

        switch (fromType->nativeType)
        {
        case NativeTypeKind::U32:
            return true;

        case NativeTypeKind::String:
            if (fromNode && fromNode->flags & AST_VALUE_COMPUTED)
            {
                char32_t ch;
                if (fromNode->computedValue.text.toChar32(ch))
                {
                    if (!(castFlags & CASTFLAG_JUST_CHECK))
                    {
                        fromNode->computedValue.reg.ch = ch;
                        fromNode->typeInfo             = g_TypeMgr.typeInfoChar;
                    }

                    return true;
                }
            }
            break;
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
        case NativeTypeKind::Bool:
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
        if (castFlags & CASTFLAG_STRICT)
        {
            if (fromType->nativeType != NativeTypeKind::U8)
            {
                return castError(context, g_TypeMgr.typeInfoU8, fromType, fromNode, castFlags);
            }
        }

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
            else if ((fromType->flags & TYPEINFO_UNTYPED_INTEGER) || (fromType->flags & TYPEINFO_UNTYPED_BINHEXA))
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
        case NativeTypeKind::Bool:
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
        if (castFlags & CASTFLAG_STRICT)
        {
            if (fromType->nativeType != NativeTypeKind::U16)
            {
                return castError(context, g_TypeMgr.typeInfoU16, fromType, fromNode, castFlags);
            }
        }

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
            else if ((fromType->flags & TYPEINFO_UNTYPED_INTEGER) || (fromType->flags & TYPEINFO_UNTYPED_BINHEXA))
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
        case NativeTypeKind::Bool:
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
        if (castFlags & CASTFLAG_STRICT)
        {
            if (fromType->nativeType != NativeTypeKind::U32)
            {
                return castError(context, g_TypeMgr.typeInfoU32, fromType, fromNode, castFlags);
            }
        }

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
            else if ((fromType->flags & TYPEINFO_UNTYPED_INTEGER) || (fromType->flags & TYPEINFO_UNTYPED_BINHEXA))
            {
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

    if (fromType->kind == TypeInfoKind::Pointer)
    {
        if (castFlags & CASTFLAG_EXPLICIT)
        {
            if (!(castFlags & CASTFLAG_JUST_CHECK))
                fromNode->typeInfo = g_TypeMgr.typeInfoU64;
            return true;
        }
    }

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        switch (fromType->nativeType)
        {
        case NativeTypeKind::Char:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::Bool:
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
        if (castFlags & CASTFLAG_STRICT)
        {
            if (fromType->nativeType != NativeTypeKind::U64)
            {
                return castError(context, g_TypeMgr.typeInfoU64, fromType, fromNode, castFlags);
            }
        }

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
            else if ((fromType->flags & TYPEINFO_UNTYPED_INTEGER) || (fromType->flags & TYPEINFO_UNTYPED_BINHEXA))
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
        case NativeTypeKind::Bool:
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
        if (castFlags & CASTFLAG_STRICT)
        {
            if (fromType->nativeType != NativeTypeKind::S8)
            {
                return castError(context, g_TypeMgr.typeInfoS8, fromType, fromNode, castFlags);
            }
        }

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
        case NativeTypeKind::Bool:
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
        if (castFlags & CASTFLAG_STRICT)
        {
            if (fromType->nativeType != NativeTypeKind::S16)
            {
                return castError(context, g_TypeMgr.typeInfoS16, fromType, fromNode, castFlags);
            }
        }

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
        case NativeTypeKind::Bool:
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
        if (castFlags & CASTFLAG_STRICT)
        {
            if (fromType->nativeType != NativeTypeKind::S32)
            {
                return castError(context, g_TypeMgr.typeInfoS32, fromType, fromNode, castFlags);
            }
        }

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
        case NativeTypeKind::Bool:
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
        if (castFlags & CASTFLAG_STRICT)
        {
            if (fromType->nativeType != NativeTypeKind::S64)
            {
                return castError(context, g_TypeMgr.typeInfoS64, fromType, fromNode, castFlags);
            }
        }

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

    if (castFlags & CASTFLAG_STRICT)
    {
        if (fromType->nativeType != NativeTypeKind::F32)
        {
            return castError(context, g_TypeMgr.typeInfoF32, fromType, fromNode, castFlags);
        }
    }

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

    case NativeTypeKind::Bool:
        if (castFlags & CASTFLAG_EXPLICIT)
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->computedValue.reg.f32 = fromNode->computedValue.reg.b ? 1.0f : 0.0f;
                fromNode->typeInfo              = g_TypeMgr.typeInfoF32;
            }
            return true;
        }
        break;

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

    if (castFlags & CASTFLAG_STRICT)
    {
        if (fromType->nativeType != NativeTypeKind::F64)
        {
            return castError(context, g_TypeMgr.typeInfoF64, fromType, fromNode, castFlags);
        }
    }

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

    case NativeTypeKind::Bool:
        if (castFlags & CASTFLAG_EXPLICIT)
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->computedValue.reg.f64 = fromNode->computedValue.reg.b ? 1.0f : 0.0f;
                fromNode->typeInfo              = g_TypeMgr.typeInfoF64;
            }
            return true;
        }
        break;

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

bool TypeManager::castToNative(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags)
{
    // Pick the best order
    if (castFlags & CASTFLAG_BIJECTIF)
    {
        if ((toType->flags & TYPEINFO_UNTYPED_INTEGER) && !(fromType->flags & TYPEINFO_UNTYPED_INTEGER))
        {
            swap(toType, fromType);
            swap(toNode, fromNode);
        }
        else if ((toType->flags & TYPEINFO_UNTYPED_FLOAT) && !(fromType->flags & TYPEINFO_UNTYPED_FLOAT))
        {
            swap(toType, fromType);
            swap(toNode, fromNode);
        }
    }

    return castToNative(context, toType, fromType, fromNode, castFlags);
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
        return castToString(context, toType, fromType, fromNode, castFlags);
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castExpressionList(SemanticContext* context, TypeInfoList* fromTypeList, TypeInfo* toType, AstNode* fromNode, uint32_t castFlags)
{
    auto fromSize = fromTypeList->subTypes.size();
    while (fromNode && fromNode->kind != AstNodeKind::ExpressionList)
        fromNode = fromNode->childs.front();
    SWAG_ASSERT(!fromNode || fromSize == fromNode->childs.size());

    // Need to recompute total size, as the size of each element can have been changed by the cast
    uint32_t newSizeof = 0;

    // Compute if expression is constexpr
    if (fromNode)
        fromNode->flags |= AST_CONST_EXPR;

    TypeInfoList* toTypeList = nullptr;
    if (toType->kind == TypeInfoKind::TypeListTuple || toType->kind == TypeInfoKind::TypeListArray)
        toTypeList = CastTypeInfo<TypeInfoList>(toType, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);

    for (int i = 0; i < fromSize; i++)
    {
        auto child     = fromNode ? fromNode->childs[i] : nullptr;
        auto convertTo = toTypeList ? toTypeList->subTypes[i]->typeInfo : toType;
        SWAG_CHECK(TypeManager::makeCompatibles(context, convertTo, fromTypeList->subTypes[i]->typeInfo, nullptr, child, castFlags));
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

bool TypeManager::castToString(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType == g_TypeMgr.typeInfoNull)
    {
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->typeInfo       = toType;
            fromNode->castedTypeInfo = g_TypeMgr.typeInfoNull;
        }

        return true;
    }

    if (castFlags & CASTFLAG_EXPLICIT)
    {
        // [..] u8 to string, this is possible !
        if (fromType->kind == TypeInfoKind::Slice)
        {
            auto fromTypeSlice = CastTypeInfo<TypeInfoSlice>(fromType, TypeInfoKind::Slice);
            if (fromTypeSlice->pointedType == g_TypeMgr.typeInfoU8)
            {
                if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->castedTypeInfo = fromNode->typeInfo;
                    fromNode->typeInfo       = g_TypeMgr.typeInfoString;
                }

                return true;
            }
        }

        // [] u8 to string, this is possible !
        if (fromType->kind == TypeInfoKind::Array)
        {
            auto fromTypeArray = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
            if (fromTypeArray->pointedType == g_TypeMgr.typeInfoU8)
            {
                if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->castedTypeInfo = fromNode->typeInfo;
                    fromNode->typeInfo       = g_TypeMgr.typeInfoString;
                }

                return true;
            }
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToFromAny(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* toNode, AstNode* fromNode, uint32_t castFlags)
{
    if (toType->isNative(NativeTypeKind::Any))
    {
        if (castFlags & CASTFLAG_BIJECTIF)
        {
            if (toNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                toNode->castedTypeInfo = toType;
                toNode->typeInfo       = fromType;
                auto& typeTable        = context->sourceFile->module->typeTable;
                SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, fromType, nullptr, &toNode->concreteTypeInfoStorage, CONCRETE_ZERO));
            }

            return true;
        }

        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->castedTypeInfo = fromType;
            fromNode->typeInfo       = toType;
            auto& typeTable          = context->sourceFile->module->typeTable;
            SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, fromNode->castedTypeInfo, nullptr, &fromNode->concreteTypeInfoStorage, CONCRETE_ZERO));
        }
    }
    else if (fromType->isNative(NativeTypeKind::Any))
    {
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->castedTypeInfo = fromType;
            fromNode->typeInfo       = toType;
            auto& typeTable          = context->sourceFile->module->typeTable;
            SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, toType, nullptr, &fromNode->concreteTypeInfoStorage, CONCRETE_ZERO));
        }
    }

    return true;
}

bool TypeManager::castToReference(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    auto toTypeReference = CastTypeInfo<TypeInfoReference>(toType, TypeInfoKind::Reference);

    // Same referenced type
    if (toTypeReference->pointedType->isSame(fromType, ISSAME_CAST))
        return true;

    // Structure to interface reference
    if (fromType->kind == TypeInfoKind::Struct && toTypeReference->pointedType->kind == TypeInfoKind::Interface)
    {
        auto toTypeItf      = CastTypeInfo<TypeInfoStruct>(toTypeReference->pointedType, TypeInfoKind::Interface);
        auto fromTypeStruct = CastTypeInfo<TypeInfoStruct>(fromType, TypeInfoKind::Struct);
        if (!fromTypeStruct->hasInterface(toTypeItf))
            return castError(context, toType, fromType, fromNode, castFlags);

        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->castedTypeInfo = fromType;
            fromNode->typeInfo       = toTypeItf;
        }

        return true;
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToPointer(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    auto toTypePointer = CastTypeInfo<TypeInfoPointer>(toType, TypeInfoKind::Pointer);

    // Pointer to struct to pointer to struct. Take care of using
    if (fromType->kind == TypeInfoKind::Pointer && toTypePointer->ptrCount == 1 && toTypePointer->finalType->kind == TypeInfoKind::Struct)
    {
        auto fromTypePointer = CastTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer);
        if (fromTypePointer->ptrCount == 1 && fromTypePointer->finalType->kind == TypeInfoKind::Struct)
        {
            auto fromStruct = CastTypeInfo<TypeInfoStruct>(fromTypePointer->finalType, TypeInfoKind::Struct);
            auto toStruct   = CastTypeInfo<TypeInfoStruct>(toTypePointer->finalType, TypeInfoKind::Struct);

            TypeInfoParam* done = nullptr;
            for (auto field : fromStruct->fields)
            {
                if (field->typeInfo != toStruct || !field->hasUsing)
                    continue;

                if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
                {
                    // Ambiguous ! Two fields with a 'using' on the same struct
                    if (done)
                    {
                        Diagnostic diag{fromNode, fromNode->token, format("cannot cast from '%s' to '%s' because structure '%s' has multiple fields of type '%s' with 'using'", fromType->name.c_str(), toType->name.c_str(), fromStruct->name.c_str(), toStruct->name.c_str())};
                        Diagnostic note1{done->node, "this is one", DiagnosticLevel::Note};
                        Diagnostic note2{field->node, "this is another", DiagnosticLevel::Note};
                        return context->report(diag, &note1, &note2);
                    }

                    fromNode->castOffset     = field->offset;
                    fromNode->castedTypeInfo = fromNode->typeInfo;
                    fromNode->typeInfo       = toType;
                }

                done = field;
            }

            if (done)
                return true;
        }
    }

    // Pointer to pointer
    if (fromType->kind == TypeInfoKind::Pointer)
    {
        if (castFlags & CASTFLAG_EXPLICIT)
            return true;

        // Fine to compare pointers of TypeInfos, even if not of the same type
        if (castFlags & CASTFLAG_COMPARE)
        {
            if ((fromType->isPointerToTypeInfo()) && (toType->isPointerToTypeInfo()))
                return true;
        }
    }

    // Array to pointer of the same type
    if (fromType->kind == TypeInfoKind::Array)
    {
        if (toTypePointer->ptrCount == 1)
        {
            auto fromTypeArray = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
            if (toTypePointer->finalType->isNative(NativeTypeKind::Void) ||
                toTypePointer->finalType->isSame(fromTypeArray->pointedType, ISSAME_CAST) ||
                (castFlags & CASTFLAG_EXPLICIT))
            {
                if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->castedTypeInfo = fromNode->typeInfo;
                    fromNode->typeInfo       = toType;
                }

                return true;
            }
        }
    }

    // Slice to pointer of the same type
    if (fromType->kind == TypeInfoKind::Slice)
    {
        if (toTypePointer->ptrCount == 1)
        {
            auto fromTypeSlice = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Slice);
            if (toTypePointer->finalType->isNative(NativeTypeKind::Void) ||
                toTypePointer->finalType->isSame(fromTypeSlice->pointedType, ISSAME_CAST) ||
                (castFlags & CASTFLAG_EXPLICIT))
            {
                if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
                {
                    fromNode->castedTypeInfo = fromNode->typeInfo;
                    fromNode->typeInfo       = toType;
                }

                return true;
            }
        }
    }

    // Struct/Interface to pointer
    if (fromType->kind == TypeInfoKind::Struct || fromType->kind == TypeInfoKind::Interface)
    {
        if ((castFlags & CASTFLAG_EXPLICIT) || (toTypePointer->flags & TYPEINFO_SELF) || toTypePointer->isConst())
        {
            if (toTypePointer->ptrCount == 1)
            {
                // to *void or *structure
                if (toTypePointer->finalType->isNative(NativeTypeKind::Void) ||
                    toTypePointer->finalType->isSame(fromType, ISSAME_CAST))
                {
                    if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK) && !(toTypePointer->flags & TYPEINFO_SELF))
                    {
                        fromNode->castedTypeInfo = fromNode->typeInfo;
                        fromNode->typeInfo       = toType;
                    }

                    return true;
                }
            }
        }
    }

    // Struct/Interface reference to pointer
    if (fromType->kind == TypeInfoKind::Reference)
    {
        auto typeRef = TypeManager::concreteReference(fromType);
        if (typeRef->kind == TypeInfoKind::Struct || typeRef->kind == TypeInfoKind::Interface)
        {
            if ((castFlags & CASTFLAG_EXPLICIT) || (toTypePointer->flags & TYPEINFO_SELF) || toTypePointer->isConst())
            {
                if (toTypePointer->ptrCount == 1)
                {
                    // to *void or *structure
                    if (toTypePointer->finalType->isNative(NativeTypeKind::Void) ||
                        toTypePointer->finalType->isSame(typeRef, ISSAME_CAST))
                    {
                        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK) && !(toTypePointer->flags & TYPEINFO_SELF))
                        {
                            fromNode->castedTypeInfo = fromNode->typeInfo;
                            fromNode->typeInfo       = toType;
                        }

                        return true;
                    }
                }
            }
        }
    }

    // Struct pointer to interface pointer
    if (fromType->isPointerTo(TypeInfoKind::Struct) && toType->isPointerTo(TypeInfoKind::Interface))
    {
        auto fromTypePointer = CastTypeInfo<TypeInfoPointer>(fromType, TypeInfoKind::Pointer);
        auto toTypeItf       = CastTypeInfo<TypeInfoStruct>(toTypePointer->pointedType, TypeInfoKind::Interface);
        auto fromTypeStruct  = CastTypeInfo<TypeInfoStruct>(fromTypePointer->pointedType, TypeInfoKind::Struct);
        if (!fromTypeStruct->hasInterface(toTypeItf))
            return castError(context, toType, fromType, fromNode, castFlags);

        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->castedTypeInfo = fromType;
            fromNode->typeInfo       = toTypeItf;
        }

        return true;
    }

    // String to const *u8
    if (fromType->isNative(NativeTypeKind::String))
    {
        if (toType == g_TypeMgr.typeInfoNull)
            return true;

        if (castFlags & CASTFLAG_EXPLICIT)
        {
            if (toTypePointer->ptrCount == 1)
            {
                if (toTypePointer->pointedType->isNative(NativeTypeKind::U8) && toTypePointer->isConst())
                {
                    if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
                    {
                        fromNode->castedTypeInfo = fromNode->typeInfo;
                        fromNode->typeInfo       = toType;
                    }

                    return true;
                }
            }
        }
    }

    // u64 to whatever pointer
    if (fromType->isNative(NativeTypeKind::U64))
    {
        if (castFlags & CASTFLAG_EXPLICIT)
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromNode->typeInfo;
                fromNode->typeInfo       = toType;
            }

            return true;
        }
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToArray(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    TypeInfoArray* toTypeArray = CastTypeInfo<TypeInfoArray>(toType, TypeInfoKind::Array);
    if (fromType->kind == TypeInfoKind::TypeListArray)
    {
        TypeInfoList* fromTypeList = CastTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeListArray);
        auto          fromSize     = fromTypeList->subTypes.size();
        if (toTypeArray->count != fromSize)
        {
            if (!(castFlags & CASTFLAG_NO_ERROR))
            {
                if (toTypeArray->count > fromTypeList->subTypes.size())
                    context->report({fromNode, format("cannot cast, not enough initializers ('%d' provided, '%d' requested)", fromTypeList->subTypes.size(), toTypeArray->count)});
                else
                    context->report({fromNode, format("cannot cast, too many initializers ('%d' provided, '%d' requested)", fromTypeList->subTypes.size(), toTypeArray->count)});
            }

            return false;
        }

        SWAG_CHECK(castExpressionList(context, fromTypeList, toTypeArray->pointedType, fromNode, castFlags));
        return true;
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToInterface(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    if (fromType == g_TypeMgr.typeInfoNull)
    {
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->castedTypeInfo = fromNode->typeInfo;
            fromNode->typeInfo       = toType;
        }

        return true;
    }

    // Struct to interface
    if (fromType->kind == TypeInfoKind::Struct)
    {
        auto toTypeItf      = CastTypeInfo<TypeInfoStruct>(toType, TypeInfoKind::Interface);
        auto fromTypeStruct = CastTypeInfo<TypeInfoStruct>(fromType, TypeInfoKind::Struct);
        if (!fromTypeStruct->hasInterface(toTypeItf))
            return castError(context, toType, fromType, fromNode, castFlags);
        if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
        {
            fromNode->castedTypeInfo = fromType;
            fromNode->typeInfo       = toTypeItf;
        }

        return true;
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}

bool TypeManager::castToSlice(SemanticContext* context, TypeInfo* toType, TypeInfo* fromType, AstNode* fromNode, uint32_t castFlags)
{
    TypeInfoSlice* toTypeSlice = CastTypeInfo<TypeInfoSlice>(toType, TypeInfoKind::Slice);

    if (fromType->kind == TypeInfoKind::TypeListArray)
    {
        TypeInfoList* fromTypeList = CastTypeInfo<TypeInfoList>(fromType, TypeInfoKind::TypeListArray);
        SWAG_CHECK(castExpressionList(context, fromTypeList, toTypeSlice->pointedType, fromNode, castFlags));
        return true;
    }
    else if (fromType->kind == TypeInfoKind::Array)
    {
        TypeInfoArray* fromTypeArray = CastTypeInfo<TypeInfoArray>(fromType, TypeInfoKind::Array);
        if (toTypeSlice->pointedType->isSame(fromTypeArray->pointedType, ISSAME_CAST))
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromNode->typeInfo;
                fromNode->typeInfo       = toTypeSlice;
            }

            return true;
        }
    }
    else if (fromType->isNative(NativeTypeKind::String))
    {
        if (toTypeSlice->pointedType->isNative(NativeTypeKind::U8) && toTypeSlice->isConst())
        {
            if (fromNode && !(castFlags & CASTFLAG_JUST_CHECK))
            {
                fromNode->castedTypeInfo = fromNode->typeInfo;
                fromNode->typeInfo       = toTypeSlice;
            }

            return true;
        }
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

void TypeManager::promoteUntypedInteger(AstNode* left, AstNode* right)
{
    TypeInfo* leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    TypeInfo* rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    SWAG_ASSERT(leftTypeInfo->flags & TYPEINFO_UNTYPED_INTEGER);
    auto leftNative = CastTypeInfo<TypeInfoNative>(leftTypeInfo, TypeInfoKind::Native);
    if (rightTypeInfo->flags & TYPEINFO_INTEGER)
    {
        if ((leftNative->valueInteger > 0) && (rightTypeInfo->flags & TYPEINFO_UNSIGNED))
        {
            if (leftNative->valueInteger <= UINT8_MAX)
                left->typeInfo = g_TypeMgr.typeInfoU8;
            else if (leftNative->valueInteger <= UINT16_MAX)
                left->typeInfo = g_TypeMgr.typeInfoU16;
            else if (leftNative->valueInteger <= UINT32_MAX)
                left->typeInfo = g_TypeMgr.typeInfoU32;
            else
                left->typeInfo = g_TypeMgr.typeInfoU64;
        }
        else if (leftNative->valueInteger < 0)
        {
            if (leftNative->valueInteger >= INT8_MIN && leftNative->valueInteger <= INT8_MAX)
                left->typeInfo = g_TypeMgr.typeInfoS8;
            else if (leftNative->valueInteger >= INT16_MIN && leftNative->valueInteger <= INT16_MAX)
                left->typeInfo = g_TypeMgr.typeInfoS16;
            else if (leftNative->valueInteger >= INT32_MIN && leftNative->valueInteger <= INT32_MAX)
                left->typeInfo = g_TypeMgr.typeInfoS32;
            else
                left->typeInfo = g_TypeMgr.typeInfoS64;
        }
        else if (leftNative->valueInteger == 0)
        {
            left->typeInfo = rightTypeInfo;
        }
    }
}

void TypeManager::promoteOne(AstNode* left, AstNode* right)
{
    TypeInfo* leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    TypeInfo* rightTypeInfo = TypeManager::concreteType(right->typeInfo);
    if ((leftTypeInfo->kind != TypeInfoKind::Native) || (rightTypeInfo->kind != TypeInfoKind::Native))
        return;

    if ((leftTypeInfo->flags & TYPEINFO_UNTYPED_INTEGER) && !(rightTypeInfo->flags & TYPEINFO_UNTYPED_INTEGER))
    {
        promoteUntypedInteger(left, right);
        leftTypeInfo = left->typeInfo;
    }

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

bool TypeManager::convertLiteralTupleToStruct(SemanticContext* context, TypeInfo* toType, AstNode* fromNode)
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

    // Parent of the identifier that will reference the new variable
    auto parentForRef = fromNode;
    if (fromNode->parent->kind == AstNodeKind::FuncCallParam)
        parentForRef = fromNode->parent;

    // Declare a variable
    auto varNode = Ast::newVarDecl(sourceFile, format("__tmp_%d", g_Global.uniqueID.fetch_add(1)), fromNode->parent);

    // The variable will be inserted after its reference (below), so we need to inverse the order of evaluation.
    // Seems a little bit like a hack. Not sure this will always work.
    fromNode->parent->flags |= AST_REVERSE_SEMANTIC;

    varNode->inheritTokenLocation(fromNode->token);
    varNode->flags |= AST_GENERATED;

    auto typeNode = Ast::newTypeExpression(sourceFile, varNode);
    typeNode->inheritTokenLocation(fromNode->token);
    typeNode->flags |= AST_HAS_STRUCT_PARAMETERS;
    varNode->type = typeNode;

    typeNode->identifier = Ast::newIdentifierRef(sourceFile, typeStruct->declNode->name, typeNode);
    typeNode->identifier->flags |= AST_GENERATED;
    typeNode->identifier->inheritTokenLocation(fromNode->token);

    auto back = typeNode->identifier->childs.back();
    back->flags &= ~AST_NO_BYTECODE;
    back->flags |= AST_IN_TYPE_VAR_DECLARATION;

    // And make a reference to that variable
    auto identifierRef = Ast::newIdentifierRef(sourceFile, varNode->name, parentForRef);
    identifierRef->flags |= AST_R_VALUE | AST_TRANSIENT | AST_GENERATED;

    // Make parameters
    auto identifier = CastAst<AstIdentifier>(typeNode->identifier->childs.back(), AstNodeKind::Identifier);
    identifier->inheritTokenLocation(fromNode->token);
    identifier->callParameters = Ast::newFuncCallParams(sourceFile, identifier);
    identifier->callParameters->flags |= AST_CALL_FOR_STRUCT;

    int countParams = (int) fromNode->childs.size();
    if (parentForRef == fromNode)
        countParams--;
    for (int i = 0; i < countParams; i++)
    {
        auto         oneChild = fromNode->childs[i];
        auto         oneParam = Ast::newFuncCallParam(sourceFile, identifier->callParameters);
        CloneContext cloneContext;
        cloneContext.parent = oneParam;
        oneChild->clone(cloneContext);
        oneChild->flags |= AST_NO_BYTECODE | AST_NO_SEMANTIC;
    }

    // For a tuple initialization, every parameters must be covered
    if ((typeStruct->flags & TYPEINFO_STRUCT_IS_TUPLE) && countParams != typeStruct->fields.size())
        return context->report({identifier, format("not enough parameters in tuple initialization ('%d' expected, '%d' provided)", typeStruct->fields.size(), countParams)});

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
    SWAG_ASSERT(fromNode->typeInfo);
    auto fromType = concreteType(fromNode->typeInfo, CONCRETE_ALIAS);
    if (fromType->kind == TypeInfoKind::TypeListTuple && toType->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(convertLiteralTupleToStruct(context, toType, fromNode));
        return true;
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
        if (fromNode->typeInfo->kind == TypeInfoKind::TypeListTuple || fromNode->typeInfo->kind == TypeInfoKind::TypeListArray)
        {
            while (fromNode && fromNode->kind != AstNodeKind::ExpressionList)
                fromNode = fromNode->childs.empty() ? nullptr : fromNode->childs.front();

            if (fromNode && (fromNode->flags & AST_CONST_EXPR))
            {
#ifdef SWAG_HAS_ASSERT
                TypeInfoList* typeList = CastTypeInfo<TypeInfoList>(fromNode->typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);
                SWAG_ASSERT(typeList->subTypes.size() == fromNode->childs.size());
#endif

                auto module   = context->sourceFile->module;
                auto exprList = CastAst<AstExpressionList>(fromNode, AstNodeKind::ExpressionList);
                if (exprList && !(exprList->doneFlags & AST_DONE_EXPRLIST_CST))
                {
                    exprList->doneFlags |= AST_DONE_EXPRLIST_CST;
                    SWAG_CHECK(SemanticJob::reserveAndStoreToSegment(context, exprList->computedValue.reg.offset, &module->constantSegment, nullptr, fromNode->typeInfo, exprList));
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
    if (realFromType->kind == TypeInfoKind::TypeListTuple)
    {
        if (realToType->kind == TypeInfoKind::Struct)
            return true;

        if (realToType->kind == TypeInfoKind::Reference && realToType->isConst())
        {
            auto ptrRef = CastTypeInfo<TypeInfoReference>(realToType, TypeInfoKind::Reference);
            if (ptrRef->pointedType->kind == TypeInfoKind::Struct)
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

    // Cast from a reference to the type of the reference
    if (fromType->kind == TypeInfoKind::Reference)
    {
        auto typeRef = CastTypeInfo<TypeInfoReference>(fromType, TypeInfoKind::Reference);
        fromType     = typeRef->pointedType;
    }
    if (toType->kind == TypeInfoKind::Reference && fromType->kind != TypeInfoKind::Struct)
    {
        auto typeRef = CastTypeInfo<TypeInfoReference>(toType, TypeInfoKind::Reference);
        toType       = typeRef->pointedType;
    }

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

    // Everything can be casted to or from type 'any'
    if (toType->isNative(NativeTypeKind::Any) || fromType->isNative(NativeTypeKind::Any))
        return castToFromAny(context, toType, fromType, toNode, fromNode, castFlags);

    // Variadic
    if (fromType->kind == TypeInfoKind::TypedVariadic)
        fromType = ((TypeInfoVariadic*) fromType)->rawType;
    if (toType->kind == TypeInfoKind::TypedVariadic)
        toType = ((TypeInfoVariadic*) toType)->rawType;

    // Const mismatch
    if (toType->kind != TypeInfoKind::Generic && !(castFlags & CASTFLAG_FORCE_UNCONST))
    {
        if (!toType->isConst() && fromType->isConst())
        {
            if (!toType->isNative(NativeTypeKind::String) &&
                (!toType->isNative(NativeTypeKind::Bool) || !(castFlags & CASTFLAG_AUTO_BOOL)) &&
                (!toType->isNative(NativeTypeKind::U64) || fromType->kind != TypeInfoKind::Pointer))
            {
                if (!(castFlags & CASTFLAG_UNCONST))
                    return castError(context, toType, fromType, fromNode, castFlags);

                // We can affect a const to an unconst if type is by copy, and we are in an affectation
                if (!(fromType->flags & TYPEINFO_RETURN_BY_COPY) && !(toType->flags & TYPEINFO_RETURN_BY_COPY))
                    return castError(context, toType, fromType, fromNode, castFlags);
            }
        }
    }

    // From a reference
    if (fromType->kind == TypeInfoKind::Reference)
    {
        auto fromTypeRef = TypeManager::concreteReference(fromType);
        if (fromTypeRef == toType)
            return true;
        if (fromTypeRef->isSame(toType, ISSAME_CAST))
            return true;
    }

    if (fromType->isSame(toType, ISSAME_CAST))
        return true;

    if (fromType->flags & TYPEINFO_AUTO_CAST)
        castFlags |= CASTFLAG_EXPLICIT;

    // Always match against a generic
    if (toType->kind == TypeInfoKind::Generic)
        return true;

    switch (toType->kind)
    {
    // Cast to reference
    case TypeInfoKind::Reference:
        return castToReference(context, toType, fromType, fromNode, castFlags);

    // Cast to pointer
    case TypeInfoKind::Pointer:
        return castToPointer(context, toType, fromType, fromNode, castFlags);

    // Cast to native type
    case TypeInfoKind::Native:
        return castToNative(context, toType, fromType, toNode, fromNode, castFlags);

    // Cast to array
    case TypeInfoKind::Array:
        return castToArray(context, toType, fromType, fromNode, castFlags);

    // Cast to slice
    case TypeInfoKind::Slice:
        return castToSlice(context, toType, fromType, fromNode, castFlags);

    // Cast to interface
    case TypeInfoKind::Interface:
        return castToInterface(context, toType, fromType, fromNode, castFlags);
    }

    return castError(context, toType, fromType, fromNode, castFlags);
}
