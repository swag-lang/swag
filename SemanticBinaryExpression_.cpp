#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeManager.h"

bool SemanticJob::resolveBinaryOpPlus(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    SWAG_VERIFY(left->typeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left->token, "operation not yet available on that type"}));

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (left->typeInfo->nativeType)
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
        case NativeType::SX:
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
        case NativeType::UX:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 + right->computedValue.reg.u64;
            break;
        case NativeType::F32:
        case NativeType::F64:
        case NativeType::FX:
            node->computedValue.reg.f64 = left->computedValue.reg.f64 + right->computedValue.reg.f64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveBinaryOpMinus(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    SWAG_VERIFY(left->typeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left->token, "operation not yet available on that type"}));
    SWAG_VERIFY(right->typeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right->token, "operation not yet available on that type"}));

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (left->typeInfo->nativeType)
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
        case NativeType::SX:
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
        case NativeType::UX:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 - right->computedValue.reg.u64;
            break;
        case NativeType::F32:
        case NativeType::F64:
        case NativeType::FX:
            node->computedValue.reg.f64 = left->computedValue.reg.f64 - right->computedValue.reg.f64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveBinaryOpMul(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    SWAG_VERIFY(left->typeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left->token, "operation not yet available on that type"}));
    SWAG_VERIFY(right->typeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right->token, "operation not yet available on that type"}));

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (left->typeInfo->nativeType)
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
        case NativeType::SX:
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
        case NativeType::UX:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 * right->computedValue.reg.u64;
            break;
        case NativeType::F32:
        case NativeType::F64:
        case NativeType::FX:
            node->computedValue.reg.f64 = left->computedValue.reg.f64 * right->computedValue.reg.f64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveBinaryOpDiv(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    SWAG_VERIFY(left->typeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, left->token, "operation not yet available on that type"}));
    SWAG_VERIFY(right->typeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, right->token, "operation not yet available on that type"}));

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (left->typeInfo->nativeType)
        {
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
        case NativeType::SX:
        case NativeType::U8:
        case NativeType::U32:
        case NativeType::U64:
        case NativeType::UX:
            return sourceFile->report({sourceFile, left->token.startLocation, right->token.endLocation, "division of an integer is not allowed"});
        case NativeType::F32:
        case NativeType::F64:
        case NativeType::FX:
            if (right->computedValue.reg.f64 == 0)
                return sourceFile->report({sourceFile, right->token, "division by zero"});
            node->computedValue.reg.f64 = left->computedValue.reg.f64 / right->computedValue.reg.f64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveBoolExpression(SemanticContext* context)
{
    auto node      = context->node;
    auto leftNode  = node->childs[0];
    auto rightNode = node->childs[1];

    node->typeInfo = g_TypeMgr.typeInfoBool;
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, g_TypeMgr.typeInfoBool, leftNode));
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, g_TypeMgr.typeInfoBool, rightNode));

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
        }
    }

    return true;
}

bool SemanticJob::resolveFactorExpression(SemanticContext* context)
{
    auto node  = context->node;
    auto left  = node->childs[0];
    auto right = node->childs[1];

    TypeManager::promote(left, right);
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, left, right));
    node->typeInfo = left->typeInfo;

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
    }

    return true;
}

bool SemanticJob::resolveCompOpEqual(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node = context->node;
    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        switch (left->typeInfo->nativeType)
        {
        case NativeType::Bool:
            node->computedValue.reg.b = left->computedValue.reg.b == right->computedValue.reg.b;
            break;
        case NativeType::F32:
        case NativeType::F64:
        case NativeType::FX:
            node->computedValue.reg.b = left->computedValue.reg.f64 == right->computedValue.reg.f64;
            break;
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
        case NativeType::SX:
            node->computedValue.reg.b = left->computedValue.reg.s64 == right->computedValue.reg.s64;
            break;
        case NativeType::U8:
        case NativeType::U16:
        case NativeType::U32:
        case NativeType::Char:
        case NativeType::U64:
        case NativeType::UX:
            node->computedValue.reg.b = left->computedValue.reg.u64 == right->computedValue.reg.u64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveCompOpLower(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node = context->node;
    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        switch (left->typeInfo->nativeType)
        {
        case NativeType::Bool:
            node->computedValue.reg.b = left->computedValue.reg.b < right->computedValue.reg.b;
            break;
        case NativeType::F32:
        case NativeType::F64:
        case NativeType::FX:
            node->computedValue.reg.b = left->computedValue.reg.f64 < right->computedValue.reg.f64;
            break;
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
        case NativeType::SX:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 < right->computedValue.reg.s64;
            break;
        case NativeType::U8:
        case NativeType::U16:
        case NativeType::U32:
        case NativeType::Char:
        case NativeType::U64:
        case NativeType::UX:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 < right->computedValue.reg.u64;
            break;
        }
    }

	return true;
}

bool SemanticJob::resolveCompOpGreater(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto node = context->node;
    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        switch (left->typeInfo->nativeType)
        {
        case NativeType::Bool:
            node->computedValue.reg.b = left->computedValue.reg.b > right->computedValue.reg.b;
            break;
        case NativeType::F32:
        case NativeType::F64:
        case NativeType::FX:
            node->computedValue.reg.b = left->computedValue.reg.f64 > right->computedValue.reg.f64;
            break;
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
        case NativeType::SX:
            node->computedValue.reg.s64 = left->computedValue.reg.s64 > right->computedValue.reg.s64;
            break;
        case NativeType::U8:
        case NativeType::U16:
        case NativeType::U32:
        case NativeType::Char:
        case NativeType::U64:
        case NativeType::UX:
            node->computedValue.reg.u64 = left->computedValue.reg.u64 > right->computedValue.reg.u64;
            break;
        }
    }

	return true;
}

bool SemanticJob::resolveCompareExpression(SemanticContext* context)
{
    auto node  = context->node;
    auto left  = node->childs[0];
    auto right = node->childs[1];

    node->typeInfo = g_TypeMgr.typeInfoBool;
    TypeManager::promote(left, right);
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, left, right));

    node->inheritAndFlag(left, right, AST_CONST_EXPR);

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;
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
        }
    }

    return true;
}
