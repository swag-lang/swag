#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeManager.h"
#include "Scope.h"

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

bool SemanticJob::resolveIdentifier(SemanticContext* context)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;

    auto scope = node->scope;
    while (scope)
    {
        auto symTable = scope->symTable;
        {
            scoped_lock lk(symTable->mutex);
            auto        name = symTable->findNoLock(node->name);
            if (name)
            {
                if (!name->overloads.empty())
                {
                    node->typeInfo  = name->overloads[0]->typeInfo;
                    context->result = SemanticResult::Done;
                    return true;
                }

                name->dependentJobs.push_back(context->job);
                context->result = SemanticResult::Pending;
                return true;
            }
        }

        scope = scope->parentScope;
    }

    return sourceFile->report({sourceFile, node->token, format("unknown identifier '%s'", node->name.c_str())});
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
        return sourceFile->report({sourceFile, op->token, format("minus operation not available on type '%s'", TypeManager::nativeTypeName(op->typeInfo).c_str())});
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
        case NativeType::S64:
            op->computedValue.variant.s64 = -op->computedValue.variant.s64;
            break;
        case NativeType::F32:
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
        return sourceFile->report({sourceFile, op->token, format("boolean inversion not available on type '%s'", TypeManager::nativeTypeName(op->typeInfo).c_str())});
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
