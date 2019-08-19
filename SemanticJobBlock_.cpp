#include "pch.h"
#include "Global.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "SemanticJob.h"
#include "TypeInfo.h"
#include "Ast.h"
#include "TypeManager.h"
#include "Scope.h"
#include "ByteCodeGenJob.h"

bool SemanticJob::resolveIf(SemanticContext* context)
{
    auto node = CastAst<AstIf>(context->node, AstNodeKind::If);
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, g_TypeMgr.typeInfoBool, node->boolExpression));

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
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, g_TypeMgr.typeInfoBool, node->boolExpression));
    node->byteCodeFct                       = &ByteCodeGenJob::emitWhile;
    node->boolExpression->byteCodeBeforeFct = &ByteCodeGenJob::emitWhileBeforeExpr;
    node->boolExpression->byteCodeAfterFct  = &ByteCodeGenJob::emitWhileAfterExpr;
    node->block->byteCodeAfterFct           = &ByteCodeGenJob::emitWhileAfterBlock;
    return true;
}

bool SemanticJob::resolveForBefore(SemanticContext* context)
{
    auto node                        = CastAst<AstFor>(context->node, AstNodeKind::For);
    node->ownerScope->startStackSize = node->ownerScope->parentScope->startStackSize;
    return true;
}

bool SemanticJob::resolveFor(SemanticContext* context)
{
    auto node = CastAst<AstFor>(context->node, AstNodeKind::For);
    SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, g_TypeMgr.typeInfoBool, node->boolExpression));
    node->byteCodeFct                       = &ByteCodeGenJob::emitFor;
    node->boolExpression->byteCodeBeforeFct = &ByteCodeGenJob::emitForBeforeExpr;
    node->boolExpression->byteCodeAfterFct  = &ByteCodeGenJob::emitForAfterExpr;
    node->postExpression->byteCodeAfterFct  = &ByteCodeGenJob::emitForAfterPost;
    node->block->byteCodeAfterFct           = &ByteCodeGenJob::emitForAfterBlock;
    return true;
}

bool SemanticJob::resolveSwitch(SemanticContext* context)
{
    auto node                          = CastAst<AstSwitch>(context->node, AstNodeKind::Switch);
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
        SWAG_CHECK(TypeManager::makeCompatibles(context->sourceFile, node->ownerSwitch->expression, oneExpression));
    }

    node->typeInfo                 = node->ownerSwitch->expression->typeInfo;
    node->block->byteCodeBeforeFct = &ByteCodeGenJob::emitSwitchCaseBeforeBlock;
    node->block->byteCodeAfterFct  = &ByteCodeGenJob::emitSwitchCaseAfterBlock;
    return true;
}

bool SemanticJob::resolveLoop(SemanticContext* context)
{
    auto node       = CastAst<AstLoop>(context->node, AstNodeKind::Loop);
    auto sourceFile = context->sourceFile;
    auto expression = node->expression;

    if (!resolveCountProperty(context, node->expression, expression->typeInfo))
    {
        auto typeInfo = node->expression->typeInfo;
        SWAG_VERIFY(typeInfo->flags & TYPEINFO_INTEGER, sourceFile->report({sourceFile, expression, format("expression should be of type integer, but is '%s'", typeInfo->name.c_str())}));
        SWAG_VERIFY(typeInfo->sizeOf <= 4, sourceFile->report({sourceFile, expression, format("expression should be a 32 bit integer, but is '%s'", typeInfo->name.c_str())}));
        if (expression->flags & AST_VALUE_COMPUTED)
        {
            if (!(typeInfo->flags & TYPEINFO_UNSIGNED))
            {
                switch (typeInfo->nativeType)
                {
                case NativeType::S8:
                    if (expression->computedValue.reg.s8 < 0)
                        return sourceFile->report({sourceFile, expression, format("constant value should be unsigned, but is '%d'", expression->computedValue.reg.s8)});
                    break;
                case NativeType::S16:
                    if (expression->computedValue.reg.s16 < 0)
                        return sourceFile->report({sourceFile, expression, format("constant value should be unsigned, but is '%d'", expression->computedValue.reg.s16)});
                    break;
                case NativeType::S32:
                    if (expression->computedValue.reg.s32 < 0)
                        return sourceFile->report({sourceFile, expression, format("constant value should be unsigned, but is '%d'", expression->computedValue.reg.s32)});
                    break;
                }
            }
        }
        else
        {
            SWAG_VERIFY(typeInfo->flags & TYPEINFO_UNSIGNED, sourceFile->report({sourceFile, expression, format("expression should be of type unsigned integer, but is '%s'", typeInfo->name.c_str())}));
        }
    }

    node->typeInfo                     = expression->typeInfo;
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