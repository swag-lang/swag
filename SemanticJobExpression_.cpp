#include "pch.h"
#include "SemanticJob.h"
#include "SourceFile.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Scope.h"
#include "Module.h"

bool SemanticJob::resolveLiteral(SemanticContext* context)
{
    auto node         = context->node;
    node->byteCodeFct = &ByteCodeGenJob::emitLiteral;
    node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED | AST_R_VALUE;
    node->typeInfo           = node->token.literalType;
    node->computedValue.reg  = node->token.literalValue;
    node->computedValue.text = node->token.text;
    return true;
}

bool SemanticJob::resolveExpressionListCurly(SemanticContext* context)
{
    auto  sourceFile = context->sourceFile;
    auto& typeTable  = sourceFile->module->typeTable;
    auto  node       = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

    auto typeInfo      = g_Pool_typeInfoList.alloc();
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

        if (!child->name.empty())
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
    node->byteCodeBeforeFct = &ByteCodeGenJob::emitExpressionListBefore;
    node->byteCodeFct       = &ByteCodeGenJob::emitExpressionList;

    if (node->flags & AST_CONST_EXPR)
        typeInfo->setConst();
    node->typeInfo = typeTable.registerType(typeInfo);

    // Reserve
    if (!(node->flags & AST_CONST_EXPR) && node->ownerScope && node->ownerFct)
    {
        node->storageOffset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += node->typeInfo->sizeOf;
        node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
    }

    return true;
}

bool SemanticJob::resolveExpressionListArray(SemanticContext* context)
{
    auto  sourceFile = context->sourceFile;
    auto& typeTable  = sourceFile->module->typeTable;
    auto  node       = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

    auto typeInfo      = g_Pool_typeInfoList.alloc();
    typeInfo->listKind = node->listKind;
    typeInfo->name     = "[";

    node->flags |= AST_CONST_EXPR | AST_R_VALUE;
    for (auto child : node->childs)
    {
        SWAG_CHECK(checkIsConcrete(context, child));

        if (!typeInfo->childs.empty())
            typeInfo->name += ", ";
        typeInfo->childs.push_back(child->typeInfo);
        typeInfo->name += child->typeInfo->name;
        typeInfo->sizeOf += child->typeInfo->sizeOf;
        if (!(child->flags & AST_CONST_EXPR))
            node->flags &= ~AST_CONST_EXPR;
        if (!(child->flags & AST_R_VALUE))
            node->flags &= ~AST_R_VALUE;
    }

    typeInfo->name += "]";
    node->byteCodeBeforeFct = &ByteCodeGenJob::emitExpressionListBefore;
    node->byteCodeFct       = &ByteCodeGenJob::emitExpressionList;
    node->typeInfo          = typeTable.registerType(typeInfo);

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
        if (context->result == SemanticResult::Pending)
            return true;
    }

    return true;
}