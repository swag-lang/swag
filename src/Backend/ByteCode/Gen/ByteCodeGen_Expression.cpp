#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Scope.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Wmf/Module.h"

bool ByteCodeGen::emitNullConditionalOp(ByteCodeGenContext* context)
{
    const auto node     = context->node;
    const auto child0   = node->firstChild();
    const auto child1   = node->secondChild();
    const auto typeInfo = TypeManager::concreteType(child0->typeInfo);

    if (!child0->hasSemFlag(SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, child0, typeInfo, child0->typeInfoCast));
        YIELD();
        child0->addSemFlag(SEMFLAG_CAST1);
    }

    // User special function
    if (node->hasSpecialFuncCall())
    {
        SWAG_CHECK(emitUserOp(context, child0, nullptr, false));
        YIELD();
        EMIT_INST1(context, ByteCodeOp::JumpIfZero64, node->resultRegisterRc)->b.s32 = static_cast<int32_t>(child0->resultRegisterRc.size()); // After the "if not null"
        freeRegisterRC(context, node->resultRegisterRc);
        node->resultRegisterRc = child1->resultRegisterRc;
    }
    else
    {
        node->resultRegisterRc = child1->resultRegisterRc;

        // For an interface, check the itable pointer
        int regIdx = 0;
        if (child0->typeInfo->isInterface())
            regIdx = 1;

        ByteCodeInstruction* inst = nullptr;
        switch (typeInfo->sizeOf)
        {
            case 1:
                inst = EMIT_INST1(context, ByteCodeOp::JumpIfZero8, child0->resultRegisterRc[regIdx]);
                break;
            case 2:
                inst = EMIT_INST1(context, ByteCodeOp::JumpIfZero16, child0->resultRegisterRc[regIdx]);
                break;
            case 4:
                inst = EMIT_INST1(context, ByteCodeOp::JumpIfZero32, child0->resultRegisterRc[regIdx]);
                break;
            default:
                inst = EMIT_INST1(context, ByteCodeOp::JumpIfZero64, child0->resultRegisterRc[regIdx]);
                break;
        }

        inst->b.s32 = static_cast<int32_t>(child0->resultRegisterRc.size()); // After the "if not null"
    }

    // If not null
    for (uint32_t r = 0; r < node->resultRegisterRc.size(); r++)
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc[r], child0->resultRegisterRc[r]);

    freeRegisterRC(context, child0);
    return true;
}

bool ByteCodeGen::emitConditionalOpAfterExpr(ByteCodeGenContext* context)
{
    const auto expr    = context->node;
    const auto binNode = castAst<AstConditionalOpNode>(expr->parent, AstNodeKind::ConditionalExpression);

    // We need to cast right now, in case the shortcut is activated
    SWAG_CHECK(emitCast(context, expr, TypeManager::concreteType(expr->typeInfo), expr->typeInfoCast));
    YIELD();
    binNode->addSemFlag(SEMFLAG_CAST1);

    // Jump to ifFalse child
    binNode->seekJumpIfFalse = context->bc->numInstructions;
    EMIT_INST1(context, ByteCodeOp::JumpIfFalse, expr->resultRegisterRc);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitConditionalOpAfterIfTrue(ByteCodeGenContext* context)
{
    const auto ifTrue  = context->node;
    const auto binNode = castAst<AstConditionalOpNode>(ifTrue->parent, AstNodeKind::ConditionalExpression);

    // Reserve registers in the main node to copy the result
    reserveRegisterRC(context, binNode->resultRegisterRc, ifTrue->resultRegisterRc.size());
    for (uint32_t r = 0; r < ifTrue->resultRegisterRc.size(); r++)
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, binNode->resultRegisterRc[r], ifTrue->resultRegisterRc[r]);

    // Jump after ifFalse block
    binNode->seekJumpAfterIfFalse = context->bc->numInstructions;
    EMIT_INST1(context, ByteCodeOp::Jump, ifTrue->resultRegisterRc);

    // After this, this is the IfFalse block, so update jump value after the expression
    const auto inst = &context->bc->out[binNode->seekJumpIfFalse];
    inst->b.s32     = static_cast<int32_t>(context->bc->numInstructions - binNode->seekJumpIfFalse - 1);

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitConditionalOp(ByteCodeGenContext* context)
{
    const auto node       = castAst<AstConditionalOpNode>(context->node, AstNodeKind::ConditionalExpression);
    const auto expression = node->firstChild();
    const auto ifTrue     = node->secondChild();
    const auto ifFalse    = node->children[2];

    // Copy If false result
    EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc[0], ifFalse->resultRegisterRc[0]);
    if (node->resultRegisterRc.size() == 2 && ifFalse->resultRegisterRc.size() == 2)
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc[1], ifFalse->resultRegisterRc[1]);
    else if (node->resultRegisterRc.size() == 2 && ifFalse->resultRegisterRc.size() == 1)
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->resultRegisterRc[1], ifFalse->resultRegisterRc[0]);

    // Update jump after the IfTrue block
    const auto inst = &context->bc->out[node->seekJumpAfterIfFalse];
    inst->b.s32     = static_cast<int32_t>(context->bc->numInstructions - node->seekJumpAfterIfFalse - 1);

    freeRegisterRC(context, expression);
    freeRegisterRC(context, ifTrue);
    freeRegisterRC(context, ifFalse);

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitExpressionListBefore(ByteCodeGenContext* context)
{
    const auto node = context->node;

    // Do not generate bytecode for children in case of a constant expression, because
    // the full content of the expression is in the constant segment
    if (node->hasAstFlag(AST_CONST_EXPR))
        node->addAstFlag(AST_NO_BYTECODE_CHILDREN);

    return true;
}

void ByteCodeGen::collectLiteralsChildren(AstNode* node, VectorNative<AstNode*>* orderedChildren)
{
    for (auto child : node->children)
    {
        if (child->is(AstNodeKind::ExpressionList))
            collectLiteralsChildren(child, orderedChildren);
        else
            orderedChildren->push_back(child);
    }
}

bool ByteCodeGen::emitExpressionList(ByteCodeGenContext* context)
{
    const auto node     = castAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);
    TypeInfoList *typeList;

    if(node->typeInfoCast)
        typeList = castTypeInfo<TypeInfoList>(node->typeInfoCast, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);
    else
        typeList = castTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);

    // A non const expression list will be collected by the top ExpressionList
    if (!node->hasAstFlag(AST_CONST_EXPR))
    {
        if (node->parent->is(AstNodeKind::ExpressionList))
            return true;
    }

    if (node->hasSpecFlag(AstExpressionList::SPEC_FLAG_FOR_TUPLE))
        node->resultRegisterRc = reserveRegisterRC(context);
    else
        reserveLinearRegisterRC2(context, node->resultRegisterRc);

    if (!node->hasAstFlag(AST_CONST_EXPR))
    {
        context->collectChildren.clear();
        collectLiteralsChildren(node, &context->collectChildren);

        // Wait for generated struct if necessary
        for (const auto child : context->collectChildren)
        {
            const auto typeChild = TypeManager::concreteType(child->typeInfo);
            if (typeChild->isStruct())
            {
                Semantic::waitStructGenerated(context->baseJob, typeChild);
                YIELD();
            }
        }

        // Must be called only on the real node !
        const auto listNode = castAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

        // If in a return expression, just push the caller retval
        AstNode* parentReturn = listNode->inSimpleReturn();
        uint32_t startOffset  = 0;
        bool     canDrop      = true;
        if (parentReturn)
        {
            if (node->hasOwnerInline())
            {
                freeRegisterRC(context, node->resultRegisterRc);
                node->resultRegisterRc            = node->ownerInline()->resultRegisterRc;
                node->resultRegisterRc.cannotFree = true;
            }
            else
            {
                EMIT_INST1(context, ByteCodeOp::CopyRRtoRA, node->resultRegisterRc);
            }

            context->bc->maxCallResults = max(context->bc->maxCallResults, 1);
            parentReturn->addSemFlag(SEMFLAG_RETVAL);
            canDrop = false;
        }

        // If initializing a variable, then push the variable storage directly if possible.
        // That way can avoid one affectation.
        else if (listNode->parent->is(AstNodeKind::VarDecl))
        {
            const auto varDecl = castAst<AstVarDecl>(listNode->parent, AstNodeKind::VarDecl);
            if (varDecl->assignment == listNode)
            {
                const auto typeVar = TypeManager::concreteType(varDecl->typeInfo, CONCRETE_ALIAS);
                if (!typeVar->isSlice() && !varDecl->hasExtraPointer(ExtraPointerKind::UserOp))
                {
                    startOffset     = listNode->parent->resolvedSymbolOverload()->computedValue.storageOffset;
                    const auto inst = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRc);
                    inst->b.u64     = startOffset;
                    canDrop         = false;
                }
            }
        }

        // Default : assign to temporary storage
        if (canDrop)
        {
            startOffset     = listNode->computedValue()->storageOffset;
            const auto inst = EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRc);
            inst->b.u64     = startOffset;
        }

        // Emit one affectation per child
        const auto   oneOffset   = typeList->subTypes.front()->typeInfo->sizeOf;
        size_t       totalOffset = 0;
        RegisterList r0;
        reserveRegisterRC(context, r0, 1);
        for (const auto child : context->collectChildren)
        {
            child->addAstFlag(AST_NO_LEFT_DROP);

            const auto inst = EMIT_INST3(context, ByteCodeOp::IncPointer64, node->resultRegisterRc, 0, r0);
            SWAG_ASSERT(totalOffset != 0xFFFFFFFF);
            inst->b.u64 = totalOffset;
            inst->addFlag(BCI_IMM_B);

            const auto saveR = context->node->resultRegisterRc;
            SWAG_CHECK(emitCast(context, child, child->typeInfo, child->typeInfoCast));
            context->node->resultRegisterRc = saveR;

            emitAffectEqual(context, r0, child->resultRegisterRc, child->typeInfo, child);

            SWAG_ASSERT(context->result == ContextResult::Done);
            freeRegisterRC(context, child);

            if (canDrop)
            {
                const auto varOffset = static_cast<uint32_t>(listNode->computedValue()->storageOffset + totalOffset);
                node->ownerScope->symTable.addVarToDrop(nullptr, child->typeInfo, varOffset);
            }

            totalOffset += oneOffset;
        }
        freeRegisterRC(context, r0);

        // Reference to the stack, and store the number of element in a register
        if (canDrop)
        {
            EMIT_INST1(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRc[0])->b.u64 = startOffset;
            if (!node->hasSpecFlag(AstExpressionList::SPEC_FLAG_FOR_TUPLE))
                EMIT_INST1(context, ByteCodeOp::SetImmediate64, node->resultRegisterRc[1])->b.u64 = listNode->childCount();
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
        if (!node->hasSemFlag(SEMFLAG_EXPR_LIST_CST))
        {
            node->addSemFlag(SEMFLAG_EXPR_LIST_CST);
            node->allocateComputedValue();
            node->computedValue()->storageSegment = Semantic::getConstantSegFromContext(node);
            SWAG_CHECK(Semantic::reserveAndStoreToSegment(context, node->computedValue()->storageSegment, node->computedValue()->storageOffset, nullptr, typeList, node));
        }

        emitMakeSegPointer(context, node->computedValue()->storageSegment, node->computedValue()->storageOffset, node->resultRegisterRc[0]);
        if (!node->hasSpecFlag(AstExpressionList::SPEC_FLAG_FOR_TUPLE))
            EMIT_INST1(context, ByteCodeOp::SetImmediate64, node->resultRegisterRc[1])->b.u64 = typeList->subTypes.size();
    }

    return true;
}

bool ByteCodeGen::emitLiteral(ByteCodeGenContext* context)
{
    const auto node = context->node;
    if (node->hasSemFlag(SEMFLAG_LITERAL_SUFFIX))
        return context->report({node->firstChild(), formErr(Err0267, node->firstChild()->token.cstr())});
    SWAG_CHECK(emitLiteral(context, node, nullptr, node->resultRegisterRc));
    return true;
}

bool ByteCodeGen::emitLiteral(ByteCodeGenContext* context, AstNode* node, const TypeInfo* toType, RegisterList& regList)
{
    auto typeInfo = node->typeInfoCast ? node->typeInfoCast : node->typeInfo;
    typeInfo      = TypeManager::concreteType(typeInfo);

    AstIdentifierRef* identifierRef = nullptr;
    if (node->is(AstNodeKind::Identifier))
    {
        const auto identifier = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
        identifierRef         = identifier->identifierRef();
    }
    else if (node->is(AstNodeKind::ArrayPointerIndex))
    {
        identifierRef = castAst<AstIdentifierRef>(node->findParent(AstNodeKind::IdentifierRef));
    }

    // A reference to a segment
    if (node->isForceTakeAddress() || (typeInfo->isPointer() && !typeInfo->isPointerNull()))
    {
        if (node->computedValue()->storageSegment && node->computedValue()->storageOffset != UINT32_MAX)
        {
            regList = reserveRegisterRC(context);
            emitMakeSegPointer(context, node->computedValue()->storageSegment, node->computedValue()->storageOffset, regList[0]);
            if (identifierRef)
                identifierRef->resultRegisterRc = regList;
            return true;
        }
    }

    if (typeInfo->isAny())
    {
        // If we need a cast to an any, then first resolve literal with its real type
        if (node->typeInfoCast)
        {
            typeInfo = node->typeInfoCast;
        }

        // Otherwise we dereference the any in 2 registers
        else
        {
            reserveLinearRegisterRC2(context, regList);
            emitMakeSegPointer(context, node->computedValue()->storageSegment, node->computedValue()->storageOffset, regList);
            EMIT_INST2(context, ByteCodeOp::DeRefStringSlice, regList[0], regList[1]);
            return true;
        }
    }

    // We have null
    if (node->typeInfoCast && node->typeInfoCast->isPointerNull())
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
        emitMakeSegPointer(context, node->computedValue()->storageSegment, node->computedValue()->storageOffset, regList[0]);
    }
    else if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
            case NativeTypeKind::Bool:
                EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.b = node->computedValue()->reg.b;
                if (mustEmitSafety(context, SAFETY_BOOL) && node->computedValue()->reg.u8 & 0xFE)
                    return context->report({node, toErr(Saf0003)});
                return true;
            case NativeTypeKind::U8:
                EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.u8 = node->computedValue()->reg.u8;
                return true;
            case NativeTypeKind::U16:
                EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.u16 = node->computedValue()->reg.u16;
                return true;
            case NativeTypeKind::U32:
                EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.u64 = node->computedValue()->reg.u32;
                return true;
            case NativeTypeKind::U64:
                EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList)->b.u64 = node->computedValue()->reg.u64;
                return true;
            case NativeTypeKind::S8:
                EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.s8 = node->computedValue()->reg.s8;
                return true;
            case NativeTypeKind::S16:
                EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.s16 = node->computedValue()->reg.s16;
                return true;
            case NativeTypeKind::S32:
                EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.s32 = node->computedValue()->reg.s32;
                return true;
            case NativeTypeKind::S64:
                EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList)->b.s64 = node->computedValue()->reg.s64;
                return true;
            case NativeTypeKind::F32:
                EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.f32 = node->computedValue()->reg.f32;
                if (mustEmitSafety(context, SAFETY_NAN) && isnan(node->computedValue()->reg.f32))
                    return context->report({node, toErr(Saf0017)});
                return true;
            case NativeTypeKind::F64:
                EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList)->b.f64 = node->computedValue()->reg.f64;
                if (mustEmitSafety(context, SAFETY_NAN) && isnan(node->computedValue()->reg.f64))
                    return context->report({node, toErr(Saf0017)});
                return true;
            case NativeTypeKind::Rune:
                EMIT_INST1(context, ByteCodeOp::SetImmediate32, regList)->b.u64 = node->computedValue()->reg.u32;
                return true;
            case NativeTypeKind::String:
            {
                reserveLinearRegisterRC2(context, regList);
                const auto storageSegment = Semantic::getConstantSegFromContext(node);
                const auto storageOffset  = storageSegment->addString(node->computedValue()->text);
                SWAG_ASSERT(storageOffset != UINT32_MAX);
                emitMakeSegPointer(context, storageSegment, storageOffset, regList[0]);
                EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[1])->b.u64 = node->computedValue()->text.length();
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
        const auto typeArray = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        reserveLinearRegisterRC2(context, regList);
        emitMakeSegPointer(context, node->computedValue()->storageSegment, node->computedValue()->storageOffset, regList[0]);
        EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[1])->b.u64 = typeArray->count;
    }
    else if (typeInfo->isStruct() || typeInfo->isListTuple() || typeInfo->isListArray())
    {
        emitMakeSegPointer(context, node->computedValue()->storageSegment, node->computedValue()->storageOffset, regList[0]);
    }
    else if (typeInfo->isPointer() && node->typeInfoCast && node->typeInfoCast->isString())
    {
        const auto storageSegment = Semantic::getConstantSegFromContext(node);
        const auto storageOffset  = storageSegment->addString(node->computedValue()->text);
        SWAG_ASSERT(storageOffset != UINT32_MAX);
        emitMakeSegPointer(context, storageSegment, storageOffset, regList[0]);
    }
    else if (typeInfo->isSlice() && node->typeInfoCast && node->typeInfoCast->isString())
    {
        reserveLinearRegisterRC2(context, regList);
        const auto storageSegment = Semantic::getConstantSegFromContext(node);
        const auto storageOffset  = storageSegment->addString(node->computedValue()->text);
        SWAG_ASSERT(storageOffset != UINT32_MAX);
        emitMakeSegPointer(context, storageSegment, storageOffset, regList[0]);
        EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[1])->b.u64 = node->computedValue()->text.length();
    }
    else if (typeInfo->isSlice())
    {
        // :SliceLiteral
        reserveLinearRegisterRC2(context, regList);
        const auto slice        = static_cast<SwagSlice*>(node->computedValue()->getStorageAddr());
        const auto offsetValues = node->computedValue()->storageSegment->offset(static_cast<uint8_t*>(slice->buffer));
        emitMakeSegPointer(context, node->computedValue()->storageSegment, offsetValues, regList[0]);
        EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList[1])->b.u64 = slice->count;
    }
    else if (typeInfo->isPointer())
    {
        EMIT_INST1(context, ByteCodeOp::SetImmediate64, regList)->b.u64 = node->computedValue()->reg.u64;
    }
    else
    {
        return Report::internalError(context->node, form("emitLiteral, unsupported type [[%s]]", typeInfo->getDisplayNameC()).cstr());
    }

    return true;
}

bool ByteCodeGen::emitComputedValue(ByteCodeGenContext* context)
{
    const auto node = context->node;
    SWAG_CHECK(emitLiteral(context));
    SWAG_CHECK(emitCast(context, node, TypeManager::concreteType(node->typeInfo), node->typeInfoCast));
    SWAG_ASSERT(context->result == ContextResult::Done);

    // To be sure that cast is treated once
    node->typeInfoCast = nullptr;

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitDefer(ByteCodeGenContext* context)
{
    const auto node = castAst<AstDefer>(context->node, AstNodeKind::Defer);
    SWAG_ASSERT(node->childCount() == 1);
    node->ownerScope->deferredNodes.push_back(node);
    return true;
}
