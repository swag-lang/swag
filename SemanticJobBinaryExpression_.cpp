#include "pch.h"
#include "SemanticJob.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"

bool SemanticJob::resolveBinaryOpPlus(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto typeInfo   = TypeManager::concreteType(left->typeInfo);

    switch (typeInfo->nativeType)
    {
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    case NativeType::F32:
    case NativeType::F64:
        break;
    default:
        return sourceFile->report({sourceFile, node, format("addition not allowed on type '%s'", typeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (typeInfo->nativeType)
        {
        case NativeType::S8:
        {
            auto result = left->computedValue.reg.s8 + right->computedValue.reg.s8;
            if (result < INT8_MIN || result > INT8_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's8'"});
            node->computedValue.reg.s8 = left->computedValue.reg.s8 + right->computedValue.reg.s8;
        }
        break;
        case NativeType::S16:
        {
            auto result = left->computedValue.reg.s16 + right->computedValue.reg.s16;
            if (result < INT16_MIN || result > INT16_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's16'"});
            node->computedValue.reg.s16 = left->computedValue.reg.s16 + right->computedValue.reg.s16;
        }
        break;
        case NativeType::S32:
        {
            auto result = left->computedValue.reg.s32 + right->computedValue.reg.s32;
            if (result < INT32_MIN || result > INT32_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's32'"});
            node->computedValue.reg.s32 = left->computedValue.reg.s32 + right->computedValue.reg.s32;
        }
        break;
        case NativeType::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 + right->computedValue.reg.s64;
            break;
        case NativeType::U8:
            if (left->computedValue.reg.u64 + right->computedValue.reg.u64 > UINT8_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u8'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 + right->computedValue.reg.u64;
            break;
        case NativeType::U16:
            if (left->computedValue.reg.u64 + right->computedValue.reg.u64 > UINT16_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u16'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 + right->computedValue.reg.u64;
            break;
        case NativeType::U32:
            if (left->computedValue.reg.u64 + right->computedValue.reg.u64 > UINT32_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u32'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 + right->computedValue.reg.u64;
            break;
        case NativeType::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 + right->computedValue.reg.u64;
            break;
        case NativeType::F32:
            node->computedValue.reg.f32 = left->computedValue.reg.f32 + right->computedValue.reg.f32;
            break;
        case NativeType::F64:
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
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto typeInfo   = TypeManager::concreteType(left->typeInfo);

    switch (typeInfo->nativeType)
    {
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    case NativeType::F32:
    case NativeType::F64:
        break;
    default:
        return sourceFile->report({sourceFile, node, format("substraction not allowed on type '%s'", typeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (typeInfo->nativeType)
        {
        case NativeType::S8:
        {
            auto result = left->computedValue.reg.s8 - right->computedValue.reg.s8;
            if (result < INT8_MIN || result > INT8_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's8'"});
            node->computedValue.reg.s8 = left->computedValue.reg.s8 - right->computedValue.reg.s8;
        }
        break;
        case NativeType::S16:
        {
            auto result = left->computedValue.reg.s16 - right->computedValue.reg.s16;
            if (result < INT16_MIN || result > INT16_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's16'"});
            node->computedValue.reg.s16 = left->computedValue.reg.s16 - right->computedValue.reg.s16;
        }
        break;
        case NativeType::S32:
        {
            auto result = left->computedValue.reg.s32 - right->computedValue.reg.s32;
            if (result < INT32_MIN || result > INT32_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's32'"});
            node->computedValue.reg.s32 = left->computedValue.reg.s32 - right->computedValue.reg.s32;
        }
        break;
        case NativeType::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 - right->computedValue.reg.s64;
            break;
        case NativeType::U8:
            if (left->computedValue.reg.u64 - right->computedValue.reg.u64 > UINT8_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u8'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 - right->computedValue.reg.u64;
            break;
        case NativeType::U16:
            if (left->computedValue.reg.u64 - right->computedValue.reg.u64 > UINT16_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u16'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 - right->computedValue.reg.u64;
            break;
        case NativeType::U32:
            if (left->computedValue.reg.u64 - right->computedValue.reg.u64 > UINT32_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u32'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 - right->computedValue.reg.u64;
            break;
        case NativeType::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 - right->computedValue.reg.u64;
            break;
        case NativeType::F32:
            node->computedValue.reg.f32 = left->computedValue.reg.f32 - right->computedValue.reg.f32;
            break;
        case NativeType::F64:
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
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto typeInfo   = TypeManager::concreteType(left->typeInfo);

    switch (typeInfo->nativeType)
    {
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
    case NativeType::F32:
    case NativeType::F64:
        break;
    default:
        return sourceFile->report({sourceFile, node, format("multiplication not allowed on type '%s'", typeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (typeInfo->nativeType)
        {
        case NativeType::S8:
        {
            auto result = left->computedValue.reg.s8 * right->computedValue.reg.s8;
            if (result < INT8_MIN || result > INT8_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's8'"});
            node->computedValue.reg.s8 = left->computedValue.reg.s8 * right->computedValue.reg.s8;
        }
        break;
        case NativeType::S16:
        {
            auto result = left->computedValue.reg.s16 * right->computedValue.reg.s16;
            if (result < INT16_MIN || result > INT16_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's16'"});
            node->computedValue.reg.s16 = left->computedValue.reg.s16 * right->computedValue.reg.s16;
        }
        break;
        case NativeType::S32:
        {
            auto result = left->computedValue.reg.s32 * right->computedValue.reg.s32;
            if (result < INT32_MIN || result > INT32_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 's32'"});
            node->computedValue.reg.s32 = left->computedValue.reg.s32 * right->computedValue.reg.s32;
        }
        break;
        case NativeType::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 * right->computedValue.reg.s64;
            break;
        case NativeType::U8:
            if (left->computedValue.reg.u64 * right->computedValue.reg.u64 > UINT8_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u8'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 * right->computedValue.reg.u64;
            break;
        case NativeType::U16:
            if (left->computedValue.reg.u64 * right->computedValue.reg.u64 > UINT16_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u16'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 * right->computedValue.reg.u64;
            break;
        case NativeType::U32:
            if (left->computedValue.reg.u64 * right->computedValue.reg.u64 > UINT32_MAX)
                return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "operation overflow for type 'u32'"});
            node->computedValue.reg.u64 = left->computedValue.reg.u64 * right->computedValue.reg.u64;
            break;
        case NativeType::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 * right->computedValue.reg.u64;
            break;
        case NativeType::F32:
            node->computedValue.reg.f32 = left->computedValue.reg.f32 * right->computedValue.reg.f32;
            break;
        case NativeType::F64:
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
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto typeInfo   = TypeManager::concreteType(left->typeInfo);

    switch (typeInfo->nativeType)
    {
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::U8:
    case NativeType::U16:
    case NativeType::U32:
    case NativeType::U64:
        return sourceFile->report({sourceFile, node, "integer division is not allowed"});
    case NativeType::F32:
    case NativeType::F64:
        break;
    default:
        return sourceFile->report({sourceFile, node, format("division not allowed on type '%s'", typeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (typeInfo->nativeType)
        {
        case NativeType::F32:
            if (right->computedValue.reg.f32 == 0)
                return sourceFile->report({sourceFile, right->token, "division by zero"});
            node->computedValue.reg.f32 = left->computedValue.reg.f32 / right->computedValue.reg.f32;
            break;
        case NativeType::F64:
            if (right->computedValue.reg.f64 == 0)
                return sourceFile->report({sourceFile, right->token, "division by zero"});
            node->computedValue.reg.f64 = left->computedValue.reg.f64 / right->computedValue.reg.f64;
            break;
        default:
            return internalError(context, "resolveBinaryOpDiv, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveBitmaskOr(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto typeInfo   = TypeManager::concreteType(left->typeInfo);

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::U32:
    case NativeType::U64:
        break;
    default:
        return sourceFile->report({sourceFile, node, format("bitmask 'or' operation not allowed on type '%s'", typeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (typeInfo->nativeType)
        {
        case NativeType::S8:
            node->computedValue.reg.s8 = left->computedValue.reg.s8 | right->computedValue.reg.s8;
            break;
        case NativeType::S16:
            node->computedValue.reg.s16 = left->computedValue.reg.s16 | right->computedValue.reg.s16;
            break;
        case NativeType::S32:
            node->computedValue.reg.s32 = left->computedValue.reg.s32 | right->computedValue.reg.s32;
            break;
        case NativeType::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 | right->computedValue.reg.s64;
            break;
        case NativeType::U8:
            node->computedValue.reg.u8 = left->computedValue.reg.u8 | right->computedValue.reg.u8;
            break;
        case NativeType::U16:
            node->computedValue.reg.u16 = left->computedValue.reg.u16 | right->computedValue.reg.u16;
            break;
        case NativeType::U32:
            node->computedValue.reg.u32 = left->computedValue.reg.u32 | right->computedValue.reg.u32;
            break;
        case NativeType::U64:
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
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto typeInfo   = TypeManager::concreteType(left->typeInfo);

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::U32:
    case NativeType::U64:
        break;
    default:
        return sourceFile->report({sourceFile, node, format("bitmask 'and' operation not allowed on type '%s'", typeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (typeInfo->nativeType)
        {
        case NativeType::S8:
            node->computedValue.reg.s8 = left->computedValue.reg.s8 & right->computedValue.reg.s8;
            break;
        case NativeType::S16:
            node->computedValue.reg.s16 = left->computedValue.reg.s16 & right->computedValue.reg.s16;
            break;
        case NativeType::S32:
            node->computedValue.reg.s32 = left->computedValue.reg.s32 & right->computedValue.reg.s32;
            break;
        case NativeType::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 & right->computedValue.reg.s64;
            break;
        case NativeType::U8:
            node->computedValue.reg.u8 = left->computedValue.reg.u8 & right->computedValue.reg.u8;
            break;
        case NativeType::U16:
            node->computedValue.reg.u16 = left->computedValue.reg.u16 & right->computedValue.reg.u16;
            break;
        case NativeType::U32:
            node->computedValue.reg.u32 = left->computedValue.reg.u32 & right->computedValue.reg.u32;
            break;
        case NativeType::U64:
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
    auto sourceFile    = context->sourceFile;
    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::U32:
    case NativeType::U64:
        break;
    default:
        return sourceFile->report({sourceFile, left, format("shift left operation not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    switch (rightTypeInfo->nativeType)
    {
    case NativeType::U32:
        break;
    default:
        return sourceFile->report({sourceFile, right, format("shift operand must be 'u32' and not '%s'", rightTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (leftTypeInfo->nativeType)
        {
        case NativeType::S32:
            node->computedValue.reg.s32 = left->computedValue.reg.s32 << right->computedValue.reg.u32;
            break;
        case NativeType::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 << right->computedValue.reg.u32;
            break;
        case NativeType::U32:
            node->computedValue.reg.u32 = left->computedValue.reg.u32 << right->computedValue.reg.u32;
            break;
        case NativeType::U64:
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
    auto sourceFile    = context->sourceFile;
    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::U32:
    case NativeType::U64:
        break;
    default:
        return sourceFile->report({sourceFile, left, format("shift right operation not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    switch (rightTypeInfo->nativeType)
    {
    case NativeType::U32:
        break;
    default:
        return sourceFile->report({sourceFile, right, format("shift operand must be 'u32' and not '%s'", rightTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (leftTypeInfo->nativeType)
        {
        case NativeType::S32:
            node->computedValue.reg.s32 = left->computedValue.reg.s32 >> right->computedValue.reg.u32;
            break;
        case NativeType::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 >> right->computedValue.reg.u32;
            break;
        case NativeType::U32:
            node->computedValue.reg.u32 = left->computedValue.reg.u32 >> right->computedValue.reg.u32;
            break;
        case NativeType::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 >> right->computedValue.reg.u32;
            break;
        default:
            return internalError(context, "resolveShiftRight, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveXor(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node         = context->node;
    auto sourceFile   = context->sourceFile;
    auto leftTypeInfo = TypeManager::concreteType(left->typeInfo);

    switch (leftTypeInfo->nativeType)
    {
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::U32:
    case NativeType::U64:
        break;
    default:
        return sourceFile->report({sourceFile, context->node, format("xor operation not allowed on type '%s'", leftTypeInfo->name.c_str())});
    }

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (leftTypeInfo->nativeType)
        {
        case NativeType::S32:
            node->computedValue.reg.s32 = left->computedValue.reg.s32 ^ right->computedValue.reg.s32;
            break;
        case NativeType::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 ^ right->computedValue.reg.s64;
            break;
        case NativeType::U32:
            node->computedValue.reg.u32 = left->computedValue.reg.u32 ^ right->computedValue.reg.u32;
            break;
        case NativeType::U64:
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
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto left       = node->childs[0];
    auto right      = node->childs[1];

    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);
    SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left, format("operation not allowed on %s '%s'", TypeInfo::getNakedName(leftTypeInfo), leftTypeInfo->name.c_str())}));
    SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right, format("operation  not allowed on %s '%s'", TypeInfo::getNakedName(rightTypeInfo), rightTypeInfo->name.c_str())}));

    node->inheritLocation();
    TypeManager::promote(left, right);
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, left, right));
    node->typeInfo = left->typeInfo;

    node->byteCodeFct = &ByteCodeGenJob::emitBinaryOp;
    node->inheritAndFlag(left, right, AST_CONST_EXPR);

    switch (node->token.id)
    {
    case TokenId::SymPlus:
        SWAG_CHECK(resolveBinaryOpPlus(context, left, right));
        break;
    case TokenId::SymMinus:
        SWAG_CHECK(resolveBinaryOpMinus(context, left, right));
        break;
    case TokenId::SymAsterisk:
        SWAG_CHECK(resolveBinaryOpMul(context, left, right));
        break;
    case TokenId::SymSlash:
        SWAG_CHECK(resolveBinaryOpDiv(context, left, right));
        break;
    case TokenId::SymVertical:
        SWAG_CHECK(resolveBitmaskOr(context, left, right));
        break;
    case TokenId::SymAmpersand:
        SWAG_CHECK(resolveBitmaskAnd(context, left, right));
        break;
    case TokenId::SymCircumflex:
        SWAG_CHECK(resolveXor(context, left, right));
        break;
    default:
        return internalError(context, "resolveFactorExpression, token not supported");
    }

    return true;
}

bool SemanticJob::resolveShiftExpression(SemanticContext* context)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto left       = node->childs[0];
    auto right      = node->childs[1];

    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);
    SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left, format("operation not allowed on %s '%s'", TypeInfo::getNakedName(leftTypeInfo), leftTypeInfo->name.c_str())}));
    SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right, format("operation  not allowed on %s '%s'", TypeInfo::getNakedName(rightTypeInfo), rightTypeInfo->name.c_str())}));

    node->inheritLocation();
    TypeManager::promote(left, right);
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, g_TypeMgr.typeInfoU32, right));
    node->typeInfo = left->typeInfo;

    node->byteCodeFct = &ByteCodeGenJob::emitBinaryOp;
    node->inheritAndFlag(left, right, AST_CONST_EXPR);

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
    auto node      = context->node;
    auto leftNode  = node->childs[0];
    auto rightNode = node->childs[1];

    node->inheritLocation();
    node->typeInfo = g_TypeMgr.typeInfoBool;
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, g_TypeMgr.typeInfoBool, leftNode));
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, g_TypeMgr.typeInfoBool, rightNode));

    node->byteCodeFct = &ByteCodeGenJob::emitBinaryOp;
    node->inheritAndFlag(leftNode, rightNode, AST_CONST_EXPR);

    if ((leftNode->flags & AST_VALUE_COMPUTED) && (rightNode->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;
        switch (node->token.id)
        {
        case TokenId::SymAmpersandAmpersand:
            node->computedValue.reg.b = leftNode->computedValue.reg.b && rightNode->computedValue.reg.b;
            break;
        case TokenId::SymVerticalVertical:
            node->computedValue.reg.b = leftNode->computedValue.reg.b || rightNode->computedValue.reg.b;
            break;
        default:
            return internalError(context, "resolveBoolExpression, token not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveCompOpEqual(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node = context->node;

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;
        switch (left->typeInfo->nativeType)
        {
        case NativeType::Bool:
            node->computedValue.reg.b = left->computedValue.reg.b == right->computedValue.reg.b;
            break;
        case NativeType::F32:
            node->computedValue.reg.b = left->computedValue.reg.f32 == right->computedValue.reg.f32;
            break;
        case NativeType::F64:
            node->computedValue.reg.b = left->computedValue.reg.f64 == right->computedValue.reg.f64;
            break;
        case NativeType::S8:
        case NativeType::U8:
            node->computedValue.reg.b = left->computedValue.reg.u8 == right->computedValue.reg.u8;
            break;
        case NativeType::S16:
        case NativeType::U16:
            node->computedValue.reg.b = left->computedValue.reg.u16 == right->computedValue.reg.u16;
            break;
        case NativeType::S32:
        case NativeType::U32:
        case NativeType::Char:
            node->computedValue.reg.b = left->computedValue.reg.u32 == right->computedValue.reg.u32;
            break;
        case NativeType::S64:
        case NativeType::U64:
            node->computedValue.reg.b = left->computedValue.reg.u64 == right->computedValue.reg.u64;
            break;
        default:
            return internalError(context, "resolveCompOpEqual, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveCompOpLower(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node = context->node;

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;
        switch (left->typeInfo->nativeType)
        {
        case NativeType::Bool:
            node->computedValue.reg.b = left->computedValue.reg.b < right->computedValue.reg.b;
            break;
        case NativeType::F32:
            node->computedValue.reg.b = left->computedValue.reg.f32 < right->computedValue.reg.f32;
            break;
        case NativeType::F64:
            node->computedValue.reg.b = left->computedValue.reg.f64 < right->computedValue.reg.f64;
            break;
        case NativeType::S8:
            node->computedValue.reg.s8 = left->computedValue.reg.s8 < right->computedValue.reg.s8;
            break;
        case NativeType::S16:
            node->computedValue.reg.s16 = left->computedValue.reg.s16 < right->computedValue.reg.s16;
            break;
        case NativeType::S32:
            node->computedValue.reg.s32 = left->computedValue.reg.s32 < right->computedValue.reg.s32;
            break;
        case NativeType::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 < right->computedValue.reg.s64;
            break;
        case NativeType::U8:
            node->computedValue.reg.u8 = left->computedValue.reg.u8 < right->computedValue.reg.u8;
            break;
        case NativeType::U16:
            node->computedValue.reg.u16 = left->computedValue.reg.u16 < right->computedValue.reg.u16;
            break;
        case NativeType::U32:
        case NativeType::Char:
            node->computedValue.reg.u32 = left->computedValue.reg.u32 < right->computedValue.reg.u32;
            break;
        case NativeType::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 < right->computedValue.reg.u64;
            break;
        default:
            return internalError(context, "resolveCompOpLower, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveCompOpGreater(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node = context->node;

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;
        switch (left->typeInfo->nativeType)
        {
        case NativeType::Bool:
            node->computedValue.reg.b = left->computedValue.reg.b > right->computedValue.reg.b;
            break;
        case NativeType::F32:
            node->computedValue.reg.b = left->computedValue.reg.f32 > right->computedValue.reg.f32;
            break;
        case NativeType::F64:
            node->computedValue.reg.b = left->computedValue.reg.f64 > right->computedValue.reg.f64;
            break;
        case NativeType::S8:
            node->computedValue.reg.s8 = left->computedValue.reg.s8 > right->computedValue.reg.s8;
            break;
        case NativeType::S16:
            node->computedValue.reg.s16 = left->computedValue.reg.s16 > right->computedValue.reg.s16;
            break;
        case NativeType::S32:
            node->computedValue.reg.s32 = left->computedValue.reg.s32 > right->computedValue.reg.s32;
            break;
        case NativeType::S64:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 > right->computedValue.reg.s64;
            break;
        case NativeType::U8:
            node->computedValue.reg.u8 = left->computedValue.reg.u8 > right->computedValue.reg.u8;
            break;
        case NativeType::U16:
            node->computedValue.reg.u16 = left->computedValue.reg.u16 > right->computedValue.reg.u16;
            break;
        case NativeType::U32:
        case NativeType::Char:
            node->computedValue.reg.u32 = left->computedValue.reg.u32 > right->computedValue.reg.u32;
            break;
        case NativeType::U64:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 > right->computedValue.reg.u64;
            break;
        default:
            return internalError(context, "resolveCompOpGreater, type not supported");
        }
    }

    return true;
}

bool SemanticJob::resolveCompareExpression(SemanticContext* context)
{
    auto node       = context->node;
    auto left       = node->childs[0];
    auto right      = node->childs[1];
    auto sourceFile = context->sourceFile;

    auto leftTypeInfo  = TypeManager::concreteType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteType(right->typeInfo);
    SWAG_VERIFY(leftTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left, format("operation not allowed on %s '%s'", TypeInfo::getNakedName(leftTypeInfo), leftTypeInfo->name.c_str())}));
    SWAG_VERIFY(rightTypeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right, format("operation not allowed on %s '%s'", TypeInfo::getNakedName(rightTypeInfo), rightTypeInfo->name.c_str())}));

    node->inheritLocation();
    node->typeInfo = g_TypeMgr.typeInfoBool;
    TypeManager::promote(left, right);
    left->typeInfo  = TypeManager::flattenType(left->typeInfo);
    right->typeInfo = TypeManager::flattenType(right->typeInfo);
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, left, right));

    node->byteCodeFct = &ByteCodeGenJob::emitCompareOp;
    node->inheritAndFlag(left, right, AST_CONST_EXPR);

    switch (node->token.id)
    {
    case TokenId::SymEqualEqual:
        SWAG_CHECK(resolveCompOpEqual(context, left, right));
        break;
    case TokenId::SymExclamEqual:
        SWAG_CHECK(resolveCompOpEqual(context, left, right));
        node->computedValue.reg.b = !node->computedValue.reg.b;
        break;
    case TokenId::SymLower:
        SWAG_CHECK(resolveCompOpLower(context, left, right));
        break;
    case TokenId::SymGreater:
        SWAG_CHECK(resolveCompOpGreater(context, left, right));
        break;
    case TokenId::SymLowerEqual:
        SWAG_CHECK(resolveCompOpGreater(context, left, right));
        node->computedValue.reg.b = !node->computedValue.reg.b;
        break;
    case TokenId::SymGreaterEqual:
        SWAG_CHECK(resolveCompOpLower(context, left, right));
        node->computedValue.reg.b = !node->computedValue.reg.b;
        break;
    default:
        return internalError(context, "resolveCompareExpression, token not supported");
    }

    return true;
}

bool SemanticJob::resolveArrayAccess(SemanticContext* context)
{
    auto sourceFile = context->sourceFile;
    auto arrayNode  = CastAst<AstArrayAccess>(context->node, AstNodeKind::ArrayAccess);
    auto arrayType  = arrayNode->array->typeInfo;
    SWAG_VERIFY(arrayType->kind == TypeInfoKind::Pointer, sourceFile->report({sourceFile, arrayNode->array, format("type '%s' can't be referenced like a pointer", arrayType->name.c_str())}));

    auto typePtr = static_cast<TypeInfoPointer*>(arrayType);
    if (typePtr->ptrCount == 1)
    {
        arrayNode->typeInfo = typePtr->pointedType;
    }
    else
    {
        auto newType        = g_Pool_typeInfoPointer.alloc();
        newType->name       = typePtr->name;
        newType->ptrCount   = typePtr->ptrCount - 1;
        newType->sizeOf     = typePtr->sizeOf;
        arrayNode->typeInfo = g_TypeMgr.registerType(newType);
    }

    return true;
}