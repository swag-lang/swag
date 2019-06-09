#include "pch.h"
#include "ThreadManager.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"
#include "ByteCodeGenJob.h"
#include "SourceFile.h"
#include "ByteCode.h"
#include "Ast.h"
#include "SymTable.h"

bool ByteCodeGenJob::emitIf(ByteCodeGenContext* context)
{
    auto ifNode = CastAst<AstIf>(context->node, AstNodeKind::If);

    // Resolve ByteCodeOp::JumpNotTrue expression
    auto instruction = context->bc->out + ifNode->seekJumpExpression;
    auto diff        = ifNode->seekJumpAfterIf - ifNode->seekJumpExpression;

    // Resolve ByteCodeOp::Jump expression, after the if block
    if (ifNode->elseBlock)
    {
        instruction->b.s32 = diff;

        instruction        = context->bc->out + ifNode->seekJumpAfterIf;
        diff               = context->bc->numInstructions - ifNode->seekJumpAfterIf;
        instruction->a.s32 = diff - 1;
    }
    else
        instruction->b.s32 = diff - 1;

    return true;
}

bool ByteCodeGenJob::emitIfAfterExpr(ByteCodeGenContext* context)
{
    auto module                = context->sourceFile->module;
    auto node                  = context->node;
    auto ifNode                = CastAst<AstIf>(node->parent, AstNodeKind::If);
    ifNode->seekJumpExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::JumpNotTrue, node->resultRegisterRC);
    module->freeRegisterRC(node->resultRegisterRC);
    return true;
}

bool ByteCodeGenJob::emitIfAfterIf(ByteCodeGenContext* context)
{
    auto node   = context->node;
    auto ifNode = CastAst<AstIf>(node->parent, AstNodeKind::If);

    // This is the end of the if block. Need to jump after the else block, if there's one
    ifNode->seekJumpAfterIf = context->bc->numInstructions;
    if (ifNode->elseBlock)
        emitInstruction(context, ByteCodeOp::Jump);

    return true;
}

bool ByteCodeGenJob::emitWhile(ByteCodeGenContext* context)
{
    auto whileNode = CastAst<AstWhile>(context->node, AstNodeKind::While);

    // Resolve ByteCodeOp::JumpNotTrue expression
    auto instruction   = context->bc->out + whileNode->seekJumpExpression;
    auto diff          = whileNode->seekJumpAfterBlock - whileNode->seekJumpExpression;
    instruction->b.s32 = diff - 1;

    return true;
}

bool ByteCodeGenJob::emitWhileBeforeExpr(ByteCodeGenContext* context)
{
    auto node                           = context->node;
    auto whileNode                      = CastAst<AstWhile>(node->parent, AstNodeKind::While);
    whileNode->seekJumpBeforeExpression = context->bc->numInstructions;
    return true;
}

bool ByteCodeGenJob::emitWhileAfterExpr(ByteCodeGenContext* context)
{
    auto module                   = context->sourceFile->module;
    auto node                     = context->node;
    auto whileNode                = CastAst<AstWhile>(node->parent, AstNodeKind::While);
    whileNode->seekJumpExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::JumpNotTrue, node->resultRegisterRC);
    module->freeRegisterRC(node->resultRegisterRC);
    return true;
}

bool ByteCodeGenJob::emitWhileAfterBlock(ByteCodeGenContext* context)
{
    auto node      = context->node;
    auto whileNode = CastAst<AstWhile>(node->parent, AstNodeKind::While);

    auto inst   = emitInstruction(context, ByteCodeOp::Jump);
    auto diff   = whileNode->seekJumpBeforeExpression - context->bc->numInstructions;
    inst->a.s32 = diff;

    whileNode->seekJumpAfterBlock = context->bc->numInstructions;
    return true;
}
