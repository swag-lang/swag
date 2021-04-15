#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "ByteCodeOp.h"
#include "Module.h"
#include "Ast.h"
#include "Runtime.h"

void ByteCodeGenJob::emitAssert(ByteCodeGenContext* context, uint32_t reg, const char* msg)
{
    emitInstruction(context, ByteCodeOp::JumpIfTrue, reg)->b.s32   = 1;
    emitInstruction(context, ByteCodeOp::InternalPanic)->d.pointer = (uint8_t*) msg;
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
    if (bits == 8)
        emitInstruction(context, ByteCodeOp::JumpIfNotZero8, r)->b.s32 = 1;
    else if (bits == 16)
        emitInstruction(context, ByteCodeOp::JumpIfNotZero16, r)->b.s32 = 1;
    else if (bits == 32)
        emitInstruction(context, ByteCodeOp::JumpIfNotZero32, r)->b.s32 = 1;
    else if (bits == 64)
        emitInstruction(context, ByteCodeOp::JumpIfNotZero64, r)->b.s32 = 1;
    else
        SWAG_ASSERT(false);
    emitInstruction(context, ByteCodeOp::InternalPanic)->d.pointer = (uint8_t*) message;
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
    emitInstruction(context, ByteCodeOp::CopyRBtoRA64, re, r);
    auto inst = emitInstruction(context, ByteCodeOp::BinOpBitmaskAnd64, re, 0, re);
    inst->flags |= BCI_IMM_B;
    inst->b.u64 = ~SWAG_LAMBDA_MARKER_MASK;

    emitSafetyNotZero(context, re, 64, message);

    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyLeftShift(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, TypeInfo* typeInfo)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_OF_ON, ATTRIBUTE_SAFETY_OF_OFF))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    auto re  = reserveRegisterRC(context);
    auto re1 = reserveRegisterRC(context);

    auto opNode = CastAst<AstOp>(context->node, AstNodeKind::AffectOp, AstNodeKind::FactorOp);

    // Check operand size
    if (!(opNode->opFlags & OPFLAG_SMALL))
    {
        PushLocation lc(context, &context->node->childs[1]->token.startLocation);
        switch (typeInfo->sizeOf)
        {
        case 1:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 8, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, "[safety] (8 bits) '<<' shift operand is greater than '7'");
            break;
        case 2:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 16, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, "[safety] (16 bits) '<<' shift operand is greater than '15'");
            break;
        case 4:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 32, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, "[safety] (32 bits) '<<' operand is greater than '31'");
            break;
        case 8:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 64, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, "[safety] (64 bits) '<<' shift operand is greater than '63'");
            break;
        }
    }

    // Check if we lose bits
    if (typeInfo->isNativeUnsignedOrChar())
    {
        switch (typeInfo->sizeOf)
        {
        case 1:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU8, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU8, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual8, re, r0, re1);
            emitAssert(context, re1, "[safety] (8 bits) '<<' shift overflow");
            break;
        case 2:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU16, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU16, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual16, re, r0, re1);
            emitAssert(context, re1, "[safety] (16 bits) '<<' shift overflow");
            break;
        case 4:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU32, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU32, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual32, re, r0, re1);
            emitAssert(context, re1, "[safety] (32 bits) '<<' shift overflow");
            break;
        case 8:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU64, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU64, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual64, re, r0, re1);
            emitAssert(context, re1, "[safety] (64 bits) '<<' shift overflow");
            break;
        }
    }
    else
    {
        // Same, except right shift is arithmetic (signed)
        switch (typeInfo->sizeOf)
        {
        case 1:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU8, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS8, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual8, re, r0, re1);
            emitAssert(context, re1, "[safety] (8 bits) '<<' shift overflow");
            break;
        case 2:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU16, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS16, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual16, re, r0, re1);
            emitAssert(context, re1, "[safety] (16 bits) '<<' shift overflow");
            break;
        case 4:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU32, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS32, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual32, re, r0, re1);
            emitAssert(context, re1, "[safety] (32 bits) '<<' shift overflow");
            break;
        case 8:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU64, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS64, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual64, re, r0, re1);
            emitAssert(context, re1, "[safety] (64 bits) '<<' shift overflow");
            break;
        }
    }

    freeRegisterRC(context, re);
    freeRegisterRC(context, re1);
}

void ByteCodeGenJob::emitSafetyRightShift(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, TypeInfo* typeInfo)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_OF_ON, ATTRIBUTE_SAFETY_OF_OFF))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    auto re  = reserveRegisterRC(context);
    auto re1 = reserveRegisterRC(context);

    auto opNode = CastAst<AstOp>(context->node, AstNodeKind::AffectOp, AstNodeKind::FactorOp);

    // Check operand size
    if (!(opNode->opFlags & OPFLAG_SMALL))
    {
        PushLocation lc(context, &context->node->childs[1]->token.startLocation);
        switch (typeInfo->sizeOf)
        {
        case 1:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 8, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, "[safety] (8 bits) '>>' shift operand is greater than '7'");
            break;
        case 2:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 16, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, "[safety] (16 bits) '>>' shift operand is greater than '15'");
            break;
        case 4:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 32, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, "[safety] (32 bits) '>>' shift operand is greater than '31'");
            break;
        case 8:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 64, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, "[safety] (64 bits) '>>' shift operand is greater than '63'");
            break;
        }
    }

    if (typeInfo->isNativeUnsignedOrChar())
    {
        switch (typeInfo->sizeOf)
        {
        case 1:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU8, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU8, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual8, re, r0, re1);
            emitAssert(context, re1, "[safety] (8 bits) '>>' shift overflow");
            break;
        case 2:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU16, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU16, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual16, re, r0, re1);
            emitAssert(context, re1, "[safety] (16 bits) '>>' shift overflow");
            break;
        case 4:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU32, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU32, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual32, re, r0, re1);
            emitAssert(context, re1, "[safety] (32 bits) '>>' shift overflow");
            break;
        case 8:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU64, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU64, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual64, re, r0, re1);
            emitAssert(context, re1, "[safety] (64 bits) '>>' shift overflow");
            break;
        }
    }
    else
    {
        // Same, except right shift is arithmetic (signed)
        switch (typeInfo->sizeOf)
        {
        case 1:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS8, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU8, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual8, re, r0, re1);
            emitAssert(context, re1, "[safety] (8 bits) '>>' shift overflow");
            break;
        case 2:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS16, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU16, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual16, re, r0, re1);
            emitAssert(context, re1, "[safety] (16 bits) '>>' shift overflow");
            break;
        case 4:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS32, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU32, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual32, re, r0, re1);
            emitAssert(context, re1, "[safety] (32 bits) '>>' shift overflow");
            break;
        case 8:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS64, r0, r1, re);
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU64, re, r1, re);
            emitInstruction(context, ByteCodeOp::CompareOpEqual64, re, r0, re1);
            emitAssert(context, re1, "[safety] (64 bits) '>>' shift overflow");
            break;
        }
    }

    freeRegisterRC(context, re);
    freeRegisterRC(context, re1);
}

void ByteCodeGenJob::emitSafetyLeftShiftEq(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, TypeInfo* typeInfo)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_OF_ON, ATTRIBUTE_SAFETY_OF_OFF))
        return;

    PushICFlags ic(context, BCI_SAFETY);
    auto        re = reserveRegisterRC(context);
    switch (typeInfo->sizeOf)
    {
    case 1:
        emitInstruction(context, ByteCodeOp::DeRef8, re, r0);
        break;
    case 2:
        emitInstruction(context, ByteCodeOp::DeRef16, re, r0);
        break;
    case 4:
        emitInstruction(context, ByteCodeOp::DeRef32, re, r0);
        break;
    case 8:
        emitInstruction(context, ByteCodeOp::DeRef64, re, r0);
        break;
    }

    emitSafetyLeftShift(context, re, r1, typeInfo);
    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyRightShiftEq(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, TypeInfo* typeInfo)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_OF_ON, ATTRIBUTE_SAFETY_OF_OFF))
        return;

    PushICFlags ic(context, BCI_SAFETY);
    auto        re = reserveRegisterRC(context);
    switch (typeInfo->sizeOf)
    {
    case 1:
        emitInstruction(context, ByteCodeOp::DeRef8, re, r0);
        break;
    case 2:
        emitInstruction(context, ByteCodeOp::DeRef16, re, r0);
        break;
    case 4:
        emitInstruction(context, ByteCodeOp::DeRef32, re, r0);
        break;
    case 8:
        emitInstruction(context, ByteCodeOp::DeRef64, re, r0);
        break;
    }

    emitSafetyRightShift(context, re, r1, typeInfo);
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

void ByteCodeGenJob::emitSafetyNeg(ByteCodeGenContext* context, uint32_t r0, TypeInfo* typeInfo)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_OF_ON, ATTRIBUTE_SAFETY_OF_OFF))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);
    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    {
        auto inst   = emitInstruction(context, ByteCodeOp::CompareOpNotEqual8, r0, 0, re);
        inst->b.s64 = INT8_MIN;
        inst->flags |= BCI_IMM_B;
        emitAssert(context, re, "[safety] (8 bits) '-' integer overflow");
        break;
    }
    case NativeTypeKind::S16:
    {
        auto inst   = emitInstruction(context, ByteCodeOp::CompareOpNotEqual16, r0, 0, re);
        inst->b.s64 = INT16_MIN;
        inst->flags |= BCI_IMM_B;
        emitAssert(context, re, "[safety] (16 bits) '-' integer overflow");
        break;
    }
    case NativeTypeKind::S32:
    {
        auto inst   = emitInstruction(context, ByteCodeOp::CompareOpNotEqual32, r0, 0, re);
        inst->b.s64 = INT32_MIN;
        inst->flags |= BCI_IMM_B;
        emitAssert(context, re, "[safety] (32 bits) '-' integer overflow");
        break;
    }
    case NativeTypeKind::S64:
    {
        auto inst   = emitInstruction(context, ByteCodeOp::CompareOpNotEqual64, r0, 0, re);
        inst->b.s64 = INT64_MIN;
        inst->flags |= BCI_IMM_B;
        emitAssert(context, re, "[safety] (64 bits) '-' integer overflow");
        break;
    }
    }

    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyRelativePointerS64(ByteCodeGenContext* context, uint32_t r0, int offsetSize)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_BC_ON, ATTRIBUTE_SAFETY_BC_OFF))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);

    int64_t minValue = 0, maxValue = 0;
    switch (offsetSize)
    {
    case 1:
        minValue = INT8_MIN;
        maxValue = INT8_MAX;
        break;
    case 2:
        minValue = INT16_MIN;
        maxValue = INT16_MAX;
        break;
    case 4:
        minValue = INT32_MIN;
        maxValue = INT32_MAX;
        break;
    case 8:
        minValue = INT64_MIN;
        maxValue = INT64_MAX;
        break;
    default:
        SWAG_ASSERT(false);
        break;
    }

    auto inst = emitInstruction(context, ByteCodeOp::TestNotZero64, re, r0);
    emitAssert(context, re, "[safety] relative pointer points to itself, this is not legit");

    inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS64, r0, 0, re);
    inst->flags |= BCI_IMM_B;
    inst->b.s64 = minValue;
    emitAssert(context, re, "[safety] relative pointer out of range");

    inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqS64, r0, 0, re);
    inst->flags |= BCI_IMM_B;
    inst->b.s64 = maxValue;
    emitAssert(context, re, "[safety] relative pointer out of range");

    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyBoundCheckLowerEqU64(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);
    emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, r0, r1, re);
    emitAssert(context, re, "[safety] index out of range");
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
    inst->b.u32         = SWAG_COMPARE_CAST_ANY;
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
        emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, node->lowerBound->resultRegisterRC, node->upperBound->resultRegisterRC, re);
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

    PushICFlags        ic(context, BCI_SAFETY);
    auto               re   = reserveRegisterRC(context);
    auto               r1   = reserveRegisterRC(context);
    static const char* msg  = "[safety] integer cast truncated bits";
    static const char* msg1 = "[safety] cast from negative value to unsigned integer";

    switch (typeInfo->nativeType)
    {
    // To S8
    ////////////////////////////////////////////////////
    case NativeTypeKind::S8:
        switch (fromTypeInfo->nativeType)
        {
        case NativeTypeKind::U8:
        {
            auto inst   = emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC);
            inst->b.u64 = 0xFF;
            inst        = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U16:
        {
            auto inst   = emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC);
            inst->b.u64 = 0xFFFF;
            inst        = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S16:
        {
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r1, exprNode->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::CastS16S32, r1);
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, r1, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT8_MIN;
            emitAssert(context, re, msg);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqS32, r1, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S32:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT8_MIN;
            emitAssert(context, re, msg);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT8_MIN;
            emitAssert(context, re, msg);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F32:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = (float) INT8_MIN - 0.5f;
            emitAssert(context, re, msg);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = (float) INT8_MAX + 0.5f;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) INT8_MIN - 0.5;
            emitAssert(context, re, msg);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) INT8_MAX + 0.5;
            emitAssert(context, re, msg);
            break;
        }
        }
        break;

    // To S16
    ////////////////////////////////////////////////////
    case NativeTypeKind::S16:
        switch (fromTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT16_MIN;
            emitAssert(context, re, msg);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT16_MIN;
            emitAssert(context, re, msg);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U16:
        {
            auto inst   = emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC);
            inst->b.u64 = 0xFFFF;
            inst        = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F32:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = (float) INT16_MIN - 0.5f;
            emitAssert(context, re, msg);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = (float) INT16_MAX + 0.5f;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) INT16_MIN - 0.5;
            emitAssert(context, re, msg);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) INT16_MAX + 0.5;
            emitAssert(context, re, msg);
            break;
        }
        }
        break;

    // To S32
    ////////////////////////////////////////////////////
    case NativeTypeKind::S32:
        switch (fromTypeInfo->nativeType)
        {
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT32_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT32_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT32_MIN;
            emitAssert(context, re, msg);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT32_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F32:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = (float) INT32_MIN - 0.5f;
            emitAssert(context, re, msg);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = (float) INT32_MAX + 0.5f;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) INT32_MIN - 0.5;
            emitAssert(context, re, msg);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) INT32_MAX + 0.5;
            ;
            emitAssert(context, re, msg);
            break;
        }
        }
        break;

    // To S64
    ////////////////////////////////////////////////////
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        switch (fromTypeInfo->nativeType)
        {
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT64_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) INT64_MAX + 0.5;
            ;
            emitAssert(context, re, msg);
            break;
        }
        }
        break;

    // To U8
    ////////////////////////////////////////////////////
    case NativeTypeKind::U8:
        switch (fromTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        {
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r1, exprNode->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::CastS8S32, r1);

            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);

            inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, r1, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S16:
        {
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r1, exprNode->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::CastS16S32, r1);

            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU32, r1, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S32:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U16:
        {
            auto inst   = emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC);
            inst->b.u64 = 0xFFFF;
            inst        = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F32:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = -0.5f;
            emitAssert(context, re, msg1);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = UINT8_MAX + 0.5;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = -0.5;
            emitAssert(context, re, msg1);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = UINT8_MAX + 0.5;
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
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r1, exprNode->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::CastS8S32, r1);
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, r1, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S16:
        {
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r1, exprNode->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::CastS16S32, r1);
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, r1, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S32:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F32:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = -0.5f;
            emitAssert(context, re, msg1);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = UINT16_MAX + 0.5;
            ;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = -0.5;
            emitAssert(context, re, msg1);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = UINT16_MAX + 0.5;
            ;
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
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r1, exprNode->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::CastS8S32, r1);
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, r1, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S16:
        {
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r1, exprNode->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::CastS16S32, r1);
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, r1, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S32:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT32_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT32_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F32:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = -0.5f;
            emitAssert(context, re, msg1);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = (float) UINT32_MAX + 0.5f;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = -0.5;
            emitAssert(context, re, msg1);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = UINT32_MAX + 0.5;
            ;
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
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r1, exprNode->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::CastS8S32, r1);
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, r1, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S16:
        {
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r1, exprNode->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::CastS16S32, r1);
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, r1, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S32:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::F32:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = -0.5f;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = emitInstruction(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = -0.5;
            emitAssert(context, re, msg1);

            inst = emitInstruction(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) UINT64_MAX + 0.5;
            emitAssert(context, re, msg);
            break;
        }
        }
        break;
    }

    freeRegisterRC(context, r1);
    freeRegisterRC(context, re);
}