#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool ByteCodeGen::emitInlineBefore(ByteCodeGenContext* context)
{
    emitDebugNop(context);

    const auto node         = castAst<AstInline>(context->node, AstNodeKind::Inline);
    const auto typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(node->func->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
    const auto returnType   = typeInfoFunc->concreteReturnType();

    // Missing try/catch...
    auto parent = node->parent;
    if (parent->is(AstNodeKind::Identifier) && parent->parent)
        parent = parent->parent;
    if (parent->is(AstNodeKind::IdentifierRef) && parent->parent)
        parent = parent->parent;
    SWAG_CHECK(checkCatchError(context, node->func->typeInfo->declNode, node, node->func, parent, node->func->typeInfo));

    // Reserve registers for return value
    reserveRegisterRC(context, node->resultRegisterRc, node->func->returnType->typeInfo->numRegisters());
    node->parent->resultRegisterRc = node->resultRegisterRc;

    // If the inline returns a copy, then initialize the register with the address of the temporary
    // variable on the stack, so that the inline block can copy its result to it. Of course, this is
    // not the top for speed, but anyway there's room for improvement for inline in all cases.
    if (returnType->isStruct() || CallConv::returnByStackAddress(typeInfoFunc))
    {
        const auto inst = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRc);
        inst->b.u64     = node->computedValue()->storageOffset;
        node->ownerScope->symTable.addVarToDrop(nullptr, returnType, node->computedValue()->storageOffset);
    }

    AstNode* allParams        = nullptr;
    uint32_t numCallParams    = 0;
    bool     canFreeRegParams = true;
    parent                    = node->parent;

    AstNode parameters;
    Ast::constructNode(&parameters);
    parameters.inheritTokenLocation(parent->token);
    parameters.inheritOwners(parent);

    if (parent->is(AstNodeKind::ArrayPointerIndex) || parent->is(AstNodeKind::ArrayPointerSlicing))
    {
        allParams = parent;
        SWAG_ASSERT(!allParams->children.empty());
        numCallParams = allParams->childCount() - 1; // Remove the inline block
        while (parent->is(AstNodeKind::ArrayPointerIndex) || parent->is(AstNodeKind::ArrayPointerSlicing))
            parent = parent->parent;
        parent->resultRegisterRc = node->resultRegisterRc;
    }
    else if (parent->is(AstNodeKind::Identifier))
    {
        const auto identifier                         = castAst<AstIdentifier>(parent, AstNodeKind::Identifier);
        identifier->identifierRef()->resultRegisterRc = node->resultRegisterRc;
        allParams                                     = identifier->callParameters;
        numCallParams                                 = allParams ? allParams->childCount() : 0;
    }
    else if (parent->is(AstNodeKind::Loop))
    {
        SWAG_ASSERT(parent->hasSpecialFuncCall(g_LangSpec->name_opCount));
        allParams     = parent;
        numCallParams = 1;
    }
    else if (parent->is(AstNodeKind::AutoSlicingUp))
    {
        SWAG_ASSERT(parent->hasSpecialFuncCall(g_LangSpec->name_opCount));
        const auto slicing   = castAst<AstArrayPointerSlicing>(parent->parent, AstNodeKind::ArrayPointerSlicing);
        const auto arrayNode = slicing->array;
        parameters.children.push_back(arrayNode);
        allParams        = &parameters;
        numCallParams    = parameters.childCount();
        canFreeRegParams = false;
    }
    else if (parent->is(AstNodeKind::SwitchCase) && parent->hasSpecialFuncCall(g_LangSpec->name_opEquals))
    {
        const auto caseNode   = castAst<AstSwitchCase>(parent, AstNodeKind::SwitchCase);
        const auto switchNode = castAst<AstSwitch>(caseNode->ownerSwitch, AstNodeKind::Switch);
        parameters.children.push_back(switchNode->expression);
        parameters.children.push_back(caseNode->firstChild());
        allParams     = &parameters;
        numCallParams = parameters.childCount();
    }
    else if (parent->is(AstNodeKind::SwitchCase))
    {
        const auto caseNode     = castAst<AstSwitchCase>(parent, AstNodeKind::SwitchCase);
        const auto switchNode   = castAst<AstSwitch>(caseNode->ownerSwitch, AstNodeKind::Switch);
        const auto rangeNode    = castAst<AstRange>(caseNode->firstChild(), AstNodeKind::Range);
        const auto block        = castAst<AstSwitchCaseBlock>(caseNode->secondChild(), AstNodeKind::SwitchCaseBlock);
        block->resultRegisterRc = caseNode->resultRegisterRc;
        parameters.children.push_back(switchNode->expression);
        if (caseNode->hasSemFlag(SEMFLAG_TRY_1))
            parameters.children.push_back(!caseNode->hasSemFlag(SEMFLAG_TRY_2) ? rangeNode->expressionUp : rangeNode->expressionLow);
        else
            parameters.children.push_back(!caseNode->hasSemFlag(SEMFLAG_TRY_2) ? rangeNode->expressionLow : rangeNode->expressionUp);
        allParams     = &parameters;
        numCallParams = parameters.childCount();
    }
    else if (parent->is(AstNodeKind::AffectOp) && (parent->hasSpecialFuncCall(g_LangSpec->name_opIndexAffect) || parent->hasSpecialFuncCall(g_LangSpec->name_opIndexAssign)))
    {
        SWAG_ASSERT(parent->firstChild()->is(AstNodeKind::IdentifierRef));
        const auto ptIdx = castAst<AstArrayPointerIndex>(parent->firstChild()->lastChild(), AstNodeKind::ArrayPointerIndex);
        auto       arr   = ptIdx->array;

        auto ptIdx1 = ptIdx;
        while (ptIdx1->array->is(AstNodeKind::ArrayPointerIndex))
        {
            ptIdx1 = castAst<AstArrayPointerIndex>(ptIdx1->array, AstNodeKind::ArrayPointerIndex);
            arr    = ptIdx1->array;
        }

        parameters.children.push_back(arr);
        while (ptIdx1 != ptIdx)
        {
            parameters.children.push_back(ptIdx1->access);
            ptIdx1 = castAst<AstArrayPointerIndex>(ptIdx1->parent, AstNodeKind::ArrayPointerIndex);
        }

        parameters.children.push_back(ptIdx->access);
        parameters.children.push_back(parent->secondChild());
        allParams     = &parameters;
        numCallParams = parameters.childCount();
    }
    else if (parent->hasSpecialFuncCall(g_LangSpec->name_opBinary) ||
             parent->hasSpecialFuncCall(g_LangSpec->name_opAffect) ||
             parent->hasSpecialFuncCall(g_LangSpec->name_opAssign))
    {
        allParams     = parent;
        numCallParams = 2;
    }
    else if (parent->hasSpecialFuncCall(g_LangSpec->name_opUnary))
    {
        allParams     = parent;
        numCallParams = 1;
    }
    else
    {
        allParams     = parent;
        numCallParams = allParams->childCount() - 1; // Remove the inline block
    }

    // Need to Map all call parameters to function arguments
    const auto func = node->func;
    if (func->parameters)
    {
        const auto numFuncParams = func->parameters->childCount();

        // Sort children by parameter index
        Semantic::sortParameters(allParams);

        // Or invert in case of commutative operator
        if (allParams->hasSemFlag(SEMFLAG_INVERSE_PARAMS))
            allParams->swap2Children();

        // Simple case, every parameters are covered by the call, and there's no named param
        if (numFuncParams == numCallParams)
        {
            for (uint32_t i = 0; i < numCallParams; i++)
            {
                const auto funcParam = castAst<AstVarDecl>(func->parameters->children[i], AstNodeKind::FuncDeclParam);
                const auto callParam = allParams->children[i];
                const auto symbol    = node->parametersScope->symTable.find(funcParam->token.text);
                SWAG_ASSERT(symbol);
                for (const auto overload : symbol->overloads)
                {
                    if (overload->hasFlag(OVERLOAD_VAR_INLINE))
                    {
                        overload->setRegisters(callParam->resultRegisterRc, OVERLOAD_INLINE_REG);
                        SWAG_ASSERT(overload->symRegisters.countResults > 0);
                        if (!overload->symRegisters.cannotFree && canFreeRegParams)
                        {
                            overload->symRegisters.cannotFree = true;
                            node->allocateExtension(ExtensionKind::Misc);
                            for (uint32_t r = 0; r < overload->symRegisters.size(); r++)
                                node->extMisc()->registersToRelease.push_back(overload->symRegisters[r]);
                        }
                        break;
                    }
                }
            }
        }
        else
        {
            // Determine if this parameter has been covered by the call
            for (uint32_t i = 0; i < numFuncParams; i++)
            {
                const auto funcParam = castAst<AstVarDecl>(func->parameters->children[i], AstNodeKind::FuncDeclParam);
                bool       covered   = false;
                for (uint32_t j = 0; j < numCallParams; j++)
                {
                    const auto callParam = castAst<AstFuncCallParam>(allParams->children[j], AstNodeKind::FuncCallParam);
                    if (callParam->indexParam == i)
                    {
                        if (callParam->hasSemFlag(SEMFLAG_AUTO_CODE_PARAM))
                        {
                            covered = true;
                            break;
                        }

                        const auto symbol = node->parametersScope->symTable.find(funcParam->token.text);
                        SWAG_ASSERT(symbol);
                        for (const auto overload : symbol->overloads)
                        {
                            if (overload->hasFlag(OVERLOAD_VAR_INLINE))
                            {
                                overload->setRegisters(callParam->resultRegisterRc, OVERLOAD_INLINE_REG);
                                if (!overload->symRegisters.cannotFree)
                                {
                                    SWAG_ASSERT(canFreeRegParams);
                                    overload->symRegisters.cannotFree = true;
                                    node->allocateExtension(ExtensionKind::Misc);
                                    for (uint32_t r = 0; r < overload->symRegisters.size(); r++)
                                        node->extMisc()->registersToRelease.push_back(overload->symRegisters[r]);
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
                    const auto defaultParam = castAst<AstVarDecl>(func->parameters->children[i], AstNodeKind::FuncDeclParam);
                    SWAG_ASSERT(defaultParam->assignment);

                    const auto symbol = node->parametersScope->symTable.find(defaultParam->token.text);
                    SWAG_ASSERT(symbol);
                    for (const auto overload : symbol->overloads)
                    {
                        if (overload->hasFlag(OVERLOAD_VAR_INLINE))
                        {
                            RegisterList rl;
                            SWAG_CHECK(emitDefaultParamValue(context, defaultParam, rl));
                            overload->setRegisters(rl, OVERLOAD_INLINE_REG);

                            SWAG_ASSERT(canFreeRegParams);
                            overload->symRegisters.cannotFree = true;
                            node->allocateExtension(ExtensionKind::Misc);
                            for (uint32_t r = 0; r < overload->symRegisters.size(); r++)
                                node->extMisc()->registersToRelease.push_back(overload->symRegisters[r]);
                            break;
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool ByteCodeGen::emitInline(ByteCodeGenContext* context)
{
    const auto node = castAst<AstInline>(context->node, AstNodeKind::Inline);

    // Update all returns to jump at the end of the inline block
    for (const auto r : node->returnList)
        context->bc->out[r->seekJump].b.s32 = static_cast<int32_t>(context->bc->numInstructions - r->seekJump - 1);

    // If the inlined function returns a reference, and we want a value, we need to unref
    if (node->parent->hasSemFlag(SEMFLAG_FROM_REF) && !node->parent->isForceTakeAddress())
    {
        SWAG_CHECK(emitTypeDeRef(context, node->resultRegisterRc, node->typeInfo));
        node->parent->resultRegisterRc = node->resultRegisterRc;
    }

    // Release persistent list of registers (except if mixin, because in that
    // case, the inline node does not own the scope)
    if (node->hasExtMisc() && !node->extMisc()->registersToRelease.empty())
    {
        if (!node->hasAttribute(ATTRIBUTE_MIXIN))
        {
            for (const auto r : node->extMisc()->registersToRelease)
                freeRegisterRC(context, r);
        }
        else
        {
            // Happens when the mixin is in a constexpr expression, inside a scope which is the real non constexpr code.
            // In that case we need to release our registers, because it's a compile-time execution done.
            if (node->parent->hasAstFlag(AST_CONST_EXPR) != node->ownerScope->owner->hasAstFlag(AST_CONST_EXPR))
            {
                for (const auto r : node->extMisc()->registersToRelease)
                    freeRegisterRC(context, r);
            }
            else
            {
                // Transfers registers to release to the parent scope owner
                node->ownerScope->owner->allocateExtension(ExtensionKind::Misc);
                for (auto r : node->extMisc()->registersToRelease)
                    node->ownerScope->owner->extMisc()->registersToRelease.push_back(r);
            }
        }

        node->extMisc()->registersToRelease.clear();
    }

    // Be sure this is done only once
    // :EmitInlineOnce
    node->addAstFlag(AST_NO_BYTECODE_CHILDREN);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitIf(ByteCodeGenContext* context)
{
    const auto ifNode = castAst<AstIf>(context->node, AstNodeKind::If);

    // Resolve ByteCodeOp::JumpIfFalse expression
    auto instruction = context->bc->out + ifNode->seekJumpExpression;
    auto diff        = ifNode->seekJumpAfterIf - ifNode->seekJumpExpression;

    // Resolve ByteCodeOp::Jump expression, after the if block
    if (ifNode->elseBlock)
    {
        instruction->b.s32 = static_cast<int32_t>(diff);

        instruction        = context->bc->out + ifNode->seekJumpAfterIf;
        diff               = context->bc->numInstructions - ifNode->seekJumpAfterIf;
        instruction->b.s32 = static_cast<int32_t>(diff - 1);
    }
    else
        instruction->b.s32 = static_cast<int32_t>(diff - 1);

    return true;
}

bool ByteCodeGen::emitIfAfterExpr(ByteCodeGenContext* context)
{
    const auto node   = context->node;
    const auto ifNode = castAst<AstIf>(node->parent, AstNodeKind::If);

    SWAG_CHECK(emitCast(context, node, node->typeInfo, node->castedTypeInfo));
    YIELD();

    ifNode->seekJumpExpression = context->bc->numInstructions;
    EMIT_INST1(context, ByteCodeOp::JumpIfFalse, node->resultRegisterRc);
    freeRegisterRC(context, node);
    return true;
}

bool ByteCodeGen::emitIfAfterIf(ByteCodeGenContext* context)
{
    const auto node = context->node;

    SWAG_CHECK(computeLeaveScope(context, node->ownerScope));
    YIELD();

    // This is the end of the if block. Need to jump after the else block, if there's one
    PushLocation pl(context, &node->token.endLocation);
    const auto   ifNode     = castAst<AstIf>(node->parent, AstNodeKind::If);
    ifNode->seekJumpAfterIf = context->bc->numInstructions;
    if (ifNode->elseBlock)
        EMIT_INST0(context, ByteCodeOp::Jump);

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitLoop(ByteCodeGenContext* context)
{
    const auto node = castAst<AstBreakable>(context->node);

    // Resolve ByteCodeOp::Jump expression
    // Be sure this is not an infinite loop without a jump instruction
    bool hasJump = true;
    if (!node->hasSpecialFuncCall() && node->is(AstNodeKind::Loop))
    {
        const auto loopNode = castAst<AstLoop>(node, AstNodeKind::Loop);
        if (!loopNode->expression)
            hasJump = false;
    }

    if (hasJump)
    {
        const auto instruction = context->bc->out + node->seekJumpExpression;
        const auto diff        = node->seekJumpAfterBlock - node->seekJumpExpression;
        instruction->b.s32     = static_cast<int32_t>(diff - 1);
    }

    if (!node->hasSpecialFuncCall())
    {
        switch (node->kind)
        {
            case AstNodeKind::Loop:
            {
                const auto loopNode = castAst<AstLoop>(node, AstNodeKind::Loop);
                if (loopNode->expression)
                {
                    if (loopNode->expression->is(AstNodeKind::Range))
                    {
                        const auto rangeNode = castAst<AstRange>(loopNode->expression, AstNodeKind::Range);
                        freeRegisterRC(context, rangeNode->expressionLow);
                        freeRegisterRC(context, rangeNode->expressionUp);
                    }
                    else
                    {
                        freeRegisterRC(context, loopNode->expression);
                    }
                }
                break;
            }

            case AstNodeKind::While:
            {
                const auto whileNode = castAst<AstWhile>(node, AstNodeKind::While);
                freeRegisterRC(context, whileNode->boolExpression);
                break;
            }
            case AstNodeKind::For:
            {
                const auto forNode = castAst<AstFor>(node, AstNodeKind::For);
                freeRegisterRC(context, forNode->boolExpression);
                break;
            }
            default:
                break;
        }
    }

    freeRegisterRC(context, node);
    if (node->needIndex())
        freeRegisterRC(context, node->registerIndex);

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitLoopBeforeBlock(ByteCodeGenContext* context)
{
    const auto node         = context->node;
    const auto loopNode     = castAst<AstLoop>(node->parent, AstNodeKind::Loop);
    loopNode->registerIndex = reserveRegisterRC(context);
    loopNode->breakableFlags.add(BREAKABLE_NEED_INDEX);

    EMIT_INST1(context, ByteCodeOp::SetImmediate64, loopNode->registerIndex)->b.s64 = -1;

    loopNode->seekJumpBeforeExpression = context->bc->numInstructions;
    loopNode->seekJumpBeforeContinue   = loopNode->seekJumpBeforeExpression;

    EMIT_INST1(context, ByteCodeOp::IncrementRA64, loopNode->registerIndex);

    return true;
}

bool ByteCodeGen::emitLoopAfterExpr(ByteCodeGenContext* context)
{
    const auto node     = context->node;
    const auto loopNode = castAst<AstLoop>(node->parent, AstNodeKind::Loop);

    // User special function
    if (loopNode->hasSpecialFuncCall())
    {
        SWAG_CHECK(emitUserOp(context, nullptr, loopNode));
        YIELD();

        // If opCount has been inlined, then the register of the inline block contains the result
        if (loopNode->lastChild()->is(AstNodeKind::Inline))
            node->resultRegisterRc = loopNode->lastChild()->resultRegisterRc;
    }

    SWAG_CHECK(emitCast(context, node, node->typeInfo, node->castedTypeInfo));
    SWAG_ASSERT(context->result == ContextResult::Done);

    // To store the 'index' of the loop
    loopNode->registerIndex = reserveRegisterRC(context);
    loopNode->breakableFlags.add(BREAKABLE_NEED_INDEX);

    // Normal loop
    if (loopNode->expression->isNot(AstNodeKind::Range))
    {
        if (loopNode->hasSpecFlag(AstLoop::SPEC_FLAG_BACK))
        {
            if (loopNode->expression->hasFlagComputedValue())
            {
                const auto inst = EMIT_INST1(context, ByteCodeOp::SetImmediate64, loopNode->registerIndex);
                inst->b.u64     = loopNode->expression->computedValue()->reg.u64;
            }
            else
            {
                EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, loopNode->registerIndex, loopNode->expression->resultRegisterRc[0]);
            }

            loopNode->seekJumpBeforeExpression = context->bc->numInstructions;
            loopNode->seekJumpBeforeContinue   = loopNode->seekJumpBeforeExpression;
            EMIT_INST1(context, ByteCodeOp::DecrementRA64, loopNode->registerIndex);
            loopNode->seekJumpExpression = context->bc->numInstructions;

            const auto inst = EMIT_INST1(context, ByteCodeOp::JumpIfEqual64, loopNode->registerIndex);
            inst->c.u64     = 0xFFFFFFFF'FFFFFFFF;
            inst->addFlag(BCI_IMM_C);

            return true;
        }

        EMIT_INST1(context, ByteCodeOp::SetImmediate64, loopNode->registerIndex)->b.s64 = -1;

        loopNode->seekJumpBeforeExpression = context->bc->numInstructions;
        loopNode->seekJumpBeforeContinue   = loopNode->seekJumpBeforeExpression;
        EMIT_INST1(context, ByteCodeOp::IncrementRA64, loopNode->registerIndex);
        loopNode->seekJumpExpression = context->bc->numInstructions;

        const auto inst = EMIT_INST3(context, ByteCodeOp::JumpIfEqual64, loopNode->registerIndex, 0, node->resultRegisterRc);
        if (loopNode->expression->hasFlagComputedValue())
        {
            inst->c.u64 = loopNode->expression->computedValue()->reg.u64;
            inst->addFlag(BCI_IMM_C);
        }

        return true;
    }

    // Static range
    const auto rangeNode = castAst<AstRange>(loopNode->expression, AstNodeKind::Range);
    if (rangeNode->expressionLow->hasFlagComputedValue() && rangeNode->expressionUp->hasFlagComputedValue())
    {
        if (rangeNode->expressionLow->computedValue()->reg.s64 > rangeNode->expressionUp->computedValue()->reg.s64)
        {
            Diagnostic err{rangeNode->expressionLow, formErr(Err0224, rangeNode->expressionLow->computedValue()->reg.s64, rangeNode->expressionUp->computedValue()->reg.s64)};
            err.addNote(rangeNode->expressionUp, toNte(Nte0211));
            return context->report(err);
        }

        if (loopNode->hasSpecFlag(AstLoop::SPEC_FLAG_BACK))
        {
            auto inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate64, loopNode->registerIndex);
            inst->b.u64 = rangeNode->expressionUp->computedValue()->reg.u64 + 1;
            if (rangeNode->hasSpecFlag(AstRange::SPEC_FLAG_EXCLUDE_UP))
                inst->b.u64--;

            loopNode->seekJumpBeforeExpression = context->bc->numInstructions;
            loopNode->seekJumpBeforeContinue   = loopNode->seekJumpBeforeExpression;

            EMIT_INST1(context, ByteCodeOp::DecrementRA64, loopNode->registerIndex);
            loopNode->seekJumpExpression = context->bc->numInstructions;

            inst        = EMIT_INST1(context, ByteCodeOp::JumpIfEqual64, loopNode->registerIndex);
            inst->c.u64 = rangeNode->expressionLow->computedValue()->reg.u64 - 1;
            inst->addFlag(BCI_IMM_C);
            return true;
        }

        auto inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate64, loopNode->registerIndex);
        inst->b.u64 = rangeNode->expressionLow->computedValue()->reg.u64 - 1;

        loopNode->seekJumpBeforeExpression = context->bc->numInstructions;
        loopNode->seekJumpBeforeContinue   = loopNode->seekJumpBeforeExpression;

        EMIT_INST1(context, ByteCodeOp::IncrementRA64, loopNode->registerIndex);
        loopNode->seekJumpExpression = context->bc->numInstructions;

        inst        = EMIT_INST1(context, ByteCodeOp::JumpIfEqual64, loopNode->registerIndex);
        inst->c.u64 = rangeNode->expressionUp->computedValue()->reg.u64;
        if (!rangeNode->hasSpecFlag(AstRange::SPEC_FLAG_EXCLUDE_UP))
            inst->c.u64++;
        inst->addFlag(BCI_IMM_C);
        return true;
    }

    SWAG_CHECK(emitCast(context, rangeNode->expressionLow, rangeNode->expressionLow->typeInfo, rangeNode->expressionLow->castedTypeInfo));
    SWAG_ASSERT(context->result == ContextResult::Done);
    SWAG_CHECK(emitCast(context, rangeNode->expressionUp, rangeNode->expressionUp->typeInfo, rangeNode->expressionUp->castedTypeInfo));
    SWAG_ASSERT(context->result == ContextResult::Done);

    emitSafetyRange(context, rangeNode);

    if (loopNode->hasSpecFlag(AstLoop::SPEC_FLAG_BACK))
    {
        EMIT_INST1(context, ByteCodeOp::DecrementRA64, rangeNode->expressionLow->resultRegisterRc[0]);

        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, loopNode->registerIndex, rangeNode->expressionUp->resultRegisterRc[0]);
        if (!rangeNode->hasSpecFlag(AstRange::SPEC_FLAG_EXCLUDE_UP))
            EMIT_INST1(context, ByteCodeOp::IncrementRA64, loopNode->registerIndex);

        loopNode->seekJumpBeforeExpression = context->bc->numInstructions;
        loopNode->seekJumpBeforeContinue   = loopNode->seekJumpBeforeExpression;

        EMIT_INST1(context, ByteCodeOp::DecrementRA64, loopNode->registerIndex);
        loopNode->seekJumpExpression = context->bc->numInstructions;
        EMIT_INST3(context, ByteCodeOp::JumpIfEqual64, loopNode->registerIndex, 0, rangeNode->expressionLow->resultRegisterRc[0]);
        return true;
    }

    if (!rangeNode->hasSpecFlag(AstRange::SPEC_FLAG_EXCLUDE_UP))
        EMIT_INST1(context, ByteCodeOp::IncrementRA64, rangeNode->expressionUp->resultRegisterRc[0]);

    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, loopNode->registerIndex, rangeNode->expressionLow->resultRegisterRc[0]);
    EMIT_INST1(context, ByteCodeOp::DecrementRA64, loopNode->registerIndex);

    loopNode->seekJumpBeforeExpression = context->bc->numInstructions;
    loopNode->seekJumpBeforeContinue   = loopNode->seekJumpBeforeExpression;

    EMIT_INST1(context, ByteCodeOp::IncrementRA64, loopNode->registerIndex);
    loopNode->seekJumpExpression = context->bc->numInstructions;

    EMIT_INST3(context, ByteCodeOp::JumpIfEqual64, loopNode->registerIndex, 0, rangeNode->expressionUp->resultRegisterRc[0]);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitLabelBeforeBlock(ByteCodeGenContext* context)
{
    const auto node                  = context->node;
    const auto loopNode              = castAst<AstBreakable>(node->parent);
    loopNode->seekJumpBeforeContinue = context->bc->numInstructions;
    return true;
}

bool ByteCodeGen::emitLoopAfterBlock(ByteCodeGenContext* context)
{
    const auto   node = context->node;
    PushLocation pl(context, &node->token.endLocation);

    SWAG_CHECK(computeLeaveScope(context, node->ownerScope));
    YIELD();

    const auto loopNode = castAst<AstBreakable>(node->parent);

    if (node->parent->isNot(AstNodeKind::ScopeBreakable))
    {
        const auto inst = EMIT_INST0(context, ByteCodeOp::Jump);
        const auto diff = loopNode->seekJumpBeforeContinue - context->bc->numInstructions;
        inst->b.s32     = static_cast<int32_t>(diff);
    }

    loopNode->seekJumpAfterBlock = context->bc->numInstructions;

    // Resolve all continue instructions
    for (const auto continueNode : loopNode->continueList)
    {
        const auto inst = context->bc->out + continueNode->jumpInstruction;
        const auto diff = loopNode->seekJumpBeforeContinue - continueNode->jumpInstruction - 1;
        inst->b.s32     = static_cast<int32_t>(diff);
    }

    // Resolve all break instructions
    for (const auto breakNode : loopNode->breakList)
    {
        const auto inst = context->bc->out + breakNode->jumpInstruction;
        const auto diff = context->bc->numInstructions - breakNode->jumpInstruction - 1;
        inst->b.s32     = static_cast<int32_t>(diff);
    }

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitWhileBeforeExpr(ByteCodeGenContext* context)
{
    const auto node      = context->node;
    const auto whileNode = castAst<AstWhile>(node->parent, AstNodeKind::While);

    // To store the 'index' of the loop
    if (whileNode->needIndex())
    {
        whileNode->registerIndex = reserveRegisterRC(context);
        const auto inst          = EMIT_INST1(context, ByteCodeOp::SetImmediate64, whileNode->registerIndex);
        inst->b.s64              = -1;
    }

    whileNode->seekJumpBeforeExpression = context->bc->numInstructions;
    whileNode->seekJumpBeforeContinue   = whileNode->seekJumpBeforeExpression;
    return true;
}

bool ByteCodeGen::emitWhileAfterExpr(ByteCodeGenContext* context)
{
    const auto node = context->node;
    SWAG_CHECK(emitCast(context, node, node->typeInfo, node->castedTypeInfo));
    SWAG_ASSERT(context->result == ContextResult::Done);

    const auto whileNode          = castAst<AstWhile>(node->parent, AstNodeKind::While);
    whileNode->seekJumpExpression = context->bc->numInstructions;
    EMIT_INST1(context, ByteCodeOp::JumpIfFalse, node->resultRegisterRc);

    // Increment the index
    if (whileNode->needIndex())
        EMIT_INST1(context, ByteCodeOp::IncrementRA64, whileNode->registerIndex);

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitForBeforeExpr(ByteCodeGenContext* context)
{
    const auto node    = context->node;
    const auto forNode = castAst<AstFor>(node->parent, AstNodeKind::For);

    // Set the jump to the start of the expression
    const auto inst = context->bc->out + forNode->seekJumpToExpression;
    const auto diff = context->bc->numInstructions - forNode->seekJumpToExpression - 1;
    inst->b.s32     = static_cast<int32_t>(diff);

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitForAfterExpr(ByteCodeGenContext* context)
{
    const auto node    = context->node;
    const auto forNode = castAst<AstFor>(node->parent, AstNodeKind::For);

    forNode->seekJumpExpression = context->bc->numInstructions;
    EMIT_INST1(context, ByteCodeOp::JumpIfFalse, node->resultRegisterRc);

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitForBeforePost(ByteCodeGenContext* context)
{
    const auto node    = context->node;
    const auto forNode = castAst<AstFor>(node->parent, AstNodeKind::For);

    // To store the 'index' of the loop
    if (forNode->needIndex())
    {
        forNode->registerIndex = reserveRegisterRC(context);
        EMIT_INST1(context, ByteCodeOp::ClearRA, forNode->registerIndex);
    }

    // Jump to the test expression
    forNode->seekJumpToExpression = context->bc->numInstructions;
    EMIT_INST0(context, ByteCodeOp::Jump);

    // This is the start of the post statement
    forNode->seekJumpBeforeExpression = context->bc->numInstructions;
    forNode->seekJumpBeforeContinue   = forNode->seekJumpBeforeExpression;

    // Increment the index
    if (forNode->needIndex())
        EMIT_INST1(context, ByteCodeOp::IncrementRA64, forNode->registerIndex);

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitSwitch(ByteCodeGenContext* context)
{
    const auto node       = context->node;
    const auto switchNode = castAst<AstSwitch>(node, AstNodeKind::Switch);

    if (switchNode->expression)
        freeRegisterRC(context, switchNode->expression->resultRegisterRc);

    // Resolve the jump to go outside the switch
    auto inst   = context->bc->out + switchNode->seekJumpExpression;
    auto diff   = context->bc->numInstructions - switchNode->seekJumpExpression - 1;
    inst->b.s32 = static_cast<int32_t>(diff);

    // Resolve all break instructions
    for (const auto breakNode : switchNode->breakList)
    {
        inst        = context->bc->out + breakNode->jumpInstruction;
        diff        = context->bc->numInstructions - breakNode->jumpInstruction - 1;
        inst->b.s32 = static_cast<int32_t>(diff);
    }

    // Resolve all fallthrough instructions
    for (const auto fallNode : switchNode->fallThroughList)
    {
        SWAG_ASSERT(fallNode->switchCase);
        SWAG_ASSERT(fallNode->switchCase->caseIndex < switchNode->cases.size() - 1);

        const auto nextCase      = switchNode->cases[fallNode->switchCase->caseIndex + 1];
        const auto nextCaseBlock = castAst<AstSwitchCaseBlock>(nextCase->block, AstNodeKind::SwitchCaseBlock);

        inst        = context->bc->out + fallNode->jumpInstruction;
        diff        = nextCaseBlock->seekStart - fallNode->jumpInstruction - 1;
        inst->b.s32 = static_cast<int32_t>(diff);
    }

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitBeforeSwitch(ByteCodeGenContext* context)
{
    const auto node       = context->node;
    const auto switchNode = castAst<AstSwitch>(node, AstNodeKind::Switch);

    // Jump to the first case
    EMIT_INST0(context, ByteCodeOp::Jump)->b.s32 = 1;

    // Jump to exit the switch
    switchNode->seekJumpExpression = context->bc->numInstructions;
    EMIT_INST0(context, ByteCodeOp::Jump);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitSwitchAfterExpr(ByteCodeGenContext* context)
{
    const auto node       = context->node;
    const auto switchNode = castAst<AstSwitch>(node->parent, AstNodeKind::Switch);

    // Jump to the first case
    EMIT_INST0(context, ByteCodeOp::Jump)->b.s32 = 1;

    // Jump to exit the switch
    switchNode->seekJumpExpression = context->bc->numInstructions;
    EMIT_INST0(context, ByteCodeOp::Jump);
    return true;
}

bool ByteCodeGen::emitSwitchCaseBeforeCase(ByteCodeGenContext* context)
{
    const auto node = castAst<AstSwitchCase>(context->node, AstNodeKind::SwitchCase);
    context->pushLocation(&node->ownerSwitch->token.startLocation);
    return true;
}

bool ByteCodeGen::emitSwitchCaseBeforeBlock(ByteCodeGenContext* context)
{
    const auto node      = context->node;
    const auto blockNode = castAst<AstSwitchCaseBlock>(node, AstNodeKind::SwitchCaseBlock);
    const auto caseNode  = blockNode->ownerCase;

    if (caseNode->ownerSwitch->expression)
        caseNode->ownerSwitch->resultRegisterRc = caseNode->ownerSwitch->expression->resultRegisterRc;

    // Default case does not have expressions
    VectorNative<uint32_t> allJumps;
    if (!caseNode->expressions.empty())
    {
        // Normal switch, with an expression
        if (caseNode->ownerSwitch->expression)
        {
            for (const auto expr : caseNode->expressions)
            {
                RegisterList r0;
                if (expr->is(AstNodeKind::Range))
                {
                    const auto rangeNode = castAst<AstRange>(expr, AstNodeKind::Range);
                    SWAG_CHECK(emitSwitchCaseRange(context, caseNode, rangeNode, r0));
                    YIELD();
                }
                else if (caseNode->hasSpecialFuncCall())
                {
                    SWAG_CHECK(emitSwitchCaseSpecialFunc(context, caseNode, expr, TokenId::SymEqualEqual, r0));
                    YIELD();
                }
                else if (caseNode->hasSpecFlag(AstSwitchCase::SPEC_FLAG_IS_TRUE))
                {
                    r0              = reserveRegisterRC(context);
                    const auto inst = EMIT_INST1(context, ByteCodeOp::SetImmediate64, r0);
                    inst->b.u64     = 1;
                }
                else if (caseNode->hasSpecFlag(AstSwitchCase::SPEC_FLAG_IS_FALSE))
                {
                    r0 = reserveRegisterRC(context);
                    EMIT_INST1(context, ByteCodeOp::ClearRA, r0);
                }
                else
                {
                    r0 = reserveRegisterRC(context);
                    SWAG_CHECK(emitCompareOpEqual(context, caseNode, expr, caseNode->ownerSwitch->resultRegisterRc, expr->resultRegisterRc, r0));
                }

                allJumps.push_back(context->bc->numInstructions);
                const auto inst = EMIT_INST1(context, ByteCodeOp::JumpIfTrue, r0);
                inst->b.u64     = context->bc->numInstructions; // Remember start of the jump, to compute the relative offset
                freeRegisterRC(context, r0);
            }
        }

        // A switch without an expression
        else
        {
            for (const auto expr : caseNode->expressions)
            {
                allJumps.push_back(context->bc->numInstructions);
                const auto inst = EMIT_INST1(context, ByteCodeOp::JumpIfTrue, expr->resultRegisterRc);
                inst->b.u64     = context->bc->numInstructions; // Remember start of the jump, to compute the relative offset
            }
        }

        for (const auto expr : caseNode->expressions)
            freeRegisterRC(context, expr->resultRegisterRc);

        // Jump to the next case, except for the default, which is the last
        blockNode->seekJumpNextCase = context->bc->numInstructions;
        EMIT_INST0(context, ByteCodeOp::Jump);

        // Save start of the case
        blockNode->seekStart = context->bc->numInstructions;

        // Now this is the beginning of the block, so we can resolve all Jump
        for (const auto jumpIdx : allJumps)
        {
            ByteCodeInstruction* jump = context->bc->out + jumpIdx;
            jump->b.s32               = static_cast<int32_t>(context->bc->numInstructions - jump->b.u32);
        }

        // Pop the location from emitSwitchCaseBeforeCase
        context->popLocation();
    }

    return true;
}

bool ByteCodeGen::emitSwitchCaseAfterBlock(ByteCodeGenContext* context)
{
    const auto node = context->node;
    SWAG_CHECK(computeLeaveScope(context, node->ownerScope));
    YIELD();

    const auto blockNode = castAst<AstSwitchCaseBlock>(node, AstNodeKind::SwitchCaseBlock);

    // For the default case, do nothing, fallback to the end of the switch
    if (blockNode->ownerCase->hasSpecFlag(AstSwitchCase::SPEC_FLAG_IS_DEFAULT))
        return true;

    // Jump to exit the switch
    context->setNoLocation();
    auto inst   = EMIT_INST0(context, ByteCodeOp::Jump);
    inst->b.s32 = static_cast<int32_t>(blockNode->ownerCase->ownerSwitch->seekJumpExpression - context->bc->numInstructions);
    context->restoreNoLocation();

    // Resolve jump from case to case
    inst        = context->bc->out + blockNode->seekJumpNextCase;
    inst->b.s32 = static_cast<int32_t>(context->bc->numInstructions - blockNode->seekJumpNextCase - 1);
    return true;
}

bool ByteCodeGen::emitFallThrough(ByteCodeGenContext* context)
{
    const auto node     = context->node;
    const auto fallNode = castAst<AstBreakContinue>(node, AstNodeKind::FallThrough);

    Scope::collectScopeFromToExcluded(fallNode->ownerScope, fallNode->ownerBreakable()->ownerScope, context->collectScopes);
    for (const auto scope : context->collectScopes)
    {
        SWAG_CHECK(computeLeaveScope(context, scope));
        YIELD();
    }

    emitDebugLine(context);
    fallNode->jumpInstruction = context->bc->numInstructions;
    EMIT_INST0(context, ByteCodeOp::Jump);
    return true;
}

bool ByteCodeGen::emitBreak(ByteCodeGenContext* context)
{
    const auto node      = context->node;
    const auto breakNode = castAst<AstBreakContinue>(node, AstNodeKind::Break);

    Scope::collectScopeFromToExcluded(breakNode->ownerScope, breakNode->ownerBreakable()->ownerScope, context->collectScopes);
    for (const auto scope : context->collectScopes)
    {
        SWAG_CHECK(computeLeaveScope(context, scope));
        YIELD();
    }

    emitDebugLine(context);
    breakNode->jumpInstruction = context->bc->numInstructions;
    EMIT_INST0(context, ByteCodeOp::Jump);
    return true;
}

bool ByteCodeGen::emitContinue(ByteCodeGenContext* context)
{
    const auto node         = context->node;
    const auto continueNode = castAst<AstBreakContinue>(node, AstNodeKind::Continue);

    Scope::collectScopeFromToExcluded(continueNode->ownerScope, continueNode->ownerBreakable()->ownerScope, context->collectScopes);
    for (const auto scope : context->collectScopes)
    {
        SWAG_CHECK(computeLeaveScope(context, scope));
        YIELD();
    }

    emitDebugLine(context);
    continueNode->jumpInstruction = context->bc->numInstructions;
    EMIT_INST0(context, ByteCodeOp::Jump);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitIndex(ByteCodeGenContext* context)
{
    const auto node        = context->node;
    node->resultRegisterRc = reserveRegisterRC(context);

    auto ownerBreakable = node->safeOwnerBreakable();
    while (ownerBreakable && !ownerBreakable->breakableFlags.has(BREAKABLE_CAN_HAVE_INDEX))
        ownerBreakable = ownerBreakable->safeOwnerBreakable();
    SWAG_ASSERT(ownerBreakable);
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc, ownerBreakable->registerIndex);
    return true;
}

bool ByteCodeGen::emitLeaveScopeDrop(const ByteCodeGenContext* context, Scope* scope, VectorNative<SymbolOverload*>* forceNoDrop)
{
    if (!scope)
        return true;

    auto&      table = scope->symTable;
    ScopedLock lock(table.mutex);

    const auto count = table.structVarsToDrop.size() - 1;
    if (count == UINT32_MAX)
        return true;

    // Need to wait for all the structures to be ok, in order to call the opDrop function
    for (uint32_t i = count; i != UINT32_MAX; i--)
    {
        const auto one = table.structVarsToDrop[i];
        if (!one.typeStruct)
            continue;

        Semantic::waitStructGenerated(context->baseJob, one.typeStruct);
        YIELD();
    }

    for (uint32_t i = count; i != UINT32_MAX; i--)
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
        if (one.overload && !one.overload->hasFlag(OVERLOAD_EMITTED))
            continue;

        if (one.typeInfo->isArray())
        {
            const auto typeArray = castTypeInfo<TypeInfoArray>(one.typeInfo, TypeInfoKind::Array);
            if (typeArray->totalCount == 1)
            {
                RegisterList r1   = reserveRegisterRC(context);
                const auto   inst = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, r1);
                inst->b.u64       = one.storageOffset;
                EMIT_INST1(context, ByteCodeOp::PushRAParam, r1);
                emitOpCallUser(context, one.typeStruct->opUserDropFct, one.typeStruct->opDrop, false);
                freeRegisterRC(context, r1);
            }
            else
            {
                // Need to loop on every element of the array in order to initialize them
                RegisterList r0 = reserveRegisterRC(context);
                RegisterList r1 = reserveRegisterRC(context);

                EMIT_INST1(context, ByteCodeOp::SetImmediate64, r0[0])->b.u64 = typeArray->totalCount;

                auto inst           = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, r1);
                inst->b.u64         = one.storageOffset;
                const auto seekJump = context->bc->numInstructions;

                EMIT_INST1(context, ByteCodeOp::PushRAParam, r1);
                emitOpCallUser(context, one.typeStruct->opUserDropFct, one.typeStruct->opDrop, false);

                inst        = EMIT_INST3(context, ByteCodeOp::IncPointer64, r1, 0, r1);
                inst->b.u64 = one.typeStruct->sizeOf;
                inst->addFlag(BCI_IMM_B);

                EMIT_INST1(context, ByteCodeOp::DecrementRA64, r0[0]);
                EMIT_INST1(context, ByteCodeOp::JumpIfNotZero64, r0[0])->b.s32 = static_cast<int32_t>(seekJump - context->bc->numInstructions - 1);

                freeRegisterRC(context, r0);
                freeRegisterRC(context, r1);
            }
        }
        else
        {
            const auto r0   = reserveRegisterRC(context);
            const auto inst = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, r0);
            inst->b.u64     = one.storageOffset;
            EMIT_INST1(context, ByteCodeOp::PushRAParam, r0);
            emitOpCallUser(context, one.typeStruct->opUserDropFct, one.typeStruct->opDrop, false);
            freeRegisterRC(context, r0);
        }
    }

    return true;
}

bool ByteCodeGen::emitDeferredStatements(ByteCodeGenContext* context, Scope* scope, bool forError)
{
    if (!scope)
        return true;

    const auto numDeferred = scope->deferredNodes.size();
    if (numDeferred)
    {
        context->result = ContextResult::NewChildren;
        const auto job  = context->baseJob;
        for (uint32_t i = 0; i < numDeferred; i++)
        {
            const auto node = scope->deferredNodes[i];

            if (!forError && node->deferKind == DeferKind::Error)
                continue;
            if (forError && node->deferKind == DeferKind::NoError)
                continue;

            // We duplicate because when we emit a node, some stuff will be reset (like the cast).
            // Se wo want to be sure to emit the same node multiple times without side effects from the previous defer usage.
            // We also do not want to change the number of children of "node", that's why we fill "parent" of the
            // clone without registering the node in the list of children.
            CloneContext cloneContext;
            cloneContext.cloneFlags      = CLONE_RAW;
            cloneContext.ownerDeferScope = scope;

            node->allocateExtension(ExtensionKind::Owner);

            auto child           = node->firstChild()->clone(cloneContext);
            child->parent        = node;
            child->bytecodeState = AstNodeResolveState::Enter;
            child->removeAstFlag(AST_NO_BYTECODE);
            node->extOwner()->nodesToFree.push_back(child);
            job->nodes.push_back(child);
        }
    }

    return true;
}

bool ByteCodeGen::emitLeaveScopeReturn(ByteCodeGenContext* context, VectorNative<SymbolOverload*>* forceNoDrop, bool forError)
{
    const auto node     = context->node;
    const auto funcNode = node->ownerFct;

    // Leave all scopes
    const Scope* topScope;
    if (node->hasOwnerInline() && (node->hasSemFlag(SEMFLAG_EMBEDDED_RETURN) || node->isNot(AstNodeKind::Return)))
    {
        // If the inline comes from a mixin, then the node->ownerInline->scope is the one the mixin is included
        // inside. We do not want to release that scope, as we do not own it ! But we want to release all the
        // scopes until that one (excluded)
        if (node->ownerInline()->func->hasAttribute(ATTRIBUTE_MIXIN))
        {
            topScope   = node->ownerInline()->scope;
            auto scope = node->ownerScope;
            if (scope == topScope)
                return true;
            while (scope->parentScope != topScope)
                scope = scope->parentScope;
            topScope = scope;
        }
        else
            topScope = node->ownerInline()->scope;
    }
    else
        topScope = funcNode->scope;

    Scope::collectScopeFromToExcluded(node->ownerScope, topScope->parentScope, context->collectScopes);
    for (const auto scope : context->collectScopes)
    {
        SWAG_CHECK(computeLeaveScope(context, scope, forceNoDrop, forError));
        YIELD();
    }

    return true;
}

bool ByteCodeGen::computeLeaveScope(ByteCodeGenContext* context, Scope* scope, VectorNative<SymbolOverload*>* forceNoDrop, bool forError)
{
    PushLocation pl(context, &context->node->token.endLocation);

    // Emit all 'defer' statements
    if (!scope->doneDefer.contains(context->node))
    {
        SWAG_CHECK(emitDeferredStatements(context, scope, forError));
        SWAG_ASSERT(context->result != ContextResult::Pending);
        scope->doneDefer.push_back(context->node);
        if (context->result == ContextResult::NewChildren)
            return true;
    }

    // Emit all drops
    if (!scope->doneDrop.contains(context->node))
    {
        SWAG_CHECK(emitLeaveScopeDrop(context, scope, forceNoDrop));
        YIELD();
        scope->doneDrop.push_back(context->node);
    }

    // Free some registers
    if (context->node->hasExtMisc())
    {
        for (const auto r : context->node->extMisc()->registersToRelease)
            freeRegisterRC(context, r);
        context->node->extMisc()->registersToRelease.clear();
    }

    return true;
}

bool ByteCodeGen::emitLeaveScope(ByteCodeGenContext* context)
{
    const auto node = context->node;

    switch (node->kind)
    {
        case AstNodeKind::CompilerMacro:
        {
            const auto macroNode = castAst<AstCompilerMacro>(node, AstNodeKind::CompilerMacro);
            SWAG_CHECK(computeLeaveScope(context, macroNode->scope));
            break;
        }
        default:
            SWAG_CHECK(computeLeaveScope(context, node->ownerScope));
            break;
    }

    return true;
}
