#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "ErrorIds.h"

bool SemanticJob::resolveUnaryOpMinus(SemanticContext* context, AstNode* op)
{
    auto typeInfo = TypeManager::concreteReferenceType(op->typeInfo);

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
        return context->report({op, op->token, format(Msg0827, typeInfo->getDisplayName().c_str())});
    default:
        return context->report({op, op->token, format(Msg0828, typeInfo->getDisplayName().c_str())});
    }

    if (op->flags & AST_VALUE_COMPUTED && !(op->doneFlags & AST_DONE_NEG_EATEN))
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
            if (op->computedValue.reg.s8 <= INT8_MIN)
                return context->report({op, format(Msg0829, op->computedValue.reg.s8, -op->computedValue.reg.s8)});
            op->computedValue.reg.s64 = -op->computedValue.reg.s8;
            break;
        case NativeTypeKind::S16:
            if (op->computedValue.reg.s16 <= INT16_MIN)
                return context->report({op, format(Msg0830, op->computedValue.reg.s16, -op->computedValue.reg.s16)});
            op->computedValue.reg.s64 = -op->computedValue.reg.s16;
            break;
        case NativeTypeKind::S32:
            if (op->computedValue.reg.s32 <= INT32_MIN)
                return context->report({op, format(Msg0831, op->computedValue.reg.s32, -op->computedValue.reg.s32)});
            op->computedValue.reg.s64 = -op->computedValue.reg.s32;
            if (typeInfo->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                int32_t newValue = -CastTypeInfo<TypeInfoNative>(typeInfo, typeInfo->kind)->valueInteger;
                op->typeInfo     = TypeManager::makeUntypedType(typeInfo, *(uint32_t*) &newValue);
            }
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            if (op->computedValue.reg.s64 <= INT64_MIN)
                return context->report({op, format(Msg0832, op->computedValue.reg.s64, -op->computedValue.reg.s64)});
            op->computedValue.reg.s64 = -op->computedValue.reg.s64;
            break;

        case NativeTypeKind::F32:
            op->computedValue.reg.f32 = -op->computedValue.reg.f32;
            if (typeInfo->flags & TYPEINFO_UNTYPED_FLOAT)
            {
                float newValue = -CastTypeInfo<TypeInfoNative>(typeInfo, typeInfo->kind)->valueFloat;
                op->typeInfo   = TypeManager::makeUntypedType(typeInfo, *(uint32_t*) &newValue);
            }

            break;
        case NativeTypeKind::F64:
            op->computedValue.reg.f64 = -op->computedValue.reg.f64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveUnaryOpExclam(SemanticContext* context, AstNode* op)
{
    auto typeInfo = TypeManager::concreteReferenceType(op->typeInfo, CONCRETE_ALIAS);
    if (typeInfo->kind == TypeInfoKind::Lambda)
        return true;

    typeInfo = TypeManager::concreteType(typeInfo);
    if (typeInfo->kind == TypeInfoKind::Pointer)
        return true;

    SWAG_CHECK(checkTypeIsNative(context, op, typeInfo));
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, op, CASTFLAG_AUTO_BOOL));
    if (op->flags & AST_VALUE_COMPUTED)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            op->computedValue.reg.b = !op->computedValue.reg.b;
            break;
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            op->computedValue.reg.b = op->computedValue.reg.u8 ? false : true;
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            op->computedValue.reg.b = op->computedValue.reg.u16 ? false : true;
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            op->computedValue.reg.b = op->computedValue.reg.u32 ? false : true;
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            op->computedValue.reg.b = op->computedValue.reg.u64 ? false : true;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveUnaryOpInvert(SemanticContext* context, AstNode* op)
{
    auto typeInfo = TypeManager::concreteReferenceType(op->typeInfo);

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
        return context->report({op, op->token, format(Msg0833, typeInfo->getDisplayName().c_str())});
    }

    if (op->flags & AST_VALUE_COMPUTED)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            op->computedValue.reg.u64 = op->computedValue.reg.u8;
            op->computedValue.reg.u8  = ~op->computedValue.reg.u8;
            break;

        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            op->computedValue.reg.u64 = op->computedValue.reg.u16;
            op->computedValue.reg.u16 = ~op->computedValue.reg.u16;
            break;

        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
            op->computedValue.reg.u64 = op->computedValue.reg.u32;
            op->computedValue.reg.u32 = ~op->computedValue.reg.u32;
            break;

        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            op->computedValue.reg.u64 = ~op->computedValue.reg.u64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveUnaryOp(SemanticContext* context)
{
    auto node = context->node;
    auto op   = node->childs[0];

    node->typeInfo    = op->typeInfo;
    node->byteCodeFct = ByteCodeGenJob::emitUnaryOp;

    node->inheritOrFlag(op, AST_CONST_EXPR | AST_SIDE_EFFECTS);
    SWAG_CHECK(checkIsConcrete(context, op));
    node->flags |= AST_R_VALUE;

    // Special case for enum : nothing is possible, except for flags
    auto typeInfo = TypeManager::concreteReferenceType(op->typeInfo, CONCRETE_ALIAS);
    if (typeInfo->kind == TypeInfoKind::Enum)
    {
        if (!(typeInfo->flags & TYPEINFO_ENUM_FLAGS))
            return notAllowed(context, node, typeInfo);
    }

    typeInfo = TypeManager::concreteReferenceType(op->typeInfo);

    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        switch (node->token.id)
        {
        case TokenId::SymExclam:
            SWAG_CHECK(resolveUserOp(context, "opUnary", "!", nullptr, op, nullptr, false));
            break;
        case TokenId::SymMinus:
            SWAG_CHECK(resolveUserOp(context, "opUnary", "-", nullptr, op, nullptr, false));
            break;
        case TokenId::SymTilde:
            SWAG_CHECK(resolveUserOp(context, "opUnary", "~", nullptr, op, nullptr, false));
            break;
        }

        node->typeInfo = typeInfo;
        node->flags |= AST_TRANSIENT;
        return true;
    }

    switch (node->token.id)
    {
    case TokenId::SymExclam:
        SWAG_CHECK(resolveUnaryOpExclam(context, op));
        break;
    case TokenId::SymMinus:
        SWAG_CHECK(checkTypeIsNative(context, node, typeInfo));
        SWAG_CHECK(resolveUnaryOpMinus(context, op));
        break;
    case TokenId::SymTilde:
        SWAG_CHECK(checkTypeIsNative(context, node, typeInfo));
        SWAG_CHECK(resolveUnaryOpInvert(context, op));
        break;
    }

    node->typeInfo       = op->typeInfo;
    node->castedTypeInfo = op->castedTypeInfo;
    node->inheritComputedValue(op);
    return true;
}
