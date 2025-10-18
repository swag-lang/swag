#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Wmf/Module.h"

bool ByteCodeGen::emitInitStackTrace(ByteCodeGenContext* context)
{
    if (context->sourceFile->module->buildCfg.errorStackTrace)
    {
        PushICFlags ic(context, BCI_TRY_CATCH);
        const auto  node = context->node;
        SWAG_ASSERT(node->ownerFct && node->ownerFct->registerGetContext != UINT32_MAX);
        EMIT_INST1(context, ByteCodeOp::InternalInitStackTrace, node->ownerFct->registerGetContext);
    }

    return true;
}

bool ByteCodeGen::emitTryThrowExit(ByteCodeGenContext* context, AstNode* fromNode)
{
    const auto node = castAst<AstTryCatchAssume>(fromNode, AstNodeKind::Try, AstNodeKind::TryCatch, AstNodeKind::Throw);

    // Push current error context in case the leave scope triggers some errors too
    if (!context->node->hasSemFlag(SEMFLAG_STACK_TRACE))
    {
        EMIT_INST0(context, ByteCodeOp::InternalPushErr);
        context->node->addSemFlag(SEMFLAG_STACK_TRACE);
        context->tryCatchScope++;
    }

    // Leave the current scope
    SWAG_CHECK(emitLeaveScopeReturn(context, nullptr, true));
    YIELD();

    // Restore the error context, and keep error trace of current call
    if (!context->node->hasSemFlag(SEMFLAG_STACK_TRACE1))
    {
        SWAG_ASSERT(context->tryCatchScope);
        context->tryCatchScope--;
        EMIT_INST0(context, ByteCodeOp::InternalPopErr);

        if (context->sourceFile->module->buildCfg.errorStackTrace)
        {
            const auto   r0 = reserveRegisterRC(context);
            uint32_t     storageOffset;
            DataSegment* storageSegment;
            computeSourceLocation(context, context->node, &storageOffset, &storageSegment);
            if (storageSegment->kind == SegmentKind::Constant)
            {
                EMIT_INST2(context, ByteCodeOp::InternalStackTraceConst, r0, storageOffset);
            }
            else
            {
                emitMakeSegPointer(context, storageSegment, storageOffset, r0);
                EMIT_INST1(context, ByteCodeOp::InternalStackTrace, r0);
            }
            freeRegisterRC(context, r0);
        }

        context->node->addSemFlag(SEMFLAG_STACK_TRACE1);
    }

    TypeInfo* returnType = nullptr;
    if (node->hasOwnerInline())
        returnType = node->ownerInline()->func->returnType->typeInfo->getConcreteAlias();
    else
        returnType = node->ownerFct->returnType->typeInfo->getConcreteAlias();

    // Set default value
    if (!returnType->isVoid())
    {
        if (returnType->isStruct())
        {
            if (node->hasOwnerInline())
                node->regInit = node->ownerInline()->resultRegisterRc;
            else if (!context->node->hasSemFlag(SEMFLAG_TRY_2))
            {
                reserveRegisterRC(context, node->regInit, 1);
                EMIT_INST1(context, ByteCodeOp::CopyRRtoRA, node->regInit);
                context->node->addSemFlag(SEMFLAG_TRY_2);
            }

            TypeInfoPointer pt;
            pt.pointedType = returnType;
            SWAG_CHECK(emitInit(context, returnType, node->regInit, 1, nullptr, nullptr));
            YIELD();

            if (!node->hasOwnerInline())
                freeRegisterRC(context, node->regInit);
        }
        else if (returnType->isArray())
        {
            const auto typeArr = castTypeInfo<TypeInfoArray>(returnType, TypeInfoKind::Array);
            if (!typeArr->finalType->isStruct())
            {
                if (node->hasOwnerInline())
                {
                    EMIT_INST1(context, ByteCodeOp::SetZeroAtPointerX, node->ownerInline()->resultRegisterRc)->b.u64 = typeArr->sizeOf;
                }
                else
                {
                    const auto r0 = reserveRegisterRC(context);
                    EMIT_INST1(context, ByteCodeOp::CopyRRtoRA, r0);
                    EMIT_INST1(context, ByteCodeOp::SetZeroAtPointerX, r0)->b.u64 = typeArr->sizeOf;
                    freeRegisterRC(context, r0);
                }
            }
            else
            {
                if (!context->node->hasSemFlag(SEMFLAG_TRY_2))
                {
                    reserveRegisterRC(context, node->regInit, 1);
                    if (node->hasOwnerInline())
                        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, node->regInit, node->ownerInline()->resultRegisterRc);
                    else
                        EMIT_INST1(context, ByteCodeOp::CopyRRtoRA, node->regInit);
                    context->node->addSemFlag(SEMFLAG_TRY_2);
                }

                TypeInfoPointer pt;
                pt.pointedType = typeArr->finalType;
                SWAG_CHECK(emitInit(context, typeArr->finalType, node->regInit, typeArr->totalCount, nullptr, nullptr));
                YIELD();

                freeRegisterRC(context, node->regInit);
            }
        }
        else if (returnType->numRegisters() == 1)
        {
            if (node->hasOwnerInline())
            {
                EMIT_INST1(context, ByteCodeOp::ClearRA, node->ownerInline()->resultRegisterRc[0]);
            }
            else
            {
                const auto r0 = reserveRegisterRC(context);
                EMIT_INST1(context, ByteCodeOp::ClearRA, r0);
                const auto inst = EMIT_INST1(context, ByteCodeOp::CopyRAtoRR, r0);
                inst->flags.add(BCI_NOT_NULL);
                freeRegisterRC(context, r0);
            }
        }
        else if (returnType->numRegisters() == 2)
        {
            if (node->hasOwnerInline())
            {
                EMIT_INST1(context, ByteCodeOp::ClearRA, node->ownerInline()->resultRegisterRc[0]);
                EMIT_INST1(context, ByteCodeOp::ClearRA, node->ownerInline()->resultRegisterRc[1]);
            }
            else
            {
                const auto r0 = reserveRegisterRC(context);
                EMIT_INST1(context, ByteCodeOp::ClearRA, r0);
                const auto inst = EMIT_INST2(context, ByteCodeOp::CopyRARBtoRR2, r0, r0);
                inst->flags.add(BCI_NOT_NULL);
                freeRegisterRC(context, r0);
            }
        }
        else
        {
            Report::internalError(context->node, "emitTry, unsupported return type");
        }
    }

    // Return from function
    if (node->hasOwnerInline())
    {
        node->seekJump = context->bc->numInstructions;
        EMIT_INST0(context, ByteCodeOp::Jump);
        node->ownerInline()->returnList.push_back(node);
    }
    else
    {
        EMIT_INST0(context, ByteCodeOp::Ret);
    }

    return true;
}

bool ByteCodeGen::checkEscapedThrow(ByteCodeGenContext* context)
{
    auto node = context->node;
    if (!node->hasAstFlag(AST_IN_DEFER))
        return true;

    const auto parent = node->findParent(AstNodeKind::Defer);
    SWAG_ASSERT(parent);
    const auto defer = castAst<AstDefer>(parent, AstNodeKind::Defer);
    if (defer->deferKind == DeferKind::NoError)
        return true;

    if (node->hasOwnerInline())
    {
        while (node->hasOwnerInline())
            node = node->ownerInline();

        auto hasCatch = node;
        while (hasCatch != defer)
        {
            if (hasCatch->is(AstNodeKind::Catch))
                return true;
            hasCatch = hasCatch->parent;
        }
    }

    Diagnostic err{node, toErr(Err0214)};
    err.addNote(Diagnostic::hereIs(defer));
    return context->report(err);
}

bool ByteCodeGen::emitThrow(ByteCodeGenContext* context)
{
    SWAG_CHECK(checkEscapedThrow(context));

    PushICFlags ic(context, BCI_TRY_CATCH);
    const auto  node = castAst<AstTryCatchAssume>(context->node, AstNodeKind::Throw);
    const auto  expr = node->firstChild();

    if (!node->hasSemFlag(SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, expr, TypeManager::concreteType(expr->typeInfo), expr->typeInfoCast));
        YIELD();
        node->addSemFlag(SEMFLAG_CAST1);
    }

    if (!node->hasSemFlag(SEMFLAG_TRY_1))
    {
        emitSafetyErrCheck(context, expr->resultRegisterRc[1]);
        EMIT_INST2(context, ByteCodeOp::InternalSetErr, expr->resultRegisterRc[0], expr->resultRegisterRc[1]);
        node->addSemFlag(SEMFLAG_TRY_1);
        EMIT_INST0(context, ByteCodeOp::PopRR);
    }

    // In a top level function, 'throw' is like a failed 'assume'
    const auto parentFct = node->hasSemFlag(SEMFLAG_EMBEDDED_RETURN) ? node->ownerInline()->func : node->ownerFct;
    if (parentFct->hasAttribute(ATTRIBUTE_SHARP_FUNC))
    {
        if (context->sourceFile->module->buildCfg.errorStackTrace)
        {
            SWAG_ASSERT(node->ownerFct && node->ownerFct->registerGetContext != UINT32_MAX);
            EMIT_INST1(context, ByteCodeOp::InternalInitStackTrace, node->ownerFct->registerGetContext);
        }

        uint32_t     storageOffset;
        DataSegment* storageSegment;
        computeSourceLocation(context, node, &storageOffset, &storageSegment);

        const auto r1 = reserveRegisterRC(context);
        emitMakeSegPointer(context, storageSegment, storageOffset, r1);
        EMIT_INST1(context, ByteCodeOp::InternalFailedAssume, r1);
        freeRegisterRC(context, expr->resultRegisterRc);
        freeRegisterRC(context, r1);
    }
    else
    {
        freeRegisterRC(context, expr->resultRegisterRc);
        SWAG_CHECK(emitTryThrowExit(context, context->node));
        YIELD();
    }

    return true;
}

bool ByteCodeGen::emitTry(ByteCodeGenContext* context)
{
    SWAG_CHECK(checkEscapedThrow(context));

    PushICFlags ic(context, BCI_TRY_CATCH);
    const auto  node    = context->node;
    const auto  tryNode = castAst<AstTryCatchAssume>(node->ownerTryCatchAssume(), AstNodeKind::Try);

    // try in a top level function is equivalent to assume
    const AstFuncDecl* parentFct = nullptr;
    if (node->hasOwnerInline() && tryNode->hasSemFlag(SEMFLAG_EMBEDDED_RETURN))
        parentFct = node->ownerInline()->func;
    else
        parentFct = node->ownerFct;
    if (parentFct->hasAttribute(ATTRIBUTE_SHARP_FUNC))
        return emitAssume(context);

    if (!node->hasSemFlag(SEMFLAG_TRY_1))
    {
        SWAG_ASSERT(node->ownerFct->registerGetContext != UINT32_MAX);
        const auto r0 = reserveRegisterRC(context);
        EMIT_INST2(context, ByteCodeOp::InternalHasErr, r0, node->ownerFct->registerGetContext);
        tryNode->seekInsideJump = context->bc->numInstructions;
        EMIT_INST1(context, ByteCodeOp::JumpIfZero32, r0);
        freeRegisterRC(context, r0);
        node->addSemFlag(SEMFLAG_TRY_1);
    }

    SWAG_CHECK(emitTryThrowExit(context, tryNode));
    YIELD();

    context->bc->out[tryNode->seekInsideJump].b.s32 = static_cast<int32_t>(context->bc->numInstructions - tryNode->seekInsideJump - 1);
    SWAG_ASSERT(context->bc->out[tryNode->seekInsideJump].b.s32);
    return true;
}

bool ByteCodeGen::emitTryCatch(ByteCodeGenContext* context)
{
    const auto node    = context->node;
    const auto tryNode = castAst<AstTryCatchAssume>(node->ownerTryCatchAssume(), AstNodeKind::TryCatch);

    if (!node->hasSemFlag(SEMFLAG_TRY_1))
    {
        SWAG_ASSERT(node->ownerFct->registerGetContext != UINT32_MAX);
        const auto r0 = reserveRegisterRC(context);
        EMIT_INST2(context, ByteCodeOp::InternalHasErr, r0, node->ownerFct->registerGetContext);
        tryNode->seekInsideJump = context->bc->numInstructions;
        EMIT_INST1(context, ByteCodeOp::JumpIfZero32, r0);
        freeRegisterRC(context, r0);
        node->addSemFlag(SEMFLAG_TRY_1);
        EMIT_INST0(context, ByteCodeOp::InternalCatchErr);
    }

    SWAG_CHECK(emitTryThrowExit(context, tryNode));
    YIELD();

    context->bc->out[tryNode->seekInsideJump].b.s32 = static_cast<int32_t>(context->bc->numInstructions - tryNode->seekInsideJump - 1);
    SWAG_ASSERT(context->bc->out[tryNode->seekInsideJump].b.s32);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitCatch(ByteCodeGenContext* context)
{
    EMIT_INST0(context, ByteCodeOp::InternalCatchErr);
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitAssumeNotNull(ByteCodeGenContext* context)
{
    if (!context->sourceFile->module->buildCfg.byteCodeEmitAssume)
        return true;

    const auto node = context->node;
    EMIT_INST1(context, ByteCodeOp::JumpIfNotZero64, node->resultRegisterRc)->b.s32 = 1;
    EMIT_INST0(context, ByteCodeOp::InternalPanic)->d.pointer = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>("assume failed"));
    return true;
}

bool ByteCodeGen::emitAssume(ByteCodeGenContext* context)
{
    if (!context->sourceFile->module->buildCfg.byteCodeEmitAssume)
        return true;

    PushICFlags ic(context, BCI_TRY_CATCH);

    const auto node       = context->node;
    const auto assumeNode = castAst<AstTryCatchAssume>(node->ownerTryCatchAssume(), AstNodeKind::Try, AstNodeKind::Assume);

    SWAG_ASSERT(node->ownerFct->registerGetContext != UINT32_MAX);
    const auto rt = reserveRegisterRC(context);
    EMIT_INST2(context, ByteCodeOp::InternalHasErr, rt, node->ownerFct->registerGetContext);
    assumeNode->seekInsideJump = context->bc->numInstructions;
    EMIT_INST1(context, ByteCodeOp::JumpIfZero32, rt);
    freeRegisterRC(context, rt);

    uint32_t     storageOffset;
    DataSegment* storageSegment;
    computeSourceLocation(context, context->node, &storageOffset, &storageSegment);

    const auto r1 = reserveRegisterRC(context);
    emitMakeSegPointer(context, storageSegment, storageOffset, r1);
    EMIT_INST1(context, ByteCodeOp::InternalFailedAssume, r1);
    freeRegisterRC(context, r1);

    context->bc->out[assumeNode->seekInsideJump].b.s32 = static_cast<int32_t>(context->bc->numInstructions - assumeNode->seekInsideJump - 1);
    return true;
}
