#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Module.h"
#include "Ast.h"

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
    emitInstruction(context, ByteCodeOp::IntrinsicAssert, r0)->d.pointer = (uint8_t*) message;
    freeRegisterRC(context, r0);
}

void ByteCodeGenJob::emitSafetyNullPointer(ByteCodeGenContext* context, uint32_t r, const char* message)
{
    if (!mustEmitSafety(context))
        return;

    emitSafetyNotZero(context, r, 64, message);
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
    emitInstruction(context, ByteCodeOp::IntrinsicAssert, re)->d.pointer = (uint8_t*) "index out of range";

    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyBoundCheckLowerEq(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);

    emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, r0, r1, re);
    emitInstruction(context, ByteCodeOp::NegBool, re);
    emitInstruction(context, ByteCodeOp::IntrinsicAssert, re)->d.pointer = (uint8_t*) "index out of range";

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

    emitSafetyBoundCheckLower(context, r0, r1);
}

void ByteCodeGenJob::emitSafetyBoundCheckArray(ByteCodeGenContext* context, uint32_t r0, TypeInfoArray* typeInfo)
{
    if (!mustEmitSafety(context))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    auto r1 = reserveRegisterRC(context);

    auto inst   = emitInstruction(context, ByteCodeOp::SetImmediate32, r1);
    inst->b.u32 = typeInfo->count;
    emitSafetyBoundCheckLower(context, r0, r1);

    freeRegisterRC(context, r1);
}

void ByteCodeGenJob::emitSafetyBoundCheckVariadic(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    if (!mustEmitSafety(context))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    auto r2 = reserveRegisterRC(context);

    emitInstruction(context, ByteCodeOp::CopyRBtoRA, r2, r1);
    emitInstruction(context, ByteCodeOp::DeRef64, r2);
    emitInstruction(context, ByteCodeOp::ClearMaskU64, r2)->b.u64 = 0x00000000'FFFFFFFF;
    emitSafetyBoundCheckLower(context, r0, r2);

    freeRegisterRC(context, r2);
}

void ByteCodeGenJob::emitSafetyCastAny(ByteCodeGenContext* context, AstNode* exprNode)
{
    if (!mustEmitSafety(context))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    auto r0   = reserveRegisterRC(context);
    auto inst = emitInstruction(context, ByteCodeOp::MakeTypeSegPointer, r0);
    SWAG_ASSERT(exprNode->concreteTypeInfoStorage != UINT32_MAX);
    inst->b.u32 = exprNode->concreteTypeInfoStorage;

    RegisterList result = reserveRegisterRC(context);
    inst                = emitInstruction(context, ByteCodeOp::CompareOpEqualTypeInfo, r0, exprNode->resultRegisterRC[1], COMPARE_CAST_ANY, result);

    inst            = emitInstruction(context, ByteCodeOp::IntrinsicAssert, result, r0, exprNode->resultRegisterRC[1]);
    inst->d.pointer = (uint8_t*) "invalid cast from any";
    inherhitLocation(inst, exprNode);

    freeRegisterRC(context, result);
    freeRegisterRC(context, r0);
}

void ByteCodeGenJob::emitSafetyMakeSlice(ByteCodeGenContext* context, AstArrayPointerSlicing* node)
{
    if (!mustEmitSafety(context))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    uint32_t maxBound     = UINT32_MAX;
    bool     freeMaxBound = false;

    // Check type, and safety check
    auto typeVar = node->array->typeInfo;
    if (typeVar->kind == TypeInfoKind::Array)
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array);
        maxBound       = reserveRegisterRC(context);
        auto inst      = emitInstruction(context, ByteCodeOp::SetImmediate32, maxBound);
        inst->b.u32    = typeArray->count;
        freeMaxBound   = true;
    }
    else if (typeVar->isNative(NativeTypeKind::String))
    {
        maxBound = node->array->resultRegisterRC[1];
    }
    else if (typeVar->kind == TypeInfoKind::Slice)
    {
        maxBound = node->array->resultRegisterRC[1];
    }

    // Lower bound <= upper bound
    {
        auto re = reserveRegisterRC(context);
        context->pushLocation(&node->lowerBound->token.startLocation);
        emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, node->lowerBound->resultRegisterRC, node->upperBound->resultRegisterRC, re);
        emitInstruction(context, ByteCodeOp::NegBool, re);
        emitInstruction(context, ByteCodeOp::IntrinsicAssert, re)->d.pointer = (uint8_t*) "bad slicing, lower bound is greater than upper bound";
        context->popLocation();
        freeRegisterRC(context, re);
    }

    // Upper bound < maxBound
    if (maxBound != UINT32_MAX)
    {
        auto re = reserveRegisterRC(context);
        context->pushLocation(&node->upperBound->token.startLocation);
        emitInstruction(context, ByteCodeOp::CompareOpLowerU32, node->upperBound->resultRegisterRC, maxBound, re);
        emitInstruction(context, ByteCodeOp::IntrinsicAssert, re)->d.pointer = (uint8_t*) "bad slicing, upper bound is out of range";
        context->popLocation();
        freeRegisterRC(context, re);
    }

    if (maxBound != UINT32_MAX && freeMaxBound)
        freeRegisterRC(context, maxBound);
}