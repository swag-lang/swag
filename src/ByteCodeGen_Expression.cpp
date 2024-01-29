#include "pch.h"
#include "Ast.h"
#include "ByteCode.h"
#include "ByteCodeGen.h"
#include "Module.h"
#include "Report.h"
#include "Scope.h"
#include "Semantic.h"
#include "TypeManager.h"

bool ByteCodeGen::emitNullConditionalOp(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto child0   = node->childs[0];
    auto child1   = node->childs[1];
    auto typeInfo = TypeManager::concreteType(child0->typeInfo);

    if (!(child0->semFlags & SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, child0, typeInfo, child0->castedTypeInfo));
        YIELD();
        child0->semFlags |= SEMFLAG_CAST1;
    }

    // User special function
    if (node->hasSpecialFuncCall())
    {
        SWAG_CHECK(emitUserOp(context, child0, nullptr, false));
        YIELD();
        EMIT_INST1(context, ByteCodeOp::JumpIfZero64, node->resultRegisterRC)->b.s32 = child0->resultRegisterRC.size(); // After the "if not null"
        freeRegisterRC(context, node->resultRegisterRC);
        node->resultRegisterRC = child1->resultRegisterRC;
    }
    else
    {
        node->resultRegisterRC = child1->resultRegisterRC;

        // For an interface, check the itable pointer
        int regIdx = 0;
        if (child0->typeInfo->isInterface())
            regIdx = 1;

        ByteCodeInstruction* inst = nullptr;
        switch (typeInfo->sizeOf)
        {
        case 1:
            inst = EMIT_INST1(context, ByteCodeOp::JumpIfZero8, child0->resultRegisterRC[regIdx]);
            break;
        case 2:
            inst = EMIT_INST1(context, ByteCodeOp::JumpIfZero16, child0->resultRegisterRC[regIdx]);
            break;
        case 4:
            inst = EMIT_INST1(context, ByteCodeOp::JumpIfZero32, child0->resultRegisterRC[regIdx]);
            break;
        default:
            inst = EMIT_INST1(context, ByteCodeOp::JumpIfZero64, child0->resultRegisterRC[regIdx]);
            break;
        }

        inst->b.s32 = child0->resultRegisterRC.size(); // After the "if not null"
    }

    // If not null
    for (int r = 0; r < node->resultRegisterRC.size(); r++)
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[r], child0->resultRegisterRC[r]);

    freeRegisterRC(context, child0);
    return true;
}

bool ByteCodeGen::emitConditionalOpAfterExpr(ByteCodeGenContext* context)
{
    auto expr    = context->node;
    auto binNode = CastAst<AstConditionalOpNode>(expr->parent, AstNodeKind::ConditionalExpression);

    // We need to cast right now, in case the shortcut is activated
    SWAG_CHECK(emitCast(context, expr, TypeManager::concreteType(expr->typeInfo), expr->castedTypeInfo));
    YIELD();
    binNode->semFlags |= SEMFLAG_CAST1;

    // Jump to ifFalse child
    binNode->seekJumpIfFalse = context->bc->numInstructions;
    EMIT_INST1(context, ByteCodeOp::JumpIfFalse, expr->resultRegisterRC);
    return true;
}

bool ByteCodeGen::emitConditionalOpAfterIfTrue(ByteCodeGenContext* context)
{
    auto ifTrue  = context->node;
    auto binNode = CastAst<AstConditionalOpNode>(ifTrue->parent, AstNodeKind::ConditionalExpression);

    // Reserve registers in the main node to copy the result
    reserveRegisterRC(context, binNode->resultRegisterRC, ifTrue->resultRegisterRC.size());
    for (int r = 0; r < ifTrue->resultRegisterRC.size(); r++)
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, binNode->resultRegisterRC[r], ifTrue->resultRegisterRC[r]);

    // Jump after ifFalse block
    binNode->seekJumpAfterIfFalse = context->bc->numInstructions;
    EMIT_INST1(context, ByteCodeOp::Jump, ifTrue->resultRegisterRC);

    // After this, this is the IfFalse block, so update jump value after the expression
    auto inst   = &context->bc->out[binNode->seekJumpIfFalse];
    inst->b.s32 = context->bc->numInstructions - binNode->seekJumpIfFalse - 1;

    return true;
}

bool ByteCodeGen::emitConditionalOp(ByteCodeGenContext* context)
{
    auto node       = CastAst<AstConditionalOpNode>(context->node, AstNodeKind::ConditionalExpression);
    auto expression = node->childs[0];
    auto ifTrue     = node->childs[1];
    auto ifFalse    = node->childs[2];

    // Copy If false result
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[0], ifFalse->resultRegisterRC[0]);
    if (node->resultRegisterRC.size() == 2 && ifFalse->resultRegisterRC.size() == 2)
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[1], ifFalse->resultRegisterRC[1]);
    else if (node->resultRegisterRC.size() == 2 && ifFalse->resultRegisterRC.size() == 1)
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[1], ifFalse->resultRegisterRC[0]);

    // Update jump after the IfTrue block
    auto inst   = &context->bc->out[node->seekJumpAfterIfFalse];
    inst->b.s32 = context->bc->numInstructions - node->seekJumpAfterIfFalse - 1;

    freeRegisterRC(context, expression);
    freeRegisterRC(context, ifTrue);
    freeRegisterRC(context, ifFalse);

    return true;
}

bool ByteCodeGen::emitExpressionListBefore(ByteCodeGenContext* context)
{
    auto node = context->node;

    // Do not generate bytecode for childs in case of a constant expression, because
    // the full content of the expression is in the constant segment
    if (node->flags & AST_CONST_EXPR)
        node->flags |= AST_NO_BYTECODE_CHILDS;

    return true;
}

void ByteCodeGen::collectLiteralsChilds(AstNode* node, VectorNative<AstNode*>* orderedChilds)
{
    for (auto child : node->childs)
    {
        if (child->kind == AstNodeKind::ExpressionList)
            collectLiteralsChilds(child, orderedChilds);
        else
            orderedChilds->push_back(child);
    }
}

bool ByteCodeGen::emitExpressionList(ByteCodeGenContext* context)
{
    auto node     = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);
    auto typeList = CastTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);

    // A non const expression list will be collected by the top ExpressionList
    if (!(node->flags & AST_CONST_EXPR))
    {
        if (node->parent->kind == AstNodeKind::ExpressionList)
            return true;
    }

    if (node->specFlags & AstExpressionList::SPECFLAG_FOR_TUPLE)
        node->resultRegisterRC = reserveRegisterRC(context);
    else
        reserveLinearRegisterRC2(context, node->resultRegisterRC);

    if (!(node->flags & AST_CONST_EXPR))
    {
        context->collectChilds.clear();
        collectLiteralsChilds(node, &context->collectChilds);

        // Wait for generated struct if necessary
        for (auto child : context->collectChilds)
        {
            auto typeChild = TypeManager::concreteType(child->typeInfo);
            if (typeChild->isStruct())
            {
                Semantic::waitStructGenerated(context->baseJob, typeChild);
                YIELD();
            }
        }

        // Must be called only on the real node !
        auto listNode = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

        // If in a return expression, just push the caller retval
        AstNode* parentReturn = listNode->inSimpleReturn();
        uint32_t startOffset  = 0;
        bool     canDrop      = true;
        if (parentReturn)
        {
            if (node->ownerInline)
                EMIT_INST1(context, ByteCodeOp::CopyRAtoRT, node->ownerInline->resultRegisterRC);
            else
            {
                EMIT_INST1(context, ByteCodeOp::CopyRRtoRA, node->resultRegisterRC);
                EMIT_INST1(context, ByteCodeOp::CopyRAtoRT, node->resultRegisterRC);
            }

            context->bc->maxCallResults = max(context->bc->maxCallResults, 1);
            parentReturn->semFlags |= SEMFLAG_RETVAL;
            canDrop = false;
        }

        // If initializating a variable, then push the variable storage directly if possible.
        // That way can avoid one affectation.
        else if (listNode->parent->kind == AstNodeKind::VarDecl)
        {
            auto varDecl = CastAst<AstVarDecl>(listNode->parent, AstNodeKind::VarDecl);
            if (varDecl->assignment == listNode)
            {
                auto typeVar = TypeManager::concreteType(varDecl->typeInfo, CONCRETE_ALIAS);
                if (!typeVar->isSlice() &&
                    (!varDecl->hasExtMisc() || !varDecl->extMisc()->resolvedUserOpSymbolOverload))
                {
                    startOffset = listNode->parent->resolvedSymbolOverload->computedValue.storageOffset;
                    auto inst   = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
                    inst->b.u64 = startOffset;
                    canDrop     = false;
                }
            }
        }

        // Default : assign to temporary storage
        if (canDrop)
        {
            startOffset = listNode->computedValue->storageOffset;
            auto inst   = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
            inst->b.u64 = startOffset;
        }

        // Emit one affectation per child
        auto         oneOffset   = typeList->subTypes.front()->typeInfo->sizeOf;
        size_t       totalOffset = 0;
        RegisterList r0;
        reserveRegisterRC(context, r0, 1);
        for (auto child : context->collectChilds)
        {
            child->flags |= AST_NO_LEFT_DROP;

            auto inst = EMIT_INST3(context, ByteCodeOp::IncPointer64, node->resultRegisterRC, 0, r0);
            SWAG_ASSERT(totalOffset != 0xFFFFFFFF);
            inst->b.u64 = totalOffset;
            inst->flags |= BCI_IMM_B;

            auto saveR = context->node->resultRegisterRC;
            SWAG_CHECK(emitCast(context, child, child->typeInfo, child->castedTypeInfo));
            context->node->resultRegisterRC = saveR;

            emitAffectEqual(context, r0, child->resultRegisterRC, child->typeInfo, child);

            SWAG_ASSERT(context->result == ContextResult::Done);
            freeRegisterRC(context, child);

            if (canDrop)
            {
                auto varOffset = (uint32_t) (listNode->computedValue->storageOffset + totalOffset);
                node->ownerScope->symTable.addVarToDrop(nullptr, child->typeInfo, varOffset);
            }

            totalOffset += oneOffset;
        }
        freeRegisterRC(context, r0);

        // Reference to the stack, and store the number of element in a register
        if (canDrop)
        {
            EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC[0])->b.u64 = startOffset;
            if (!(node->specFlags & AstExpressionList::SPECFLAG_FOR_TUPLE))
                EMIT_INST1(context, ByteCodeOp::SetImmediate64, node->resultRegisterRC[1])->b.u64 = listNode->childs.size();
        }
        else
        {
            // :ForceNoAffect
            freeRegisterRC(context, node);
        }
    }
    else
    {
        // Be sure it has been collected to the constant segment. Usually, this is done after casting, because
        // we could need to change the values before the collect. If it's not done yet, that means that the cast
        // did not take place (when passing to a varargs for example).
        if (!(node->semFlags & SEMFLAG_EXPRLIST_CST))
        {
            node->semFlags |= SEMFLAG_EXPRLIST_CST;
            node->allocateComputedValue();
            node->computedValue->storageSegment = Semantic::getConstantSegFromContext(node);
            SWAG_CHECK(Semantic::reserveAndStoreToSegment(context, node->computedValue->storageSegment, node->computedValue->storageOffset, nullptr, typeList, node));
        }

        emitMakeSegPointer(context, node->computedValue->storageSegment, node->computedValue->storageOffset, node->resultRegisterRC[0]);
        if (!(node->specFlags & AstExpressionList::SPECFLAG_FOR_TUPLE))
            EMIT_INST1(context, ByteCodeOp::SetImmediate64, node->resultRegisterRC[1])->b.u64 = typeList->subTypes.size();
    }

    return true;
}

bool ByteCodeGen::emitLiteral(ByteCodeGenContext* context)
{
    auto node = context->node;
    if (node->semFlags & SEMFLAG_LITERAL_SUFFIX)
        return context->report({node->childs.front(), Fmt(Err(Err0403), node->childs.front()->token.ctext())});
    SWAG_CHECK(emitLiteral(context, node, nullptr, node->resultRegisterRC));
    return true;
}

bool ByteCodeGen::emitLiteral(ByteCodeGenContext* context, AstNode* node, TypeInfo* toType, RegisterList& regList)
{
    auto typeInfo = node->castedTypeInfo ? node->castedTypeInfo : node->typeInfo;
    typeInfo      = TypeManager::concreteType(typeInfo);

    AstIdentifierRef* identifierRef = nullptr;
    if (node->kind == AstNodeKind::Identifier)
    {
        auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
        identifierRef   = identifier->identifierRef();
    }
    else if (node->kind == AstNodeKind::ArrayPointerIndex)
    {
        identifierRef = (AstIdentifierRef*) node->findParent(AstNodeKind::IdentifierRef);
    }

    // A reference to a segment
    if (node->forceTakeAddress() || (typeInfo->isPointer() && !typeInfo->isPointerNull()))
    {
        if (node->computedValue->storageSegment && node->computedValue->storageOffset != UINT32_MAX)
        {
            regList = reserveRegisterRC(context);
            emitMakeSegPointer(context, node->computedValue->storageSegment, node->computedValue->storageOffset, regList[0]);
            if (identifierRef)
                identifierRef->resultRegisterRC = regList;
            return true;
        }
    }

    if (typeInfo->isAny())
    {
        // If we need a cast to an any, then first resolve literal with its real type
        if (node->castedTypeInfo)
        {
            typeInfo = node->castedTypeInfo;
        }

        // Otherwise we dereference the any in 2 registers
        else
        {
            reserveLinearRegisterRC2(context, regList);
            emitMakeSegPointer(context, node->computedValue->storageSegment, node->computedValue->storageOffset, regList);
            EMIT_INST2(context, ByteCodeOp::DeRefStringSlice, regList[0], regList[1]);
            return true;
        }
    }

    // We have null
    if (node->castedTypeInfo && node->castedTypeInfo->isPointerNull())
    {
        // We want a string or a slice
        if (node->typeInfo->nativeType == NativeTypeKind::String ||
            node->typeInfo->isSlice() ||
            node->typeInfo->isInterface())
        {
            reserveLinearRegisterRC2(context, regList);
            EMIT_INST1(context, ByteCodeOp::ClearRA, regList[0]);
            EMIT_INST1(context, ByteCodeOp::ClearRA, regList[1]);
            return true;
        }
    }

    regList = reserveRegisterRC(context);

    if (node->isConstantGenTypeInfo())
    {
        emitMakeSegPointer(context, node->computedValue->storageSegment, node->computedValue->storageOffset, regList[0]);
    }
    else if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.b = node->computedValue->reg.b;
            if (mustEmitSafety(context, SAFETY_BOOL) && node->computedValue->reg.u8 & 0xFE)
                return context->report({node, Err(Saf0003)});
            return true;
        case NativeTypeKind::U8:
            EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.u8 = node->computedValue->reg.u8;
            return true;
        case NativeTypeKind::U16:
            EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.u16 = node->computedValue->reg.u16;
            return true;
        case NativeTypeKind::U32:
            EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.u64 = node->computedValue->reg.u32;
            return true;
        case NativeTypeKind::U64:
            EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList)->b.u64 = node->computedValue->reg.u64;
            return true;
        case NativeTypeKind::S8:
            EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.s8 = node->computedValue->reg.s8;
            return true;
        case NativeTypeKind::S16:
            EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.s16 = node->computedValue->reg.s16;
            return true;
        case NativeTypeKind::S32:
            EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.s32 = node->computedValue->reg.s32;
            return true;
        case NativeTypeKind::S64:
            EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList)->b.s64 = node->computedValue->reg.s64;
            return true;
        case NativeTypeKind::F32:
            EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.f32 = node->computedValue->reg.f32;
            if (mustEmitSafety(context, SAFETY_NAN) && isnan(node->computedValue->reg.f32))
                return context->report({node, Err(Saf0016)});
            return true;
        case NativeTypeKind::F64:
            EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList)->b.f64 = node->computedValue->reg.f64;
            if (mustEmitSafety(context, SAFETY_NAN) && isnan(node->computedValue->reg.f64))
                return context->report({node, Err(Saf0016)});
            return true;
        case NativeTypeKind::Rune:
            EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.u64 = node->computedValue->reg.u32;
            return true;
        case NativeTypeKind::String:
        {
            reserveLinearRegisterRC2(context, regList);
            auto storageSegment = Semantic::getConstantSegFromContext(node);
            auto storageOffset  = storageSegment->addString(node->computedValue->text);
            SWAG_ASSERT(storageOffset != UINT32_MAX);
            emitMakeSegPointer(context, storageSegment, storageOffset, regList[0]);
            EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[1])->b.u64 = node->computedValue->text.length();
            return true;
        }
        default:
            return Report::internalError(context->node, "emitLiteral, type not supported");
        }
    }
    else if (typeInfo->isPointerNull())
    {
        if (toType && (toType->isSlice() || toType->isString()))
        {
            reserveLinearRegisterRC2(context, regList);
            EMIT_INST1(context, ByteCodeOp::ClearRA, regList[0]);
            EMIT_INST1(context, ByteCodeOp::ClearRA, regList[1]);
        }
        else
        {
            EMIT_INST1(context, ByteCodeOp::ClearRA, regList);
        }
    }
    else if (typeInfo->isArray())
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        reserveLinearRegisterRC2(context, regList);
        emitMakeSegPointer(context, node->computedValue->storageSegment, node->computedValue->storageOffset, regList[0]);
        EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[1])->b.u64 = typeArray->count;
    }
    else if (typeInfo->isStruct() || typeInfo->isListTuple() || typeInfo->isListArray())
    {
        emitMakeSegPointer(context, node->computedValue->storageSegment, node->computedValue->storageOffset, regList[0]);
    }
    else if (typeInfo->isPointer() && node->castedTypeInfo && node->castedTypeInfo->isString())
    {
        auto storageSegment = Semantic::getConstantSegFromContext(node);
        auto storageOffset  = storageSegment->addString(node->computedValue->text);
        SWAG_ASSERT(storageOffset != UINT32_MAX);
        emitMakeSegPointer(context, storageSegment, storageOffset, regList[0]);
    }
    else if (typeInfo->isSlice() && node->castedTypeInfo && node->castedTypeInfo->isString())
    {
        reserveLinearRegisterRC2(context, regList);
        auto storageSegment = Semantic::getConstantSegFromContext(node);
        auto storageOffset  = storageSegment->addString(node->computedValue->text);
        SWAG_ASSERT(storageOffset != UINT32_MAX);
        emitMakeSegPointer(context, storageSegment, storageOffset, regList[0]);
        EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[1])->b.u64 = node->computedValue->text.length();
    }
    else if (typeInfo->isSlice())
    {
        // :SliceLiteral
        reserveLinearRegisterRC2(context, regList);
        SWAG_ASSERT(node->computedValue);
        auto slice        = (SwagSlice*) node->computedValue->getStorageAddr();
        auto offsetValues = node->computedValue->storageSegment->offset((uint8_t*) slice->buffer);
        emitMakeSegPointer(context, node->computedValue->storageSegment, offsetValues, regList[0]);
        EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[1])->b.u64 = slice->count;
    }
    else if (typeInfo->isPointer())
    {
        EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList)->b.u64 = node->computedValue->reg.u64;
    }
    else
    {
        return Report::internalError(context->node, Fmt("emitLiteral, unsupported type [[%s]]", typeInfo->getDisplayNameC()).c_str());
    }

    return true;
}

bool ByteCodeGen::emitComputedValue(ByteCodeGenContext* context)
{
    auto node = context->node;
    SWAG_CHECK(emitLiteral(context));
    SWAG_CHECK(emitCast(context, node, TypeManager::concreteType(node->typeInfo), node->castedTypeInfo));
    SWAG_ASSERT(context->result == ContextResult::Done);

    // To be sure that cast is treated once
    node->castedTypeInfo = nullptr;

    return true;
}

bool ByteCodeGen::emitDefer(ByteCodeGenContext* context)
{
    auto node = CastAst<AstDefer>(context->node, AstNodeKind::Defer);
    SWAG_ASSERT(node->childs.size() == 1);
    node->ownerScope->deferredNodes.push_back(node);
    return true;
}