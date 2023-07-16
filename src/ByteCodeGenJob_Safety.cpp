#include "pch.h"
#include "ByteCodeGenJob.h"
#include "ByteCode.h"
#include "Module.h"
#include "TypeManager.h"
#include "Ast.h"

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
        case SafetyMsg::CastAnyNull:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0028), toType->name.c_str());
            break;
        case SafetyMsg::CastAny:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0002), toType->name.c_str());
            break;
        case SafetyMsg::NullCheck:
            typedMsg[m][0][0] = Err(Saf0001);
            break;
        case SafetyMsg::InvalidBool:
            typedMsg[m][0][0] = Err(Saf0020);
            break;
        case SafetyMsg::InvalidFloat:
            typedMsg[m][0][0] = Err(Saf0021);
            break;
        case SafetyMsg::NotZero:
            typedMsg[m][0][0] = Err(Saf0007);
            break;
        case SafetyMsg::BadSlicingUp:
            typedMsg[m][0][0] = Err(Saf0004);
            break;
        case SafetyMsg::BadSlicingDown:
            typedMsg[m][0][0] = Err(Saf0005);
            break;
        case SafetyMsg::IndexRange:
            typedMsg[m][0][0] = Err(Saf0008);
            break;
        case SafetyMsg::SwitchComplete:
            typedMsg[m][0][0] = Err(Saf0019);
            break;
        case SafetyMsg::CastTruncated:
            SWAG_ASSERT(toType && fromType);
            typedMsg[m][i][j] = Fmt(Err(Saf0018), fromType->name.c_str(), toType->name.c_str());
            break;
        case SafetyMsg::CastNeg:
            SWAG_ASSERT(toType && fromType);
            typedMsg[m][i][j] = Fmt(Err(Saf0006), fromType->name.c_str(), toType->name.c_str());
            break;
        case SafetyMsg::Plus:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0012), toType->name.c_str());
            break;
        case SafetyMsg::Minus:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0014), toType->name.c_str());
            break;
        case SafetyMsg::Mul:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0010), toType->name.c_str());
            break;
        case SafetyMsg::PlusEq:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0011), toType->name.c_str());
            break;
        case SafetyMsg::MinusEq:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0013), toType->name.c_str());
            break;
        case SafetyMsg::MulEq:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0009), toType->name.c_str());
            break;
        case SafetyMsg::Neg:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0017), toType->name.c_str());
            break;
        case SafetyMsg::IntrinsicAbs:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0003), toType->name.c_str());
            break;
        case SafetyMsg::IntrinsicSqrt:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0022), toType->name.c_str());
            break;
        case SafetyMsg::IntrinsicLog:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0023), toType->name.c_str());
            break;
        case SafetyMsg::IntrinsicLog2:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0024), toType->name.c_str());
            break;
        case SafetyMsg::IntrinsicLog10:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0025), toType->name.c_str());
            break;
        case SafetyMsg::IntrinsicASin:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0026), toType->name.c_str());
            break;
        case SafetyMsg::IntrinsicACos:
            SWAG_ASSERT(toType);
            typedMsg[m][i][j] = Fmt(Err(Saf0027), toType->name.c_str());
            break;
        default:
            break;
        }
    }

    return typedMsg[m][i][j].c_str();
}

void ByteCodeGenJob::emitAssert(ByteCodeGenContext* context, uint32_t reg, const char* msg)
{
    PushICFlags ic(context, BCI_SAFETY);
    EMIT_INST1(context, ByteCodeOp::JumpIfTrue, reg)->b.s32   = 1;
    EMIT_INST0(context, ByteCodeOp::InternalPanic)->d.pointer = (uint8_t*) msg;
}

bool ByteCodeGenJob::mustEmitSafety(ByteCodeGenContext* context, uint16_t what)
{
    if (context->contextFlags & BCC_FLAG_NOSAFETY)
        return false;
    if (!context->sourceFile->module->mustEmitSafety(context->node, what))
        return false;
    return true;
}

void ByteCodeGenJob::emitSafetyNotZero(ByteCodeGenContext* context, uint32_t r, uint32_t bits, const char* message)
{
    PushICFlags ic(context, BCI_SAFETY);
    if (bits == 8)
        EMIT_INST1(context, ByteCodeOp::JumpIfNotZero8, r)->b.s32 = 1;
    else if (bits == 16)
        EMIT_INST1(context, ByteCodeOp::JumpIfNotZero16, r)->b.s32 = 1;
    else if (bits == 32)
        EMIT_INST1(context, ByteCodeOp::JumpIfNotZero32, r)->b.s32 = 1;
    else if (bits == 64)
        EMIT_INST1(context, ByteCodeOp::JumpIfNotZero64, r)->b.s32 = 1;
    else
        SWAG_ASSERT(false);
    EMIT_INST0(context, ByteCodeOp::InternalPanic)->d.pointer = (uint8_t*) message;
}

void ByteCodeGenJob::emitSafetyNullCheck(ByteCodeGenContext* context, uint32_t r)
{
    if (!mustEmitSafety(context, SAFETY_NULL))
        return;
    emitSafetyNotZero(context, r, 64, safetyMsg(SafetyMsg::NullCheck));
}

bool ByteCodeGenJob::emitSafetyUnreachable(ByteCodeGenContext* context)
{
    if ((context->contextFlags & BCC_FLAG_NOSAFETY) ||
        !context->sourceFile->module->mustEmitSafety(context->node->parent, SAFETY_UNREACHABLE))
    {
        EMIT_INST0(context, ByteCodeOp::InternalUnreachable);
    }
    else
    {
        EMIT_INST0(context, ByteCodeOp::Unreachable);
    }

    return true;
}

bool ByteCodeGenJob::emitSafetySwitchDefault(ByteCodeGenContext* context)
{
    if ((context->contextFlags & BCC_FLAG_NOSAFETY) ||
        !context->sourceFile->module->mustEmitSafety(context->node->parent, SAFETY_SWITCH))
    {
        EMIT_INST0(context, ByteCodeOp::InternalUnreachable);
        return true;
    }

    EMIT_INST0(context, ByteCodeOp::InternalPanic)->d.pointer = (uint8_t*) ByteCodeGenJob::safetyMsg(SafetyMsg::SwitchComplete);
    return true;
}

bool ByteCodeGenJob::emitSafetyValue(ByteCodeGenContext* context, int r, TypeInfo* typeInfo)
{
    if (typeInfo->isNative(NativeTypeKind::Bool))
    {
        if (!mustEmitSafety(context, SAFETY_BOOL))
            return true;

        PushICFlags ic(context, BCI_SAFETY);
        typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_FORCEALIAS);

        auto r0     = reserveRegisterRC(context);
        auto inst   = EMIT_INST3(context, ByteCodeOp::BinOpBitmaskAnd8, r, 0, r0);
        inst->b.u32 = 0xFE;
        inst->flags |= BCI_IMM_B;
        EMIT_INST1(context, ByteCodeOp::JumpIfZero8, r0)->b.s32   = 1;
        EMIT_INST0(context, ByteCodeOp::InternalPanic)->d.pointer = (uint8_t*) ByteCodeGenJob::safetyMsg(SafetyMsg::InvalidBool);
        freeRegisterRC(context, r0);
        return true;
    }

    if (typeInfo->isNative(NativeTypeKind::F32))
    {
        if (!mustEmitSafety(context, SAFETY_NAN))
            return true;

        PushICFlags ic(context, BCI_SAFETY);
        typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_FORCEALIAS);

        auto r0 = reserveRegisterRC(context);
        EMIT_INST3(context, ByteCodeOp::CompareOpEqualF32, r, r, r0);
        EMIT_INST1(context, ByteCodeOp::JumpIfTrue, r0)->b.s32    = 1;
        EMIT_INST0(context, ByteCodeOp::InternalPanic)->d.pointer = (uint8_t*) ByteCodeGenJob::safetyMsg(SafetyMsg::InvalidFloat);
        freeRegisterRC(context, r0);
        return true;
    }

    if (typeInfo->isNative(NativeTypeKind::F64))
    {
        if (!mustEmitSafety(context, SAFETY_NAN))
            return true;

        PushICFlags ic(context, BCI_SAFETY);
        typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_FORCEALIAS);

        auto r0 = reserveRegisterRC(context);
        EMIT_INST3(context, ByteCodeOp::CompareOpEqualF64, r, r, r0);
        EMIT_INST1(context, ByteCodeOp::JumpIfTrue, r0)->b.s32    = 1;
        EMIT_INST0(context, ByteCodeOp::InternalPanic)->d.pointer = (uint8_t*) ByteCodeGenJob::safetyMsg(SafetyMsg::InvalidFloat);
        freeRegisterRC(context, r0);
        return true;
    }

    return true;
}

void ByteCodeGenJob::emitSafetyDivZero(ByteCodeGenContext* context, uint32_t r, uint32_t bits)
{
    if (!mustEmitSafety(context, SAFETY_MATH))
        return;

    emitSafetyNotZero(context, r, bits, safetyMsg(SafetyMsg::NotZero));
}

void ByteCodeGenJob::emitSafetyBoundCheckLowerU32(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);
    EMIT_INST3(context, ByteCodeOp::CompareOpLowerU32, r0, r1, re);
    emitAssert(context, re, safetyMsg(SafetyMsg::IndexRange));
    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyNeg(ByteCodeGenContext* context, uint32_t r0, TypeInfo* typeInfo, bool forAbs)
{
    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);

    auto msg  = safetyMsg(SafetyMsg::IntrinsicAbs, typeInfo);
    auto msg1 = safetyMsg(SafetyMsg::Neg, typeInfo);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    {
        auto inst   = EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual8, r0, 0, re);
        inst->b.s64 = INT8_MIN;
        inst->flags |= BCI_IMM_B;
        emitAssert(context, re, forAbs ? msg : msg1);
        break;
    }
    case NativeTypeKind::S16:
    {
        auto inst   = EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual16, r0, 0, re);
        inst->b.s64 = INT16_MIN;
        inst->flags |= BCI_IMM_B;
        emitAssert(context, re, forAbs ? msg : msg1);
        break;
    }
    case NativeTypeKind::S32:
    {
        auto inst   = EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual32, r0, 0, re);
        inst->b.s64 = INT32_MIN;
        inst->flags |= BCI_IMM_B;
        emitAssert(context, re, forAbs ? msg : msg1);
        break;
    }
    case NativeTypeKind::S64:
    {
        auto inst   = EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual64, r0, 0, re);
        inst->b.s64 = INT64_MIN;
        inst->flags |= BCI_IMM_B;
        emitAssert(context, re, forAbs ? msg : msg1);
        break;
    }
    default:
        break;
    }

    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyBoundCheckLowerU64(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    PushICFlags ic(context, BCI_SAFETY);

    auto re = reserveRegisterRC(context);
    EMIT_INST3(context, ByteCodeOp::CompareOpLowerU64, r0, r1, re);
    emitAssert(context, re, safetyMsg(SafetyMsg::IndexRange));
    freeRegisterRC(context, re);
}

void ByteCodeGenJob::emitSafetyBoundCheckString(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    if (!mustEmitSafety(context, SAFETY_BOUNDCHECK))
        return;

    emitSafetyBoundCheckLowerU64(context, r0, r1);
}

void ByteCodeGenJob::emitSafetyBoundCheckSlice(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    if (!mustEmitSafety(context, SAFETY_BOUNDCHECK))
        return;

    emitSafetyBoundCheckLowerU64(context, r0, r1);
}

void ByteCodeGenJob::emitSafetyBoundCheckArray(ByteCodeGenContext* context, uint32_t r0, TypeInfoArray* typeInfoArray)
{
    if (!mustEmitSafety(context, SAFETY_BOUNDCHECK))
        return;

    PushICFlags ic(context, BCI_SAFETY);
    auto        r1   = reserveRegisterRC(context);
    auto        inst = EMIT_INST1(context, ByteCodeOp::SetImmediate64, r1);
    inst->b.u64      = typeInfoArray->count;
    emitSafetyBoundCheckLowerU64(context, r0, r1);
    freeRegisterRC(context, r1);
}

void ByteCodeGenJob::emitSafetyCastAny(ByteCodeGenContext* context, AstNode* exprNode, bool isExplicit)
{
    if (!mustEmitSafety(context, SAFETY_ANY))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    auto r0 = reserveRegisterRC(context);
    auto r1 = reserveRegisterRC(context);

    emitSafetyNotZero(context, exprNode->resultRegisterRC[1], 64, safetyMsg(SafetyMsg::CastAnyNull, exprNode->typeInfo));

    // :AnyTypeSegment
    SWAG_ASSERT(exprNode->hasExtMisc());
    SWAG_ASSERT(exprNode->extMisc()->anyTypeSegment);
    emitMakeSegPointer(context, exprNode->extMisc()->anyTypeSegment, exprNode->extMisc()->anyTypeOffset, r0);

    auto rflags = reserveRegisterRC(context);
    auto inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate32, rflags);
    inst->b.u64 = SWAG_COMPARE_CAST_ANY;
    inst        = EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, r0, exprNode->resultRegisterRC[1], rflags, r1);
    freeRegisterRC(context, rflags);
    emitSafetyNotZero(context, r1, 8, safetyMsg(SafetyMsg::CastAny, exprNode->typeInfo));

    freeRegisterRC(context, r0);
    freeRegisterRC(context, r1);
}

void ByteCodeGenJob::emitSafetyArrayPointerSlicing(ByteCodeGenContext* context, AstArrayPointerSlicing* node)
{
    if (!mustEmitSafety(context, SAFETY_BOUNDCHECK))
        return;

    PushICFlags ic(context, BCI_SAFETY);
    uint32_t    maxBoundReg     = UINT32_MAX;
    bool        freeMaxBoundReg = false;

    // Check type, and safety check
    auto typeVar = TypeManager::concreteType(node->array->typeInfo);
    if (typeVar->isArray())
    {
        auto typeArray  = CastTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array);
        maxBoundReg     = reserveRegisterRC(context);
        auto inst       = EMIT_INST1(context, ByteCodeOp::SetImmediate64, maxBoundReg);
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
        EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, node->lowerBound->resultRegisterRC, node->upperBound->resultRegisterRC, re);
        emitAssert(context, re, safetyMsg(SafetyMsg::BadSlicingUp));
        context->popLocation();
        freeRegisterRC(context, re);
    }

    // Upper bound < maxBound
    if (maxBoundReg != UINT32_MAX)
    {
        auto re = reserveRegisterRC(context);
        context->pushLocation(&node->upperBound->token.startLocation);
        EMIT_INST3(context, ByteCodeOp::CompareOpLowerU64, node->upperBound->resultRegisterRC, maxBoundReg, re);
        emitAssert(context, re, safetyMsg(SafetyMsg::BadSlicingDown));
        context->popLocation();
        freeRegisterRC(context, re);
        if (freeMaxBoundReg)
            freeRegisterRC(context, maxBoundReg);
    }
}

void ByteCodeGenJob::emitSafetyCastOverflow(ByteCodeGenContext* context, TypeInfo* typeInfo, TypeInfo* fromTypeInfo, AstNode* exprNode)
{
    if (!mustEmitSafety(context, SAFETY_OVERFLOW))
        return;
    if (!typeInfo->isNative())
        return;
    if (!fromTypeInfo->isNative())
        return;

    PushLocation lc(context, &exprNode->token.startLocation);
    PushICFlags  ic(context, BCI_SAFETY);
    auto         re   = reserveRegisterRC(context);
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
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU8, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U16:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU16, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S16:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS16, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT8_MIN;
            emitAssert(context, re, msg);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS16, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S32:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT8_MIN;
            emitAssert(context, re, msg);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT8_MIN;
            emitAssert(context, re, msg);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F32:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = (float) INT8_MIN - 0.5f;
            emitAssert(context, re, msg);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = (float) INT8_MAX + 0.5f;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) INT8_MIN - 0.5;
            emitAssert(context, re, msg);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) INT8_MAX + 0.5;
            emitAssert(context, re, msg);
            break;
        }
        default:
            break;
        }
        break;

    // To S16
    ////////////////////////////////////////////////////
    case NativeTypeKind::S16:
        switch (fromTypeInfo->nativeType)
        {
        case NativeTypeKind::S32:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT16_MIN;
            emitAssert(context, re, msg);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT16_MIN;
            emitAssert(context, re, msg);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U16:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU16, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F32:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = (float) INT16_MIN - 0.5f;
            emitAssert(context, re, msg);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = (float) INT16_MAX + 0.5f;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) INT16_MIN - 0.5;
            emitAssert(context, re, msg);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) INT16_MAX + 0.5;
            emitAssert(context, re, msg);
            break;
        }
        default:
            break;
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
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT32_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT32_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT32_MIN;
            emitAssert(context, re, msg);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.s64 = INT32_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F32:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = (float) INT32_MIN - 0.5f;
            emitAssert(context, re, msg);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = (float) INT32_MAX + 0.5f;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) INT32_MIN - 0.5;
            emitAssert(context, re, msg);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) INT32_MAX + 0.5;
            emitAssert(context, re, msg);
            break;
        }
        default:
            break;
        }
        break;

    // To S64
    ////////////////////////////////////////////////////
    case NativeTypeKind::S64:
        switch (fromTypeInfo->nativeType)
        {
        case NativeTypeKind::U64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = INT64_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) INT64_MAX + 0.5;
            emitAssert(context, re, msg);
            break;
        }
        default:
            break;
        }
        break;

    // To U8
    ////////////////////////////////////////////////////
    case NativeTypeKind::U8:
        switch (fromTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS8, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS8, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S16:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS16, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU16, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S32:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U16:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU16, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT8_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F32:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = -SAFETY_ZERO_EPSILON;
            emitAssert(context, re, msg1);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = (float) UINT8_MAX + 0.5;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = -SAFETY_ZERO_EPSILON;
            emitAssert(context, re, msg1);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) UINT8_MAX + 0.5;
            emitAssert(context, re, msg);
            break;
        }
        default:
            break;
        }
        break;

    // To U16
    ////////////////////////////////////////////////////
    case NativeTypeKind::U16:
        switch (fromTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS8, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S16:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S32:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT16_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F32:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = -SAFETY_ZERO_EPSILON;
            emitAssert(context, re, msg1);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = UINT16_MAX + 0.5;
            ;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = -SAFETY_ZERO_EPSILON;
            emitAssert(context, re, msg1);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) UINT16_MAX + 0.5;
            emitAssert(context, re, msg);
            break;
        }
        default:
            break;
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
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS8, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S16:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS16, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S32:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::S64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT32_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::U64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = UINT32_MAX;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F32:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = -SAFETY_ZERO_EPSILON;
            emitAssert(context, re, msg1);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = (float) UINT32_MAX + 0.5f;
            emitAssert(context, re, msg);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = -SAFETY_ZERO_EPSILON;
            emitAssert(context, re, msg1);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) UINT32_MAX + 0.5;
            emitAssert(context, re, msg);
            break;
        }
        default:
            break;
        }
        break;

    // To U64
    ////////////////////////////////////////////////////
    case NativeTypeKind::U64:
        switch (fromTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS8, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S16:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS16, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S32:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::S64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.u64 = 0;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::F32:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f32 = -SAFETY_ZERO_EPSILON;
            emitAssert(context, re, msg1);
            break;
        }

        case NativeTypeKind::F64:
        {
            auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = -SAFETY_ZERO_EPSILON;
            emitAssert(context, re, msg1);

            inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRC, 0, re);
            inst->flags |= BCI_IMM_B;
            inst->b.f64 = (double) UINT64_MAX + 0.5;
            emitAssert(context, re, msg);
            break;
        }
        default:
            break;
        }
        break;
    default:
        break;
    }

    freeRegisterRC(context, re);
}