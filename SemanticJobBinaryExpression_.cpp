#include "pch.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "Concat.h"
#include "Ast.h"

bool SemanticJob::resolveBinaryOpPlus(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto sourceFile    = context->sourceFile;
    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "+", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    // Pointer arithmetic
    if (leftTypeInfo->kind == TypeInfoKind::Pointer)
    {
        node->typeInfo = leftTypeInfo;
        SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, context->report({node, format("operator '+' not allowed on a pointer with type '%s'", leftTypeInfo->name.c_str())}));
        switch (rightTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            break;
        default:
            return context->report({node, format("operator '+' not allowed on a pointer with type '%s'", leftTypeInfo->name.c_str())});
        }

        return true;
    }

    SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, context->report({node, format("operator '+' not allowed on type '%s'", rightTypeInfo->name.c_str())}));
    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));
    leftTypeInfo = TypeManager::concreteType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
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
    case NativeTypeKind::F64:
        break;
    default:
        return context->report({node, format("operator '+' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    node->typeInfo = leftTypeInfo;
    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        {
            auto result = left->computedValue.reg.s8 + right->computedValue.reg.s8;
            if (result < INT8_MIN || result > INT8_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's8'"});
            node->computedValue.reg.s8 = left->computedValue.reg.s8 + right->computedValue.reg.s8;
        }
        break;
        case NativeTypeKind::S16:
        {
            auto result = left->computedValue.reg.s16 + right->computedValue.reg.s16;
            if (result < INT16_MIN || result > INT16_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's16'"});
            node->computedValue.reg.s16 = left->computedValue.reg.s16 + right->computedValue.reg.s16;
        }
        break;
        case NativeTypeKind::S32:
        {
            auto result = left->computedValue.reg.s32 + right->computedValue.reg.s32;
            if (result < INT32_MIN || result > INT32_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's32'"});
            node->computedValue.reg.s32 = left->computedValue.reg.s32 + right->computedValue.reg.s32;
        }
        break;
        case NativeTypeKind::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 + right->computedValue.reg.s64;
            break;
        case NativeTypeKind::U8:
            if (left->computedValue.reg.u64 + right->computedValue.reg.u64 > UINT8_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u8'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 + right->computedValue.reg.u64;
            break;
        case NativeTypeKind::U16:
            if (left->computedValue.reg.u64 + right->computedValue.reg.u64 > UINT16_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u16'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 + right->computedValue.reg.u64;
            break;
        case NativeTypeKind::U32:
            if (left->computedValue.reg.u64 + right->computedValue.reg.u64 > UINT32_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u32'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 + right->computedValue.reg.u64;
            break;
        case NativeTypeKind::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 + right->computedValue.reg.u64;
            break;
        case NativeTypeKind::F32:
            node->computedValue.reg.f32 = left->computedValue.reg.f32 + right->computedValue.reg.f32;
            break;
        case NativeTypeKind::F64:
            node->computedValue.reg.f64 = left->computedValue.reg.f64 + right->computedValue.reg.f64;
            break;
        default:
            return internalError(context, "resolveBinaryOpPlus, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveBinaryOpMinus(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto sourceFile    = context->sourceFile;
    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "-", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    // Pointer arithmetic
    if (leftTypeInfo->kind == TypeInfoKind::Pointer)
    {
        node->typeInfo = leftTypeInfo;
        SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, context->report({node, format("operator '-' not allowed on a pointer with type '%s'", leftTypeInfo->name.c_str())}));
        switch (rightTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            break;
        default:
            return context->report({node, format("operator '-' not allowed on a pointer with type '%s'", leftTypeInfo->name.c_str())});
        }

        return true;
    }

    SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, context->report({node, format("operator '-' not allowed on type '%s'", rightTypeInfo->name.c_str())}));
    SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));
    leftTypeInfo = TypeManager::concreteType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
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
    case NativeTypeKind::F64:
        break;
    default:
        return context->report({node, format("operator '-' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    node->typeInfo = leftTypeInfo;
    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        {
            auto result = left->computedValue.reg.s8 - right->computedValue.reg.s8;
            if (result < INT8_MIN || result > INT8_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's8'"});
            node->computedValue.reg.s8 = left->computedValue.reg.s8 - right->computedValue.reg.s8;
        }
        break;
        case NativeTypeKind::S16:
        {
            auto result = left->computedValue.reg.s16 - right->computedValue.reg.s16;
            if (result < INT16_MIN || result > INT16_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's16'"});
            node->computedValue.reg.s16 = left->computedValue.reg.s16 - right->computedValue.reg.s16;
        }
        break;
        case NativeTypeKind::S32:
        {
            auto result = left->computedValue.reg.s32 - right->computedValue.reg.s32;
            if (result < INT32_MIN || result > INT32_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's32'"});
            node->computedValue.reg.s32 = left->computedValue.reg.s32 - right->computedValue.reg.s32;
        }
        break;
        case NativeTypeKind::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 - right->computedValue.reg.s64;
            break;
        case NativeTypeKind::U8:
            if (left->computedValue.reg.u64 - right->computedValue.reg.u64 > UINT8_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u8'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 - right->computedValue.reg.u64;
            break;
        case NativeTypeKind::U16:
            if (left->computedValue.reg.u64 - right->computedValue.reg.u64 > UINT16_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u16'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 - right->computedValue.reg.u64;
            break;
        case NativeTypeKind::U32:
            if (left->computedValue.reg.u64 - right->computedValue.reg.u64 > UINT32_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u32'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 - right->computedValue.reg.u64;
            break;
        case NativeTypeKind::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 - right->computedValue.reg.u64;
            break;
        case NativeTypeKind::F32:
            node->computedValue.reg.f32 = left->computedValue.reg.f32 - right->computedValue.reg.f32;
            break;
        case NativeTypeKind::F64:
            node->computedValue.reg.f64 = left->computedValue.reg.f64 - right->computedValue.reg.f64;
            break;
        default:
            return internalError(context, "resolveBinaryOpMinus, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveBinaryOpMul(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto sourceFile   = context->sourceFile;
    auto leftTypeInfo = TypeManager::concreteType(left->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "*", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    switch (leftTypeInfo->nativeType)
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
    case NativeTypeKind::F64:
        break;
    default:
        return context->report({node, format("operator '*' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        {
            auto result = left->computedValue.reg.s8 * right->computedValue.reg.s8;
            if (result < INT8_MIN || result > INT8_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's8'"});
            node->computedValue.reg.s8 = (int8_t) result;
        }
        break;
        case NativeTypeKind::S16:
        {
            auto result = left->computedValue.reg.s16 * right->computedValue.reg.s16;
            if (result < INT16_MIN || result > INT16_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's16'"});
            node->computedValue.reg.s16 = (int16_t) result;
        }
        break;
        case NativeTypeKind::S32:
        {
            auto result = left->computedValue.reg.s32 * right->computedValue.reg.s32;
            if (result < INT32_MIN || result > INT32_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's32'"});
            node->computedValue.reg.s32 = result;
        }
        break;
        case NativeTypeKind::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 * right->computedValue.reg.s64;
            break;
        case NativeTypeKind::U8:
            if (left->computedValue.reg.u64 * right->computedValue.reg.u64 > UINT8_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u8'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 * right->computedValue.reg.u64;
            break;
        case NativeTypeKind::U16:
            if (left->computedValue.reg.u64 * right->computedValue.reg.u64 > UINT16_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u16'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 * right->computedValue.reg.u64;
            break;
        case NativeTypeKind::U32:
            if (left->computedValue.reg.u64 * right->computedValue.reg.u64 > UINT32_MAX)
                return context->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u32'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 * right->computedValue.reg.u64;
            break;
        case NativeTypeKind::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 * right->computedValue.reg.u64;
            break;
        case NativeTypeKind::F32:
            node->computedValue.reg.f32 = left->computedValue.reg.f32 * right->computedValue.reg.f32;
            break;
        case NativeTypeKind::F64:
            node->computedValue.reg.f64 = left->computedValue.reg.f64 * right->computedValue.reg.f64;
            break;
        default:
            return internalError(context, "resolveBinaryOpMul, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveBinaryOpDiv(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = TypeManager::concreteType(left->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "/", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U8:
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
        return context->report({node, "operator '/' not allowed on integers"});
    case NativeTypeKind::F32:
    case NativeTypeKind::F64:
        break;
    default:
        return context->report({node, format("operator '/' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::F32:
            if (right->computedValue.reg.f32 == 0)
                return context->report({right, right->token, "division by zero"});
            node->computedValue.reg.f32 = left->computedValue.reg.f32 / right->computedValue.reg.f32;
            break;
        case NativeTypeKind::F64:
            if (right->computedValue.reg.f64 == 0)
                return context->report({right, right->token, "division by zero"});
            node->computedValue.reg.f64 = left->computedValue.reg.f64 / right->computedValue.reg.f64;
            break;
        default:
            return internalError(context, "resolveBinaryOpDiv, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveBinaryOpModulo(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = TypeManager::concreteType(left->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "%", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U8:
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
        break;
    default:
        return context->report({node, format("operator '%' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
            if (right->computedValue.reg.s8 == 0)
                return context->report({right, right->token, "modulo by zero"});
            node->computedValue.reg.s8 = left->computedValue.reg.s8 % right->computedValue.reg.s8;
            break;
        case NativeTypeKind::S16:
            if (right->computedValue.reg.s16 == 0)
                return context->report({right, right->token, "modulo by zero"});
            node->computedValue.reg.s16 = left->computedValue.reg.s16 % right->computedValue.reg.s16;
            break;
        case NativeTypeKind::S32:
            if (right->computedValue.reg.s32 == 0)
                return context->report({right, right->token, "modulo by zero"});
            node->computedValue.reg.s32 = left->computedValue.reg.s32 % right->computedValue.reg.s32;
            break;
        case NativeTypeKind::S64:
            if (right->computedValue.reg.s64 == 0)
                return context->report({right, right->token, "modulo by zero"});
            node->computedValue.reg.s64 = left->computedValue.reg.s64 % right->computedValue.reg.s64;
            break;
        case NativeTypeKind::U8:
            if (right->computedValue.reg.u8 == 0)
                return context->report({right, right->token, "modulo by zero"});
            node->computedValue.reg.u8 = left->computedValue.reg.u8 % right->computedValue.reg.u8;
            break;
        case NativeTypeKind::U16:
            if (right->computedValue.reg.u16 == 0)
                return context->report({right, right->token, "modulo by zero"});
            node->computedValue.reg.u16 = left->computedValue.reg.u16 % right->computedValue.reg.u16;
            break;
        case NativeTypeKind::U32:
            if (right->computedValue.reg.u32 == 0)
                return context->report({right, right->token, "modulo by zero"});
            node->computedValue.reg.u32 = left->computedValue.reg.u32 % right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U64:
            if (right->computedValue.reg.u64 == 0)
                return context->report({right, right->token, "modulo by zero"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 % right->computedValue.reg.u64;
            break;
        default:
            return internalError(context, "resolveBinaryOpModulo, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveBitmaskOr(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = TypeManager::concreteType(left->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "|", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
        break;
    default:
        return context->report({node, format("operator '|' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
            node->computedValue.reg.s8 = left->computedValue.reg.s8 | right->computedValue.reg.s8;
            break;
        case NativeTypeKind::S16:
            node->computedValue.reg.s16 = left->computedValue.reg.s16 | right->computedValue.reg.s16;
            break;
        case NativeTypeKind::S32:
            node->computedValue.reg.s32 = left->computedValue.reg.s32 | right->computedValue.reg.s32;
            break;
        case NativeTypeKind::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 | right->computedValue.reg.s64;
            break;
        case NativeTypeKind::U8:
            node->computedValue.reg.u8 = left->computedValue.reg.u8 | right->computedValue.reg.u8;
            break;
        case NativeTypeKind::U16:
            node->computedValue.reg.u16 = left->computedValue.reg.u16 | right->computedValue.reg.u16;
            break;
        case NativeTypeKind::U32:
            node->computedValue.reg.u32 = left->computedValue.reg.u32 | right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 | right->computedValue.reg.u64;
            break;
        default:
            return internalError(context, "resolveBitmaskOr, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveBitmaskAnd(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = TypeManager::concreteType(left->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "&", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
        break;
    default:
        return context->report({node, format("operator '&' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
            node->computedValue.reg.s8 = left->computedValue.reg.s8 & right->computedValue.reg.s8;
            break;
        case NativeTypeKind::S16:
            node->computedValue.reg.s16 = left->computedValue.reg.s16 & right->computedValue.reg.s16;
            break;
        case NativeTypeKind::S32:
            node->computedValue.reg.s32 = left->computedValue.reg.s32 & right->computedValue.reg.s32;
            break;
        case NativeTypeKind::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 & right->computedValue.reg.s64;
            break;
        case NativeTypeKind::U8:
            node->computedValue.reg.u8 = left->computedValue.reg.u8 & right->computedValue.reg.u8;
            break;
        case NativeTypeKind::U16:
            node->computedValue.reg.u16 = left->computedValue.reg.u16 & right->computedValue.reg.u16;
            break;
        case NativeTypeKind::U32:
            node->computedValue.reg.u32 = left->computedValue.reg.u32 & right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 & right->computedValue.reg.u64;
            break;
        default:
            return internalError(context, "resolveBitmaskAnd, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveShiftLeft(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "<<", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
        break;
    default:
        return context->report({left, format("operator '<<' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    switch (rightTypeInfo->nativeType)
    {
    case NativeTypeKind::U32:
        break;
    default:
        return context->report({right, format("shift operand must be 'u32' and not '%s'", rightTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            node->computedValue.reg.s32 = left->computedValue.reg.s32 << right->computedValue.reg.u32;
            break;
        case NativeTypeKind::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 << right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U32:
            node->computedValue.reg.u32 = left->computedValue.reg.u32 << right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 << right->computedValue.reg.u32;
            break;
        default:
            return internalError(context, "resolveShiftLeft, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveShiftRight(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node          = context->node;
    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", ">>", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
        break;
    default:
        return context->report({left, format("operator '>>' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    switch (rightTypeInfo->nativeType)
    {
    case NativeTypeKind::U32:
        break;
    default:
        return context->report({right, format("shift operand must be 'u32' and not '%s'", rightTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            node->computedValue.reg.s32 = left->computedValue.reg.s32 >> right->computedValue.reg.u32;
            break;
        case NativeTypeKind::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 >> right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U32:
            node->computedValue.reg.u32 = left->computedValue.reg.u32 >> right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 >> right->computedValue.reg.u32;
            break;
        default:
            return internalError(context, "resolveShiftRight, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveTilde(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node = context->node;
    SWAG_VERIFY(left->flags & AST_VALUE_COMPUTED, context->report({left, "expression cannot be evaluated at compile time"}));
    SWAG_VERIFY(right->flags & AST_VALUE_COMPUTED, context->report({right, "expression cannot be evaluated at compile time"}));
    left->computedValue.text  = Ast::literalToString(left->typeInfo, left->computedValue.text, left->computedValue.reg);
    right->computedValue.text = Ast::literalToString(right->typeInfo, right->computedValue.text, right->computedValue.reg);
    node->computedValue.text  = left->computedValue.text + right->computedValue.text;
    node->typeInfo            = g_TypeMgr.typeInfoString;
    node->flags |= AST_VALUE_COMPUTED | AST_CONST_EXPR;
    return true;
}

bool SemanticJob::resolveXor(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto leftTypeInfo = TypeManager::concreteType(left->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opBinary", "^", nullptr, left, right));
        node->typeInfo = leftTypeInfo;
        return true;
    }

    switch (leftTypeInfo->nativeType)
    {
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
        break;
    default:
        return context->report({context->node, format("operator '^' not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            node->computedValue.reg.s32 = left->computedValue.reg.s32 ^ right->computedValue.reg.s32;
            break;
        case NativeTypeKind::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 ^ right->computedValue.reg.s64;
            break;
        case NativeTypeKind::U32:
            node->computedValue.reg.u32 = left->computedValue.reg.u32 ^ right->computedValue.reg.u32;
            break;
        case NativeTypeKind::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 ^ right->computedValue.reg.u64;
            break;
        default:
            return internalError(context, "resolveXor, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveFactorExpression(SemanticContext* context)
{
    auto node  = context->node;
    auto left  = node->childs[0];
    auto right = node->childs[1];

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcrete(context, right));

    // Special case for enum : nothing is possible, except for flags
    bool isEnumFlags   = false;
    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo, CONCRETE_ALIAS);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo, CONCRETE_ALIAS);
    if (leftTypeInfo->kind == TypeInfoKind::Enum || rightTypeInfo->kind == TypeInfoKind::Enum)
    {
        SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));

        bool ok = true;
        if (!(leftTypeInfo->flags & TYPEINFO_ENUM_FLAGS) || !(rightTypeInfo->flags & TYPEINFO_ENUM_FLAGS))
            ok = false;
        if (node->token.id != TokenId::SymVertical &&
            node->token.id != TokenId::SymAmpersand &&
            node->token.id != TokenId::SymCircumflex)
            ok = false;
        if (!ok)
            return notAllowed(context, node, leftTypeInfo);
        isEnumFlags = true;
    }

    leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    // Keep it generic if it's generic on one side
    if (leftTypeInfo->kind == TypeInfoKind::Generic)
    {
        node->typeInfo = leftTypeInfo;
        return true;
    }
    if (rightTypeInfo->kind == TypeInfoKind::Generic)
    {
        node->typeInfo = rightTypeInfo;
        return true;
    }

    node->byteCodeFct = ByteCodeGenJob::emitBinaryOp;
    node->inheritAndFlag(AST_CONST_EXPR);
    node->inheritAndFlag(AST_R_VALUE);
    TypeManager::promote(left, right);

    // Must do move and not copy
    if (leftTypeInfo->kind == TypeInfoKind::Struct)
        node->flags |= AST_TRANSIENT;

    switch (node->token.id)
    {
    case TokenId::SymPlus:
        SWAG_CHECK(resolveBinaryOpPlus(context, left, right));
        break;
    case TokenId::SymMinus:
        SWAG_CHECK(resolveBinaryOpMinus(context, left, right));
        break;
    case TokenId::SymAsterisk:
        SWAG_CHECK(checkTypeIsNative(context, left, leftTypeInfo));
        SWAG_CHECK(checkTypeIsNative(context, right, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));
        node->typeInfo = TypeManager::concreteType(left->typeInfo);
        SWAG_CHECK(resolveBinaryOpMul(context, left, right));
        break;
    case TokenId::SymSlash:
        SWAG_CHECK(checkTypeIsNative(context, left, leftTypeInfo));
        SWAG_CHECK(checkTypeIsNative(context, right, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));
        node->typeInfo = TypeManager::concreteType(left->typeInfo);
        SWAG_CHECK(resolveBinaryOpDiv(context, left, right));
        break;
    case TokenId::SymPercent:
        SWAG_CHECK(checkTypeIsNative(context, left, leftTypeInfo));
        SWAG_CHECK(checkTypeIsNative(context, right, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));
        node->typeInfo = TypeManager::concreteType(left->typeInfo);
        SWAG_CHECK(resolveBinaryOpModulo(context, left, right));
        break;

    case TokenId::SymVertical:
        SWAG_CHECK(checkTypeIsNative(context, left, leftTypeInfo));
        SWAG_CHECK(checkTypeIsNative(context, right, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));
        node->typeInfo = isEnumFlags ? left->typeInfo : TypeManager::concreteType(left->typeInfo);
        SWAG_CHECK(resolveBitmaskOr(context, left, right));
        break;
    case TokenId::SymAmpersand:
        SWAG_CHECK(checkTypeIsNative(context, left, leftTypeInfo));
        SWAG_CHECK(checkTypeIsNative(context, right, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));
        node->typeInfo = isEnumFlags ? left->typeInfo : TypeManager::concreteType(left->typeInfo);
        SWAG_CHECK(resolveBitmaskAnd(context, left, right));
        break;
    case TokenId::SymCircumflex:
        SWAG_CHECK(checkTypeIsNative(context, left, leftTypeInfo));
        SWAG_CHECK(checkTypeIsNative(context, right, rightTypeInfo));
        SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));
        node->typeInfo = isEnumFlags ? left->typeInfo : TypeManager::concreteType(left->typeInfo);
        SWAG_CHECK(resolveXor(context, left, right));
        break;
    case TokenId::SymTilde:
        SWAG_CHECK(resolveTilde(context, left, right));
        break;
    default:
        return internalError(context, "resolveFactorExpression, token not supported");
    }

    return true;
}

bool SemanticJob::resolveShiftExpression(SemanticContext* context)
{
    auto node  = context->node;
    auto left  = node->childs[0];
    auto right = node->childs[1];

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcrete(context, right));

    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    // Keep it generic if it's generic on one side
    if (leftTypeInfo->kind == TypeInfoKind::Generic)
    {
        node->typeInfo = leftTypeInfo;
        return true;
    }
    if (rightTypeInfo->kind == TypeInfoKind::Generic)
    {
        node->typeInfo = rightTypeInfo;
        return true;
    }

    SWAG_CHECK(checkTypeIsNative(context, left, leftTypeInfo));
    SWAG_CHECK(checkTypeIsNative(context, right, rightTypeInfo));

    TypeManager::promote(left, right);
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoU32, nullptr, right));
    node->typeInfo = left->typeInfo;

    node->byteCodeFct = ByteCodeGenJob::emitBinaryOp;
    node->inheritAndFlag(AST_CONST_EXPR);
    node->inheritAndFlag(AST_R_VALUE);

    switch (node->token.id)
    {
    case TokenId::SymLowerLower:
        SWAG_CHECK(resolveShiftLeft(context, left, right));
        break;
    case TokenId::SymGreaterGreater:
        SWAG_CHECK(resolveShiftRight(context, left, right));
        break;
    default:
        return internalError(context, "resolveShiftExpression, token not supported");
    }

    return true;
}

bool SemanticJob::resolveBoolExpression(SemanticContext* context)
{
    auto node  = context->node;
    auto left  = node->childs[0];
    auto right = node->childs[1];

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcrete(context, right));

    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    // Keep it generic if it's generic on one side
    if (leftTypeInfo->kind == TypeInfoKind::Generic)
    {
        node->typeInfo = leftTypeInfo;
        return true;
    }
    if (rightTypeInfo->kind == TypeInfoKind::Generic)
    {
        node->typeInfo = rightTypeInfo;
        return true;
    }

    node->typeInfo = g_TypeMgr.typeInfoBool;
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, left, CASTFLAG_AUTO_BOOL));
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, right, CASTFLAG_AUTO_BOOL));

    node->byteCodeFct = ByteCodeGenJob::emitBinaryOp;
    node->inheritAndFlag(AST_CONST_EXPR);
    node->inheritAndFlag(AST_R_VALUE);

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;
        switch (node->token.id)
        {
        case TokenId::SymAmpersandAmpersand:
            node->computedValue.reg.b = left->computedValue.reg.b && right->computedValue.reg.b;
            break;
        case TokenId::SymVerticalVertical:
            node->computedValue.reg.b = left->computedValue.reg.b || right->computedValue.reg.b;
            break;
        default:
            return internalError(context, "resolveBoolExpression, token not supported");
        }
    }

    return true;
}
