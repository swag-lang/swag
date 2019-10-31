#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"

bool SemanticJob::resolveIf(SemanticContext* context)
{
    auto node = CastAst<AstIf>(context->node, AstNodeKind::If);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, node->boolExpression, CASTFLAG_AUTO_BOOL));

    // Do not generate backend if 'if' is constant, and has already been evaluated
    if (node->boolExpression->flags & AST_VALUE_COMPUTED)
    {
        node->boolExpression->flags |= AST_NO_BYTECODE;
        if (node->boolExpression->computedValue.reg.b)
        {
            if (node->elseBlock)
                node->elseBlock->flags |= AST_NO_BYTECODE;
        }
        else
        {
            node->ifBlock->flags |= AST_NO_BYTECODE;
        }
    }
    else
    {
        node->byteCodeFct                      = &ByteCodeGenJob::emitIf;
        node->boolExpression->byteCodeAfterFct = &ByteCodeGenJob::emitIfAfterExpr;
        node->ifBlock->byteCodeAfterFct        = &ByteCodeGenJob::emitIfAfterIf;
    }

    return true;
}

bool SemanticJob::resolveWhile(SemanticContext* context)
{
    auto node = CastAst<AstWhile>(context->node, AstNodeKind::While);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, node->boolExpression, CASTFLAG_AUTO_BOOL));
    node->byteCodeFct                       = &ByteCodeGenJob::emitLoop;
    node->boolExpression->byteCodeBeforeFct = &ByteCodeGenJob::emitWhileBeforeExpr;
    node->boolExpression->byteCodeAfterFct  = &ByteCodeGenJob::emitWhileAfterExpr;
    node->block->byteCodeAfterFct           = &ByteCodeGenJob::emitLoopAfterBlock;
    return true;
}

bool SemanticJob::resolveInlineBefore(SemanticContext* context)
{
    auto sourceFile             = context->sourceFile;
    auto node                   = CastAst<AstInline>(context->node, AstNodeKind::Inline);
    node->scope->startStackSize = node->ownerScope->startStackSize;

    // Register all function parameters
    if (node->func->parameters)
    {
        for (int i = 0; i < node->func->parameters->childs.size(); i++)
        {
            auto funcParam         = node->func->parameters->childs[i];
            auto overload          = node->scope->symTable->addSymbolTypeInfo(sourceFile, funcParam, funcParam->typeInfo, SymbolKind::FuncParam, nullptr, OVERLOAD_VAR_INLINE);
            overload->storageIndex = i;
        }
    }

    return true;
}

bool SemanticJob::resolveForBefore(SemanticContext* context)
{
    auto node                        = CastAst<AstFor>(context->node, AstNodeKind::For);
    node->ownerScope->startStackSize = node->ownerScope->parentScope->startStackSize;
    return true;
}

bool SemanticJob::resolveLoopBefore(SemanticContext* context)
{
    auto node                        = CastAst<AstLoop>(context->node, AstNodeKind::Loop);
    node->ownerScope->startStackSize = node->ownerScope->parentScope->startStackSize;
    return true;
}

bool SemanticJob::resolveFor(SemanticContext* context)
{
    auto node = CastAst<AstFor>(context->node, AstNodeKind::For);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, node->boolExpression));
    node->byteCodeFct                       = &ByteCodeGenJob::emitLoop;
    node->boolExpression->byteCodeBeforeFct = &ByteCodeGenJob::emitForBeforeExpr;
    node->boolExpression->byteCodeAfterFct  = &ByteCodeGenJob::emitForAfterExpr;
    node->postExpression->byteCodeAfterFct  = &ByteCodeGenJob::emitForAfterPost;
    node->block->byteCodeAfterFct           = &ByteCodeGenJob::emitLoopAfterBlock;
    return true;
}

bool SemanticJob::resolveSwitch(SemanticContext* context)
{
    auto node = CastAst<AstSwitch>(context->node, AstNodeKind::Switch);
    SWAG_CHECK(checkIsConcrete(context, node->expression));

    node->typeInfo                     = node->expression->typeInfo;
    node->byteCodeFct                  = &ByteCodeGenJob::emitSwitch;
    node->expression->byteCodeAfterFct = &ByteCodeGenJob::emitSwitchAfterExpr;
    return true;
}

bool SemanticJob::resolveCase(SemanticContext* context)
{
    auto node = CastAst<AstSwitchCase>(context->node, AstNodeKind::SwitchCase);
    for (auto oneExpression : node->expressions)
    {
        SWAG_CHECK(checkIsConcrete(context, oneExpression));
        SWAG_CHECK(TypeManager::makeCompatibles(context, node->ownerSwitch->expression, oneExpression));
    }

    node->typeInfo                 = node->ownerSwitch->expression->typeInfo;
    node->block->byteCodeBeforeFct = &ByteCodeGenJob::emitSwitchCaseBeforeBlock;
    node->block->byteCodeAfterFct  = &ByteCodeGenJob::emitSwitchCaseAfterBlock;
    return true;
}

bool SemanticJob::resolveLoop(SemanticContext* context)
{
    auto node = CastAst<AstLoop>(context->node, AstNodeKind::Loop);
    SWAG_CHECK(checkIsConcrete(context, node->expression));

    auto expression = node->expression;

    if (!resolveCountProperty(context, expression, expression->typeInfo))
    {
        auto typeInfo = expression->typeInfo;
        SWAG_VERIFY(typeInfo->flags & TYPEINFO_INTEGER, context->errorContext.report({expression, format("expression should be of type integer, but is '%s'", typeInfo->name.c_str())}));
        SWAG_VERIFY(typeInfo->sizeOf <= 4, context->errorContext.report({expression, format("expression should be a 32 bit integer, but is '%s'", typeInfo->name.c_str())}));
        if (expression->flags & AST_VALUE_COMPUTED)
        {
            if (!(typeInfo->flags & TYPEINFO_UNSIGNED))
            {
                switch (typeInfo->nativeType)
                {
                case NativeTypeKind::S8:
                    if (expression->computedValue.reg.s8 < 0)
                        return context->errorContext.report({expression, format("constant value should be unsigned, but is '%d'", expression->computedValue.reg.s8)});
                    break;
                case NativeTypeKind::S16:
                    if (expression->computedValue.reg.s16 < 0)
                        return context->errorContext.report({expression, format("constant value should be unsigned, but is '%d'", expression->computedValue.reg.s16)});
                    break;
                case NativeTypeKind::S32:
                    if (expression->computedValue.reg.s32 < 0)
                        return context->errorContext.report({expression, format("constant value should be unsigned, but is '%d'", expression->computedValue.reg.s32)});
                    break;
                }
            }
        }
        else
        {
            SWAG_VERIFY(typeInfo->flags & TYPEINFO_UNSIGNED, context->errorContext.report({expression, format("expression should be of type unsigned integer, but is '%s'", typeInfo->name.c_str())}));
        }
    }

    node->typeInfo                     = g_TypeMgr.typeInfoU32;
    node->byteCodeFct                  = &ByteCodeGenJob::emitLoop;
    node->expression->byteCodeAfterFct = &ByteCodeGenJob::emitLoopAfterExpr;
    node->block->byteCodeAfterFct      = &ByteCodeGenJob::emitLoopAfterBlock;
    return true;
}

bool SemanticJob::resolveIndex(SemanticContext* context)
{
    auto node = context->node;
    node->ownerBreakable->breakableFlags |= BREAKABLE_NEED_INDEX;
    node->typeInfo    = g_TypeMgr.typeInfoU32;
    node->byteCodeFct = &ByteCodeGenJob::emitIndex;
    return true;
}

bool SemanticJob::resolveBreak(SemanticContext* context)
{
    SWAG_CHECK(checkUnreachableCode(context));
    auto node         = context->node;
    node->byteCodeFct = &ByteCodeGenJob::emitBreak;
    return true;
}

bool SemanticJob::resolveContinue(SemanticContext* context)
{
    SWAG_CHECK(checkUnreachableCode(context));
    auto node         = context->node;
    node->byteCodeFct = &ByteCodeGenJob::emitContinue;
    return true;
}