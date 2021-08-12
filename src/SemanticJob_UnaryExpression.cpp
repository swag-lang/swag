#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"

bool SemanticJob::resolveUnaryOpMinus(SemanticContext* context, AstNode* child)
{
    auto typeInfo = TypeManager::concreteReferenceType(child->typeInfo);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::F32:
    case NativeTypeKind::F64:
    case NativeTypeKind::Int:
        break;
    case NativeTypeKind::U8:
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        return context->report({child, child->token, Utf8::format(Msg0827, typeInfo->getDisplayName().c_str())});
    default:
        return context->report({child, child->token, Utf8::format(Msg0828, typeInfo->getDisplayName().c_str())});
    }

    if (child->flags & AST_VALUE_COMPUTED && !(child->doneFlags & AST_DONE_NEG_EATEN))
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
            if (child->computedValue->reg.s8 <= INT8_MIN)
                return context->report({child, Utf8::format(Msg0829, child->computedValue->reg.s8, -child->computedValue->reg.s8)});
            child->computedValue->reg.s64 = -child->computedValue->reg.s8;
            break;
        case NativeTypeKind::S16:
            if (child->computedValue->reg.s16 <= INT16_MIN)
                return context->report({child, Utf8::format(Msg0830, child->computedValue->reg.s16, -child->computedValue->reg.s16)});
            child->computedValue->reg.s64 = -child->computedValue->reg.s16;
            break;
        case NativeTypeKind::S32:
            if (child->computedValue->reg.s32 <= INT32_MIN)
                return context->report({child, Utf8::format(Msg0831, child->computedValue->reg.s32, -child->computedValue->reg.s32)});
            child->computedValue->reg.s64 = -child->computedValue->reg.s32;
            if (typeInfo->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                int32_t newValue = -CastTypeInfo<TypeInfoNative>(typeInfo, typeInfo->kind)->valueInteger;
                child->typeInfo  = TypeManager::makeUntypedType(typeInfo, *(uint32_t*) &newValue);
            }
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (child->computedValue->reg.s64 <= INT64_MIN)
                return context->report({child, Utf8::format(Msg0832, child->computedValue->reg.s64, -child->computedValue->reg.s64)});
            child->computedValue->reg.s64 = -child->computedValue->reg.s64;
            break;

        case NativeTypeKind::F32:
            child->computedValue->reg.f32 = -child->computedValue->reg.f32;
            if (typeInfo->flags & TYPEINFO_UNTYPED_FLOAT)
            {
                float newValue  = -CastTypeInfo<TypeInfoNative>(typeInfo, typeInfo->kind)->valueFloat;
                child->typeInfo = TypeManager::makeUntypedType(typeInfo, *(uint32_t*) &newValue);
            }

            break;
        case NativeTypeKind::F64:
            child->computedValue->reg.f64 = -child->computedValue->reg.f64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveUnaryOpExclam(SemanticContext* context, AstNode* child)
{
    auto typeInfo = TypeManager::concreteReferenceType(child->typeInfo, CONCRETE_ALIAS);
    if (typeInfo->kind == TypeInfoKind::Lambda)
        return true;

    typeInfo = TypeManager::concreteType(typeInfo);
    if (typeInfo->kind == TypeInfoKind::Pointer || typeInfo->kind == TypeInfoKind::Interface || typeInfo->kind == TypeInfoKind::Slice)
        return true;

    SWAG_CHECK(checkTypeIsNative(context, context->node, typeInfo));
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, child, CASTFLAG_AUTO_BOOL));
    if (child->flags & AST_VALUE_COMPUTED)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            child->computedValue->reg.b = !child->computedValue->reg.b;
            break;
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            child->computedValue->reg.b = child->computedValue->reg.u8 ? false : true;
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            child->computedValue->reg.b = child->computedValue->reg.u16 ? false : true;
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            child->computedValue->reg.b = child->computedValue->reg.u32 ? false : true;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            child->computedValue->reg.b = child->computedValue->reg.u64 ? false : true;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveUnaryOpInvert(SemanticContext* context, AstNode* child)
{
    auto typeInfo = TypeManager::concreteReferenceType(child->typeInfo);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U8:
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    default:
        return context->report({child, child->token, Utf8::format(Msg0833, typeInfo->getDisplayName().c_str())});
    }

    if (child->flags & AST_VALUE_COMPUTED)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            child->computedValue->reg.u64 = child->computedValue->reg.u8;
            child->computedValue->reg.u8  = ~child->computedValue->reg.u8;
            break;

        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            child->computedValue->reg.u64 = child->computedValue->reg.u16;
            child->computedValue->reg.u16 = ~child->computedValue->reg.u16;
            break;

        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
            child->computedValue->reg.u64 = child->computedValue->reg.u32;
            child->computedValue->reg.u32 = ~child->computedValue->reg.u32;
            break;

        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            child->computedValue->reg.u64 = ~child->computedValue->reg.u64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveUnaryOp(SemanticContext* context)
{
    auto op    = context->node;
    auto child = op->childs[0];

    op->typeInfo    = child->typeInfo;
    op->byteCodeFct = ByteCodeGenJob::emitUnaryOp;

    op->inheritOrFlag(child, AST_CONST_EXPR | AST_SIDE_EFFECTS);
    SWAG_CHECK(checkIsConcrete(context, child));
    op->flags |= AST_R_VALUE;

    // Special case for enum : nothing is possible, except for flags
    auto typeInfo = TypeManager::concreteReferenceType(child->typeInfo, CONCRETE_ALIAS);
    if (typeInfo->kind == TypeInfoKind::Enum)
    {
        if (!(typeInfo->flags & TYPEINFO_ENUM_FLAGS))
            return notAllowed(context, op, typeInfo);
    }

    typeInfo = TypeManager::concreteReferenceType(child->typeInfo);

    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        switch (op->token.id)
        {
        case TokenId::SymExclam:
            SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opUnary, "!", nullptr, child, nullptr, false));
            break;
        case TokenId::SymMinus:
            SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opUnary, "-", nullptr, child, nullptr, false));
            break;
        case TokenId::SymTilde:
            SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opUnary, "~", nullptr, child, nullptr, false));
            break;
        }

        op->typeInfo = typeInfo;
        op->flags |= AST_TRANSIENT;
        return true;
    }

    switch (op->token.id)
    {
    case TokenId::SymExclam:
        SWAG_CHECK(resolveUnaryOpExclam(context, child));
        break;
    case TokenId::SymMinus:
        SWAG_CHECK(checkTypeIsNative(context, op, typeInfo));
        SWAG_CHECK(resolveUnaryOpMinus(context, child));
        break;
    case TokenId::SymTilde:
        SWAG_CHECK(checkTypeIsNative(context, op, typeInfo));
        SWAG_CHECK(resolveUnaryOpInvert(context, child));
        break;
    }

    op->typeInfo       = child->typeInfo;
    op->castedTypeInfo = child->castedTypeInfo;
    op->inheritComputedValue(child);
    return true;
}
