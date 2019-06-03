#include "pch.h"
#include "SemanticJob.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeInfo.h"
#include "SourceFile.h"

bool SemanticJob::resolveSingleOpMinus(SemanticContext* context, AstNode* op)
{
    auto sourceFile = context->sourceFile;
    SWAG_VERIFY(op->typeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, op->token, "operation not yet available on that type"}));

    switch (op->typeInfo->nativeType)
    {
    case NativeType::S8:
    case NativeType::S16:
    case NativeType::S32:
    case NativeType::S64:
    case NativeType::F32:
    case NativeType::F64:
        break;
    default:
        return sourceFile->report({sourceFile, op->token, format("minus operation not available on type '%s'", op->typeInfo->name.c_str())});
    }

    if (op->flags & AST_VALUE_COMPUTED)
    {
        switch (op->typeInfo->nativeType)
        {
        case NativeType::S8:
        case NativeType::S16:
        case NativeType::S32:
        case NativeType::S64:
            op->computedValue.reg.s64 = -op->computedValue.reg.s64;
            break;
        case NativeType::F32:
        case NativeType::F64:
            op->computedValue.reg.f64 = -op->computedValue.reg.f64;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveSingleOpExclam(SemanticContext* context, AstNode* op)
{
    auto sourceFile = context->sourceFile;

    SWAG_VERIFY(op->typeInfo->kind == TypeInfoKind::Native, sourceFile->report({sourceFile, op->token, "boolean inversion not available on that type"}));
    switch (op->typeInfo->nativeType)
    {
    case NativeType::Bool:
        break;
    default:
        return sourceFile->report({sourceFile, op->token, format("boolean inversion not available on type '%s'", op->typeInfo->name.c_str())});
    }

    if (op->flags & AST_VALUE_COMPUTED)
    {
        switch (op->typeInfo->nativeType)
        {
        case NativeType::Bool:
            op->computedValue.reg.b = !op->computedValue.reg.b;
            break;
        }
    }

    return true;
}

bool SemanticJob::resolveSingleOp(SemanticContext* context)
{
    auto node = context->node;
    auto op   = node->childs[0];
    node->inheritLocation();

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
    node->inheritComputedValue(op);

    context->result = SemanticResult::Done;
    return true;
}
