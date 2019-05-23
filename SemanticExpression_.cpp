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
    SWAG_VERIFY(op->typeInfo->flags & TYPEINFO_NATIVE, sourceFile->report({sourceFile, op->token, "operation not yet available on that type"}));

    switch (op->typeInfo->nativeType)
    {
    case NativeType::SX:
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::F32:
    case NativeType::F64:
	case NativeType::FX:
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
        case NativeType::F32:
            op->computedValue.variant.f32 = -op->computedValue.variant.f32;
            break;
        case NativeType::F64:
		case NativeType::FX:
            op->computedValue.variant.f64 = -op->computedValue.variant.f64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveSingleOpExclam(SemanticContext* context, AstNode* op)
{
    auto sourceFile = context->sourceFile;

    SWAG_VERIFY(op->typeInfo->flags & TYPEINFO_NATIVE, sourceFile->report({sourceFile, op->token, "boolean inversion not available on that type"}));
    switch (op->typeInfo->nativeType)
    {
    case NativeType::Bool:
        break;
    default:
        sourceFile->report({sourceFile, op->token, format("boolean inversion not available on type '%s'", TypeManager::nativeTypeName(op->typeInfo).c_str())});
        break;
    }

    if (op->flags & AST_VALUE_COMPUTED)
    {
        switch (op->typeInfo->nativeType)
        {
        case NativeType::Bool:
            op->computedValue.variant.b = !op->computedValue.variant.b;
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
    case TokenId::SymExclam:
        SWAG_CHECK(resolveSingleOpExclam(context, op));
        break;
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
