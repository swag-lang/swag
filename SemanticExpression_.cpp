#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "AstNode.h"
#include "Utf8.h"
#include "Global.h"
#include "TypeInfo.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "Scope.h"
#include "TypeManager.h"

bool SemanticJob::resolveLiteral(SemanticContext* context)
{
    auto node = context->node;
    node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
    node->typeInfo              = node->token.literalType;
    node->computedValue.variant = node->token.literalValue;
    node->computedValue.text    = node->token.text;
    context->result             = SemanticResult::Done;
    return true;
}

bool SemanticJob::resolveSingleOpMinus(SemanticContext* context, AstNode* op)
{
    auto sourceFile = context->sourceFile;

    SWAG_VERIFY(op->typeInfo->flags & TYPEINFO_NATIVE, sourceFile->report({sourceFile, op->token, "minus operation not available on that type"}));
    switch (op->typeInfo->nativeType)
    {
    case NativeType::SX:
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
        break;
    default:
        sourceFile->report({sourceFile, op->token, format("minus operation not available on type '%s'", TypeManager::nativeTypeName(op->typeInfo).c_str())});
        break;
    }

    if (op->flags & AST_VALUE_COMPUTED)
    {
        switch (op->typeInfo->nativeType)
        {
        case NativeType::S8:
            op->computedValue.variant.s8 = -op->computedValue.variant.s8;
            break;
        case NativeType::S16:
            op->computedValue.variant.s16 = -op->computedValue.variant.s16;
            break;
        case NativeType::S32:
            op->computedValue.variant.s32 = -op->computedValue.variant.s32;
            break;
        case NativeType::SX:
            op->computedValue.variant.s64 = -op->computedValue.variant.s64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveSingleOp(SemanticContext* context)
{
    auto node = context->node;
    auto op   = node->childs[0];

    switch (node->token.id)
    {
    case TokenId::SymMinus:
        SWAG_CHECK(resolveSingleOpMinus(context, op));
        break;
    }

    node->typeInfo = op->typeInfo;
    node->inheritAndFlag(op, AST_CONST_EXPR);
    node->inherhitComputedValue(op);

    context->result = SemanticResult::Done;
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

#define CMP_OP(__OP)                                                                                                   \
    switch (leftNode->typeInfo->nativeType)                                                                            \
    {                                                                                                                  \
    case NativeType::Bool:                                                                                             \
        node->computedValue.variant.b = leftNode->computedValue.variant.b __OP rightNode->computedValue.variant.b;     \
        break;                                                                                                         \
    case NativeType::F32:                                                                                              \
        node->computedValue.variant.b = leftNode->computedValue.variant.f32 __OP rightNode->computedValue.variant.f32; \
        break;                                                                                                         \
    case NativeType::F64:                                                                                              \
        node->computedValue.variant.b = leftNode->computedValue.variant.f64 __OP rightNode->computedValue.variant.f64; \
        break;                                                                                                         \
    default:                                                                                                           \
        node->computedValue.variant.b = leftNode->computedValue.variant.s64 __OP rightNode->computedValue.variant.s64; \
        break;                                                                                                         \
    }

bool SemanticJob::resolveCompareExpression(SemanticContext* context)
{
    auto node      = context->node;
    auto leftNode  = node->childs[0];
    auto rightNode = node->childs[1];

    node->typeInfo = &g_TypeInfoBool;
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, leftNode, rightNode, CASTFLAG_DBLSIDE));

    node->inheritAndFlag(leftNode, rightNode, AST_CONST_EXPR);

    if ((leftNode->flags & AST_VALUE_COMPUTED) && (rightNode->flags & AST_VALUE_COMPUTED))
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