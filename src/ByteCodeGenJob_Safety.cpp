#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "Module.h"
#include "TypeManager.h"
#include "Ast.h"
#include "ErrorIds.h"
#include "SemanticJob.h"

thread_local Utf8 typedMsg[(int) SafetyMsg::Count][(int) NativeTypeKind::Count][(int) NativeTypeKind::Count];

const char* ByteCodeGenJob::safetyMsg(SafetyMsg msg, TypeInfo* toType, TypeInfo* fromType)
{

    int m = (int) msg;
    int i = toType ? (int) toType->nativeType : 0;
    int j = fromType ? (int) fromType->nativeType : 0;

    if (typedMsg[m][i][j].empty())
    {
        switch (msg)
        {
        case SafetyMsg::CastTruncated:
            SWAG_ASSERT(toType && fromType);
            typedMsg[m][i][j] = Fmt(Err(Err0207), fromType->name.c_str(), toType->name.c_str());
            break;
        case SafetyMsg::CastNeg:
            SWAG_ASSERT(toType && fromType);
            typedMsg[m][i][j] = Fmt(Err(Err0208), fromType->name.c_str(), toType->name.c_str());
            break;
        case SafetyMsg::IFPlus:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Err0209), toType->name.c_str());
            break;
        case SafetyMsg::IFMinus:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Err0210), toType->name.c_str());
            break;
        case SafetyMsg::IFMul:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Err0211), toType->name.c_str());
            break;
        case SafetyMsg::IFPlusEq:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Err0212), toType->name.c_str());
            break;
        case SafetyMsg::IFMinusEq:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Err0213), toType->name.c_str());
            break;
        case SafetyMsg::IFMulEq:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Err0214), toType->name.c_str());
            break;
        case SafetyMsg::ShiftLeftOp:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Err0215), toType->name.c_str(), (toType->sizeOf * 8) - 1);
            break;
        case SafetyMsg::ShiftRightOp:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Err0216), toType->name.c_str(), (toType->sizeOf * 8) - 1);
            break;
        case SafetyMsg::ShiftLeftOf:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Err0217), toType->name.c_str());
            break;
        case SafetyMsg::ShiftRightOf:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Err0218), toType->name.c_str());
            break;
        case SafetyMsg::NegAbs:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Err0219), toType->name.c_str());
            break;
        case SafetyMsg::Neg:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Err0220), toType->name.c_str());
            break;
        }
    }

    return typedMsg[m][i][j].c_str();
}

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

bool ByteCodeGenJob::emitSafetySwitchDefault(ByteCodeGenContext* context)
{
    if (context->contextFlags & BCC_FLAG_NOSAFETY)
        return true;
    if (!context->sourceFile->module->mustEmitSafety(context->node->parent, ATTRIBUTE_SAFETY_SWITCH_ON, ATTRIBUTE_SAFETY_SWITCH_OFF))
        return true;
    emitInstruction(context, ByteCodeOp::InternalPanic)->d.pointer = (uint8_t*) Err(Err0246);
    return true;
}

void ByteCodeGenJob::emitSafetyLeftShift(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, TypeInfo* typeInfo)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_OVERFLOW_ON, ATTRIBUTE_SAFETY_OVERFLOW_OFF))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    auto re  = reserveRegisterRC(context);
    auto re1 = reserveRegisterRC(context);

    auto opNode = CastAst<AstOp>(context->node, AstNodeKind::AffectOp, AstNodeKind::FactorOp);

    bool     isSmall    = opNode->specFlags & AST_SPEC_OP_SMALL;
    uint16_t shiftFlags = 0;
    if (isSmall)
        shiftFlags |= BCI_SHIFT_SMALL;

    auto msg  = ByteCodeGenJob::safetyMsg(SafetyMsg::ShiftLeftOp, typeInfo);
    auto msg1 = ByteCodeGenJob::safetyMsg(SafetyMsg::ShiftLeftOf, typeInfo);

    // Check operand size
    if (!isSmall)
    {
        PushLocation lc(context, &context->node->childs[1]->token.startLocation);
        switch (typeInfo->sizeOf)
        {
        case 1:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 8, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, msg);
            break;
        case 2:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 16, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, msg);
            break;
        case 4:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 32, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, msg);
            break;
        case 8:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 64, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, msg);
            break;
        }
    }

    // Check if we lose bits
    if (typeInfo->isNativeIntegerUnsignedOrRune())
    {
        switch (typeInfo->sizeOf)
        {
        case 1:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU8, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU8, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual8, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        case 2:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU16, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU16, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual16, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        case 4:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU32, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU32, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual32, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        case 8:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU64, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU64, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual64, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        }
    }
    else
    {
        // Same, except right shift is arithmetic (signed)
        switch (typeInfo->sizeOf)
        {
        case 1:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU8, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS8, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual8, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        case 2:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU16, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS16, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual16, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        case 4:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU32, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS32, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual32, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        case 8:
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU64, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS64, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual64, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        }
    }

    freeRegisterRC(context, re);
    freeRegisterRC(context, re1);
}

void ByteCodeGenJob::emitSafetyRightShift(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, TypeInfo* typeInfo)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_OVERFLOW_ON, ATTRIBUTE_SAFETY_OVERFLOW_OFF))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    auto re  = reserveRegisterRC(context);
    auto re1 = reserveRegisterRC(context);

    auto opNode = CastAst<AstOp>(context->node, AstNodeKind::AffectOp, AstNodeKind::FactorOp);

    bool     isSmall    = opNode->specFlags & AST_SPEC_OP_SMALL;
    uint16_t shiftFlags = 0;
    if (isSmall)
        shiftFlags |= BCI_SHIFT_SMALL;

    auto msg  = ByteCodeGenJob::safetyMsg(SafetyMsg::ShiftRightOp, typeInfo);
    auto msg1 = ByteCodeGenJob::safetyMsg(SafetyMsg::ShiftRightOf, typeInfo);

    // Check operand size
    if (!isSmall)
    {
        PushLocation lc(context, &context->node->childs[1]->token.startLocation);
        switch (typeInfo->sizeOf)
        {
        case 1:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 8, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, msg);
            break;
        case 2:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 16, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, msg);
            break;
        case 4:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 32, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, msg);
            break;
        case 8:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r1, 64, re)->flags |= BCI_IMM_B;
            emitAssert(context, re, msg);
            break;
        }
    }

    if (typeInfo->isNativeIntegerUnsignedOrRune())
    {
        switch (typeInfo->sizeOf)
        {
        case 1:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU8, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU8, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual8, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        case 2:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU16, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU16, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual16, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        case 4:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU32, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU32, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual32, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        case 8:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightU64, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU64, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual64, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        }
    }
    else
    {
        // Same, except right shift is arithmetic (signed)
        switch (typeInfo->sizeOf)
        {
        case 1:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS8, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU8, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual8, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        case 2:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS16, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU16, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual16, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        case 4:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS32, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU32, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual32, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        case 8:
            emitInstruction(context, ByteCodeOp::BinOpShiftRightS64, r0, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::BinOpShiftLeftU64, re, r1, re)->flags |= shiftFlags;
            emitInstruction(context, ByteCodeOp::CompareOpEqual64, re, r0, re1);
            emitAssert(context, re1, msg1);
            break;
        }
    }

    freeRegisterRC(context, re);
    freeRegisterRC(context, re1);
}

void ByteCodeGenJob::emitSafetyLeftShiftEq(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, TypeInfo* typeInfo)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_OVERFLOW_ON, ATTRIBUTE_SAFETY_OVERFLOW_OFF))
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
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_OVERFLOW_ON, ATTRIBUTE_SAFETY_OVERFLOW_OFF))
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
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_MATH_ON, ATTRIBUTE_SAFETY_MATH_OFF))
        return;

    emitSafetyNotZero(context, r, bits, Err(Err0221));
}

void ByteCodeGenJob::emitSafetyBoundCheckLowerU32(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);
    emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r0, r1, re);
    emitAssert(context, re, Err(Err0222));
    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyBoundCheckLowerU64(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);
    emitInstruction(context, ByteCodeOp::CompareOpLowerU64, r0, r1, re);
    emitAssert(context, re, Err(Err0222));
    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyNeg(ByteCodeGenContext* context, uint32_t r0, TypeInfo* typeInfo, bool forAbs)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_OVERFLOW_ON, ATTRIBUTE_SAFETY_OVERFLOW_OFF))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);

    auto msg  = safetyMsg(SafetyMsg::NegAbs, typeInfo);
    auto msg1 = safetyMsg(SafetyMsg::Neg, typeInfo);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    {
        auto inst   = emitInstruction(context, ByteCodeOp::CompareOpNotEqual8, r0, 0, re);
        inst->b.s64 = INT8_MIN;
        inst->flags |= BCI_IMM_B;
        emitAssert(context, re, forAbs ? msg : msg1);
        break;
    }
    case NativeTypeKind::S16:
    {
        auto inst   = emitInstruction(context, ByteCodeOp::CompareOpNotEqual16, r0, 0, re);
        inst->b.s64 = INT16_MIN;
        inst->flags |= BCI_IMM_B;
        emitAssert(context, re, forAbs ? msg : msg1);
        break;
    }
    case NativeTypeKind::S32:
    {
        auto inst   = emitInstruction(context, ByteCodeOp::CompareOpNotEqual32, r0, 0, re);
        inst->b.s64 = INT32_MIN;
        inst->flags |= BCI_IMM_B;
        emitAssert(context, re, forAbs ? msg : msg1);
        break;
    }
    case NativeTypeKind::S64:
    {
        auto inst   = emitInstruction(context, ByteCodeOp::CompareOpNotEqual64, r0, 0, re);
        inst->b.s64 = INT64_MIN;
        inst->flags |= BCI_IMM_B;
        emitAssert(context, re, forAbs ? msg : msg1);
        break;
    }
    }

    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyBoundCheckLowerEqU64(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);
    emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, r0, r1, re);
    emitAssert(context, re, Err(Err0222));
    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyBoundCheckString(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_BOUNDCHECK_ON, ATTRIBUTE_SAFETY_BOUNDCHECK_OFF))
        return;

    emitSafetyBoundCheckLowerEqU64(context, r0, r1);
}

void ByteCodeGenJob::emitSafetyBoundCheckSlice(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_BOUNDCHECK_ON, ATTRIBUTE_SAFETY_BOUNDCHECK_OFF))
        return;

    emitSafetyBoundCheckLowerU64(context, r0, r1);
}

void ByteCodeGenJob::emitSafetyBoundCheckArray(ByteCodeGenContext* context, uint32_t r0, TypeInfoArray* typeInfoArray)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_BOUNDCHECK_ON, ATTRIBUTE_SAFETY_BOUNDCHECK_OFF))
        return;

    PushICFlags ic(context, BCI_SAFETY);
    auto        r1   = reserveRegisterRC(context);
    auto        inst = emitInstruction(context, ByteCodeOp::SetImmediate64, r1);
    inst->b.u64      = typeInfoArray->count;
    emitSafetyBoundCheckLowerU64(context, r0, r1);
    freeRegisterRC(context, r1);
}

void ByteCodeGenJob::emitSafetyCastAny(ByteCodeGenContext* context, AstNode* exprNode, bool isExplicit)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_CAST_ON, ATTRIBUTE_SAFETY_CAST_OFF))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    auto r0 = reserveRegisterRC(context);
    auto r1 = reserveRegisterRC(context);

    // :AnyTypeSegment
    SWAG_ASSERT(exprNode->extension);
    SWAG_ASSERT(exprNode->extension->misc->anyTypeSegment);
    emitMakeSegPointer(context, exprNode->extension->misc->anyTypeSegment, exprNode->extension->misc->anyTypeOffset, r0);

    context->node->allocateComputedValue();
    computeSourceLocation(context, context->node, &context->node->computedValue->storageOffset, &context->node->computedValue->storageSegment);
    emitMakeSegPointer(context, context->node->computedValue->storageSegment, context->node->computedValue->storageOffset, r1);

    emitInstruction(context, ByteCodeOp::InternalCheckAny, r0, exprNode->resultRegisterRC[1], r1);
    freeRegisterRC(context, r0);
    freeRegisterRC(context, r1);
}

void ByteCodeGenJob::emitSafetyArrayPointerSlicing(ByteCodeGenContext* context, AstArrayPointerSlicing* node)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_BOUNDCHECK_ON, ATTRIBUTE_SAFETY_BOUNDCHECK_OFF))
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
    else if (typeVar->isString())
    {
        maxBoundReg = node->array->resultRegisterRC[1];
    }
    else if (typeVar->isSlice())
    {
        maxBoundReg = node->array->resultRegisterRC[1];
    }

    // Lower bound <= upper bound
    {
        auto re = reserveRegisterRC(context);
        context->pushLocation(&node->lowerBound->token.startLocation);
        emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, node->lowerBound->resultRegisterRC, node->upperBound->resultRegisterRC, re);
        emitAssert(context, re, Err(Err0229));
        context->popLocation();
        freeRegisterRC(context, re);
    }

    // Upper bound < maxBound
    if (maxBoundReg != UINT32_MAX)
    {
        auto re = reserveRegisterRC(context);
        context->pushLocation(&node->upperBound->token.startLocation);
        emitInstruction(context, ByteCodeOp::CompareOpLowerU64, node->upperBound->resultRegisterRC, maxBoundReg, re);
        emitAssert(context, re, Err(Err0230));
        context->popLocation();
        freeRegisterRC(context, re);
        if (freeMaxBoundReg)
            freeRegisterRC(context, maxBoundReg);
    }
}

void ByteCodeGenJob::emitSafetyCast(ByteCodeGenContext* context, TypeInfo* typeInfo, TypeInfo* fromTypeInfo, AstNode* exprNode)
{
    if (!mustEmitSafety(context, ATTRIBUTE_SAFETY_OVERFLOW_ON, ATTRIBUTE_SAFETY_OVERFLOW_OFF))
        return;
    if (typeInfo->kind != TypeInfoKind::Native)
        return;
    if (fromTypeInfo->kind != TypeInfoKind::Native)
        return;

    PushLocation lc(context, &exprNode->token.startLocation);
    PushICFlags  ic(context, BCI_SAFETY);
    auto         re   = reserveRegisterRC(context);
    auto         r1   = reserveRegisterRC(context);
    auto         msg  = safetyMsg(SafetyMsg::CastTruncated, typeInfo, fromTypeInfo);
    auto         msg1 = safetyMsg(SafetyMsg::CastNeg, typeInfo, fromTypeInfo);

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
        case NativeTypeKind::Rune:
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
        case NativeTypeKind::Rune:
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
        case NativeTypeKind::Rune:
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
        case NativeTypeKind::Rune:
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
        case NativeTypeKind::Rune:
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
    case NativeTypeKind::Rune:
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