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
    SWAG_VERIFY(left->typeInfo->flags & TYPEINFO_NATIVE, sourceFile->report({sourceFile, left->token, "operation not yet available on that type"}));

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;

        switch (left->typeInfo->nativeType)
        {
        case NativeType::S32:
            node->computedValue.variant.s32 = left->computedValue.variant.s32 + right->computedValue.variant.s32;
            break;
        case NativeType::S64:
        case NativeType::SX:
            node->computedValue.variant.s64 = left->computedValue.variant.s64 + right->computedValue.variant.s64;
            break;
        case NativeType::U32:
            node->computedValue.variant.u32 = left->computedValue.variant.u32 + right->computedValue.variant.u32;
            break;
        case NativeType::U64:
        case NativeType::UX:
            node->computedValue.variant.u64 = left->computedValue.variant.u64 + right->computedValue.variant.u64;
            break;
        case NativeType::F32:
            node->computedValue.variant.f32 = left->computedValue.variant.f32 + right->computedValue.variant.f32;
            break;
        case NativeType::F64:
        case NativeType::FX:
            node->computedValue.variant.f64 = left->computedValue.variant.f64 + right->computedValue.variant.f64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveBinaryOpMinus(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto sourceFile = context->sourceFile;
    SWAG_VERIFY(left->typeInfo->flags & TYPEINFO_NATIVE, sourceFile->report({sourceFile, left->token, "operation not yet available on that type"}));
    SWAG_VERIFY(right->typeInfo->flags & TYPEINFO_NATIVE, sourceFile->report({sourceFile, right->token, "operation not yet available on that type"}));
    return true;
}

bool SemanticJob::resolveBinaryOpMul(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto sourceFile = context->sourceFile;
    SWAG_VERIFY(left->typeInfo->flags & TYPEINFO_NATIVE, sourceFile->report({sourceFile, left->token, "operation not yet available on that type"}));
    SWAG_VERIFY(right->typeInfo->flags & TYPEINFO_NATIVE, sourceFile->report({sourceFile, right->token, "operation not yet available on that type"}));
    return true;
}

bool SemanticJob::resolveBinaryOpDiv(SemanticContext* context, AstNode* left, AstNode* right)
{
    auto sourceFile = context->sourceFile;
    SWAG_VERIFY(left->typeInfo->flags & TYPEINFO_NATIVE, sourceFile->report({sourceFile, left->token, "operation not yet available on that type"}));
    SWAG_VERIFY(right->typeInfo->flags & TYPEINFO_NATIVE, sourceFile->report({sourceFile, right->token, "operation not yet available on that type"}));
    return true;
}

bool SemanticJob::resolveBoolExpression(SemanticContext* context)
{
    auto node      = context->node;
    auto leftNode  = node->childs[0];
    auto rightNode = node->childs[1];

    node->typeInfo = &g_TypeInfoBool;
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, &g_TypeInfoBool, leftNode));
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, &g_TypeInfoBool, rightNode));

    node->inheritAndFlag(leftNode, rightNode, AST_CONST_EXPR);

    if ((leftNode->flags & AST_VALUE_COMPUTED) && (rightNode->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;
        switch (node->token.id)
        {
        case TokenId::SymAmpersandAmpersand:
            node->computedValue.variant.b = leftNode->computedValue.variant.b && rightNode->computedValue.variant.b;
            break;
        case TokenId::SymVerticalVertical:
            node->computedValue.variant.b = leftNode->computedValue.variant.b || rightNode->computedValue.variant.b;
            break;
        }
    }

    return true;
}

#define CMP_OP(__OP)                                                                                           \
    switch (left->typeInfo->nativeType)                                                                        \
    {                                                                                                          \
    case NativeType::Bool:                                                                                     \
        node->computedValue.variant.b = left->computedValue.variant.b __OP right->computedValue.variant.b;     \
        break;                                                                                                 \
    case NativeType::F32:                                                                                      \
        node->computedValue.variant.b = left->computedValue.variant.f32 __OP right->computedValue.variant.f32; \
        break;                                                                                                 \
    case NativeType::F64:                                                                                      \
    case NativeType::FX:                                                                                       \
        node->computedValue.variant.b = left->computedValue.variant.f64 __OP right->computedValue.variant.f64; \
        break;                                                                                                 \
    default:                                                                                                   \
        node->computedValue.variant.b = left->computedValue.variant.s64 __OP right->computedValue.variant.s64; \
        break;                                                                                                 \
    }

bool SemanticJob::resolveCompareExpression(SemanticContext* context)
{
    auto node  = context->node;
    auto left  = node->childs[0];
    auto right = node->childs[1];

    node->typeInfo = &g_TypeInfoBool;
    TypeManager::promote(left, right);
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, left->typeInfo, right, CASTFLAG_DBLSIDE));

    node->inheritAndFlag(left, right, AST_CONST_EXPR);

    if ((left->flags & AST_VALUE_COMPUTED) && (right->flags & AST_VALUE_COMPUTED))
    {
        node->flags |= AST_VALUE_COMPUTED;
        switch (node->token.id)
        {
        case TokenId::SymEqualEqual:
            CMP_OP(==);
            break;
        case TokenId::SymExclamEqual:
            CMP_OP(!=);
            break;
        case TokenId::SymLower:
            CMP_OP(<);
            break;
        case TokenId::SymGreater:
            CMP_OP(>);
            break;
        case TokenId::SymLowerEqual:
            CMP_OP(<=);
            break;
        case TokenId::SymGreaterEqual:
            CMP_OP(>=);
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
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, left->typeInfo, right, CASTFLAG_DBLSIDE));
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