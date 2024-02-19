#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeGen.h"
#include "ByteCodeGenContext.h"
#include "ErrorIds.h"
#include "Module.h"
#include "TypeManager.h"

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
                g_TypedMsg[m][i][j] = FMT(Err(Saf0002), toType->name.c_str());
                break;
            case SafetyMsg::CastAny:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0001), toType->name.c_str());
                break;
            case SafetyMsg::NullCheck:
                g_TypedMsg[m][0][0] = Err(Saf0017);
                break;
            case SafetyMsg::ErrCheck:
                g_TypedMsg[m][0][0] = Err(Saf0018);
                break;
            case SafetyMsg::InvalidBool:
                g_TypedMsg[m][0][0] = Err(Saf0003);
                break;
            case SafetyMsg::InvalidFloat:
                g_TypedMsg[m][0][0] = Err(Saf0016);
                break;
            case SafetyMsg::NotZero:
                g_TypedMsg[m][0][0] = Err(Saf0015);
                break;
            case SafetyMsg::BadSlicingDown:
                g_TypedMsg[m][0][0] = Err(Saf0006);
                break;
            case SafetyMsg::BadSlicingUp:
                g_TypedMsg[m][0][0] = Err(Saf0007);
                break;
            case SafetyMsg::BadRangeDown:
                g_TypedMsg[m][0][0] = Err(Saf0005);
                break;
            case SafetyMsg::IndexRange:
                g_TypedMsg[m][0][0] = Err(Saf0004);
                break;
            case SafetyMsg::SwitchComplete:
                g_TypedMsg[m][0][0] = Err(Saf0028);
                break;
            case SafetyMsg::CastTruncated:
                SWAG_ASSERT(toType && fromType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0027), fromType->name.c_str(), toType->name.c_str());
                break;
            case SafetyMsg::CastNeg:
                SWAG_ASSERT(toType && fromType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0020), fromType->name.c_str(), toType->name.c_str());
                break;
            case SafetyMsg::Plus:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0026), "+", toType->name.c_str());
                break;
            case SafetyMsg::Minus:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0026), "-", toType->name.c_str());
                break;
            case SafetyMsg::Mul:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0026), "*", toType->name.c_str());
                break;
            case SafetyMsg::PlusEq:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0026), "+=", toType->name.c_str());
                break;
            case SafetyMsg::MinusEq:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0026), "-=", toType->name.c_str());
                break;
            case SafetyMsg::MulEq:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0026), "*=", toType->name.c_str());
                break;
            case SafetyMsg::Neg:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0026), "-", toType->name.c_str());
                break;
            case SafetyMsg::IntrinsicAbs:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0008), toType->name.c_str());
                break;
            case SafetyMsg::IntrinsicSqrt:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0014), toType->name.c_str());
                break;
            case SafetyMsg::IntrinsicLog:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0011), toType->name.c_str());
                break;
            case SafetyMsg::IntrinsicLog2:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0013), toType->name.c_str());
                break;
            case SafetyMsg::IntrinsicLog10:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0012), toType->name.c_str());
                break;
            case SafetyMsg::IntrinsicASin:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0010), toType->name.c_str());
                break;
            case SafetyMsg::IntrinsicACos:
                SWAG_ASSERT(toType);
                g_TypedMsg[m][i][j] = FMT(Err(Saf0009), toType->name.c_str());
                break;
            default:
                break;
        }
    }

    return g_TypedMsg[m][i][j].c_str();
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
        default:
            break;
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
    if (!mustEmitSafety(context, SAFETY_ANY))
        return;

    PushICFlags ic(context, BCI_SAFETY);

    const auto r0 = reserveRegisterRC(context);
    const auto r1 = reserveRegisterRC(context);

    emitSafetyNotZero(context, exprNode->resultRegisterRc[1], 64, safetyMsg(SafetyMsg::CastAnyNull, toType));

    // :AnyTypeSegment
    SWAG_ASSERT(exprNode->hasExtMisc());
    SWAG_ASSERT(exprNode->extMisc()->anyTypeSegment);
    emitMakeSegPointer(context, exprNode->extMisc()->anyTypeSegment, exprNode->extMisc()->anyTypeOffset, r0);

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
        default:
            break;
    }

    freeRegisterRC(context, re);
}
