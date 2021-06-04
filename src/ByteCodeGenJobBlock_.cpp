#include "pch.h"
#include "LanguageSpec.h"
#include "ByteCodeOp.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "SemanticJob.h"
#include "Ast.h"

bool ByteCodeGenJob::emitInlineBefore(ByteCodeGenContext* context)
{
    auto node       = CastAst<AstInline>(context->node, AstNodeKind::Inline);
    auto returnType = node->func->returnType->typeInfo;

    // Missing try/catch...
    auto parent = node->parent;
    if (parent->kind == AstNodeKind::Identifier && parent->parent)
        parent = parent->parent;
    if (parent->kind == AstNodeKind::IdentifierRef && parent->parent)
        parent = parent->parent;
    SWAG_CHECK(checkCatchError(context, node, node->func, parent, node->func->typeInfo));

    // Clear current error
    if (node->func->typeInfo->flags & TYPEINFO_CAN_THROW)
    {
        RegisterList r0 = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::ClearRA, r0);
        emitInstruction(context, ByteCodeOp::IntrinsicSetErr, r0, r0);
        freeRegisterRC(context, r0);
    }

    // Reserve registers for return value
    reserveRegisterRC(context, node->resultRegisterRC, node->func->returnType->typeInfo->numRegisters());
    node->parent->resultRegisterRC = node->resultRegisterRC;

    // If the inline returns a copy, then initialize the register with the address of the temporary
    // variable on the stack, so that the inline block can copy it's result to it. Of course, this is
    // not the top for speed, but anyway there's room for improvement for inline in all cases.
    if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
    {
        auto inst   = emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
        inst->b.u64 = node->computedValue.storageOffset;
        node->ownerScope->symTable.addVarToDrop(nullptr, returnType, node->computedValue.storageOffset);
    }

    AstNode* allParams     = nullptr;
    int      numCallParams = 0;
    if (node->parent->kind == AstNodeKind::Identifier)
    {
        auto identifier                             = CastAst<AstIdentifier>(node->parent, AstNodeKind::Identifier);
        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        allParams                                   = identifier->callParameters;
        numCallParams                               = allParams ? (int) allParams->childs.size() : 0;
    }
    else if (node->parent->kind == AstNodeKind::Loop)
    {
        // This should be opCount
        SWAG_ASSERT(node->parent->extension && node->parent->extension->resolvedUserOpSymbolOverload);
        allParams     = node->parent;
        numCallParams = 1;
    }
    else
    {
        allParams     = node->parent;
        numCallParams = (int) allParams->childs.size() - 1; // Remove the inline block
    }

    // Need to map all call parameters to function arguments
    auto func = node->func;
    if (func->parameters)
    {
        auto numFuncParams = func->parameters->childs.size();

        // Sort childs by parameter index
        SemanticJob::sortParameters(allParams);

        // Simple case, every parameters are covered by the call, and there's no named param
        if (numFuncParams == numCallParams)
        {
            for (int i = 0; i < numCallParams; i++)
            {
                auto funcParam = CastAst<AstVarDecl>(func->parameters->childs[i], AstNodeKind::FuncDeclParam);
                auto callParam = allParams->childs[i];
                auto symbol    = node->parametersScope->symTable.find(funcParam->token.text);
                SWAG_ASSERT(symbol);
                for (auto overload : symbol->overloads)
                {
                    if (overload->flags & OVERLOAD_VAR_INLINE)
                    {
                        overload->registers = callParam->resultRegisterRC;
                        if (overload->registers.canFree)
                        {
                            overload->registers.canFree = false;
                            node->allocateExtension();
                            for (int r = 0; r < overload->registers.size(); r++)
                                node->extension->registersToRelease.push_back(overload->registers[r]);
                        }
                        break;
                    }
                }
            }
        }
        else
        {
            // Determine if this parameter has been covered by the call
            for (int i = 0; i < numFuncParams; i++)
            {
                auto funcParam = CastAst<AstVarDecl>(func->parameters->childs[i], AstNodeKind::FuncDeclParam);
                bool covered   = false;
                for (int j = 0; j < numCallParams; j++)
                {
                    auto callParam = CastAst<AstFuncCallParam>(allParams->childs[j], AstNodeKind::FuncCallParam);
                    if (callParam->indexParam == i)
                    {
                        if (callParam->semFlags & AST_SEM_AUTO_CODE_PARAM)
                        {
                            covered = true;
                            break;
                        }

                        auto symbol = node->parametersScope->symTable.find(funcParam->token.text);
                        SWAG_ASSERT(symbol);
                        for (auto overload : symbol->overloads)
                        {
                            if (overload->flags & OVERLOAD_VAR_INLINE)
                            {
                                overload->registers = callParam->resultRegisterRC;
                                if (overload->registers.canFree)
                                {
                                    overload->registers.canFree = false;
                                    node->allocateExtension();
                                    for (int r = 0; r < overload->registers.size(); r++)
                                        node->extension->registersToRelease.push_back(overload->registers[r]);
                                }
                                covered = true;
                                break;
                            }
                        }

                        break;
                    }
                }

                // If not covered, then this is a default value
                if (!covered)
                {
                    auto defaultParam = CastAst<AstVarDecl>(func->parameters->childs[i], AstNodeKind::FuncDeclParam);
                    SWAG_ASSERT(defaultParam->assignment);

                    auto symbol = node->parametersScope->symTable.find(defaultParam->token.text);
                    SWAG_ASSERT(symbol);
                    for (auto overload : symbol->overloads)
                    {
                        if (overload->flags & OVERLOAD_VAR_INLINE)
                        {
                            SWAG_CHECK(emitDefaultParamValue(context, defaultParam, overload->registers));
                            overload->registers.canFree = false;
                            node->allocateExtension();
                            for (int r = 0; r < overload->registers.size(); r++)
                                node->extension->registersToRelease.push_back(overload->registers[r]);
                            break;
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool ByteCodeGenJob::emitInline(ByteCodeGenContext* context)
{
    auto node = CastAst<AstInline>(context->node, AstNodeKind::Inline);

    // Update all returns to jump at the end of the inline block
    for (auto r : node->returnList)
        context->bc->out[r->seekJump].b.s32 = context->bc->numInstructions - r->seekJump - 1;

    // Release persistent list of registers (except if mixin, because in that
    // case, the inline node does not own the scope)
    if (node->extension && !node->extension->registersToRelease.empty())
    {
        if (!(node->attributeFlags & ATTRIBUTE_MIXIN))
        {
            for (auto r : node->extension->registersToRelease)
                freeRegisterRC(context, r);
        }
        else
        {
            // Transfert registers to release to the parent scope owner
            node->ownerScope->owner->allocateExtension();
            for (auto r : node->extension->registersToRelease)
                node->ownerScope->owner->extension->registersToRelease.push_back(r);
        }

        node->extension->registersToRelease.clear();
    }

    // Be sure this is done only once
    node->flags |= AST_NO_BYTECODE_CHILDS;
    return true;
}

bool ByteCodeGenJob::emitIf(ByteCodeGenContext* context)
{
    auto ifNode = CastAst<AstIf>(context->node, AstNodeKind::If);

    // Resolve ByteCodeOp::JumpIfFalse expression
    auto instruction = context->bc->out + ifNode->seekJumpExpression;
    auto diff        = ifNode->seekJumpAfterIf - ifNode->seekJumpExpression;

    // Resolve ByteCodeOp::Jump expression, after the if block
    if (ifNode->elseBlock)
    {
        instruction->b.s32 = diff;

        instruction        = context->bc->out + ifNode->seekJumpAfterIf;
        diff               = context->bc->numInstructions - ifNode->seekJumpAfterIf;
        instruction->b.s32 = diff - 1;
    }
    else
        instruction->b.s32 = diff - 1;

    return true;
}

bool ByteCodeGenJob::emitIfAfterExpr(ByteCodeGenContext* context)
{
    auto node   = context->node;
    auto ifNode = CastAst<AstIf>(node->parent, AstNodeKind::If);

    SWAG_CHECK(emitCast(context, node, node->typeInfo, node->castedTypeInfo));
    if (context->result != ContextResult::Done)
        return true;

    ifNode->seekJumpExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::JumpIfFalse, node->resultRegisterRC);
    freeRegisterRC(context, node);
    return true;
}

bool ByteCodeGenJob::emitIfAfterIf(ByteCodeGenContext* context)
{
    auto node = context->node;

    SWAG_CHECK(emitLeaveScope(context, node->ownerScope));
    if (context->result != ContextResult::Done)
        return true;

    // This is the end of the if block. Need to jump after the else block, if there's one
    PushLocation pl(context, &node->token.endLocation);
    auto         ifNode     = CastAst<AstIf>(node->parent, AstNodeKind::If);
    ifNode->seekJumpAfterIf = context->bc->numInstructions;
    if (ifNode->elseBlock)
        emitInstruction(context, ByteCodeOp::Jump);

    return true;
}

bool ByteCodeGenJob::emitLoop(ByteCodeGenContext* context)
{
    auto node = static_cast<AstBreakable*>(context->node);

    // Resolve ByteCodeOp::JumpIfFalse expression
    auto instruction   = context->bc->out + node->seekJumpExpression;
    auto diff          = node->seekJumpAfterBlock - node->seekJumpExpression;
    instruction->b.s32 = diff - 1;

    if (!node->hasSpecialFuncCall())
    {
        switch (node->kind)
        {
        case AstNodeKind::Loop:
        {
            auto loopNode = CastAst<AstLoop>(node, AstNodeKind::Loop);
            if (loopNode->expression)
            {
                if (loopNode->expression->kind == AstNodeKind::Range)
                {
                    auto rangeNode = CastAst<AstRange>(loopNode->expression, AstNodeKind::Range);
                    freeRegisterRC(context, rangeNode->expressionLow);
                    freeRegisterRC(context, rangeNode->expressionUp);
                }
                else
                    freeRegisterRC(context, loopNode->expression);
            }
            break;
        }

        case AstNodeKind::While:
            freeRegisterRC(context, ((AstWhile*) node)->boolExpression);
            break;
        case AstNodeKind::For:
            freeRegisterRC(context, ((AstFor*) node)->boolExpression);
            break;
        }
    }

    freeRegisterRC(context, node);
    if (node->needIndex())
        freeRegisterRC(context, node->registerIndex);
    if (node->needIndex1())
        freeRegisterRC(context, node->registerIndex1);

    return true;
}

bool ByteCodeGenJob::emitLoopBeforeBlock(ByteCodeGenContext* context)
{
    auto node               = context->node;
    auto loopNode           = CastAst<AstLoop>(node->parent, AstNodeKind::Loop);
    loopNode->registerIndex = reserveRegisterRC(context);
    loopNode->breakableFlags |= BREAKABLE_NEED_INDEX;

    emitInstruction(context, ByteCodeOp::SetImmediate64, loopNode->registerIndex)->b.s64 = -1;

    loopNode->seekJumpBeforeExpression = context->bc->numInstructions;
    loopNode->seekJumpBeforeContinue   = loopNode->seekJumpBeforeExpression;

    emitInstruction(context, ByteCodeOp::IncrementRA64, loopNode->registerIndex);

    return true;
}

bool ByteCodeGenJob::emitLoopAfterExpr(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto loopNode = CastAst<AstLoop>(node->parent, AstNodeKind::Loop);

    // User special function
    if (loopNode->hasSpecialFuncCall())
    {
        SWAG_CHECK(emitUserOp(context, nullptr, loopNode));
        if (context->result != ContextResult::Done)
            return true;

        // If opCount has been inlined, then the register of the inline block contains the result
        if (loopNode->childs.back()->kind == AstNodeKind::Inline)
            node->resultRegisterRC = loopNode->childs.back()->resultRegisterRC;
    }

    SWAG_CHECK(emitCast(context, node, node->typeInfo, node->castedTypeInfo));

    // To store the 'index' of the loop
    loopNode->registerIndex = reserveRegisterRC(context);
    loopNode->breakableFlags |= BREAKABLE_NEED_INDEX;

    if (loopNode->expression->kind != AstNodeKind::Range)
    {
        emitInstruction(context, ByteCodeOp::SetImmediate64, loopNode->registerIndex)->b.s64 = -1;

        loopNode->seekJumpBeforeExpression = context->bc->numInstructions;
        loopNode->seekJumpBeforeContinue   = loopNode->seekJumpBeforeExpression;
        emitInstruction(context, ByteCodeOp::IncrementRA64, loopNode->registerIndex);
        loopNode->seekJumpExpression = context->bc->numInstructions;
        emitInstruction(context, ByteCodeOp::JumpIfEqual64, loopNode->registerIndex, 0, node->resultRegisterRC);
    }
    else
    {
        auto rangeNode           = CastAst<AstRange>(loopNode->expression, AstNodeKind::Range);
        loopNode->registerIndex1 = reserveRegisterRC(context);
        loopNode->breakableFlags |= BREAKABLE_NEED_INDEX1;

        // registerIndex1 contains the increment to the index (-1 or 1)
        emitInstruction(context, ByteCodeOp::SetImmediate64, loopNode->registerIndex1)->b.s64 = 1;
        emitInstruction(context, ByteCodeOp::JumpIfLowerEqS64, rangeNode->expressionLow->resultRegisterRC, 1, rangeNode->expressionUp->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::SetImmediate64, loopNode->registerIndex1)->b.s64 = -1;

        emitInstruction(context, ByteCodeOp::CopyRBtoRA64, loopNode->registerIndex, rangeNode->expressionLow->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::BinOpMinusS64, loopNode->registerIndex, loopNode->registerIndex1, loopNode->registerIndex);

        loopNode->seekJumpBeforeExpression = context->bc->numInstructions;
        loopNode->seekJumpBeforeContinue   = loopNode->seekJumpBeforeExpression;
        loopNode->seekJumpExpression       = context->bc->numInstructions;
        emitInstruction(context, ByteCodeOp::JumpIfEqual64, loopNode->registerIndex, 0, rangeNode->expressionUp->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::BinOpPlusS64, loopNode->registerIndex, loopNode->registerIndex1, loopNode->registerIndex);
    }

    return true;
}

bool ByteCodeGenJob::emitLabelBeforeBlock(ByteCodeGenContext* context)
{
    auto node                        = context->node;
    auto loopNode                    = static_cast<AstBreakable*>(node->parent);
    loopNode->seekJumpBeforeContinue = context->bc->numInstructions;
    return true;
}

bool ByteCodeGenJob::emitLoopAfterBlock(ByteCodeGenContext* context)
{
    auto         node = context->node;
    PushLocation pl(context, &node->token.endLocation);

    SWAG_CHECK(emitLeaveScope(context, node->ownerScope));
    if (context->result != ContextResult::Done)
        return true;

    auto loopNode = static_cast<AstBreakable*>(node->parent);

    if (node->parent->kind != AstNodeKind::LabelBreakable)
    {
        auto inst   = emitInstruction(context, ByteCodeOp::Jump);
        auto diff   = loopNode->seekJumpBeforeContinue - context->bc->numInstructions;
        inst->b.s32 = diff;
    }

    loopNode->seekJumpAfterBlock = context->bc->numInstructions;

    // Resolve all continue instructions
    for (auto continueNode : loopNode->continueList)
    {
        auto inst   = context->bc->out + continueNode->jumpInstruction;
        auto diff   = loopNode->seekJumpBeforeContinue - continueNode->jumpInstruction - 1;
        inst->b.s32 = diff;
    }

    // Resolve all break instructions
    for (auto breakNode : loopNode->breakList)
    {
        auto inst   = context->bc->out + breakNode->jumpInstruction;
        auto diff   = context->bc->numInstructions - breakNode->jumpInstruction - 1;
        inst->b.s32 = diff;
    }

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
        auto inst                = emitInstruction(context, ByteCodeOp::SetImmediate64, whileNode->registerIndex);
        inst->b.s64              = -1;
    }

    whileNode->seekJumpBeforeExpression = context->bc->numInstructions;
    whileNode->seekJumpBeforeContinue   = whileNode->seekJumpBeforeExpression;
    return true;
}

bool ByteCodeGenJob::emitWhileAfterExpr(ByteCodeGenContext* context)
{
    auto node = context->node;
    SWAG_CHECK(emitCast(context, node, node->typeInfo, node->castedTypeInfo));

    auto whileNode                = CastAst<AstWhile>(node->parent, AstNodeKind::While);
    whileNode->seekJumpExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::JumpIfFalse, node->resultRegisterRC);

    // Increment the index
    if (whileNode->needIndex())
        emitInstruction(context, ByteCodeOp::IncrementRA64, whileNode->registerIndex);

    return true;
}

bool ByteCodeGenJob::emitForBeforeExpr(ByteCodeGenContext* context)
{
    auto node    = context->node;
    auto forNode = CastAst<AstFor>(node->parent, AstNodeKind::For);

    // Set the jump to the start of the expression
    auto inst   = context->bc->out + forNode->seekJumpToExpression;
    auto diff   = context->bc->numInstructions - forNode->seekJumpToExpression - 1;
    inst->b.s32 = diff;

    return true;
}

bool ByteCodeGenJob::emitForAfterExpr(ByteCodeGenContext* context)
{
    auto node    = context->node;
    auto forNode = CastAst<AstFor>(node->parent, AstNodeKind::For);

    forNode->seekJumpExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::JumpIfFalse, node->resultRegisterRC);

    return true;
}

bool ByteCodeGenJob::emitForBeforePost(ByteCodeGenContext* context)
{
    auto node    = context->node;
    auto forNode = CastAst<AstFor>(node->parent, AstNodeKind::For);

    // To store the 'index' of the loop
    if (forNode->needIndex())
    {
        forNode->registerIndex = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::ClearRA, forNode->registerIndex);
    }

    // Jump to the test expression
    forNode->seekJumpToExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::Jump);

    // This is the start of the post statement
    forNode->seekJumpBeforeExpression = context->bc->numInstructions;
    forNode->seekJumpBeforeContinue   = forNode->seekJumpBeforeExpression;

    // Increment the index
    if (forNode->needIndex())
        emitInstruction(context, ByteCodeOp::IncrementRA64, forNode->registerIndex);

    return true;
}

bool ByteCodeGenJob::emitSwitch(ByteCodeGenContext* context)
{
    auto node       = context->node;
    auto switchNode = CastAst<AstSwitch>(node, AstNodeKind::Switch);

    if (switchNode->expression)
        freeRegisterRC(context, switchNode->expression->resultRegisterRC);

    // Resolve the jump to go outside the switch
    auto inst   = context->bc->out + switchNode->seekJumpExpression;
    auto diff   = context->bc->numInstructions - switchNode->seekJumpExpression - 1;
    inst->b.s32 = diff;

    // Resolve all break instructions
    for (auto breakNode : switchNode->breakList)
    {
        inst        = context->bc->out + breakNode->jumpInstruction;
        diff        = context->bc->numInstructions - breakNode->jumpInstruction - 1;
        inst->b.s32 = diff;
    }

    // Resolve all fallthrough instructions
    for (auto fallNode : switchNode->fallThroughList)
    {
        SWAG_ASSERT(fallNode->switchCase);
        SWAG_ASSERT(fallNode->switchCase->caseIndex < switchNode->cases.size() - 1);

        auto nextCase      = switchNode->cases[fallNode->switchCase->caseIndex + 1];
        auto nextCaseBlock = CastAst<AstSwitchCaseBlock>(nextCase->block, AstNodeKind::Statement);

        inst        = context->bc->out + fallNode->jumpInstruction;
        diff        = nextCaseBlock->seekStart - fallNode->jumpInstruction - 1;
        inst->b.s32 = diff;
    }

    return true;
}

bool ByteCodeGenJob::emitBeforeSwitch(ByteCodeGenContext* context)
{
    auto node       = context->node;
    auto switchNode = CastAst<AstSwitch>(node, AstNodeKind::Switch);

    // Jump to the first case
    emitInstruction(context, ByteCodeOp::Jump)->b.s32 = 1;

    // Jump to exit the switch
    switchNode->seekJumpExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::Jump);
    return true;
}

bool ByteCodeGenJob::emitSwitchAfterExpr(ByteCodeGenContext* context)
{
    auto node       = context->node;
    auto switchNode = CastAst<AstSwitch>(node->parent, AstNodeKind::Switch);

    // Jump to the first case
    emitInstruction(context, ByteCodeOp::Jump)->b.s32 = 1;

    // Jump to exit the switch
    switchNode->seekJumpExpression = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::Jump);
    return true;
}

bool ByteCodeGenJob::emitSwitchCaseBeforeCase(ByteCodeGenContext* context)
{
    auto node = CastAst<AstSwitchCase>(context->node, AstNodeKind::SwitchCase);
    context->pushLocation(&node->ownerSwitch->token.startLocation);
    return true;
}

bool ByteCodeGenJob::emitSwitchCaseBeforeBlock(ByteCodeGenContext* context)
{
    auto node      = context->node;
    auto blockNode = CastAst<AstSwitchCaseBlock>(node, AstNodeKind::Statement);
    auto caseNode  = blockNode->ownerCase;

    if (caseNode->ownerSwitch->expression)
        caseNode->ownerSwitch->resultRegisterRC = caseNode->ownerSwitch->expression->resultRegisterRC;

    // Default case does not have expressions
    VectorNative<uint32_t> allJumps;
    if (!caseNode->expressions.empty())
    {
        // Normal switch, with an expression
        if (caseNode->ownerSwitch->expression)
        {
            for (auto expr : caseNode->expressions)
            {
                RegisterList r0;
                if (expr->kind == AstNodeKind::Range)
                {
                    r0 = reserveRegisterRC(context);
                    SWAG_CHECK(emitInRange(context, caseNode, expr, caseNode->ownerSwitch->resultRegisterRC, r0));
                    if (context->result != ContextResult::Done)
                    {
                        freeRegisterRC(context, r0);
                        return true;
                    }
                }
                else if (caseNode->hasSpecialFuncCall())
                {
                    SWAG_CHECK(emitCompareOpSpecialFunc(context, caseNode, expr, caseNode->ownerSwitch->resultRegisterRC, expr->resultRegisterRC, TokenId::SymEqualEqual));
                    if (context->result != ContextResult::Done)
                        return true;
                    r0 = node->resultRegisterRC;
                }
                else
                {
                    r0 = reserveRegisterRC(context);
                    SWAG_CHECK(emitCompareOpEqual(context, caseNode, expr, caseNode->ownerSwitch->resultRegisterRC, expr->resultRegisterRC, r0));
                }

                allJumps.push_back(context->bc->numInstructions);
                auto inst   = emitInstruction(context, ByteCodeOp::JumpIfTrue, r0);
                inst->b.u64 = context->bc->numInstructions; // Remember start of the jump, to compute the relative offset
                freeRegisterRC(context, r0);
            }
        }

        // A switch without an expression
        else
        {
            for (auto expr : caseNode->expressions)
            {
                allJumps.push_back(context->bc->numInstructions);
                auto inst   = emitInstruction(context, ByteCodeOp::JumpIfTrue, expr->resultRegisterRC);
                inst->b.u64 = context->bc->numInstructions; // Remember start of the jump, to compute the relative offset
            }
        }

        for (auto expr : caseNode->expressions)
            freeRegisterRC(context, expr->resultRegisterRC);

        // Jump to the next case, except for the default, which is the last
        blockNode->seekJumpNextCase = context->bc->numInstructions;
        emitInstruction(context, ByteCodeOp::Jump);

        // Save start of the case
        blockNode->seekStart = context->bc->numInstructions;

        // Now this is the beginning of the block, so we can resolve all Jump
        for (auto jumpIdx : allJumps)
        {
            ByteCodeInstruction* jump = context->bc->out + jumpIdx;
            jump->b.s32               = context->bc->numInstructions - jump->b.u32;
        }

        // Pop the location from emitSwitchCaseBeforeCase
        context->popLocation();
    }

    return true;
}

bool ByteCodeGenJob::emitSwitchCaseAfterBlock(ByteCodeGenContext* context)
{
    auto node = context->node;
    SWAG_CHECK(emitLeaveScope(context, node->ownerScope));
    if (context->result != ContextResult::Done)
        return true;

    auto blockNode = CastAst<AstSwitchCaseBlock>(node, AstNodeKind::Statement);

    // For the default case, do nothing, fallback to the end of the switch
    if (blockNode->ownerCase->specFlags & AST_SPEC_SWITCHCASE_ISDEFAULT)
        return true;

    // Jump to exit the switch
    context->setNoLocation();
    auto inst   = emitInstruction(context, ByteCodeOp::Jump);
    inst->b.s32 = blockNode->ownerCase->ownerSwitch->seekJumpExpression - context->bc->numInstructions;
    context->restoreNoLocation();

    // Resolve jump from case to case
    inst        = context->bc->out + blockNode->seekJumpNextCase;
    inst->b.s32 = context->bc->numInstructions - blockNode->seekJumpNextCase - 1;
    return true;
}

bool ByteCodeGenJob::emitFallThrough(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto fallNode = CastAst<AstBreakContinue>(node, AstNodeKind::FallThrough);

    Scope::collectScopeFromToExcluded(fallNode->ownerScope, fallNode->ownerBreakable->ownerScope, context->job->collectScopes);
    for (auto scope : context->job->collectScopes)
    {
        SWAG_CHECK(emitLeaveScope(context, scope));
        if (context->result != ContextResult::Done)
            return true;
    }

    fallNode->jumpInstruction = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::Jump);
    return true;
}

bool ByteCodeGenJob::emitBreak(ByteCodeGenContext* context)
{
    auto node      = context->node;
    auto breakNode = CastAst<AstBreakContinue>(node, AstNodeKind::Break);

    Scope::collectScopeFromToExcluded(breakNode->ownerScope, breakNode->ownerBreakable->ownerScope, context->job->collectScopes);
    for (auto scope : context->job->collectScopes)
    {
        SWAG_CHECK(emitLeaveScope(context, scope));
        if (context->result != ContextResult::Done)
            return true;
    }

    breakNode->jumpInstruction = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::Jump);
    return true;
}

bool ByteCodeGenJob::emitContinue(ByteCodeGenContext* context)
{
    auto node         = context->node;
    auto continueNode = CastAst<AstBreakContinue>(node, AstNodeKind::Continue);

    Scope::collectScopeFromToExcluded(continueNode->ownerScope, continueNode->ownerBreakable->ownerScope, context->job->collectScopes);
    for (auto scope : context->job->collectScopes)
    {
        SWAG_CHECK(emitLeaveScope(context, scope));
        if (context->result != ContextResult::Done)
            return true;
    }

    continueNode->jumpInstruction = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::Jump);
    return true;
}

bool ByteCodeGenJob::emitIndex(ByteCodeGenContext* context)
{
    auto node              = context->node;
    node->resultRegisterRC = reserveRegisterRC(context);

    auto ownerBreakable = node->ownerBreakable;
    while (ownerBreakable && !(ownerBreakable->breakableFlags & BREAKABLE_CAN_HAVE_INDEX))
        ownerBreakable = ownerBreakable->ownerBreakable;
    SWAG_ASSERT(ownerBreakable);

    emitInstruction(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC, ownerBreakable->registerIndex);
    return true;
}

bool ByteCodeGenJob::emitLeaveScopeDrop(ByteCodeGenContext* context, Scope* scope, VectorNative<SymbolOverload*>* forceNoDrop)
{
    if (!scope)
        return true;

    auto&       table = scope->symTable;
    scoped_lock lock(table.mutex);

    auto count = (int) table.structVarsToDrop.size() - 1;
    if (count == -1)
        return true;

    // Need to wait for all the structures to be ok, in order to call the opDrop function
    for (int i = count; i >= 0; i--)
    {
        auto one = table.structVarsToDrop[i];
        if (!one.typeStruct)
            continue;

        context->job->waitStructGenerated(one.typeStruct);
        if (context->result == ContextResult::Pending)
            return true;
    }

    for (int i = count; i >= 0; i--)
    {
        auto one = table.structVarsToDrop[i];
        if (!one.typeStruct)
            continue;
        if (!one.typeStruct->opDrop && !one.typeStruct->opUserDropFct)
            continue;

        // Overload can be registered as "no drop", because of optimization and move semantic
        if (one.overload && forceNoDrop && forceNoDrop->contains(one.overload))
            continue;

        // Need to be sure that the variable is not emitted later, after the leave scope
        if (one.overload && !(one.overload->flags & OVERLOAD_EMITTED))
            continue;

        if (!canEmitOpCallUser(context, one.typeStruct->opUserDropFct, one.typeStruct->opDrop))
            continue;

        if (one.typeInfo->kind == TypeInfoKind::Array)
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(one.typeInfo, TypeInfoKind::Array);
            if (typeArray->totalCount == 1)
            {
                RegisterList r1   = reserveRegisterRC(context);
                auto         inst = emitInstruction(context, ByteCodeOp::MakeStackPointer, r1);
                inst->b.u64       = one.storageOffset;
                emitInstruction(context, ByteCodeOp::PushRAParam, r1);
                emitOpCallUser(context, one.typeStruct->opUserDropFct, one.typeStruct->opDrop, false);
                freeRegisterRC(context, r1);
            }
            else
            {
                // Need to loop on every element of the array in order to initialize them
                RegisterList r0 = reserveRegisterRC(context);
                RegisterList r1 = reserveRegisterRC(context);

                emitInstruction(context, ByteCodeOp::SetImmediate64, r0[0])->b.u64 = typeArray->totalCount;

                auto inst     = emitInstruction(context, ByteCodeOp::MakeStackPointer, r1);
                inst->b.u64   = one.storageOffset;
                auto seekJump = context->bc->numInstructions;

                emitInstruction(context, ByteCodeOp::PushRAParam, r1);
                emitOpCallUser(context, one.typeStruct->opUserDropFct, one.typeStruct->opDrop, false);

                inst        = emitInstruction(context, ByteCodeOp::IncPointer64, r1, 0, r1);
                inst->b.u64 = one.typeStruct->sizeOf;
                inst->flags |= BCI_IMM_B;

                emitInstruction(context, ByteCodeOp::DecrementRA64, r0[0]);
                emitInstruction(context, ByteCodeOp::JumpIfNotZero64, r0[0])->b.s32 = seekJump - context->bc->numInstructions - 1;

                freeRegisterRC(context, r0);
                freeRegisterRC(context, r1);
            }
        }
        else
        {
            auto r0     = reserveRegisterRC(context);
            auto inst   = emitInstruction(context, ByteCodeOp::MakeStackPointer, r0);
            inst->b.u64 = one.storageOffset;
            emitInstruction(context, ByteCodeOp::PushRAParam, r0);
            emitOpCallUser(context, one.typeStruct->opUserDropFct, one.typeStruct->opDrop, false);
            freeRegisterRC(context, r0);
        }
    }

    return true;
}

bool ByteCodeGenJob::emitDeferredStatements(ByteCodeGenContext* context, Scope* scope, bool errDefer)
{
    if (!scope)
        return true;

    auto numDeferred = scope->deferredNodes.size();
    if (numDeferred)
    {
        context->result = ContextResult::NewChilds;
        auto job        = context->job;
        for (int i = 0; i < numDeferred; i++)
        {
            auto node = scope->deferredNodes[i];

            // ErrDefer node are emitted only in 'errDefer' mode
            if (!errDefer && node->kind == AstNodeKind::ErrDefer)
                continue;

            auto child           = node->childs.front();
            child->bytecodeState = AstNodeResolveState::Enter;
            child->flags &= ~AST_NO_BYTECODE;
            job->nodes.push_back(child);
        }
    }

    return true;
}

bool ByteCodeGenJob::emitLeaveScopeReturn(ByteCodeGenContext* context, VectorNative<SymbolOverload*>* forceNoDrop, bool errDefer)
{
    auto node     = context->node;
    auto funcNode = node->ownerFct;

    // Leave all scopes
    Scope* topScope = nullptr;
    if (node->ownerInline && ((node->semFlags & AST_SEM_EMBEDDED_RETURN) || node->kind != AstNodeKind::Return))
        topScope = node->ownerInline->scope;
    else
        topScope = funcNode->scope;

    Scope::collectScopeFromToExcluded(node->ownerScope, topScope->parentScope, context->job->collectScopes);
    for (auto scope : context->job->collectScopes)
    {
        SWAG_CHECK(emitLeaveScope(context, scope, forceNoDrop, errDefer));
        if (context->result != ContextResult::Done)
            return true;
    }

    return true;
}

bool ByteCodeGenJob::emitLeaveScope(ByteCodeGenContext* context, Scope* scope, VectorNative<SymbolOverload*>* forceNoDrop, bool errDefer)
{
    PushLocation pl(context, &context->node->token.endLocation);

    // Emit all 'defer' statements
    if (!scope->doneDefer.contains(context->node))
    {
        SWAG_CHECK(emitDeferredStatements(context, scope, errDefer));
        SWAG_ASSERT(context->result != ContextResult::Pending);
        scope->doneDefer.push_back(context->node);
        if (context->result == ContextResult::NewChilds)
            return true;
    }

    // Emit all drops
    if (!scope->doneDrop.contains(context->node))
    {
        SWAG_CHECK(emitLeaveScopeDrop(context, scope, forceNoDrop));
        if (context->result == ContextResult::Pending)
            return true;
        scope->doneDrop.push_back(context->node);
    }

    // Free some registers
    if (context->node->extension)
    {
        for (auto r : context->node->extension->registersToRelease)
            freeRegisterRC(context, r);
        context->node->extension->registersToRelease.clear();
    }

    return true;
}

bool ByteCodeGenJob::emitLeaveScope(ByteCodeGenContext* context)
{
    auto node = context->node;

    switch (node->kind)
    {
    case AstNodeKind::CompilerMacro:
    {
        auto macroNode = CastAst<AstCompilerMacro>(node, AstNodeKind::CompilerMacro);
        SWAG_CHECK(emitLeaveScope(context, macroNode->scope));
        break;
    }
    case AstNodeKind::CompilerInline:
    {
        auto inlineNode = CastAst<AstCompilerInline>(node, AstNodeKind::CompilerInline);
        SWAG_CHECK(emitLeaveScope(context, inlineNode->scope));
        break;
    }
    default:
        SWAG_CHECK(emitLeaveScope(context, node->ownerScope));
        break;
    }

    return true;
}
