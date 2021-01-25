#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Module.h"
#include "Ast.h"
#include "Runtime.h"

void ByteCodeGenJob::emitAssert(ByteCodeGenContext* context, uint32_t reg, const char* msg)
{
    emitInstruction(context, ByteCodeOp::JumpIfNotZero8, reg)->b.s32 = 1;
    emitInstruction(context, ByteCodeOp::IntrinsicAssert)->d.pointer = (uint8_t*) msg;
}

bool ByteCodeGenJob::mustEmitSafety(ByteCodeGenContext* context)
{
    if (context->contextFlags & BCC_FLAG_NOSAFETY)
        return false;
    if (!context->sourceFile->module->mustEmitSafety(context->node))
        return false;
    return true;
}

void ByteCodeGenJob::emitSafetyNotZero(ByteCodeGenContext* context, uint32_t r, uint32_t bits, const char* message)
{
    if (!mustEmitSafety(context))
        return;

    PushICFlags ic(context, BCI_SAFETY);
    auto        r0 = reserveRegisterRC(context);
    if (bits == 8)
        emitInstruction(context, ByteCodeOp::TestNotZero8, r0, r);
    else if (bits == 16)
        emitInstruction(context, ByteCodeOp::TestNotZero16, r0, r);
    else if (bits == 32)
        emitInstruction(context, ByteCodeOp::TestNotZero32, r0, r);
    else if (bits == 64)
        emitInstruction(context, ByteCodeOp::TestNotZero64, r0, r);
    else
        SWAG_ASSERT(false);
    emitAssert(context, r0, message);
    freeRegisterRC(context, r0);
}

void ByteCodeGenJob::emitSafetyNullPointer(ByteCodeGenContext* context, uint32_t r, const char* message)
{
    if (!mustEmitSafety(context))
        return;

    emitSafetyNotZero(context, r, 64, message);
}

void ByteCodeGenJob::emitSafetyNullLambda(ByteCodeGenContext* context, uint32_t r, const char* message)
{
    if (!mustEmitSafety(context))
        return;

    auto re = reserveRegisterRC(context);
    emitInstruction(context, ByteCodeOp::CopyRBtoRA, re, r);
    auto inst = emitInstruction(context, ByteCodeOp::BinOpBitmaskAndS64, re, 0, re);
    inst->flags |= BCI_IMM_B;
    inst->b.u64 = ~SWAG_LAMBDA_MARKER_MASK;

    emitSafetyNotZero(context, re, 64, message);

    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyDivZero(ByteCodeGenContext* context, uint32_t r, uint32_t bits)
{
    if (!mustEmitSafety(context))
        return;

    emitSafetyNotZero(context, r, bits, "division by zero");
}

void ByteCodeGenJob::emitSafetyBoundCheckLower(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);
    emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r0, r1, re);
    emitAssert(context, re, "index out of range");
    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyBoundCheckLower64(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    PushICFlags ic(context, BCI_SAFETY);
    auto        re = reserveRegisterRC(context);
    emitInstruction(context, ByteCodeOp::CompareOpLowerU64, r0, r1, re);
    emitAssert(context, re, "index out of range");
    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyBoundCheckLowerEq(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);
    emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, r0, r1, re);
    emitInstruction(context, ByteCodeOp::NegBool, re);
    emitAssert(context, re, "index out of range");
    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyBoundCheckString(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    if (!mustEmitSafety(context))
        return;

    emitSafetyBoundCheckLowerEq(context, r0, r1);
}

void ByteCodeGenJob::emitSafetyBoundCheckSlice(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    if (!mustEmitSafety(context))
        return;

    emitSafetyBoundCheckLower64(context, r0, r1);
}

void ByteCodeGenJob::emitSafetyBoundCheckArray(ByteCodeGenContext* context, uint32_t r0, TypeInfoArray* typeInfoArray)
{
    if (!mustEmitSafety(context))
        return;

    PushICFlags ic(context, BCI_SAFETY);
    auto        r1   = reserveRegisterRC(context);
    auto        inst = emitInstruction(context, ByteCodeOp::SetImmediate64, r1);
    inst->b.u64      = typeInfoArray->count;
    emitSafetyBoundCheckLower64(context, r0, r1);
    freeRegisterRC(context, r1);
}

void ByteCodeGenJob::emitSafetyCastAny(ByteCodeGenContext* context, AstNode* exprNode)
{
    if (!mustEmitSafety(context))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    auto r0     = reserveRegisterRC(context);
    auto inst   = emitInstruction(context, ByteCodeOp::MakeTypeSegPointer, r0);
    inst->b.u64 = exprNode->concreteTypeInfoStorage;

    RegisterList result = reserveRegisterRC(context);
    inst                = emitInstruction(context, ByteCodeOp::SetImmediate32, result);
    inst->b.u32         = Runtime::COMPARE_CAST_ANY;
    inst                = emitInstruction(context, ByteCodeOp::IntrinsicTypeCmp, r0, exprNode->resultRegisterRC[1], result, result);
    emitAssert(context, result, "invalid dynamic cast");

    freeRegisterRC(context, result);
    freeRegisterRC(context, r0);
}

void ByteCodeGenJob::emitSafetyArrayPointerSlicing(ByteCodeGenContext* context, AstArrayPointerSlicing* node)
{
    if (!mustEmitSafety(context))
        return;

    PushICFlags ic(context, BCI_SAFETY);
    uint32_t    maxBoundReg     = UINT32_MAX;
    bool        freeMaxBoundReg = false;

    // Check type, and safety check
    auto typeVar = node->array->typeInfo;
    if (typeVar->kind == TypeInfoKind::Array)
    {
        auto typeArray  = CastTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array);
        maxBoundReg     = reserveRegisterRC(context);
        auto inst       = emitInstruction(context, ByteCodeOp::SetImmediate64, maxBoundReg);
        inst->b.u64     = typeArray->count;
        freeMaxBoundReg = true;
    }
    else if (typeVar->isNative(NativeTypeKind::String))
    {
        maxBoundReg = node->array->resultRegisterRC[1];
    }
    else if (typeVar->kind == TypeInfoKind::Slice)
    {
        maxBoundReg = node->array->resultRegisterRC[1];
    }

    // Lower bound <= upper bound
    {
        auto re = reserveRegisterRC(context);
        context->pushLocation(&node->lowerBound->token.startLocation);
        emitInstruction(context, ByteCodeOp::CompareOpGreaterU64, node->lowerBound->resultRegisterRC, node->upperBound->resultRegisterRC, re);
        emitInstruction(context, ByteCodeOp::NegBool, re);
        emitAssert(context, re, "bad slicing, lower bound is greater than upper bound");
        context->popLocation();
        freeRegisterRC(context, re);
    }

    // Upper bound < maxBound
    if (maxBoundReg != UINT32_MAX)
    {
        auto re = reserveRegisterRC(context);
        context->pushLocation(&node->upperBound->token.startLocation);
        emitInstruction(context, ByteCodeOp::CompareOpLowerU64, node->upperBound->resultRegisterRC, maxBoundReg, re);
        emitAssert(context, re, "bad slicing, upper bound is out of range");
        context->popLocation();
        freeRegisterRC(context, re);
        if (freeMaxBoundReg)
            freeRegisterRC(context, maxBoundReg);
    }
}