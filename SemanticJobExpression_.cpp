#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "TypeManager.h"

bool SemanticJob::resolveLiteral(SemanticContext* context)
{
    auto node         = context->node;
    node->byteCodeFct = ByteCodeGenJob::emitLiteral;
    node->setFlagsValueIsComputed();
    node->flags |= AST_R_VALUE;
    node->typeInfo           = TypeManager::literalTypeToType(node->token);
    node->computedValue.reg  = node->token.literalValue;
    node->computedValue.text = node->token.text;
    return true;
}

bool SemanticJob::resolveExplicitNoInit(SemanticContext* context)
{
    auto node = context->node;
    node->parent->flags |= AST_EXPLICITLY_NOT_INITIALIZED;
    node->flags |= AST_CONST_EXPR;
    node->typeInfo = g_TypeMgr.typeInfoVoid;
    return true;
}

bool SemanticJob::computeExpressionListTupleType(SemanticContext* context, AstNode* node)
{
    auto typeInfo       = g_Allocator.alloc<TypeInfoList>();
    typeInfo->kind      = TypeInfoKind::TypeListTuple;
    typeInfo->nakedName = "{";

    int idx = 0;
    node->flags |= AST_CONST_EXPR | AST_R_VALUE;
    for (auto child : node->childs)
    {
        SWAG_CHECK(checkIsConcrete(context, child));

        if (!typeInfo->subTypes.empty())
            typeInfo->nakedName += ", ";

        auto typeParam      = g_Allocator.alloc<TypeInfoParam>();
        typeParam->typeInfo = child->typeInfo;
        typeInfo->subTypes.push_back(typeParam);

        // Value has been named
        if (!child->name.empty() && (child->flags & AST_IS_NAMED))
        {
            typeInfo->nakedName += child->name;
            typeInfo->nakedName += ": ";
            typeParam->namedParam = child->name;
        }

        typeInfo->nakedName += child->typeInfo->name;

        typeInfo->sizeOf += child->typeInfo->sizeOf;
        if (!(child->flags & AST_CONST_EXPR))
            node->flags &= ~AST_CONST_EXPR;
        if (!(child->flags & AST_R_VALUE))
            node->flags &= ~AST_R_VALUE;

        idx++;
    }

    typeInfo->nakedName += "}";
    typeInfo->name = typeInfo->nakedName;
    node->typeInfo = typeInfo;
    return true;
}

bool SemanticJob::resolveExpressionListTuple(SemanticContext* context)
{
    auto node = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);
    SWAG_CHECK(computeExpressionListTupleType(context, node));

    node->byteCodeBeforeFct = ByteCodeGenJob::emitExpressionListBefore;
    node->byteCodeFct       = ByteCodeGenJob::emitExpressionList;

    // If the literal tuple is not constant, then we need to reserve some space in the
    // stack in order to store it.
    // Otherwise the tuple will come from the constant segment.
    if (!(node->flags & AST_CONST_EXPR) && node->ownerScope && node->ownerFct)
    {
        node->computedValue.reg.offset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += node->typeInfo->sizeOf;
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
    }

    return true;
}

bool SemanticJob::resolveExpressionListArray(SemanticContext* context)
{
    auto node = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

    auto typeInfo  = g_Allocator.alloc<TypeInfoList>();
    typeInfo->kind = TypeInfoKind::TypeListArray;
    SWAG_ASSERT(node->childs.size());
    typeInfo->nakedName = format("[%u] %s", node->childs.size(), node->childs.front()->typeInfo->name.c_str());
    typeInfo->name      = typeInfo->nakedName;

    node->flags |= AST_CONST_EXPR | AST_R_VALUE;
    for (auto child : node->childs)
    {
        SWAG_CHECK(checkIsConcrete(context, child));
        auto typeParam      = g_Allocator.alloc<TypeInfoParam>();
        typeParam->typeInfo = child->typeInfo;
        typeInfo->subTypes.push_back(typeParam);
        typeInfo->sizeOf += child->typeInfo->sizeOf;
        if (!(child->flags & AST_CONST_EXPR))
            node->flags &= ~AST_CONST_EXPR;
        if (!(child->flags & AST_R_VALUE))
            node->flags &= ~AST_R_VALUE;
    }

    node->byteCodeBeforeFct = ByteCodeGenJob::emitExpressionListBefore;
    node->byteCodeFct       = ByteCodeGenJob::emitExpressionList;
    node->typeInfo          = typeInfo;

    // If the literal array is not constant, then we need to reserve some space in the
    // stack in order to store it.
    // Otherwise the array will come from the constant segment.
    if (!(node->flags & AST_CONST_EXPR) && node->ownerScope && node->ownerFct)
    {
        node->computedValue.reg.offset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += node->typeInfo->sizeOf;
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
    }

    return true;
}

bool SemanticJob::evaluateConstExpression(SemanticContext* context, AstNode* node)
{
    if ((node->flags & AST_CONST_EXPR) &&
        node->typeInfo->kind != TypeInfoKind::TypeListArray &&
        node->typeInfo->kind != TypeInfoKind::TypeListTuple &&
        node->typeInfo->kind != TypeInfoKind::Slice)
    {
        SWAG_CHECK(checkIsConcrete(context, node));
        SWAG_CHECK(executeNode(context, node, true));
        if (context->result == ContextResult::Pending)
            return true;
    }

    return true;
}

bool SemanticJob::resolveConditionalOp(SemanticContext* context)
{
    auto node = context->node;
    SWAG_ASSERT(node->childs.size() == 3);

    auto expression = node->childs[0];
    auto ifTrue     = node->childs[1];
    auto ifFalse    = node->childs[2];
    SWAG_CHECK(checkIsConcrete(context, expression));
    SWAG_CHECK(checkIsConcrete(context, ifTrue));
    SWAG_CHECK(checkIsConcrete(context, ifFalse));

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, expression, CASTFLAG_AUTO_BOOL));
    SWAG_CHECK(TypeManager::makeCompatibles(context, ifFalse, ifTrue, CASTFLAG_BIJECTIF));
    node->typeInfo = ifTrue->typeInfo;

    if (expression->flags & AST_VALUE_COMPUTED)
    {
        if (expression->computedValue.reg.b)
            node->inheritComputedValue(ifTrue);
        else
            node->inheritComputedValue(ifFalse);
    }

    expression->byteCodeAfterFct = ByteCodeGenJob::emitConditionalOpAfterExpr;
    ifTrue->byteCodeAfterFct     = ByteCodeGenJob::emitConditionalOpAfterIfTrue;
    node->byteCodeFct            = ByteCodeGenJob::emitConditionalOp;
    return true;
}

bool SemanticJob::resolveNullConditionalOp(SemanticContext* context)
{
    auto node = context->node;
    SWAG_ASSERT(node->childs.size() == 2);

    auto expression = node->childs[0];
    auto ifTrue     = node->childs[1];
    SWAG_CHECK(checkIsConcrete(context, expression));
    SWAG_CHECK(checkIsConcrete(context, ifTrue));

    auto typeInfo = expression->typeInfo;

    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_CHECK(resolveUserOp(context, "opData", nullptr, nullptr, expression, nullptr, false));
        if (context->result == ContextResult::Pending)
            return true;
    }
    else if (!typeInfo->isNative(NativeTypeKind::String) &&
             !typeInfo->isNative(NativeTypeKind::Char) &&
             typeInfo->kind != TypeInfoKind::Pointer &&
             typeInfo->kind != TypeInfoKind::Interface &&
             !(typeInfo->flags & TYPEINFO_INTEGER) &&
             !(typeInfo->flags & TYPEINFO_FLOAT) &&
             typeInfo->kind != TypeInfoKind::Lambda)
    {
        return context->report({expression, format("cannot use operator '??' on type '%s'", typeInfo->name.c_str())});
    }

    SWAG_CHECK(TypeManager::makeCompatibles(context, expression, ifTrue, CASTFLAG_BIJECTIF));

    node->typeInfo    = expression->typeInfo;
    node->byteCodeFct = ByteCodeGenJob::emitNullConditionalOp;
    return true;
}

bool SemanticJob::resolveDefer(SemanticContext* context)
{
    auto node = context->node;
    SWAG_ASSERT(node->childs.size() == 1);

    auto expr = node->childs.front();
    node->ownerScope->deferredNodes.push_back(expr);
    expr->flags |= AST_NO_BYTECODE;

    return true;
}