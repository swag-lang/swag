#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "SourceFile.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "Report.h"

bool ByteCodeGenJob::emitNullConditionalOp(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto child0   = node->childs[0];
    auto child1   = node->childs[1];
    auto typeInfo = TypeManager::concreteType(child0->typeInfo);

    if (!(child0->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, child0, typeInfo, child0->castedTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
        child0->doneFlags |= AST_DONE_CAST1;
    }

    // User special function
    if (node->hasSpecialFuncCall())
    {
        SWAG_CHECK(emitUserOp(context, child0, nullptr, false));
        if (context->result != ContextResult::Done)
            return true;
        emitInstruction(context, ByteCodeOp::JumpIfZero64, node->resultRegisterRC)->b.s32 = child0->resultRegisterRC.size(); // After the "if not null"
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
            inst = emitInstruction(context, ByteCodeOp::JumpIfZero8, child0->resultRegisterRC[regIdx]);
            break;
        case 2:
            inst = emitInstruction(context, ByteCodeOp::JumpIfZero16, child0->resultRegisterRC[regIdx]);
            break;
        case 4:
            inst = emitInstruction(context, ByteCodeOp::JumpIfZero32, child0->resultRegisterRC[regIdx]);
            break;
        default:
            inst = emitInstruction(context, ByteCodeOp::JumpIfZero64, child0->resultRegisterRC[regIdx]);
            break;
        }

        inst->b.s32 = child0->resultRegisterRC.size(); // After the "if not null"
    }

    // If not null
    for (int r = 0; r < node->resultRegisterRC.size(); r++)
        emitInstruction(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[r], child0->resultRegisterRC[r]);

    freeRegisterRC(context, child0);
    return true;
}

bool ByteCodeGenJob::emitConditionalOpAfterExpr(ByteCodeGenContext* context)
{
    auto expr    = context->node;
    auto binNode = CastAst<AstConditionalOpNode>(expr->parent, AstNodeKind::ConditionalExpression);

    // We need to cast right now, in case the shortcut is activated
    SWAG_CHECK(emitCast(context, expr, TypeManager::concreteType(expr->typeInfo), expr->castedTypeInfo));
    if (context->result != ContextResult::Done)
        return true;
    binNode->doneFlags |= AST_DONE_CAST1;

    // Jump to ifFalse child
    binNode->seekJumpIfFalse = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::JumpIfFalse, expr->resultRegisterRC);
    return true;
}

bool ByteCodeGenJob::emitConditionalOpAfterIfTrue(ByteCodeGenContext* context)
{
    auto ifTrue  = context->node;
    auto binNode = CastAst<AstConditionalOpNode>(ifTrue->parent, AstNodeKind::ConditionalExpression);

    // Reserve registers in the main node to copy the result
    reserveRegisterRC(context, binNode->resultRegisterRC, ifTrue->resultRegisterRC.size());
    for (int r = 0; r < ifTrue->resultRegisterRC.size(); r++)
        emitInstruction(context, ByteCodeOp::CopyRBtoRA64, binNode->resultRegisterRC[r], ifTrue->resultRegisterRC[r]);

    // Jump after ifFalse block
    binNode->seekJumpAfterIfFalse = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::Jump, ifTrue->resultRegisterRC);

    // After this, this is the IfFalse block, so update jump value after the expression
    auto inst   = &context->bc->out[binNode->seekJumpIfFalse];
    inst->b.s32 = context->bc->numInstructions - binNode->seekJumpIfFalse - 1;

    return true;
}

bool ByteCodeGenJob::emitConditionalOp(ByteCodeGenContext* context)
{
    auto node       = CastAst<AstConditionalOpNode>(context->node, AstNodeKind::ConditionalExpression);
    auto expression = node->childs[0];
    auto ifTrue     = node->childs[1];
    auto ifFalse    = node->childs[2];

    // Copy If false result
    emitInstruction(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[0], ifFalse->resultRegisterRC[0]);
    if (node->resultRegisterRC.size() == 2 && ifFalse->resultRegisterRC.size() == 2)
        emitInstruction(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[1], ifFalse->resultRegisterRC[1]);
    else if (node->resultRegisterRC.size() == 2 && ifFalse->resultRegisterRC.size() == 1)
        emitInstruction(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRC[1], ifFalse->resultRegisterRC[0]);

    // Update jump after the IfTrue block
    auto inst   = &context->bc->out[node->seekJumpAfterIfFalse];
    inst->b.s32 = context->bc->numInstructions - node->seekJumpAfterIfFalse - 1;

    freeRegisterRC(context, expression);
    freeRegisterRC(context, ifTrue);
    freeRegisterRC(context, ifFalse);

    return true;
}

bool ByteCodeGenJob::emitExpressionListBefore(ByteCodeGenContext* context)
{
    auto node = context->node;

    // Do not generate bytecode for childs in case of a constant expression, because
    // the full content of the expression is in the constant segment
    if (node->flags & AST_CONST_EXPR)
        node->flags |= AST_NO_BYTECODE_CHILDS;

    return true;
}

void ByteCodeGenJob::collectLiteralsChilds(AstNode* node, VectorNative<AstNode*>* orderedChilds)
{
    for (auto child : node->childs)
    {
        if (child->kind == AstNodeKind::ExpressionList)
            collectLiteralsChilds(child, orderedChilds);
        else
            orderedChilds->push_back(child);
    }
}

bool ByteCodeGenJob::emitExpressionList(ByteCodeGenContext* context)
{
    auto node     = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);
    auto job      = context->job;
    auto typeList = CastTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);

    // A non const expression list will be collected by the top ExpressionList
    if (!(node->flags & AST_CONST_EXPR))
    {
        if (node->parent->kind == AstNodeKind::ExpressionList)
            return true;
    }

    if (node->specFlags & AST_SPEC_EXPRLIST_FOR_TUPLE)
        node->resultRegisterRC = reserveRegisterRC(context);
    else
        reserveLinearRegisterRC2(context, node->resultRegisterRC);

    if (!(node->flags & AST_CONST_EXPR))
    {
        job->collectChilds.clear();
        collectLiteralsChilds(node, &job->collectChilds);

        // Wait for generated struct if necessary
        for (auto child : job->collectChilds)
        {
            auto typeChild = TypeManager::concreteType(child->typeInfo);
            if (typeChild->isStruct())
            {
                context->job->waitStructGenerated(typeChild);
                if (context->result == ContextResult::Pending)
                    return true;
            }
        }

        // Must be called only on the real node !
        auto listNode = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

        // If in a return expression, just push the caller retval
        AstNode* parentReturn = listNode->inSimpleReturn();
        if (parentReturn)
        {
            if (node->ownerInline)
                emitInstruction(context, ByteCodeOp::CopyRCtoRT, node->ownerInline->resultRegisterRC);
            else
            {
                emitInstruction(context, ByteCodeOp::CopyRRtoRC, node->resultRegisterRC);
                emitInstruction(context, ByteCodeOp::CopyRCtoRT, node->resultRegisterRC);
            }

            context->bc->maxCallResults = max(context->bc->maxCallResults, 1);
            parentReturn->doneFlags |= AST_DONE_RETVAL;
        }
        else
        {
            auto offsetIdx = listNode->computedValue->storageOffset;
            auto inst      = emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
            inst->b.u64    = offsetIdx;
        }

        // Emit one affectation per child
        auto         oneOffset   = typeList->subTypes.front()->typeInfo->sizeOf;
        auto         totalOffset = 0;
        RegisterList r0;
        reserveRegisterRC(context, r0, 1);
        for (auto child : job->collectChilds)
        {
            child->flags |= AST_NO_LEFT_DROP;

            auto inst = emitInstruction(context, ByteCodeOp::IncPointer64, node->resultRegisterRC, 0, r0);
            SWAG_ASSERT(totalOffset != 0xFFFFFFFF);
            inst->b.u64 = totalOffset;
            inst->flags |= BCI_IMM_B;

            auto saveR = context->node->resultRegisterRC;
            SWAG_CHECK(emitCast(context, child, child->typeInfo, child->castedTypeInfo));
            context->node->resultRegisterRC = saveR;

            emitAffectEqual(context, r0, child->resultRegisterRC, child->typeInfo, child);

            SWAG_ASSERT(context->result == ContextResult::Done);
            freeRegisterRC(context, child);
            totalOffset += oneOffset;
        }
        freeRegisterRC(context, r0);

        // Reference to the stack, and store the number of element in a register
        emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC[0])->b.u64 = listNode->computedValue->storageOffset;
        if (!(node->specFlags & AST_SPEC_EXPRLIST_FOR_TUPLE))
            emitInstruction(context, ByteCodeOp::SetImmediate64, node->resultRegisterRC[1])->b.u64 = listNode->childs.size();
    }
    else
    {
        // Be sure it has been collected to the constant segment. Usually, this is done after casting, because
        // we could need to change the values before the collect. If it's not done yet, that means that the cast
        // did not take place (when passing to a varargs for example).
        if (!(node->doneFlags & AST_DONE_EXPRLIST_CST))
        {
            node->doneFlags |= AST_DONE_EXPRLIST_CST;
            node->allocateComputedValue();
            node->computedValue->storageSegment = SemanticJob::getConstantSegFromContext(node);
            SWAG_CHECK(SemanticJob::reserveAndStoreToSegment(context, node->computedValue->storageSegment, node->computedValue->storageOffset, nullptr, typeList, node));
        }

        emitMakeSegPointer(context, node->computedValue->storageSegment, node->computedValue->storageOffset, node->resultRegisterRC[0]);
        if (!(node->specFlags & AST_SPEC_EXPRLIST_FOR_TUPLE))
            emitInstruction(context, ByteCodeOp::SetImmediate64, node->resultRegisterRC[1])->b.u64 = typeList->subTypes.size();
    }

    return true;
}

bool ByteCodeGenJob::emitLiteral(ByteCodeGenContext* context)
{
    auto node = context->node;
    if (node->semFlags & AST_SEM_LITERAL_SUFFIX)
        return context->report({node->childs.front(), Fmt(Err(Err0532), node->childs.front()->token.ctext())});
    SWAG_CHECK(emitLiteral(context, node, nullptr, node->resultRegisterRC));
    return true;
}

bool ByteCodeGenJob::emitLiteral(ByteCodeGenContext* context, AstNode* node, TypeInfo* toType, RegisterList& regList)
{
    auto typeInfo = node->castedTypeInfo ? node->castedTypeInfo : node->typeInfo;
    typeInfo      = TypeManager::concreteType(typeInfo);

    // A reference to a segment
    if (node->forceTakeAddress())
    {
        if (node->computedValue->storageSegment && node->computedValue->storageOffset != UINT32_MAX)
        {
            regList = reserveRegisterRC(context);
            emitMakeSegPointer(context, node->computedValue->storageSegment, node->computedValue->storageOffset, regList[0]);
            return true;
        }
        else
        {
            return Report::internalError(context->node, Fmt("emitLiteral, unsupported ptr type '%s'", typeInfo->getDisplayNameC()).c_str());
        }
    }

    // If we need a cast to an any, then first resolve literal with its real type
    if (typeInfo->isAny())
    {
        SWAG_ASSERT(node->castedTypeInfo);
        typeInfo = node->castedTypeInfo;
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
            emitInstruction(context, ByteCodeOp::ClearRA, regList[0]);
            emitInstruction(context, ByteCodeOp::ClearRA, regList[1]);
            return true;
        }
    }

    regList = reserveRegisterRC(context);

    if (node->flags & AST_VALUE_IS_TYPEINFO)
    {
        emitMakeSegPointer(context, node->computedValue->storageSegment, node->computedValue->storageOffset, regList[0]);
    }
    else if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            emitInstruction(context, ByteCodeOp::SetImmediate32, regList)->b.b = node->computedValue->reg.b;
            if (mustEmitSafety(context, SAFETY_BOOL) && node->computedValue->reg.u8 & 0xFE)
                return context->report({node, Err(Saf0020)});
            return true;
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::SetImmediate32, regList)->b.u8 = node->computedValue->reg.u8;
            return true;
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::SetImmediate32, regList)->b.u16 = node->computedValue->reg.u16;
            return true;
        case NativeTypeKind::U32:
            emitInstruction(context, ByteCodeOp::SetImmediate32, regList)->b.u64 = node->computedValue->reg.u32;
            return true;
        case NativeTypeKind::U64:
            emitInstruction(context, ByteCodeOp::SetImmediate64, regList)->b.u64 = node->computedValue->reg.u64;
            return true;
        case NativeTypeKind::S8:
            emitInstruction(context, ByteCodeOp::SetImmediate32, regList)->b.s8 = node->computedValue->reg.s8;
            return true;
        case NativeTypeKind::S16:
            emitInstruction(context, ByteCodeOp::SetImmediate32, regList)->b.s16 = node->computedValue->reg.s16;
            return true;
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::SetImmediate32, regList)->b.s32 = node->computedValue->reg.s32;
            return true;
        case NativeTypeKind::S64:
            emitInstruction(context, ByteCodeOp::SetImmediate64, regList)->b.s64 = node->computedValue->reg.s64;
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::SetImmediate32, regList)->b.f32 = node->computedValue->reg.f32;
            if (mustEmitSafety(context, SAFETY_NAN) && isnan(node->computedValue->reg.f32))
                return context->report({node, Err(Saf0021)});
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::SetImmediate64, regList)->b.f64 = node->computedValue->reg.f64;
            if (mustEmitSafety(context, SAFETY_NAN) && isnan(node->computedValue->reg.f64))
                return context->report({node, Err(Saf0021)});
            return true;
        case NativeTypeKind::Rune:
            emitInstruction(context, ByteCodeOp::SetImmediate32, regList)->b.u64 = node->computedValue->reg.u32;
            return true;
        case NativeTypeKind::String:
        {
            reserveLinearRegisterRC2(context, regList);
            auto storageSegment = SemanticJob::getConstantSegFromContext(node);
            auto storageOffset  = storageSegment->addString(node->computedValue->text);
            SWAG_ASSERT(storageOffset != UINT32_MAX);
            emitMakeSegPointer(context, storageSegment, storageOffset, regList[0]);
            emitInstruction(context, ByteCodeOp::SetImmediate64, regList[1])->b.u64 = node->computedValue->text.length();
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
            emitInstruction(context, ByteCodeOp::ClearRA, regList[0]);
            emitInstruction(context, ByteCodeOp::ClearRA, regList[1]);
        }
        else
        {
            emitInstruction(context, ByteCodeOp::ClearRA, regList);
        }
    }
    else if (typeInfo->isArray())
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        reserveLinearRegisterRC2(context, regList);
        emitMakeSegPointer(context, node->computedValue->storageSegment, node->computedValue->storageOffset, regList[0]);
        emitInstruction(context, ByteCodeOp::SetImmediate64, regList[1])->b.u64 = typeArray->count;
    }
    else if (typeInfo->isStruct() || typeInfo->isListTuple() || typeInfo->isListArray())
    {
        emitMakeSegPointer(context, node->computedValue->storageSegment, node->computedValue->storageOffset, regList[0]);
    }
    else if (typeInfo->isPointer() && node->castedTypeInfo && node->castedTypeInfo->isString())
    {
        auto storageSegment = SemanticJob::getConstantSegFromContext(node);
        auto storageOffset  = storageSegment->addString(node->computedValue->text);
        SWAG_ASSERT(storageOffset != UINT32_MAX);
        emitMakeSegPointer(context, storageSegment, storageOffset, regList[0]);
    }
    else if (typeInfo->isSlice() && node->castedTypeInfo && node->castedTypeInfo->isString())
    {
        reserveLinearRegisterRC2(context, regList);
        auto storageSegment = SemanticJob::getConstantSegFromContext(node);
        auto storageOffset  = storageSegment->addString(node->computedValue->text);
        SWAG_ASSERT(storageOffset != UINT32_MAX);
        emitMakeSegPointer(context, storageSegment, storageOffset, regList[0]);
        emitInstruction(context, ByteCodeOp::SetImmediate64, regList[1])->b.u64 = node->computedValue->text.length();
    }
    else if (typeInfo->isSlice())
    {
        // :SliceLiteral
        reserveLinearRegisterRC2(context, regList);
        SWAG_ASSERT(node->computedValue);
        SWAG_ASSERT(node->computedValue->storageSegment);
        SWAG_ASSERT(node->computedValue->storageOffset != UINT32_MAX);
        SWAG_ASSERT(node->computedValue->reg.u64 != 0);
        emitMakeSegPointer(context, node->computedValue->storageSegment, node->computedValue->storageOffset, regList[0]);
        emitInstruction(context, ByteCodeOp::SetImmediate64, regList[1])->b.u64 = node->computedValue->reg.u64;
    }
    else if (typeInfo->isPointer())
    {
        emitInstruction(context, ByteCodeOp::SetImmediate64, regList)->b.u64 = node->computedValue->reg.u64;
    }
    else
    {
        return Report::internalError(context->node, Fmt("emitLiteral, unsupported type '%s'", typeInfo->getDisplayNameC()).c_str());
    }

    return true;
}

bool ByteCodeGenJob::emitDefer(ByteCodeGenContext* context)
{
    auto node = CastAst<AstDefer>(context->node, AstNodeKind::Defer);
    SWAG_ASSERT(node->childs.size() == 1);
    node->ownerScope->deferredNodes.push_back(node);
    return true;
}