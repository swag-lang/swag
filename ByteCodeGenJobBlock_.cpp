#include "pch.h"
#include "LanguageSpec.h"
#include "ByteCodeOp.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "SemanticJob.h"
#include "Ast.h"

bool ByteCodeGenJob::emitInlineBefore(ByteCodeGenContext* context)
{
    auto node = CastAst<AstInline>(context->node, AstNodeKind::Inline);

    // Reserve registers for return value
    reserveRegisterRC(context, node->resultRegisterRC, node->func->returnType->typeInfo->numRegisters());
    node->parent->resultRegisterRC = node->resultRegisterRC;

    // If the inline returns a copy, then initialize the register with the address of the temporary
    // variable on the stack, so that the inline block can copy it's result to it. Of course, this is
    // not the top for speed, but anyway there's room for improvement for inline in all cases.
    auto returnType = node->func->returnType->typeInfo;
    if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
    {
        auto inst   = emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
        inst->b.u32 = node->fctCallStorageOffset;

        // Need to drop the temporary variable
        if (returnType->kind == TypeInfoKind::Struct)
        {
            StructToDrop st;
            st.overload      = nullptr;
            st.typeStruct    = CastTypeInfo<TypeInfoStruct>(returnType, TypeInfoKind::Struct);
            st.storageOffset = node->fctCallStorageOffset;
            node->ownerScope->symTable.addVarToDrop(st);
        }
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
        SWAG_ASSERT(node->parent->resolvedUserOpSymbolOverload);
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
                auto symbol    = node->scope->symTable.find(funcParam->name);
                SWAG_ASSERT(symbol);

                for (auto overload : symbol->overloads)
                {
                    if (overload->flags & OVERLOAD_VAR_INLINE)
                    {
                        overload->registers         = callParam->resultRegisterRC;
                        overload->registers.canFree = false;
                        node->scope->registersToRelease += overload->registers;
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
                    if (callParam->index == i)
                    {
                        auto symbol = node->scope->symTable.find(funcParam->name);
                        SWAG_ASSERT(symbol);
                        for (auto overload : symbol->overloads)
                        {
                            if (overload->flags & OVERLOAD_VAR_INLINE)
                            {
                                overload->registers         = callParam->resultRegisterRC;
                                overload->registers.canFree = false;
                                node->scope->registersToRelease += overload->registers;
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

                    auto symbol = node->scope->symTable.find(defaultParam->name);
                    SWAG_ASSERT(symbol);
                    for (auto overload : symbol->overloads)
                    {
                        if (overload->flags & OVERLOAD_VAR_INLINE)
                        {
                            SWAG_CHECK(emitDefaultParamValue(context, defaultParam, overload->registers));
                            overload->registers.canFree = false;
                            node->scope->registersToRelease += overload->registers;
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

    SWAG_CHECK(emitLeaveScope(context, node->scope));
    if (context->result != ContextResult::Done)
        return true;

    // Update all returns to jump at the end of the inline block
    for (auto r : node->returnList)
        context->bc->out[r->seekJump].b.s32 = context->bc->numInstructions - r->seekJump - 1;

    // Release persistent list of registers
    freeRegisterRC(context, node->scope->registersToRelease);

    // Be sure this is done only once
    node->flags |= AST_NO_BYTECODE;

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
    auto ifNode             = CastAst<AstIf>(node->parent, AstNodeKind::If);
    ifNode->seekJumpAfterIf = context->bc->numInstructions;
    if (ifNode->elseBlock)
        emitInstruction(context, ByteCodeOp::Jump);

    return true;
}

bool ByteCodeGenJob::emitLoop(ByteCodeGenContext* context)
{
    auto loopNode = static_cast<AstBreakable*>(context->node);

    // Resolve ByteCodeOp::JumpIfFalse expression
    auto instruction   = context->bc->out + loopNode->seekJumpExpression;
    auto diff          = loopNode->seekJumpAfterBlock - loopNode->seekJumpExpression;
    instruction->b.s32 = diff - 1;

    freeRegisterRC(context, loopNode);
    return true;
}

bool ByteCodeGenJob::emitLoopAfterExpr(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto loopNode = CastAst<AstLoop>(node->parent, AstNodeKind::Loop);

    if (loopNode->resolvedUserOpSymbolName && loopNode->resolvedUserOpSymbolName->kind == SymbolKind::Function)
    {
        SWAG_CHECK(emitUserOp(context, nullptr, loopNode));
        if (context->result != ContextResult::Done)
            return true;

        // If opCount has been inlined, then the register of the inline block contains the result
        if (loopNode->childs.back()->kind == AstNodeKind::Inline)
        {
            node->resultRegisterRC = loopNode->childs.back()->resultRegisterRC;
        }
    }

    // To store the 'index' of the loop
    if (loopNode->needIndex())
    {
        loopNode->registerIndex = reserveRegisterRC(context);
        auto inst               = emitInstruction(context, ByteCodeOp::SetImmediate32, loopNode->registerIndex);
        inst->b.s32             = -1;
    }

    loopNode->seekJumpBeforeExpression = context->bc->numInstructions;
    loopNode->seekJumpBeforeContinue   = loopNode->seekJumpBeforeExpression;
    loopNode->seekJumpExpression       = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::JumpIfZero32, node->resultRegisterRC);

    // Decrement the loop variable
    emitInstruction(context, ByteCodeOp::DecrementRA32, node->resultRegisterRC);

    // Increment the index
    if (loopNode->needIndex())
    {
        emitInstruction(context, ByteCodeOp::IncrementRA32, loopNode->registerIndex);
    }

    return true;
}

bool ByteCodeGenJob::emitLoopAfterBlock(ByteCodeGenContext* context)
{
    auto         node = context->node;
    PushLocation pl(context, &node->parent->token.startLocation);

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
        auto inst                = emitInstruction(context, ByteCodeOp::SetImmediate32, whileNode->registerIndex);
        inst->b.s32              = -1;
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
    {
        emitInstruction(context, ByteCodeOp::IncrementRA32, whileNode->registerIndex);
    }

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
        auto inst              = emitInstruction(context, ByteCodeOp::SetImmediate32, forNode->registerIndex);
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
    emitInstruction(context, ByteCodeOp::JumpIfFalse, node->resultRegisterRC);

    // Increment the index
    if (forNode->needIndex())
    {
        emitInstruction(context, ByteCodeOp::IncrementRA32, forNode->registerIndex);
    }

    // Jump to the block
    forNode->seekJumpToBlock = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::Jump);

    // This is the start of the post statement
    forNode->seekJumpBeforePost     = context->bc->numInstructions;
    forNode->seekJumpBeforeContinue = forNode->seekJumpBeforePost;
    return true;
}

bool ByteCodeGenJob::emitForAfterPost(ByteCodeGenContext* context)
{
    auto node    = context->node;
    auto forNode = CastAst<AstFor>(node->parent, AstNodeKind::For);

    // Jump to the test expression
    auto inst   = emitInstruction(context, ByteCodeOp::Jump);
    inst->b.s32 = forNode->seekJumpBeforeExpression - context->bc->numInstructions;

    // And set the jump to the start of the block
    inst        = context->bc->out + forNode->seekJumpToBlock;
    auto diff   = context->bc->numInstructions - forNode->seekJumpToBlock - 1;
    inst->b.s32 = diff;

    return true;
}

bool ByteCodeGenJob::emitSwitch(ByteCodeGenContext* context)
{
    auto node       = context->node;
    auto switchNode = CastAst<AstSwitch>(node, AstNodeKind::Switch);

    freeRegisterRC(context, switchNode->expression->resultRegisterRC);

    // Resolve the jump to go outside the switch
    auto inst   = context->bc->out + switchNode->seekJumpExpression;
    auto diff   = context->bc->numInstructions - switchNode->seekJumpExpression - 1;
    inst->b.s32 = diff;

    // Set location to be the same as the next instructions
    inst->flags |= BCI_LOCATION_IS_BC;
    inst->locationBC = context->bc->numInstructions;

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

bool ByteCodeGenJob::emitSwitchCaseBeforeBlock(ByteCodeGenContext* context)
{
    auto node      = context->node;
    auto blockNode = CastAst<AstSwitchCaseBlock>(node, AstNodeKind::Statement);
    auto caseNode  = blockNode->ownerCase;

    caseNode->ownerSwitch->resultRegisterRC = caseNode->ownerSwitch->expression->resultRegisterRC;

    // Default case does not have expressions
    VectorNative<uint32_t> allJumps;
    if (!caseNode->expressions.empty())
    {
        context->setNoLocation();

        RegisterList r0 = reserveRegisterRC(context);
        for (auto expr : caseNode->expressions)
        {
            SWAG_CHECK(emitCompareOpEqual(context, caseNode, expr, caseNode->ownerSwitch->resultRegisterRC, expr->resultRegisterRC, r0));
            allJumps.push_back(context->bc->numInstructions);
            auto inst   = emitInstruction(context, ByteCodeOp::JumpIfTrue, r0);
            inst->b.u32 = context->bc->numInstructions; // Remember start of the jump, to compute the relative offset
        }

        freeRegisterRC(context, r0);
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

        context->restoreNoLocation();
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
    if (blockNode->ownerCase->isDefault)
        return true;

    // Jump to exit the switch
    auto inst   = emitInstruction(context, ByteCodeOp::Jump);
    inst->b.s32 = blockNode->ownerCase->ownerSwitch->seekJumpExpression - context->bc->numInstructions;

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
    emitInstruction(context, ByteCodeOp::CopyRBtoRA, node->resultRegisterRC, node->ownerBreakable->registerIndex);
    return true;
}

bool ByteCodeGenJob::emitLeaveScopeDrop(ByteCodeGenContext* context, Scope* scope, VectorNative<SymbolOverload*>* forceNoDrop)
{
    if (!scope)
        return true;

    auto&       table = scope->symTable;
    scoped_lock lock(table.mutex);

    // Need to wait for the structure to be ok, in order to call the opDrop function
    auto count = (int) table.structVarsToDrop.size() - 1;
    if (count == -1)
        return true;

    for (int i = count; i >= 0; i--)
    {
        auto one = table.structVarsToDrop[i];
        if (!one.typeStruct)
            continue;

        waitStructGenerated(context, one.typeStruct);
        if (context->result == ContextResult::Pending)
            return true;
    }

    for (int i = count; i >= 0; i--)
    {
        auto one = table.structVarsToDrop[i];
        if (!one.typeStruct)
            continue;
        if (!one.typeStruct->opDrop)
            continue;

        // Overload can be registered as "no drop", because of optimization and move semantic
        if (one.overload && forceNoDrop && forceNoDrop->contains(one.overload))
            continue;

        // Need to be sure that the variable is not emitted later, after the leave scope
        if (one.overload && !(one.overload->flags & OVERLOAD_EMITTED))
            continue;

        auto r0     = reserveRegisterRC(context);
        auto inst   = emitInstruction(context, ByteCodeOp::MakeStackPointer, r0);
        inst->b.u32 = one.storageOffset;
        emitInstruction(context, ByteCodeOp::PushRAParam, r0);
        emitOpCallUser(context, one.typeStruct->opUserDropFct, one.typeStruct->opDrop, false);
        freeRegisterRC(context, r0);
    }

    return true;
}

bool ByteCodeGenJob::emitDeferredStatements(ByteCodeGenContext* context, Scope* scope)
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
            auto child           = scope->deferredNodes[i];
            child->bytecodeState = AstNodeResolveState::Enter;
            child->flags &= ~AST_NO_BYTECODE;
            job->nodes.push_back(child);
        }
    }

    return true;
}

bool ByteCodeGenJob::emitLeaveScope(ByteCodeGenContext* context, Scope* scope, VectorNative<SymbolOverload*>* forceNoDrop)
{
    // Emit all 'defer' statements
    if (scope->doneDefer.find(context->node) == scope->doneDefer.end())
    {
        SWAG_CHECK(emitDeferredStatements(context, scope));
        SWAG_ASSERT(context->result != ContextResult::Pending);
        scope->doneDefer.insert(context->node);
        if (context->result == ContextResult::NewChilds)
            return true;
    }

    // Emit all drops
    if (scope->doneDrop.find(context->node) == scope->doneDrop.end())
    {
        SWAG_CHECK(emitLeaveScopeDrop(context, scope, forceNoDrop));
        if (context->result == ContextResult::Pending)
            return true;
        scope->doneDrop.insert(context->node);
    }

    return true;
}

bool ByteCodeGenJob::emitLeaveScope(ByteCodeGenContext* context)
{
    auto node = context->node;
    SWAG_CHECK(emitLeaveScope(context, node->ownerScope));
    return true;
}
