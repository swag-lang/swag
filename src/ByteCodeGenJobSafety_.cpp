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

bool ByteCodeGenJob::mustEmitSafety(ByteCodeGenContext* context, uint64_t whatOn, uint64_t whatOff)
{
    if (context->contextFlags & BCC_FLAG_NOSAFETY)
        return false;
    if (!context->sourceFile->module->mustEmitSafety(context->node, whatOn, whatOff))
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
    emitAssert(context, r0, message);
    freeRegisterRC(context, r0);
}

void ByteCodeGenJob::emitSafetyNullPointer(ByteCodeGenContext* context, uint32_t r, const char* message, int sizeInBits)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_NP_ON, ATTRIBUTE_SAFETY_NP_OFF))
        return;

    emitSafetyNotZero(context, r, sizeInBits, message);
}

void ByteCodeGenJob::emitSafetyNullLambda(ByteCodeGenContext* context, uint32_t r, const char* message)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_NP_ON, ATTRIBUTE_SAFETY_NP_OFF))
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
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_MT_ON, ATTRIBUTE_SAFETY_MT_OFF))
        return;

    emitSafetyNotZero(context, r, bits, "[safety] division by zero");
}

void ByteCodeGenJob::emitSafetyBoundCheckLowerU32(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);
    emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r0, r1, re);
    emitAssert(context, re, "[safety] index out of range");
    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyBoundCheckLowerU64(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);
    emitInstruction(context, ByteCodeOp::CompareOpLowerU64, r0, r1, re);
    emitAssert(context, re, "[safety] index out of range");
    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyRelativePointerS64(ByteCodeGenContext* context, uint32_t r0, int offsetSize)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_BC_ON, ATTRIBUTE_SAFETY_BC_OFF))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);

    int64_t     minValue = 0, maxValue = 0;
    const char* msg = nullptr;
    switch (offsetSize)
    {
    case 1:
        minValue = INT8_MIN;
        maxValue = INT8_MAX;
        msg      = "[safety] relative pointer out of range (8 bits)";
        break;
    case 2:
        minValue = INT16_MIN;
        maxValue = INT16_MAX;
        msg      = "[safety] relative pointer out of range (16 bits)";
        break;
    case 4:
        minValue = INT32_MIN;
        maxValue = INT32_MAX;
        msg      = "[safety] relative pointer out of range (32 bits)";
        break;
    case 8:
        minValue = INT64_MIN;
        maxValue = INT64_MAX;
        msg      = "[safety] relative pointer out of range (64 bits)";
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterS64, r0, 0, re);
    inst->flags |= BCI_IMM_B;
    inst->b.s64 = minValue;
    emitAssert(context, re, msg);

    inst = emitInstruction(context, ByteCodeOp::CompareOpLowerS64, r0, 0, re);
    inst->flags |= BCI_IMM_B;
    inst->b.s64 = maxValue;
    emitAssert(context, re, msg);

    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyBoundCheckLowerEqU64(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);
    emitInstruction(context, ByteCodeOp::CompareOpGreaterU64, r0, r1, re);
    emitInstruction(context, ByteCodeOp::NegBool, re);
    emitAssert(context, re, "index out of range");
    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyBoundCheckString(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_BC_ON, ATTRIBUTE_SAFETY_BC_OFF))
        return;

    emitSafetyBoundCheckLowerEqU64(context, r0, r1);
}

void ByteCodeGenJob::emitSafetyBoundCheckSlice(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_BC_ON, ATTRIBUTE_SAFETY_BC_OFF))
        return;

    emitSafetyBoundCheckLowerU64(context, r0, r1);
}

void ByteCodeGenJob::emitSafetyBoundCheckArray(ByteCodeGenContext* context, uint32_t r0, TypeInfoArray* typeInfoArray)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_BC_ON, ATTRIBUTE_SAFETY_BC_OFF))
        return;

    PushICFlags ic(context, BCI_SAFETY);
    auto        r1   = reserveRegisterRC(context);
    auto        inst = emitInstruction(context, ByteCodeOp::SetImmediate64, r1);
    inst->b.u64      = typeInfoArray->count;
    emitSafetyBoundCheckLowerU64(context, r0, r1);
    freeRegisterRC(context, r1);
}

void ByteCodeGenJob::emitSafetyCastAny(ByteCodeGenContext* context, AstNode* exprNode)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_AN_ON, ATTRIBUTE_SAFETY_AN_OFF))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    auto r0     = reserveRegisterRC(context);
    auto inst   = emitInstruction(context, ByteCodeOp::MakeTypeSegPointer, r0);
    inst->b.u64 = exprNode->concreteTypeInfoStorage;

    RegisterList result = reserveRegisterRC(context);
    inst                = emitInstruction(context, ByteCodeOp::SetImmediate32, result);
    inst->b.u32         = Runtime::COMPARE_CAST_ANY;
    inst                = emitInstruction(context, ByteCodeOp::IntrinsicTypeCmp, r0, exprNode->resultRegisterRC[1], result, result);
    emitAssert(context, result, "[safety] invalid dynamic cast");

    freeRegisterRC(context, result);
    freeRegisterRC(context, r0);
}

void ByteCodeGenJob::emitSafetyArrayPointerSlicing(ByteCodeGenContext* context, AstArrayPointerSlicing* node)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_BC_ON, ATTRIBUTE_SAFETY_BC_OFF))
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
        emitAssert(context, re, "[safety] bad slicing, lower bound is greater than upper bound");
        context->popLocation();
        freeRegisterRC(context, re);
    }

    // Upper bound < maxBound
    if (maxBoundReg != UINT32_MAX)
    {
        auto re = reserveRegisterRC(context);
        context->pushLocation(&node->upperBound->token.startLocation);
        emitInstruction(context, ByteCodeOp::CompareOpLowerU64, node->upperBound->resultRegisterRC, maxBoundReg, re);
        emitAssert(context, re, "[safety] bad slicing, upper bound is out of range");
        context->popLocation();
        freeRegisterRC(context, re);
        if (freeMaxBoundReg)
            freeRegisterRC(context, maxBoundReg);
    }
}

void ByteCodeGenJob::emitSafetyCast(ByteCodeGenContext* context, TypeInfo* typeInfo, TypeInfo* fromTypeInfo, AstNode* exprNode)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_OF_ON, ATTRIBUTE_SAFETY_OF_OFF))
        return;
    if (typeInfo->kind != TypeInfoKind::Native)
        return;
    if (fromTypeInfo->kind != TypeInfoKind::Native)
        return;

    PushICFlags ic(context, BCI_SAFETY);
    auto        re  = reserveRegisterRC(context);
    const char* msg = "[safety] integer cast truncated bits";

    switch (typeInfo->nativeType)
    {
    // To U8
    ////////////////////////////////////////////////////
    case NativeTypeKind::U8:
        switch (fromTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        {
            emitInstruction(context, ByteCodeOp::CastS8S32, exprNode->resultRegisterRC);
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT8_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S16:
        {
            emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT8_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S32:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT8_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S64:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT8_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U16:
        {
            auto inst   = emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC);
            inst->b.u64 = 0xFFFF;
            inst        = emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT8_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT8_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U64:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT8_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }
        }
        break;

    // To U16
    ////////////////////////////////////////////////////
    case NativeTypeKind::U16:
        switch (fromTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        {
            emitInstruction(context, ByteCodeOp::CastS8S32, exprNode->resultRegisterRC);
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT16_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S16:
        {
            emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT16_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S32:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT16_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S64:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT16_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT16_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT16_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }
        }
        break;

    // To U32
    ////////////////////////////////////////////////////
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        switch (fromTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        {
            emitInstruction(context, ByteCodeOp::CastS8S32, exprNode->resultRegisterRC);
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT8_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S16:
        {
            emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT16_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S64:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT32_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT32_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }
        }
        break;

    // To U64
    ////////////////////////////////////////////////////
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        switch (fromTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        {
            emitInstruction(context, ByteCodeOp::CastS8S32, exprNode->resultRegisterRC);
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT8_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S16:
        {
            emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT16_MAX;
            emitInstruction(context, ByteCodeOp::NegBool, re);
            emitAssert(context, re, msg);
            break;
        }
        }
        break;
    }

    freeRegisterRC(context, re);
}