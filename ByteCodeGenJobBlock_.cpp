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
    auto node                  = context->node;
    auto ifNode                = CastAst<AstIf>(node->parent, AstNodeKind::If);
    ifNode->seekJumpExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::JumpNotTrue, node->resultRegisterRC);
    freeRegisterRC(context, node->resultRegisterRC);
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

bool ByteCodeGenJob::emitLoop(ByteCodeGenContext* context)
{
    auto loopNode = CastAst<AstLoop>(context->node, AstNodeKind::Loop);

    // Resolve ByteCodeOp::JumpNotTrue expression
    auto instruction   = context->bc->out + loopNode->seekJumpExpression;
    auto diff          = loopNode->seekJumpAfterBlock - loopNode->seekJumpExpression;
    instruction->b.s32 = diff - 1;

    freeRegisterRC(context, loopNode->resultRegisterRC);
    return true;
}

bool ByteCodeGenJob::emitLoopAfterExpr(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto loopNode = CastAst<AstLoop>(node->parent, AstNodeKind::Loop);

    loopNode->seekJumpBeforeExpression = context->bc->numInstructions;

    auto r0 = context->sourceFile->module->reserveRegisterRC(context->bc);
    emitInstruction(context, ByteCodeOp::IsNullU32, node->resultRegisterRC, r0);
    loopNode->seekJumpExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::JumpTrue, r0);
    freeRegisterRC(context, r0);

    emitInstruction(context, ByteCodeOp::DecRA, node->resultRegisterRC);
    return true;
}

bool ByteCodeGenJob::emitLoopAfterBlock(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto loopNode = CastAst<AstLoop>(node->parent, AstNodeKind::Loop);

    auto inst   = emitInstruction(context, ByteCodeOp::Jump);
    auto diff   = loopNode->seekJumpBeforeExpression - context->bc->numInstructions;
    inst->a.s32 = diff;

    loopNode->seekJumpAfterBlock = context->bc->numInstructions;

    // Resolve all continue instructions
    for (auto continueNode : loopNode->continueList)
    {
        inst        = context->bc->out + continueNode->jumpInstruction;
        diff        = loopNode->seekJumpBeforeExpression - continueNode->jumpInstruction - 1;
        inst->a.s32 = diff;
    }

    // Resolve all break instructions
    for (auto breakNode : loopNode->breakList)
    {
        inst        = context->bc->out + breakNode->jumpInstruction;
        diff        = context->bc->numInstructions - breakNode->jumpInstruction - 1;
        inst->a.s32 = diff;
    }

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
    auto node                     = context->node;
    auto whileNode                = CastAst<AstWhile>(node->parent, AstNodeKind::While);
    whileNode->seekJumpExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::JumpNotTrue, node->resultRegisterRC);
    freeRegisterRC(context, node->resultRegisterRC);
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

    // Resolve all continue instructions
    for (auto continueNode : whileNode->continueList)
    {
        inst        = context->bc->out + continueNode->jumpInstruction;
        diff        = whileNode->seekJumpBeforeExpression - continueNode->jumpInstruction - 1;
        inst->a.s32 = diff;
    }

    // Resolve all break instructions
    for (auto breakNode : whileNode->breakList)
    {
        inst        = context->bc->out + breakNode->jumpInstruction;
        diff        = context->bc->numInstructions - breakNode->jumpInstruction - 1;
        inst->a.s32 = diff;
    }

    return true;
}

bool ByteCodeGenJob::emitBreak(ByteCodeGenContext* context)
{
    auto node                  = context->node;
    auto breakNode             = CastAst<AstBreakContinue>(node, AstNodeKind::Break);
    breakNode->jumpInstruction = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::Jump);
    return true;
}

bool ByteCodeGenJob::emitContinue(ByteCodeGenContext* context)
{
    auto node                  = context->node;
    auto breakNode             = CastAst<AstBreakContinue>(node, AstNodeKind::Continue);
    breakNode->jumpInstruction = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::Jump);
    return true;
}
