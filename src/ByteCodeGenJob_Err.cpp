#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "TypeManager.h"
#include "Ast.h"
#include "Module.h"
#include "Report.h"

bool ByteCodeGenJob::emitInitStackTrace(ByteCodeGenContext* context)
{
    if (context->sourceFile->module->buildCfg.errorStackTrace)
    {
        PushICFlags ic(context, BCI_TRYCATCH);
        auto        node = context->node;
        SWAG_ASSERT(node->ownerFct && node->ownerFct->registerGetContext != UINT32_MAX);
        EMIT_INST1(context, ByteCodeOp::InternalInitStackTrace, node->ownerFct->registerGetContext);
    }

    return true;
}

bool ByteCodeGenJob::emitTryThrowExit(ByteCodeGenContext* context, AstNode* fromNode)
{
    auto node = CastAst<AstTryCatchAssume>(fromNode, AstNodeKind::Try, AstNodeKind::TryCatch, AstNodeKind::Throw);

    // Push current error context in case the leave scope triggers some errors too
    if (!(context->node->semFlags & SEMFLAG_STACK_TRACE))
    {
        EMIT_INST0(context, ByteCodeOp::InternalPushErr);
        context->node->semFlags |= SEMFLAG_STACK_TRACE;
        context->tryCatchScope++;
    }

    // Leave the current scope
    SWAG_CHECK(emitLeaveScopeReturn(context, nullptr, true));
    if (context->result != ContextResult::Done)
        return true;

    // Restore the error context, and keep error trace of current call
    if (!(context->node->semFlags & SEMFLAG_STACK_TRACE1))
    {
        SWAG_ASSERT(context->tryCatchScope);
        context->tryCatchScope--;
        EMIT_INST0(context, ByteCodeOp::InternalPopErr);

        if (context->sourceFile->module->buildCfg.errorStackTrace)
        {
            auto         r0 = reserveRegisterRC(context);
            uint32_t     storageOffset;
            DataSegment* storageSegment;
            computeSourceLocation(context, context->node, &storageOffset, &storageSegment);
            emitMakeSegPointer(context, storageSegment, storageOffset, r0);
            EMIT_INST1(context, ByteCodeOp::InternalStackTrace, r0);
            freeRegisterRC(context, r0);
        }

        context->node->semFlags |= SEMFLAG_STACK_TRACE1;
    }

    TypeInfo* returnType = nullptr;
    if (node->ownerInline)
        returnType = TypeManager::concreteType(node->ownerInline->func->returnType->typeInfo, CONCRETE_FORCEALIAS);
    else
        returnType = TypeManager::concreteType(node->ownerFct->returnType->typeInfo, CONCRETE_FORCEALIAS);

    // Set default value
    if (!returnType->isVoid())
    {
        if (returnType->isStruct())
        {
            if (node->ownerInline)
                node->regInit = node->ownerInline->resultRegisterRC;
            else if (!(context->node->semFlags & SEMFLAG_TRY_2))
            {
                reserveRegisterRC(context, node->regInit, 1);
                EMIT_INST1(context, ByteCodeOp::CopyRRtoRC, node->regInit);
                context->node->semFlags |= SEMFLAG_TRY_2;
            }

            TypeInfoPointer pt;
            pt.pointedType = returnType;
            SWAG_CHECK(emitInit(context, &pt, node->regInit, 1, nullptr, nullptr));
            if (context->result != ContextResult::Done)
                return true;

            if (!node->ownerInline)
                freeRegisterRC(context, node->regInit);
        }
        else if (returnType->isArray())
        {
            auto typeArr = CastTypeInfo<TypeInfoArray>(returnType, TypeInfoKind::Array);
            if (!typeArr->finalType->isStruct())
            {
                if (node->ownerInline)
                {
                    EMIT_INST1(context, ByteCodeOp::SetZeroAtPointerX, node->ownerInline->resultRegisterRC)->b.u64 = typeArr->sizeOf;
                }
                else
                {
                    auto r0 = reserveRegisterRC(context);
                    EMIT_INST1(context, ByteCodeOp::CopyRRtoRC, r0);
                    EMIT_INST1(context, ByteCodeOp::SetZeroAtPointerX, r0)->b.u64 = typeArr->sizeOf;
                    freeRegisterRC(context, r0);
                }
            }
            else
            {
                if (!(context->node->semFlags & SEMFLAG_TRY_2))
                {
                    reserveRegisterRC(context, node->regInit, 1);
                    if (node->ownerInline)
                        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->regInit, node->ownerInline->resultRegisterRC);
                    else
                        EMIT_INST1(context, ByteCodeOp::CopyRRtoRC, node->regInit);
                    context->node->semFlags |= SEMFLAG_TRY_2;
                }

                TypeInfoPointer pt;
                pt.pointedType = typeArr->finalType;
                SWAG_CHECK(emitInit(context, &pt, node->regInit, typeArr->totalCount, nullptr, nullptr));
                if (context->result != ContextResult::Done)
                    return true;

                freeRegisterRC(context, node->regInit);
            }
        }
        else if (returnType->numRegisters() == 1)
        {
            if (node->ownerInline)
            {
                EMIT_INST1(context, ByteCodeOp::ClearRA, node->ownerInline->resultRegisterRC[0]);
            }
            else
            {
                auto r0 = reserveRegisterRC(context);
                EMIT_INST1(context, ByteCodeOp::ClearRA, r0);
                EMIT_INST1(context, ByteCodeOp::CopyRCtoRR, r0);
                freeRegisterRC(context, r0);
            }
        }
        else if (returnType->numRegisters() == 2)
        {
            if (node->ownerInline)
            {
                EMIT_INST1(context, ByteCodeOp::ClearRA, node->ownerInline->resultRegisterRC[0]);
                EMIT_INST1(context, ByteCodeOp::ClearRA, node->ownerInline->resultRegisterRC[1]);
            }
            else
            {
                auto r0 = reserveRegisterRC(context);
                EMIT_INST1(context, ByteCodeOp::ClearRA, r0);
                EMIT_INST2(context, ByteCodeOp::CopyRCtoRR2, r0, r0);
                freeRegisterRC(context, r0);
            }
        }
        else
        {
            Report::internalError(context->node, "emitTry, unsupported return type");
        }
    }

    // Return from function
    if (node->ownerInline)
    {
        node->seekJump = context->bc->numInstructions;
        EMIT_INST0(context, ByteCodeOp::Jump);
        node->ownerInline->returnList.push_back(node);
    }
    else
    {
        EMIT_INST0(context, ByteCodeOp::Ret)->a.u32 = node->ownerFct->stackSize;
    }

    return true;
}

bool ByteCodeGenJob::emitThrow(ByteCodeGenContext* context)
{
    PushICFlags ic(context, BCI_TRYCATCH);

    auto node = CastAst<AstTryCatchAssume>(context->node, AstNodeKind::Throw);
    auto expr = node->childs.front();

    if (!(node->semFlags & SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, expr, TypeManager::concreteType(expr->typeInfo), expr->castedTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->semFlags |= SEMFLAG_CAST1;
    }

    if (!(node->semFlags & SEMFLAG_TRY_1))
    {
        emitSafetyErrCheck(context, expr->resultRegisterRC[1]);
        EMIT_INST0(context, ByteCodeOp::PushRR);
        EMIT_INST2(context, ByteCodeOp::InternalSetErr, expr->resultRegisterRC[0], expr->resultRegisterRC[1]);
        node->semFlags |= SEMFLAG_TRY_1;
        EMIT_INST0(context, ByteCodeOp::PopRR);
    }

    // In a top level function, 'throw' is like a failed 'assume'
    auto parentFct = (node->semFlags & SEMFLAG_EMBEDDED_RETURN) ? node->ownerInline->func : node->ownerFct;
    if (parentFct->attributeFlags & ATTRIBUTE_SHARP_FUNC)
    {
        if (context->sourceFile->module->buildCfg.errorStackTrace)
        {
            SWAG_ASSERT(node->ownerFct && node->ownerFct->registerGetContext != UINT32_MAX);
            EMIT_INST1(context, ByteCodeOp::InternalInitStackTrace, node->ownerFct->registerGetContext);
        }

        uint32_t     storageOffset;
        DataSegment* storageSegment;
        computeSourceLocation(context, node, &storageOffset, &storageSegment);

        auto r1 = reserveRegisterRC(context);
        emitMakeSegPointer(context, storageSegment, storageOffset, r1);
        EMIT_INST1(context, ByteCodeOp::InternalFailedAssume, r1);
        freeRegisterRC(context, expr->resultRegisterRC);
        freeRegisterRC(context, r1);
    }
    else
    {
        freeRegisterRC(context, expr->resultRegisterRC);
        SWAG_CHECK(emitTryThrowExit(context, context->node));
        if (context->result != ContextResult::Done)
            return true;
    }

    return true;
}

bool ByteCodeGenJob::emitTry(ByteCodeGenContext* context)
{
    PushICFlags ic(context, BCI_TRYCATCH);

    // try in a top level function is equivalent to assume
    auto node    = context->node;
    auto tryNode = CastAst<AstTryCatchAssume>(node->extOwner()->ownerTryCatchAssume, AstNodeKind::Try);

    AstFuncDecl* parentFct = nullptr;
    if (node->ownerInline && (tryNode->semFlags & SEMFLAG_EMBEDDED_RETURN))
        parentFct = node->ownerInline->func;
    else
        parentFct = node->ownerFct;
    if (parentFct->attributeFlags & ATTRIBUTE_SHARP_FUNC)
        return emitAssume(context);

    if (!(node->semFlags & SEMFLAG_TRY_1))
    {
        SWAG_ASSERT(node->ownerFct->registerGetContext != UINT32_MAX);
        auto r0 = reserveRegisterRC(context);
        EMIT_INST2(context, ByteCodeOp::InternalHasErr, r0, node->ownerFct->registerGetContext);
        tryNode->seekInsideJump = context->bc->numInstructions;
        EMIT_INST1(context, ByteCodeOp::JumpIfZero32, r0);
        freeRegisterRC(context, r0);
        node->semFlags |= SEMFLAG_TRY_1;
    }

    SWAG_CHECK(emitTryThrowExit(context, tryNode));
    if (context->result != ContextResult::Done)
        return true;

    context->bc->out[tryNode->seekInsideJump].b.s32 = context->bc->numInstructions - tryNode->seekInsideJump - 1;
    SWAG_ASSERT(context->bc->out[tryNode->seekInsideJump].b.s32);
    return true;
}

bool ByteCodeGenJob::emitTryCatch(ByteCodeGenContext* context)
{
    auto node    = context->node;
    auto tryNode = CastAst<AstTryCatchAssume>(node->extOwner()->ownerTryCatchAssume, AstNodeKind::TryCatch);

    if (!(node->semFlags & SEMFLAG_TRY_1))
    {
        SWAG_ASSERT(node->ownerFct->registerGetContext != UINT32_MAX);
        auto r0 = reserveRegisterRC(context);
        EMIT_INST2(context, ByteCodeOp::InternalHasErr, r0, node->ownerFct->registerGetContext);
        tryNode->seekInsideJump = context->bc->numInstructions;
        EMIT_INST1(context, ByteCodeOp::JumpIfZero32, r0);
        freeRegisterRC(context, r0);
        node->semFlags |= SEMFLAG_TRY_1;
    }

    SWAG_CHECK(emitTryThrowExit(context, tryNode));
    if (context->result != ContextResult::Done)
        return true;

    context->bc->out[tryNode->seekInsideJump].b.s32 = context->bc->numInstructions - tryNode->seekInsideJump - 1;
    SWAG_ASSERT(context->bc->out[tryNode->seekInsideJump].b.s32);
    return true;
}

bool ByteCodeGenJob::emitAssume(ByteCodeGenContext* context)
{
    if (!context->sourceFile->module->buildCfg.byteCodeEmitAssume)
        return true;

    PushICFlags ic(context, BCI_TRYCATCH);

    auto node       = context->node;
    auto assumeNode = CastAst<AstTryCatchAssume>(node->extOwner()->ownerTryCatchAssume, AstNodeKind::Try, AstNodeKind::Assume);

    SWAG_ASSERT(node->ownerFct->registerGetContext != UINT32_MAX);
    auto rt = reserveRegisterRC(context);
    EMIT_INST2(context, ByteCodeOp::InternalHasErr, rt, node->ownerFct->registerGetContext);
    assumeNode->seekInsideJump = context->bc->numInstructions;
    EMIT_INST1(context, ByteCodeOp::JumpIfZero32, rt);
    freeRegisterRC(context, rt);

    uint32_t     storageOffset;
    DataSegment* storageSegment;
    computeSourceLocation(context, context->node, &storageOffset, &storageSegment);

    auto r1 = reserveRegisterRC(context);
    emitMakeSegPointer(context, storageSegment, storageOffset, r1);
    EMIT_INST1(context, ByteCodeOp::InternalFailedAssume, r1);
    freeRegisterRC(context, r1);

    context->bc->out[assumeNode->seekInsideJump].b.s32 = context->bc->numInstructions - assumeNode->seekInsideJump - 1;
    return true;
}