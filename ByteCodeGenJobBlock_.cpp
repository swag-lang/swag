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

    if (loopNode->resolvedSymbolName && loopNode->resolvedSymbolName->kind == SymbolKind::Function)
        SWAG_CHECK(emitUserOp(context, nullptr, loopNode));

    // To store the 'index' of the loop
    if (loopNode->needIndex())
    {
        loopNode->registerIndex = reserveRegisterRC(context);
        auto inst               = emitInstruction(context, ByteCodeOp::CopyRAVB32, loopNode->registerIndex);
        inst->b.s32             = -1;
    }

    auto r0                            = reserveRegisterRC(context);
    loopNode->seekJumpBeforeExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::IsNullU32, node->resultRegisterRC, r0);
    loopNode->seekJumpExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::JumpTrue, r0);
    freeRegisterRC(context, r0);

    // Decrement the loop variable
    emitInstruction(context, ByteCodeOp::DecRA, node->resultRegisterRC);

    // Increment the index
    if (loopNode->needIndex())
    {
        emitInstruction(context, ByteCodeOp::IncRA, loopNode->registerIndex);
    }

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

    freeRegisterRC(context, whileNode->resultRegisterRC);
    return true;
}

bool ByteCodeGenJob::emitWhileBeforeExpr(ByteCodeGenContext* context)
{
    auto node      = context->node;
    auto whileNode = CastAst<AstWhile>(node->parent, AstNodeKind::While);

    // To store the 'index' of the loop
    if (whileNode->needIndex())
    {
        whileNode->registerIndex = reserveRegisterRC(context);
        auto inst                = emitInstruction(context, ByteCodeOp::CopyRAVB32, whileNode->registerIndex);
        inst->b.s32              = -1;
    }

    whileNode->seekJumpBeforeExpression = context->bc->numInstructions;
    return true;
}

bool ByteCodeGenJob::emitWhileAfterExpr(ByteCodeGenContext* context)
{
    auto node      = context->node;
    auto whileNode = CastAst<AstWhile>(node->parent, AstNodeKind::While);

    whileNode->seekJumpExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::JumpNotTrue, node->resultRegisterRC);

    // Increment the index
    if (whileNode->needIndex())
    {
        emitInstruction(context, ByteCodeOp::IncRA, whileNode->registerIndex);
    }

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

bool ByteCodeGenJob::emitFor(ByteCodeGenContext* context)
{
    auto forNode = CastAst<AstFor>(context->node, AstNodeKind::For);

    // Resolve ByteCodeOp::JumpNotTrue expression
    auto instruction   = context->bc->out + forNode->seekJumpExpression;
    auto diff          = forNode->seekJumpAfterBlock - forNode->seekJumpExpression;
    instruction->b.s32 = diff - 1;

    freeRegisterRC(context, forNode->resultRegisterRC);
    return true;
}

bool ByteCodeGenJob::emitForBeforeExpr(ByteCodeGenContext* context)
{
    auto node    = context->node;
    auto forNode = CastAst<AstFor>(node->parent, AstNodeKind::For);

    // To store the 'index' of the loop
    if (forNode->needIndex())
    {
        forNode->registerIndex = reserveRegisterRC(context);
        auto inst              = emitInstruction(context, ByteCodeOp::CopyRAVB32, forNode->registerIndex);
        inst->b.s32            = -1;
    }

    forNode->seekJumpBeforeExpression = context->bc->numInstructions;
    return true;
}

bool ByteCodeGenJob::emitForAfterExpr(ByteCodeGenContext* context)
{
    auto node    = context->node;
    auto forNode = CastAst<AstFor>(node->parent, AstNodeKind::For);

    forNode->seekJumpExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::JumpNotTrue, node->resultRegisterRC);

    // Increment the index
    if (forNode->needIndex())
    {
        emitInstruction(context, ByteCodeOp::IncRA, forNode->registerIndex);
    }

    // Jump to the block
    forNode->seekJumpToBlock = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::Jump);

    // This is the start of the post statement
    forNode->seekJumpBeforePost = context->bc->numInstructions;
    return true;
}

bool ByteCodeGenJob::emitForAfterPost(ByteCodeGenContext* context)
{
    auto node    = context->node;
    auto forNode = CastAst<AstFor>(node->parent, AstNodeKind::For);

    // Jump to the test expression
    auto inst   = emitInstruction(context, ByteCodeOp::Jump);
    inst->a.s32 = forNode->seekJumpBeforeExpression - context->bc->numInstructions;

    // And set the jump to the start of the block
    inst        = context->bc->out + forNode->seekJumpToBlock;
    auto diff   = context->bc->numInstructions - forNode->seekJumpToBlock - 1;
    inst->a.s32 = diff;

    return true;
}

bool ByteCodeGenJob::emitForAfterBlock(ByteCodeGenContext* context)
{
    auto node    = context->node;
    auto forNode = CastAst<AstFor>(node->parent, AstNodeKind::For);

    // Jump to the post expression
    auto inst   = emitInstruction(context, ByteCodeOp::Jump);
    auto diff   = forNode->seekJumpBeforePost - context->bc->numInstructions;
    inst->a.s32 = diff;

    forNode->seekJumpAfterBlock = context->bc->numInstructions;

    // Resolve all continue instructions
    for (auto continueNode : forNode->continueList)
    {
        inst        = context->bc->out + continueNode->jumpInstruction;
        diff        = forNode->seekJumpBeforePost - continueNode->jumpInstruction - 1;
        inst->a.s32 = diff;
    }

    // Resolve all break instructions
    for (auto breakNode : forNode->breakList)
    {
        inst        = context->bc->out + breakNode->jumpInstruction;
        diff        = context->bc->numInstructions - breakNode->jumpInstruction - 1;
        inst->a.s32 = diff;
    }

    return true;
}

bool ByteCodeGenJob::emitSwitch(ByteCodeGenContext* context)
{
    auto node                    = context->node;
    auto switchNode              = CastAst<AstSwitch>(node, AstNodeKind::Switch);
    switchNode->resultRegisterRC = switchNode->expression->resultRegisterRC;

    // Resolve the jump to go outside the switch
    auto inst   = context->bc->out + switchNode->seekJumpExpression;
    auto diff   = context->bc->numInstructions - switchNode->seekJumpExpression - 1;
    inst->a.s32 = diff;

    // Resolve all break instructions
    for (auto breakNode : switchNode->breakList)
    {
        inst        = context->bc->out + breakNode->jumpInstruction;
        diff        = context->bc->numInstructions - breakNode->jumpInstruction - 1;
        inst->a.s32 = diff;
    }

    return true;
}

bool ByteCodeGenJob::emitSwitchAfterExpr(ByteCodeGenContext* context)
{
    auto node       = context->node;
    auto switchNode = CastAst<AstSwitch>(node->parent, AstNodeKind::Switch);

    // Jump to the first case
    emitInstruction(context, ByteCodeOp::Jump)->a.s32 = 1;

    // Jump to exit the switch
    switchNode->seekJumpExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::Jump);

    return true;
}

bool ByteCodeGenJob::emitSwitchCaseBeforeBlock(ByteCodeGenContext* context)
{
    auto node      = context->node;
    auto blockNode = CastAst<AstSwitchCaseBlock>(node, AstNodeKind::Statement);
    auto caseNode  = blockNode->ownerCase;

    caseNode->ownerSwitch->resultRegisterRC = caseNode->ownerSwitch->expression->resultRegisterRC;

    // Default case does not have expressions
    vector<uint32_t> allJumps;
    if (!caseNode->expressions.empty())
    {
        auto r0 = reserveRegisterRC(context);
        for (auto expr : caseNode->expressions)
        {
            SWAG_CHECK(emitCompareOpEqual(context, caseNode, expr, caseNode->ownerSwitch->resultRegisterRC, expr->resultRegisterRC, r0));
            allJumps.push_back(context->bc->numInstructions);
            auto inst   = emitInstruction(context, ByteCodeOp::JumpTrue, r0);
            inst->b.u32 = context->bc->numInstructions; // Remember start of the jump, to compute the relative offset
        }

        freeRegisterRC(context, r0);

        // Jump to the next case, except for the default, which is the last
        blockNode->seekJumpNextCase = context->bc->numInstructions;
        emitInstruction(context, ByteCodeOp::Jump);

        // Now this is the beginning of the block, so we can resolve all Jump
        for (auto jumpIdx : allJumps)
        {
            ByteCodeInstruction* jump = context->bc->out + jumpIdx;
            jump->b.s32               = context->bc->numInstructions - jump->b.u32;
        }
    }

    return true;
}

bool ByteCodeGenJob::emitSwitchCaseAfterBlock(ByteCodeGenContext* context)
{
    auto node      = context->node;
    auto blockNode = CastAst<AstSwitchCaseBlock>(node, AstNodeKind::Statement);

    // For the default case, do nothing, fallback to the end of the switch
    if (blockNode->ownerCase->isDefault)
        return true;

    // Jump to exit the switch
    auto inst   = emitInstruction(context, ByteCodeOp::Jump);
    inst->a.s32 = blockNode->ownerCase->ownerSwitch->seekJumpExpression - context->bc->numInstructions;

    // Resolve jump from case to case
    inst        = context->bc->out + blockNode->seekJumpNextCase;
    inst->a.s32 = context->bc->numInstructions - blockNode->seekJumpNextCase - 1;
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

bool ByteCodeGenJob::emitIndex(ByteCodeGenContext* context)
{
    auto node              = context->node;
    node->resultRegisterRC = reserveRegisterRC(context);
    emitInstruction(context, ByteCodeOp::CopyRARB, node->resultRegisterRC, node->ownerBreakable->registerIndex);
    return true;
}