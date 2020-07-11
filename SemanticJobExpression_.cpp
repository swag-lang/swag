#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "TypeManager.h"

bool SemanticJob::resolveLiteral(SemanticContext* context)
{
    auto node         = context->node;
    node->byteCodeFct = ByteCodeGenJob::emitLiteral;
    node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED | AST_R_VALUE;
    node->typeInfo           = node->token.literalType;
    node->computedValue.reg  = node->token.literalValue;
    node->computedValue.text = node->token.text;
    return true;
}

bool SemanticJob::resolveExpressionListCurly(SemanticContext* context)
{
    auto node = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

    auto typeInfo      = g_Allocator.alloc<TypeInfoList>();
    typeInfo->listKind = node->listKind;
    typeInfo->name     = "{";

    int idx = 0;
    node->flags |= AST_CONST_EXPR | AST_R_VALUE;
    for (auto child : node->childs)
    {
        SWAG_CHECK(checkIsConcrete(context, child));

        if (!typeInfo->childs.empty())
            typeInfo->name += ", ";
        typeInfo->childs.push_back(child->typeInfo);

        // Value has been named
        if (!child->name.empty() && (child->flags & AST_IS_NAMED))
        {
            typeInfo->name += child->name;
            typeInfo->name += ": ";
            typeInfo->names.push_back(child->name);
        }

        typeInfo->name += child->typeInfo->name;

        typeInfo->sizeOf += child->typeInfo->sizeOf;
        if (!(child->flags & AST_CONST_EXPR))
            node->flags &= ~AST_CONST_EXPR;
        if (!(child->flags & AST_R_VALUE))
            node->flags &= ~AST_R_VALUE;

        idx++;
    }

    typeInfo->name += "}";
    node->byteCodeBeforeFct = ByteCodeGenJob::emitExpressionListBefore;
    node->byteCodeFct       = ByteCodeGenJob::emitExpressionList;
    node->typeInfo          = typeInfo;

    // Reserve
    if (!(node->flags & AST_CONST_EXPR) && node->ownerScope && node->ownerFct)
    {
        node->storageOffset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += node->typeInfo->sizeOf;
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
    }

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

bool SemanticJob::resolveExpressionListArray(SemanticContext* context)
{
    auto node = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

    auto typeInfo      = g_Allocator.alloc<TypeInfoList>();
    typeInfo->listKind = node->listKind;
    SWAG_ASSERT(node->childs.size());
    typeInfo->name = format("[%u] %s", node->childs.size(), node->childs.front()->typeInfo->name.c_str());

    node->flags |= AST_CONST_EXPR | AST_R_VALUE;
    for (auto child : node->childs)
    {
        SWAG_CHECK(checkIsConcrete(context, child));
        typeInfo->childs.push_back(child->typeInfo);
        typeInfo->sizeOf += child->typeInfo->sizeOf;
        if (!(child->flags & AST_CONST_EXPR))
            node->flags &= ~AST_CONST_EXPR;
        if (!(child->flags & AST_R_VALUE))
            node->flags &= ~AST_R_VALUE;
    }

    node->byteCodeBeforeFct = ByteCodeGenJob::emitExpressionListBefore;
    node->byteCodeFct       = ByteCodeGenJob::emitExpressionList;
    node->typeInfo          = typeInfo;

    // Reserve
    if (!(node->flags & AST_CONST_EXPR) && node->ownerScope && node->ownerFct)
    {
        node->storageOffset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += node->typeInfo->sizeOf;
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
    }

    return true;
}

bool SemanticJob::evaluateConstExpression(SemanticContext* context, AstNode* node)
{
    if ((node->flags & AST_CONST_EXPR) && node->typeInfo->kind != TypeInfoKind::TypeList && node->typeInfo->kind != TypeInfoKind::Slice)
    {
        SWAG_CHECK(checkIsConcrete(context, node));
        SWAG_CHECK(executeNode(context, node, true));
        if (context->result == ContextResult::Pending)
            return true;
    }

    return true;
}

bool SemanticJob::resolveTrinaryOp(SemanticContext* context)
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

    node->byteCodeFct = ByteCodeGenJob::emitTrinaryOp;
    return true;
}