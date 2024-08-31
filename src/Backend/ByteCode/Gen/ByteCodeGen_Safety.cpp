#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Report/ErrorIds.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Wmf/Module.h"

thread_local Utf8 g_TypedMsg[static_cast<int>(SafetyMsg::Count)][static_cast<int>(NativeTypeKind::Count)][static_cast<int>(NativeTypeKind::Count)];

const char* ByteCodeGen::safetyMsg(SafetyMsg msg, TypeInfo* toType, TypeInfo* fromType)
{
    const int m = static_cast<int>(msg);
    const int i = toType ? static_cast<int>(toType->nativeType) : 0;
    const int j = fromType ? static_cast<int>(fromType->nativeType) : 0;

    if (g_TypedMsg[m][i][j].empty())
    {
        switch (msg)
        {
            case SafetyMsg::CastAnyNull:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0002, toType->name.cstr());
                break;
            case SafetyMsg::CastAny:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0001, toType->name.cstr());
                break;
            case SafetyMsg::NullCheck:
                g_TypedMsg[m][0][0] = toErr(Saf0018);
                break;
            case SafetyMsg::ErrCheck:
                g_TypedMsg[m][0][0] = toErr(Saf0019);
                break;
            case SafetyMsg::InvalidBool:
                g_TypedMsg[m][0][0] = toErr(Saf0003);
                break;
            case SafetyMsg::InvalidFloat:
                g_TypedMsg[m][0][0] = toErr(Saf0017);
                break;
            case SafetyMsg::NotZero:
                g_TypedMsg[m][0][0] = toErr(Saf0016);
                break;
            case SafetyMsg::BadSlicingDown:
                g_TypedMsg[m][0][0] = toErr(Saf0006);
                break;
            case SafetyMsg::BadSlicingUp:
                g_TypedMsg[m][0][0] = toErr(Saf0007);
                break;
            case SafetyMsg::BadRangeDown:
                g_TypedMsg[m][0][0] = toErr(Saf0005);
                break;
            case SafetyMsg::IndexRange:
                g_TypedMsg[m][0][0] = toErr(Saf0004);
                break;
            case SafetyMsg::SwitchComplete:
                g_TypedMsg[m][0][0] = toErr(Saf0029);
                break;
            case SafetyMsg::CastTruncated:
                SWAG_ASSERT(toType && fromType);
                g_TypedMsg[m][i][j] = formErr(Saf0028, fromType->name.cstr(), toType->name.cstr());
                break;
            case SafetyMsg::CastNeg:
                SWAG_ASSERT(toType && fromType);
                g_TypedMsg[m][i][j] = formErr(Saf0021, fromType->name.cstr(), toType->name.cstr());
                break;
            case SafetyMsg::Plus:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0027, "+", toType->name.cstr());
                break;
            case SafetyMsg::Minus:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0027, "-", toType->name.cstr());
                break;
            case SafetyMsg::Mul:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0027, "*", toType->name.cstr());
                break;
            case SafetyMsg::Div:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0027, "/", toType->name.cstr());
                break;
            case SafetyMsg::PlusEq:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0027, "+=", toType->name.cstr());
                break;
            case SafetyMsg::MinusEq:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0027, "-=", toType->name.cstr());
                break;
            case SafetyMsg::MulEq:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0027, "*=", toType->name.cstr());
                break;
            case SafetyMsg::DivEq:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0027, "/=", toType->name.cstr());
                break;
            case SafetyMsg::Neg:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0027, "-", toType->name.cstr());
                break;
            case SafetyMsg::IntrinsicAbs:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0008, toType->name.cstr());
                break;
            case SafetyMsg::IntrinsicSqrt:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0014, toType->name.cstr());
                break;
            case SafetyMsg::IntrinsicLog:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0011, toType->name.cstr());
                break;
            case SafetyMsg::IntrinsicLog2:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0013, toType->name.cstr());
                break;
            case SafetyMsg::IntrinsicLog10:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0012, toType->name.cstr());
                break;
            case SafetyMsg::IntrinsicASin:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0010, toType->name.cstr());
                break;
            case SafetyMsg::IntrinsicACos:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0009, toType->name.cstr());
                break;
            case SafetyMsg::IntrinsicPow:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = formErr(Saf0015, toType->name.cstr());
                break;
            default:
                break;
        }
    }

    return g_TypedMsg[m][i][j];
}

void ByteCodeGen::emitAssert(ByteCodeGenContext* context, uint32_t reg, const char* message)
{
    PushICFlags ic(context, BCI_SAFETY);
    EMIT_INST1(context, ByteCodeOp::JumpIfTrue, reg)->b.s32   = 1;
    EMIT_INST0(context, ByteCodeOp::InternalPanic)->d.pointer = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(message));
}

bool ByteCodeGen::mustEmitSafety(const ByteCodeGenContext* context, SafetyFlags what)
{
    if (context->contextFlags & BCC_FLAG_NO_SAFETY)
        return false;
    if (!context->sourceFile->module->mustEmitSafety(context->node, what))
        return false;
    return true;
}

void ByteCodeGen::emitSafetyNotZero(ByteCodeGenContext* context, uint32_t r, uint32_t bits, const char* message)
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
    EMIT_INST0(context, ByteCodeOp::InternalPanic)->d.pointer = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(message));
}

void ByteCodeGen::emitSafetyNullCheck(ByteCodeGenContext* context, uint32_t r)
{
    if (!mustEmitSafety(context, SAFETY_NULL))
        return;
    emitSafetyNotZero(context, r, 64, safetyMsg(SafetyMsg::NullCheck));
}

void ByteCodeGen::emitSafetyErrCheck(ByteCodeGenContext* context, uint32_t r)
{
    if (!mustEmitSafety(context, SAFETY_NULL))
        return;
    emitSafetyNotZero(context, r, 64, safetyMsg(SafetyMsg::ErrCheck));
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitSafetyUnreachable(ByteCodeGenContext* context)
{
    if (context->contextFlags & BCC_FLAG_NO_SAFETY ||
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

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool ByteCodeGen::emitSafetySwitchDefault(ByteCodeGenContext* context)
{
    if (context->contextFlags & BCC_FLAG_NO_SAFETY ||
        !context->sourceFile->module->mustEmitSafety(context->node->parent, SAFETY_SWITCH))
    {
        EMIT_INST0(context, ByteCodeOp::InternalUnreachable);
        return true;
    }

    EMIT_INST0(context, ByteCodeOp::InternalPanic)->d.pointer = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(safetyMsg(SafetyMsg::SwitchComplete)));
    return true;
}

bool ByteCodeGen::emitSafetyValue(ByteCodeGenContext* context, int r, const TypeInfo* typeInfo)
{
    if (typeInfo->isNative(NativeTypeKind::Bool))
    {
        if (!mustEmitSafety(context, SAFETY_BOOL))
            return true;

        PushICFlags ic(context, BCI_SAFETY);

        const auto r0   = reserveRegisterRC(context);
        const auto inst = EMIT_INST3(context, ByteCodeOp::BinOpBitmaskAnd8, r, 0, r0);
        inst->b.u32     = 0xFE;
        inst->addFlag(BCI_IMM_B);
        EMIT_INST1(context, ByteCodeOp::JumpIfZero8, r0)->b.s32   = 1;
        EMIT_INST0(context, ByteCodeOp::InternalPanic)->d.pointer = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(safetyMsg(SafetyMsg::InvalidBool)));
        freeRegisterRC(context, r0);
        return true;
    }

    if (typeInfo->isNative(NativeTypeKind::F32))
    {
        if (!mustEmitSafety(context, SAFETY_NAN))
            return true;

        PushICFlags ic(context, BCI_SAFETY);

        const auto r0 = reserveRegisterRC(context);
        EMIT_INST3(context, ByteCodeOp::CompareOpEqualF32, r, r, r0);
        EMIT_INST1(context, ByteCodeOp::JumpIfTrue, r0)->b.s32    = 1;
        EMIT_INST0(context, ByteCodeOp::InternalPanic)->d.pointer = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(safetyMsg(SafetyMsg::InvalidFloat)));
        freeRegisterRC(context, r0);
        return true;
    }

    if (typeInfo->isNative(NativeTypeKind::F64))
    {
        if (!mustEmitSafety(context, SAFETY_NAN))
            return true;

        PushICFlags ic(context, BCI_SAFETY);

        const auto r0 = reserveRegisterRC(context);
        EMIT_INST3(context, ByteCodeOp::CompareOpEqualF64, r, r, r0);
        EMIT_INST1(context, ByteCodeOp::JumpIfTrue, r0)->b.s32    = 1;
        EMIT_INST0(context, ByteCodeOp::InternalPanic)->d.pointer = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(safetyMsg(SafetyMsg::InvalidFloat)));
        freeRegisterRC(context, r0);
        return true;
    }

    return true;
}

void ByteCodeGen::emitSafetyDivZero(ByteCodeGenContext* context, uint32_t r, uint32_t bits)
{
    if (!mustEmitSafety(context, SAFETY_MATH))
        return;

    emitSafetyNotZero(context, r, bits, safetyMsg(SafetyMsg::NotZero));
}

void ByteCodeGen::emitSafetyDivOverflow(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t bits, bool dref)
{
    if (!mustEmitSafety(context, SAFETY_OVERFLOW))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    const auto re = reserveRegisterRC(context);
    const auto rd = reserveRegisterRC(context);

    switch (bits)
    {
        case 8:
        {
            if (dref)
            {
                EMIT_INST2(context, ByteCodeOp::DeRef8, rd, r0);
                r0 = rd;
            }

            auto inst   = EMIT_INST3(context, ByteCodeOp::CompareOpEqual8, r1, 0, re);
            inst->b.s64 = -1;
            inst->addFlag(BCI_IMM_B);
            EMIT_INST1(context, ByteCodeOp::JumpIfFalse, re)->b.s32 = 3;

            inst       = EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual8, r0, 0, re);
            inst->b.s8 = INT8_MIN;
            inst->addFlag(BCI_IMM_B);

            emitAssert(context, re, safetyMsg(SafetyMsg::Div, g_TypeMgr->typeInfoS8));
            break;
        }

        case 16:
        {
            if (dref)
            {
                EMIT_INST2(context, ByteCodeOp::DeRef16, rd, r0);
                r0 = rd;
            }

            auto inst   = EMIT_INST3(context, ByteCodeOp::CompareOpEqual16, r1, 0, re);
            inst->b.s64 = -1;
            inst->addFlag(BCI_IMM_B);
            EMIT_INST1(context, ByteCodeOp::JumpIfFalse, re)->b.s32 = 3;

            inst        = EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual16, r0, 0, re);
            inst->b.s16 = INT16_MIN;
            inst->addFlag(BCI_IMM_B);

            emitAssert(context, re, safetyMsg(SafetyMsg::Div, g_TypeMgr->typeInfoS16));
            break;
        }

        case 32:
        {
            if (dref)
            {
                EMIT_INST2(context, ByteCodeOp::DeRef32, rd, r0);
                r0 = rd;
            }

            auto inst   = EMIT_INST3(context, ByteCodeOp::CompareOpEqual32, r1, 0, re);
            inst->b.s64 = -1;
            inst->addFlag(BCI_IMM_B);
            EMIT_INST1(context, ByteCodeOp::JumpIfFalse, re)->b.s32 = 3;

            inst        = EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual32, r0, 0, re);
            inst->b.s32 = INT32_MIN;
            inst->addFlag(BCI_IMM_B);

            emitAssert(context, re, safetyMsg(SafetyMsg::Div, g_TypeMgr->typeInfoS32));
            break;
        }

        case 64:
        {
            if (dref)
            {
                EMIT_INST2(context, ByteCodeOp::DeRef64, rd, r0);
                r0 = rd;
            }

            auto inst   = EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, r1, 0, re);
            inst->b.s64 = -1;
            inst->addFlag(BCI_IMM_B);
            EMIT_INST1(context, ByteCodeOp::JumpIfFalse, re)->b.s32 = 3;

            inst        = EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual64, r0, 0, re);
            inst->b.s64 = INT64_MIN;
            inst->addFlag(BCI_IMM_B);

            emitAssert(context, re, safetyMsg(SafetyMsg::Div, g_TypeMgr->typeInfoS64));
            break;
        }

        default:
            SWAG_ASSERT(false);
    }

    freeRegisterRC(context, rd);
    freeRegisterRC(context, re);
}

void ByteCodeGen::emitSafetyBoundCheckLowerU32(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    PushICFlags ic(context, BCI_SAFETY);

    const auto re = reserveRegisterRC(context);
    EMIT_INST3(context, ByteCodeOp::CompareOpLowerU32, r0, r1, re);
    emitAssert(context, re, safetyMsg(SafetyMsg::IndexRange));
    freeRegisterRC(context, re);
}

void ByteCodeGen::emitSafetyNeg(ByteCodeGenContext* context, uint32_t r0, TypeInfo* typeInfo, bool forAbs)
{
    PushICFlags ic(context, BCI_SAFETY);

    const auto re = reserveRegisterRC(context);

    const auto msg  = safetyMsg(SafetyMsg::IntrinsicAbs, typeInfo);
    const auto msg1 = safetyMsg(SafetyMsg::Neg, typeInfo);

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::S8:
        {
            const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual8, r0, 0, re);
            inst->b.s64     = INT8_MIN;
            inst->addFlag(BCI_IMM_B);
            emitAssert(context, re, forAbs ? msg : msg1);
            break;
        }
        case NativeTypeKind::S16:
        {
            const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual16, r0, 0, re);
            inst->b.s64     = INT16_MIN;
            inst->addFlag(BCI_IMM_B);
            emitAssert(context, re, forAbs ? msg : msg1);
            break;
        }
        case NativeTypeKind::S32:
        {
            const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual32, r0, 0, re);
            inst->b.s64     = INT32_MIN;
            inst->addFlag(BCI_IMM_B);
            emitAssert(context, re, forAbs ? msg : msg1);
            break;
        }
        case NativeTypeKind::S64:
        {
            const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual64, r0, 0, re);
            inst->b.s64     = INT64_MIN;
            inst->addFlag(BCI_IMM_B);
            emitAssert(context, re, forAbs ? msg : msg1);
            break;
        }
    }

    freeRegisterRC(context, re);
}

void ByteCodeGen::emitSafetyBoundCheckLowerU64(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    PushICFlags ic(context, BCI_SAFETY);

    const auto re = reserveRegisterRC(context);
    EMIT_INST3(context, ByteCodeOp::CompareOpLowerU64, r0, r1, re);
    emitAssert(context, re, safetyMsg(SafetyMsg::IndexRange));
    freeRegisterRC(context, re);
}

void ByteCodeGen::emitSafetyBoundCheckString(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    if (!mustEmitSafety(context, SAFETY_BOUND_CHECK))
        return;

    emitSafetyBoundCheckLowerU64(context, r0, r1);
}

void ByteCodeGen::emitSafetyBoundCheckSlice(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    if (!mustEmitSafety(context, SAFETY_BOUND_CHECK))
        return;

    emitSafetyBoundCheckLowerU64(context, r0, r1);
}

void ByteCodeGen::emitSafetyBoundCheckArray(ByteCodeGenContext* context, uint32_t r0, const TypeInfoArray* typeInfoArray)
{
    if (!mustEmitSafety(context, SAFETY_BOUND_CHECK))
        return;

    PushICFlags ic(context, BCI_SAFETY);
    const auto  r1   = reserveRegisterRC(context);
    const auto  inst = EMIT_INST1(context, ByteCodeOp::SetImmediate64, r1);
    inst->b.u64      = typeInfoArray->count;
    emitSafetyBoundCheckLowerU64(context, r0, r1);
    freeRegisterRC(context, r1);
}

void ByteCodeGen::emitSafetyCastAny(ByteCodeGenContext* context, const AstNode* exprNode, TypeInfo* toType)
{
    if (!mustEmitSafety(context, SAFETY_DYN_CAST))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    const auto r0 = reserveRegisterRC(context);
    const auto r1 = reserveRegisterRC(context);

    emitSafetyNotZero(context, exprNode->resultRegisterRc[1], 64, safetyMsg(SafetyMsg::CastAnyNull, toType));

    // :AnyTypeSegment
    SWAG_ASSERT(exprNode->hasExtraPointer(ExtraPointerKind::AnyTypeSegment));
    const auto anyTypeSegment = exprNode->extraPointer<DataSegment>(ExtraPointerKind::AnyTypeSegment);
    const auto anyTypeOffset  = exprNode->extraValue(ExtraPointerKind::AnyTypeOffset);
    emitMakeSegPointer(context, anyTypeSegment, static_cast<uint32_t>(anyTypeOffset), r0);

    const auto rflags = reserveRegisterRC(context);
    const auto inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate32, rflags);
    inst->b.u64       = SWAG_COMPARE_CAST_ANY;

    EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, r0, exprNode->resultRegisterRc[1], rflags, r1);
    freeRegisterRC(context, rflags);
    emitSafetyNotZero(context, r1, 8, safetyMsg(SafetyMsg::CastAny, toType));

    freeRegisterRC(context, r0);
    freeRegisterRC(context, r1);
}

void ByteCodeGen::emitSafetyRange(ByteCodeGenContext* context, const AstRange* node)
{
    if (!mustEmitSafety(context, SAFETY_BOUND_CHECK))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    // Lower bound <= upper bound
    const auto re = reserveRegisterRC(context);
    context->pushLocation(&node->expressionLow->token.startLocation);
    if (node->expressionLow->typeInfo->isNativeIntegerSigned())
        EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS64, node->expressionLow->resultRegisterRc, node->expressionUp->resultRegisterRc, re);
    else
        EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, node->expressionLow->resultRegisterRc, node->expressionUp->resultRegisterRc, re);
    emitAssert(context, re, safetyMsg(SafetyMsg::BadRangeDown));
    context->popLocation();
    freeRegisterRC(context, re);
}

void ByteCodeGen::emitSafetyArrayPointerSlicing(ByteCodeGenContext* context, const AstArrayPointerSlicing* node)
{
    if (!mustEmitSafety(context, SAFETY_BOUND_CHECK))
        return;

    PushICFlags ic(context, BCI_SAFETY);
    uint32_t    maxBoundReg     = UINT32_MAX;
    bool        freeMaxBoundReg = false;

    // Check type, and safety check
    const auto typeVar = TypeManager::concreteType(node->array->typeInfo);
    if (typeVar->isArray())
    {
        const auto typeArray = castTypeInfo<TypeInfoArray>(typeVar, TypeInfoKind::Array);
        maxBoundReg          = reserveRegisterRC(context);
        const auto inst      = EMIT_INST1(context, ByteCodeOp::SetImmediate64, maxBoundReg);
        inst->b.u64          = typeArray->count;
        freeMaxBoundReg      = true;
    }
    else if (typeVar->isString())
    {
        maxBoundReg = node->array->resultRegisterRc[1];
    }
    else if (typeVar->isSlice())
    {
        maxBoundReg = node->array->resultRegisterRc[1];
    }

    // Lower bound <= upper bound
    {
        const auto re = reserveRegisterRC(context);
        context->pushLocation(&node->lowerBound->token.startLocation);
        EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, node->lowerBound->resultRegisterRc, node->upperBound->resultRegisterRc, re);
        emitAssert(context, re, safetyMsg(SafetyMsg::BadSlicingDown));
        context->popLocation();
        freeRegisterRC(context, re);
    }

    // Upper bound < maxBound
    if (maxBoundReg != UINT32_MAX)
    {
        const auto re = reserveRegisterRC(context);
        context->pushLocation(&node->upperBound->token.startLocation);
        EMIT_INST3(context, ByteCodeOp::CompareOpLowerU64, node->upperBound->resultRegisterRc, maxBoundReg, re);
        emitAssert(context, re, safetyMsg(SafetyMsg::BadSlicingUp));
        context->popLocation();
        freeRegisterRC(context, re);
        if (freeMaxBoundReg)
            freeRegisterRC(context, maxBoundReg);
    }
}

void ByteCodeGen::emitSafetyCastOverflow(ByteCodeGenContext* context, TypeInfo* typeInfo, TypeInfo* fromTypeInfo, AstNode* exprNode)
{
    if (!mustEmitSafety(context, SAFETY_OVERFLOW))
        return;
    if (!typeInfo->isNative())
        return;
    if (!fromTypeInfo->isNative())
        return;

    PushLocation lc(context, &exprNode->token.startLocation);
    PushICFlags  ic(context, BCI_SAFETY);
    const auto   re   = reserveRegisterRC(context);
    const auto   msg  = safetyMsg(SafetyMsg::CastTruncated, typeInfo, fromTypeInfo);
    const auto   msg1 = safetyMsg(SafetyMsg::CastNeg, typeInfo, fromTypeInfo);

    switch (typeInfo->nativeType)
    {
        // To S8
        ////////////////////////////////////////////////////
        case NativeTypeKind::S8:
            switch (fromTypeInfo->nativeType)
            {
                case NativeTypeKind::U8:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU8, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = INT8_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::U16:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU16, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = INT8_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::U32:
                case NativeTypeKind::Rune:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = INT8_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::U64:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = INT8_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::S16:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS16, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.s64 = INT8_MIN;
                    emitAssert(context, re, msg);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS16, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.s64 = INT8_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::S32:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.s64 = INT8_MIN;
                    emitAssert(context, re, msg);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.s64 = INT8_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::S64:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.s64 = INT8_MIN;
                    emitAssert(context, re, msg);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.s64 = INT8_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::F32:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f32 = static_cast<float>(INT8_MIN) - 0.5f;
                    emitAssert(context, re, msg);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f32 = static_cast<float>(INT8_MAX) + 0.5f;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::F64:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f64 = static_cast<double>(INT8_MIN) - 0.5;
                    emitAssert(context, re, msg);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f64 = static_cast<double>(INT8_MAX) + 0.5;
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
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.s64 = INT16_MIN;
                    emitAssert(context, re, msg);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.s64 = INT16_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::S64:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.s64 = INT16_MIN;
                    emitAssert(context, re, msg);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.s64 = INT16_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::U16:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU16, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = INT16_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::U32:
                case NativeTypeKind::Rune:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = INT16_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::U64:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = INT16_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::F32:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f32 = static_cast<float>(INT16_MIN) - 0.5f;
                    emitAssert(context, re, msg);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f32 = static_cast<float>(INT16_MAX) + 0.5f;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::F64:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f64 = static_cast<double>(INT16_MIN) - 0.5;
                    emitAssert(context, re, msg);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f64 = static_cast<double>(INT16_MAX) + 0.5;
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
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = INT32_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::U64:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = INT32_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::S64:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.s64 = INT32_MIN;
                    emitAssert(context, re, msg);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.s64 = INT32_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::F32:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f32 = static_cast<float>(INT32_MIN) - 0.5f;
                    emitAssert(context, re, msg);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f32 = static_cast<float>(INT32_MAX) + 0.5f;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::F64:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f64 = static_cast<double>(INT32_MIN) - 0.5;
                    emitAssert(context, re, msg);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f64 = static_cast<double>(INT32_MAX) + 0.5;
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
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = INT64_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::F64:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f64 = static_cast<double>(INT64_MAX) + 0.5;
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
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS8, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS8, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);
                    break;
                }

                case NativeTypeKind::S16:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS16, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU16, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = UINT8_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::S32:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = UINT8_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::S64:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = UINT8_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::U16:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU16, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = UINT8_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::U32:
                case NativeTypeKind::Rune:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = UINT8_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::U64:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = UINT8_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::F32:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f32 = -SAFETY_ZERO_EPSILON;
                    emitAssert(context, re, msg1);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f32 = static_cast<float>(UINT8_MAX) + 0.5;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::F64:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f64 = -SAFETY_ZERO_EPSILON;
                    emitAssert(context, re, msg1);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f64 = static_cast<double>(UINT8_MAX) + 0.5;
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
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS8, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);
                    break;
                }

                case NativeTypeKind::S16:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);
                    break;
                }

                case NativeTypeKind::S32:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = UINT16_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::S64:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = UINT16_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::U32:
                case NativeTypeKind::Rune:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = UINT16_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::U64:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = UINT16_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::F32:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f32 = -SAFETY_ZERO_EPSILON;
                    emitAssert(context, re, msg1);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f32 = UINT16_MAX + 0.5;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::F64:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f64 = -SAFETY_ZERO_EPSILON;
                    emitAssert(context, re, msg1);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f64 = static_cast<double>(UINT16_MAX) + 0.5;
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
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS8, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);
                    break;
                }

                case NativeTypeKind::S16:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS16, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);
                    break;
                }

                case NativeTypeKind::S32:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::S64:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = UINT32_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::U64:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = UINT32_MAX;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::F32:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f32 = -SAFETY_ZERO_EPSILON;
                    emitAssert(context, re, msg1);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f32 = static_cast<float>(UINT32_MAX) + 0.5f;
                    emitAssert(context, re, msg);
                    break;
                }

                case NativeTypeKind::F64:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f64 = -SAFETY_ZERO_EPSILON;
                    emitAssert(context, re, msg1);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f64 = static_cast<double>(UINT32_MAX) + 0.5;
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
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS8, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);
                    break;
                }

                case NativeTypeKind::S16:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS16, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);
                    break;
                }

                case NativeTypeKind::S32:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);
                    break;
                }

                case NativeTypeKind::S64:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.u64 = 0;
                    emitAssert(context, re, msg1);
                    break;
                }

                case NativeTypeKind::F32:
                {
                    const auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF32, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f32 = -SAFETY_ZERO_EPSILON;
                    emitAssert(context, re, msg1);
                    break;
                }

                case NativeTypeKind::F64:
                {
                    auto inst = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f64 = -SAFETY_ZERO_EPSILON;
                    emitAssert(context, re, msg1);

                    inst = EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, exprNode->resultRegisterRc, 0, re);
                    inst->addFlag(BCI_IMM_B);
                    inst->b.f64 = static_cast<double>(UINT64_MAX) + 0.5;
                    emitAssert(context, re, msg);
                    break;
                }
                default:
                    break;
            }
            break;
    }

    freeRegisterRC(context, re);
}

void ByteCodeGen::emitSafetyIntrinsics(ByteCodeGenContext* context)
{
    const auto node       = castAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
    const auto callParams = castAst<AstNode>(node->firstChild(), AstNodeKind::FuncCallParams);

    if (!mustEmitSafety(context, SAFETY_MATH))
        return;

    PushICFlags ic(context, BCI_SAFETY);
    switch (node->token.id)
    {
        case TokenId::IntrinsicAbs:
        {
            const auto t0 = TypeManager::concreteType(callParams->firstChild()->typeInfo);
            emitSafetyNeg(context, callParams->firstChild()->resultRegisterRc, t0, true);
            break;
        }
        case TokenId::IntrinsicSqrt:
        {
            const auto t0   = TypeManager::concreteType(callParams->firstChild()->typeInfo);
            const auto re   = reserveRegisterRC(context);
            const auto op   = t0->nativeType == NativeTypeKind::F32 ? ByteCodeOp::CompareOpGreaterEqF32 : ByteCodeOp::CompareOpGreaterEqF64;
            const auto inst = EMIT_INST3(context, op, callParams->firstChild()->resultRegisterRc, 0, re);
            inst->b.f64     = 0;
            inst->addFlag(BCI_IMM_B);
            emitAssert(context, re, safetyMsg(SafetyMsg::IntrinsicSqrt, t0));
            freeRegisterRC(context, re);
            break;
        }
        case TokenId::IntrinsicLog:
        {
            const auto t0   = TypeManager::concreteType(callParams->firstChild()->typeInfo);
            const auto re   = reserveRegisterRC(context);
            const auto op   = t0->nativeType == NativeTypeKind::F32 ? ByteCodeOp::CompareOpGreaterF32 : ByteCodeOp::CompareOpGreaterF64;
            const auto inst = EMIT_INST3(context, op, callParams->firstChild()->resultRegisterRc, 0, re);
            inst->b.f64     = 0;
            inst->addFlag(BCI_IMM_B);
            emitAssert(context, re, safetyMsg(SafetyMsg::IntrinsicLog, t0));
            freeRegisterRC(context, re);
            break;
        }
        case TokenId::IntrinsicLog2:
        {
            const auto t0   = TypeManager::concreteType(callParams->firstChild()->typeInfo);
            const auto re   = reserveRegisterRC(context);
            const auto op   = t0->nativeType == NativeTypeKind::F32 ? ByteCodeOp::CompareOpGreaterF32 : ByteCodeOp::CompareOpGreaterF64;
            const auto inst = EMIT_INST3(context, op, callParams->firstChild()->resultRegisterRc, 0, re);
            inst->b.f64     = 0;
            inst->addFlag(BCI_IMM_B);
            emitAssert(context, re, safetyMsg(SafetyMsg::IntrinsicLog2, t0));
            freeRegisterRC(context, re);
            break;
        }
        case TokenId::IntrinsicLog10:
        {
            const auto t0   = TypeManager::concreteType(callParams->firstChild()->typeInfo);
            const auto re   = reserveRegisterRC(context);
            const auto op   = t0->nativeType == NativeTypeKind::F32 ? ByteCodeOp::CompareOpGreaterF32 : ByteCodeOp::CompareOpGreaterF64;
            const auto inst = EMIT_INST3(context, op, callParams->firstChild()->resultRegisterRc, 0, re);
            inst->b.f64     = 0;
            inst->addFlag(BCI_IMM_B);
            emitAssert(context, re, safetyMsg(SafetyMsg::IntrinsicLog10, t0));
            freeRegisterRC(context, re);
            break;
        }
        case TokenId::IntrinsicASin:
        case TokenId::IntrinsicACos:
        {
            const auto t0  = TypeManager::concreteType(callParams->firstChild()->typeInfo);
            const auto msg = node->token.is(TokenId::IntrinsicASin) ? safetyMsg(SafetyMsg::IntrinsicASin, t0) : safetyMsg(SafetyMsg::IntrinsicACos, t0);
            const auto re  = reserveRegisterRC(context);
            if (t0->nativeType == NativeTypeKind::F32)
            {
                auto inst   = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqF32, callParams->firstChild()->resultRegisterRc, 0, re);
                inst->b.f32 = -1;
                inst->addFlag(BCI_IMM_B);
                emitAssert(context, re, msg);
                inst        = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqF32, callParams->firstChild()->resultRegisterRc, 0, re);
                inst->b.f32 = 1;
                inst->addFlag(BCI_IMM_B);
                emitAssert(context, re, msg);
            }
            else
            {
                auto inst   = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqF64, callParams->firstChild()->resultRegisterRc, 0, re);
                inst->b.f64 = -1;
                inst->addFlag(BCI_IMM_B);
                emitAssert(context, re, msg);
                inst        = EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqF64, callParams->firstChild()->resultRegisterRc, 0, re);
                inst->b.f64 = 1;
                inst->addFlag(BCI_IMM_B);
                emitAssert(context, re, msg);
            }
            freeRegisterRC(context, re);
            break;
        }
        case TokenId::IntrinsicPow:
        {
            const auto t0  = TypeManager::concreteType(callParams->firstChild()->typeInfo);
            const auto msg = safetyMsg(SafetyMsg::IntrinsicPow, t0);
            const auto re  = reserveRegisterRC(context);
            if (t0->nativeType == NativeTypeKind::F32)
            {
                auto inst   = EMIT_INST3(context, ByteCodeOp::CompareOpEqualF32, callParams->firstChild()->resultRegisterRc, 0, re);
                inst->b.f32 = 0;
                inst->addFlag(BCI_IMM_B);
                EMIT_INST1(context, ByteCodeOp::JumpIfFalse, re)->b.s32 = 3;

                inst        = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqF32, callParams->secondChild()->resultRegisterRc, 0, re);
                inst->b.f32 = 0;
                inst->addFlag(BCI_IMM_B);
                emitAssert(context, re, msg);
            }
            else
            {
                auto inst   = EMIT_INST3(context, ByteCodeOp::CompareOpEqualF64, callParams->firstChild()->resultRegisterRc, 0, re);
                inst->b.f64 = 0;
                inst->addFlag(BCI_IMM_B);
                EMIT_INST1(context, ByteCodeOp::JumpIfFalse, re)->b.s32 = 3;

                inst        = EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqF64, callParams->secondChild()->resultRegisterRc, 0, re);
                inst->b.f64 = 0;
                inst->addFlag(BCI_IMM_B);
                emitAssert(context, re, msg);
            }
            freeRegisterRC(context, re);
            break;
        }
        default:
            break;
    }
}
