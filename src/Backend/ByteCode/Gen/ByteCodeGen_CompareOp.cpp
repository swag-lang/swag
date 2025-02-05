#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Report/Report.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"

bool ByteCodeGen::emitSwitchCaseSpecialFunc(ByteCodeGenContext* context, AstSwitchCase* caseNode, AstNode* expr, TokenId op, RegisterList& result)
{
    SWAG_ASSERT(caseNode->hasSpecialFuncCall());

    // Those registers are shared and must be kept.
    // We need to do that because the special function could be inlined, and in that case will want to release
    // the parameters, which are our registers. And we do not want them to be released except by us...
    caseNode->ownerSwitch->resultRegisterRc.cannotFree             = true;
    caseNode->ownerSwitch->expression->resultRegisterRc.cannotFree = true;
    expr->resultRegisterRc.cannotFree                              = true;

    const auto r0 = caseNode->ownerSwitch->resultRegisterRc;
    const auto r1 = expr->resultRegisterRc;

    context->allocateTempCallParams();
    context->allParamsTmp->children.push_back(caseNode);
    context->allParamsTmp->children.push_back(expr);
    caseNode->resultRegisterRc = r0;
    expr->resultRegisterRc     = r1;
    SWAG_CHECK(emitUserOp(context, context->allParamsTmp, caseNode, false));
    YIELD();
    SWAG_CHECK(emitCompareOpPostSpecialFunc(context, op));
    YIELD();

    caseNode->ownerSwitch->resultRegisterRc.cannotFree             = false;
    caseNode->ownerSwitch->expression->resultRegisterRc.cannotFree = false;
    expr->resultRegisterRc.cannotFree                              = false;

    if (caseNode->lastChild()->is(AstNodeKind::Inline))
        result = caseNode->lastChild()->resultRegisterRc;
    else
        result = context->node->resultRegisterRc;

    return true;
}

bool ByteCodeGen::emitSwitchCaseRange(ByteCodeGenContext* context, AstSwitchCase* caseNode, AstRange* rangeNode, RegisterList& result)
{
    auto low        = rangeNode->expressionLow;
    auto up         = rangeNode->expressionUp;
    bool excludeLow = false;
    bool excludeUp  = rangeNode->hasSpecFlag(AstRange::SPEC_FLAG_EXCLUDE_UP);

    const auto typeInfo = TypeManager::concretePtrRefType(low->typeInfo);
    if (!typeInfo->isNativeIntegerOrRune() && !typeInfo->isNativeFloat())
        return Report::internalError(context->node, "emitInRange, type not supported");

    // Invert test if lower bound is greater than upper bound
    bool orderIsDefined = false;
    if (low->hasFlagComputedValue() && up->hasFlagComputedValue())
    {
        orderIsDefined = true;

        bool swap = false;
        if (typeInfo->isNativeIntegerSigned() && low->computedValue()->reg.s64 > up->computedValue()->reg.s64)
            swap = true;
        else if (typeInfo->isNativeInteger() && low->computedValue()->reg.u64 > up->computedValue()->reg.u64)
            swap = true;
        else if (typeInfo->isNative(NativeTypeKind::F32) && low->computedValue()->reg.f32 > up->computedValue()->reg.f32)
            swap = true;
        else if (typeInfo->isNative(NativeTypeKind::F64) && low->computedValue()->reg.f64 > up->computedValue()->reg.f64)
            swap = true;
        else if (typeInfo->isRune() && low->computedValue()->reg.u32 > up->computedValue()->reg.u32)
            swap = true;

        if (swap)
        {
            caseNode->addSemFlag(SEMFLAG_TRY_2);
            std::swap(low, up);
            std::swap(excludeLow, excludeUp);
        }
    }

    if (!orderIsDefined)
    {
        return Report::internalError(context->node, "emitInRange, order undefined");
    }

    RegisterList ra, rb;
    const auto&  r0 = caseNode->ownerSwitch->resultRegisterRc;

    // Lower bound
    if (caseNode->hasSpecialFuncCall())
    {
        if (!caseNode->hasSemFlag(SEMFLAG_TRY_1))
        {
            SWAG_CHECK(emitSwitchCaseSpecialFunc(context, caseNode, low, excludeLow ? TokenId::SymGreater : TokenId::SymGreaterEqual, ra));
            YIELD();
            caseNode->removeAstFlag(AST_INLINED);
            caseNode->removeSemFlag(SEMFLAG_RESOLVE_INLINED);
            caseNode->addSemFlag(SEMFLAG_TRY_1);
            rangeNode->leftRegister = ra;
        }
        else
            ra = rangeNode->leftRegister;
    }
    else if (excludeLow)
    {
        ra = reserveRegisterRC(context);
        SWAG_CHECK(emitCompareOpGreater(context, caseNode, low, r0, low->resultRegisterRc, ra));
    }
    else
    {
        ra = reserveRegisterRC(context);
        SWAG_CHECK(emitCompareOpGreaterEq(context, caseNode, low, r0, low->resultRegisterRc, ra));
    }

    // Upper bound
    if (caseNode->hasSpecialFuncCall())
    {
        SWAG_CHECK(emitSwitchCaseSpecialFunc(context, caseNode, up, excludeUp ? TokenId::SymLower : TokenId::SymLowerEqual, rb));
        YIELD();
    }
    else if (excludeUp)
    {
        rb = reserveRegisterRC(context);
        SWAG_CHECK(emitCompareOpLower(context, caseNode, up, r0, up->resultRegisterRc, rb));
    }
    else
    {
        rb = reserveRegisterRC(context);
        SWAG_CHECK(emitCompareOpLowerEq(context, caseNode, up, r0, up->resultRegisterRc, rb));
    }

    result = reserveRegisterRC(context);
    EMIT_INST3(context, ByteCodeOp::CompareOpEqual8, ra, rb, result);

    freeRegisterRC(context, ra);
    freeRegisterRC(context, rb);
    freeRegisterRC(context, rangeNode->expressionLow);
    freeRegisterRC(context, rangeNode->expressionUp);
    return true;
}

bool ByteCodeGen::emitCompareOpPostSpecialFunc(const ByteCodeGenContext* context, TokenId op)
{
    const auto node = context->node;
    auto       r2   = node->resultRegisterRc;
    if (node->hasSemFlag(SEMFLAG_INVERSE_PARAMS))
    {
        switch (op)
        {
            case TokenId::SymLowerEqualGreater:
            {
                const auto rt = reserveRegisterRC(context);
                EMIT_INST2(context, ByteCodeOp::NegS32, rt, r2);
                freeRegisterRC(context, r2);
                node->resultRegisterRc = rt;
                break;
            }
            case TokenId::SymExclamEqual:
            {
                const auto rt = reserveRegisterRC(context);
                EMIT_INST2(context, ByteCodeOp::NegBool, rt, r2);
                freeRegisterRC(context, r2);
                node->resultRegisterRc = rt;
                break;
            }

            case TokenId::SymGreater:
                EMIT_INST1(context, ByteCodeOp::LowerZeroToTrue, r2);
                break;
            case TokenId::SymGreaterEqual:
                EMIT_INST1(context, ByteCodeOp::LowerEqZeroToTrue, r2);
                break;
            case TokenId::SymLower:
                EMIT_INST1(context, ByteCodeOp::GreaterZeroToTrue, r2);
                break;
            case TokenId::SymLowerEqual:
                EMIT_INST1(context, ByteCodeOp::GreaterEqZeroToTrue, r2);
                break;
            default:
                break;
        }
    }
    else
    {
        switch (op)
        {
            case TokenId::SymExclamEqual:
            {
                const auto rt = reserveRegisterRC(context);
                EMIT_INST2(context, ByteCodeOp::NegBool, rt, r2);
                freeRegisterRC(context, r2);
                node->resultRegisterRc = rt;
                break;
            }
            case TokenId::SymLower:
                EMIT_INST1(context, ByteCodeOp::LowerZeroToTrue, r2);
                break;
            case TokenId::SymGreater:
                EMIT_INST1(context, ByteCodeOp::GreaterZeroToTrue, r2);
                break;
            case TokenId::SymLowerEqual:
                EMIT_INST1(context, ByteCodeOp::LowerEqZeroToTrue, r2);
                break;
            case TokenId::SymGreaterEqual:
                EMIT_INST1(context, ByteCodeOp::GreaterEqZeroToTrue, r2);
                break;
            default:
                break;
        }
    }

    return true;
}

bool ByteCodeGen::emitCompareOpEqual(ByteCodeGenContext* context, AstNode* left, const AstNode* right, const RegisterList& r0, const RegisterList& r1, const RegisterList& r2)
{
    const auto leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo);
    const auto rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);

    if (leftTypeInfo->isNative())
    {
        switch (leftTypeInfo->nativeType)
        {
            case NativeTypeKind::Bool:
            case NativeTypeKind::S8:
            case NativeTypeKind::U8:
                EMIT_INST3(context, ByteCodeOp::CompareOpEqual8, r0, r1, r2);
                return true;
            case NativeTypeKind::S16:
            case NativeTypeKind::U16:
                EMIT_INST3(context, ByteCodeOp::CompareOpEqual16, r0, r1, r2);
                return true;
            case NativeTypeKind::S32:
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                EMIT_INST3(context, ByteCodeOp::CompareOpEqual32, r0, r1, r2);
                return true;
            case NativeTypeKind::S64:
            case NativeTypeKind::U64:
                EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
                return true;
            case NativeTypeKind::F32:
                EMIT_INST3(context, ByteCodeOp::CompareOpEqualF32, r0, r1, r2);
                return true;
            case NativeTypeKind::F64:
                EMIT_INST3(context, ByteCodeOp::CompareOpEqualF64, r0, r1, r2);
                return true;

            case NativeTypeKind::String:
                if (right->hasSemFlag(SEMFLAG_TYPE_NULL))
                {
                    EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, r0[0], r1[0], r2);
                    return true;
                }

                EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r2, r1[1]);
                EMIT_INST4(context, ByteCodeOp::IntrinsicStringCmp, r0[0], r0[1], r1[0], r2);
                return true;

            case NativeTypeKind::Any:
                if (right->hasSemFlag(SEMFLAG_TYPE_NULL))
                {
                    EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, r0[0], r1[0], r2);
                    return true;
                }

                if (context->node->hasSpecFlag(AstBinaryOpNode::SPEC_FLAG_IMPLICIT_KINDOF))
                {
                    SWAG_CHECK(emitKindOfAny(context, left));
                    const auto rFlags = reserveRegisterRC(context);
                    const auto inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate32, rFlags);
                    inst->b.u64       = SWAG_TYPECMP_STRICT;
                    EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, r0, r1, rFlags, r2);
                    freeRegisterRC(context, rFlags);
                    return true;
                }

                break;

            default:
                break;
        }
    }

    if (leftTypeInfo->isPointer())
    {
        // Special case for typeinfo, as this is not safe to just compare pointers.
        // The same typeinfo can be different if defined in two different modules, so we need
        // to make a compare by name too
        if (leftTypeInfo->isPointerToTypeInfo() || rightTypeInfo->isPointerToTypeInfo())
        {
            if (!leftTypeInfo->isPointerNull() && !rightTypeInfo->isPointerNull())
            {
                const auto rFlags = reserveRegisterRC(context);
                const auto inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate32, rFlags);
                inst->b.u64       = SWAG_TYPECMP_STRICT;
                EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, r0, r1, rFlags, r2);
                freeRegisterRC(context, rFlags);
                return true;
            }
        }

        // CString compare
        if (leftTypeInfo->hasFlag(TYPEINFO_C_STRING))
        {
            EMIT_INST3(context, ByteCodeOp::IntrinsicStrCmp, r2, r0, r1);
            EMIT_INST1(context, ByteCodeOp::ZeroToTrue, r2);
            return true;
        }

        // Simple pointer compare
        EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
        return true;
    }

    if (leftTypeInfo->isClosure())
    {
        EMIT_INST2(context, ByteCodeOp::DeRef64, r0, r0);
        EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
        return true;
    }

    if (leftTypeInfo->isLambda())
    {
        EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
        return true;
    }

    if (leftTypeInfo->isInterface())
    {
        if (rightTypeInfo->isPointerNull() || right->hasSemFlag(SEMFLAG_FROM_NULL))
        {
            EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, r0[1], r1[1], r2);
            return true;
        }

        if (rightTypeInfo->isInterface())
        {
            EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, r0[0], r1[0], r2);
            EMIT_INST1(context, ByteCodeOp::JumpIfFalse, r2)->b.u64 = 1;
            EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, r0[1], r1[1], r2);
            return true;
        }

        if (context->node->hasSpecFlag(AstBinaryOpNode::SPEC_FLAG_IMPLICIT_KINDOF))
        {
            SWAG_CHECK(emitKindOfInterface(context, left));
            const auto rFlags = reserveRegisterRC(context);
            const auto inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate32, rFlags);
            inst->b.u64       = SWAG_TYPECMP_STRICT;
            EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, r0, r1, rFlags, r2);
            freeRegisterRC(context, rFlags);
            return true;
        }
    }

    // Just compare pointers. This is enough for now, as we can only compare a slice to 'null'
    if (leftTypeInfo->isSlice())
    {
        EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, r0[1], r1[1], r2);
        return true;
    }

    return Report::internalError(context->node, "emitCompareOpEqual, type not supported");
}

bool ByteCodeGen::emitCompareOpNotEqual(ByteCodeGenContext* context, AstNode* left, const AstNode* right, const RegisterList& r0, const RegisterList& r1, const RegisterList& r2)
{
    const auto leftTypeInfo  = TypeManager::concretePtrRefType(left->typeInfo);
    const auto rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);

    if (leftTypeInfo->isNative())
    {
        switch (leftTypeInfo->nativeType)
        {
            case NativeTypeKind::Bool:
            case NativeTypeKind::S8:
            case NativeTypeKind::U8:
                EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual8, r0, r1, r2);
                return true;
            case NativeTypeKind::S16:
            case NativeTypeKind::U16:
                EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual16, r0, r1, r2);
                return true;
            case NativeTypeKind::S32:
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual32, r0, r1, r2);
                return true;
            case NativeTypeKind::F32:
                EMIT_INST3(context, ByteCodeOp::CompareOpNotEqualF32, r0, r1, r2);
                return true;
            case NativeTypeKind::S64:
            case NativeTypeKind::U64:
                EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual64, r0, r1, r2);
                return true;
            case NativeTypeKind::F64:
                EMIT_INST3(context, ByteCodeOp::CompareOpNotEqualF64, r0, r1, r2);
                return true;

            case NativeTypeKind::String:
            {
                if (right->hasSemFlag(SEMFLAG_TYPE_NULL))
                {
                    EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual64, r0[0], r1[0], r2);
                    return true;
                }

                const auto rt = reserveRegisterRC(context);
                EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, rt, r1[1]);
                EMIT_INST4(context, ByteCodeOp::IntrinsicStringCmp, r0[0], r0[1], r1[0], rt);
                EMIT_INST2(context, ByteCodeOp::NegBool, r2, rt);
                freeRegisterRC(context, rt);
                return true;
            }

            case NativeTypeKind::Any:
                if (right->hasSemFlag(SEMFLAG_TYPE_NULL))
                {
                    EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual64, r0[0], r1[0], r2);
                    return true;
                }

                if (context->node->hasSpecFlag(AstBinaryOpNode::SPEC_FLAG_IMPLICIT_KINDOF))
                {
                    SWAG_CHECK(emitKindOfAny(context, left));
                    const auto rFlags = reserveRegisterRC(context);
                    const auto rt     = reserveRegisterRC(context);
                    const auto inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate32, rFlags);
                    inst->b.u64       = SWAG_TYPECMP_STRICT;
                    EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, r0, r1, rFlags, rt);
                    EMIT_INST2(context, ByteCodeOp::NegBool, r2, rt);
                    freeRegisterRC(context, rFlags);
                    freeRegisterRC(context, rt);
                    return true;
                }

                break;

            default:
                break;
        }
    }

    if (leftTypeInfo->isPointer())
    {
        // Special case for typeinfo, as this is not safe to just compare pointers.
        // The same typeinfo can be different if defined in two different modules, so we need
        // to make a compare by name too
        if (leftTypeInfo->isPointerToTypeInfo() || rightTypeInfo->isPointerToTypeInfo())
        {
            if (!leftTypeInfo->isPointerNull() && !rightTypeInfo->isPointerNull())
            {
                const auto rFlags = reserveRegisterRC(context);
                const auto rt     = reserveRegisterRC(context);
                const auto inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate32, rFlags);
                inst->b.u64       = SWAG_TYPECMP_STRICT;
                EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, r0, r1, rFlags, rt);
                EMIT_INST2(context, ByteCodeOp::NegBool, r2, rt);
                freeRegisterRC(context, rFlags);
                freeRegisterRC(context, rt);
                return true;
            }
        }

        // Simple pointer compare
        EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual64, r0, r1, r2);
        return true;
    }

    if (leftTypeInfo->isClosure())
    {
        EMIT_INST2(context, ByteCodeOp::DeRef64, r0, r0);
        EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual64, r0, r1, r2);
        return true;
    }

    if (leftTypeInfo->isLambda())
    {
        EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual64, r0, r1, r2);
        return true;
    }

    if (leftTypeInfo->isInterface())
    {
        if (rightTypeInfo->isPointerNull() || right->hasSemFlag(SEMFLAG_FROM_NULL))
        {
            EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual64, r0[1], r1[1], r2);
            return true;
        }

        if (rightTypeInfo->isInterface())
        {
            EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual64, r0[0], r1[0], r2);
            EMIT_INST1(context, ByteCodeOp::JumpIfTrue, r2)->b.u64 = 1;
            EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual64, r0[1], r1[1], r2);
            return true;
        }

        if (context->node->hasSpecFlag(AstBinaryOpNode::SPEC_FLAG_IMPLICIT_KINDOF))
        {
            SWAG_ASSERT(rightTypeInfo->isPointerToTypeInfo());
            SWAG_CHECK(emitKindOfInterface(context, left));
            const auto rFlags = reserveRegisterRC(context);
            const auto rt     = reserveRegisterRC(context);
            const auto inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate32, rFlags);
            inst->b.u64       = SWAG_TYPECMP_STRICT;
            EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, r0, r1, rFlags, rt);
            EMIT_INST2(context, ByteCodeOp::NegBool, r2, rt);
            freeRegisterRC(context, rFlags);
            freeRegisterRC(context, rt);
            return true;
        }
    }

    if (leftTypeInfo->isSlice())
    {
        // Just compare pointers. This is enough for now, as we can only compare a slice to 'null'
        EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual64, r0[1], r1[1], r2);
        return true;
    }

    return Report::internalError(context->node, "emitCompareOpNotEqual, invalid type");
}

bool ByteCodeGen::emitCompareOpEqual(ByteCodeGenContext* context, const RegisterList& r0, const RegisterList& r1, const RegisterList& r2)
{
    const auto node  = context->node;
    const auto left  = node->firstChild();
    const auto right = node->lastChild();
    SWAG_CHECK(emitCompareOpEqual(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGen::emitCompareOpNotEqual(ByteCodeGenContext* context, const RegisterList& r0, const RegisterList& r1, const RegisterList& r2)
{
    const auto node  = context->node;
    const auto left  = node->firstChild();
    const auto right = node->lastChild();
    SWAG_CHECK(emitCompareOpNotEqual(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGen::emitCompareOp3Way(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const AstNode* node     = context->node;
    const auto     typeInfo = TypeManager::concreteType(node->firstChild()->typeInfo);
    if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
            case NativeTypeKind::S8:
            case NativeTypeKind::U8:
                EMIT_INST3(context, ByteCodeOp::CompareOp3Way8, r0, r1, r2);
                return true;
            case NativeTypeKind::S16:
            case NativeTypeKind::U16:
                EMIT_INST3(context, ByteCodeOp::CompareOp3Way16, r0, r1, r2);
                return true;
            case NativeTypeKind::S32:
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                EMIT_INST3(context, ByteCodeOp::CompareOp3Way32, r0, r1, r2);
                return true;
            case NativeTypeKind::S64:
            case NativeTypeKind::U64:
                EMIT_INST3(context, ByteCodeOp::CompareOp3Way64, r0, r1, r2);
                return true;
            case NativeTypeKind::F32:
                EMIT_INST3(context, ByteCodeOp::CompareOp3WayF32, r0, r1, r2);
                return true;
            case NativeTypeKind::F64:
                EMIT_INST3(context, ByteCodeOp::CompareOp3WayF64, r0, r1, r2);
                return true;
            default:
                return Report::internalError(context->node, "emitCompareOp3Way, type not supported");
        }
    }

    if (typeInfo->isPointer())
    {
        EMIT_INST3(context, ByteCodeOp::CompareOp3Way64, r0, r1, r2);
        return true;
    }

    return Report::internalError(context->node, "emitCompareOp3Way, type not native");
}

bool ByteCodeGen::emitCompareOpLower(const ByteCodeGenContext* context, const AstNode* left, AstNode* /*right*/, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto typeInfo = TypeManager::concretePtrRefType(left->typeInfo);
    if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
            case NativeTypeKind::S8:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerS8, r0, r1, r2);
                return true;
            case NativeTypeKind::S16:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerS16, r0, r1, r2);
                return true;
            case NativeTypeKind::S32:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerS32, r0, r1, r2);
                return true;
            case NativeTypeKind::S64:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerS64, r0, r1, r2);
                return true;
            case NativeTypeKind::U8:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerU8, r0, r1, r2);
                return true;
            case NativeTypeKind::U16:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerU16, r0, r1, r2);
                return true;
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerU32, r0, r1, r2);
                return true;
            case NativeTypeKind::U64:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerU64, r0, r1, r2);
                return true;
            case NativeTypeKind::F32:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerF32, r0, r1, r2);
                return true;
            case NativeTypeKind::F64:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerF64, r0, r1, r2);
                return true;
            default:
                return Report::internalError(context->node, "emitCompareOpLower, type not supported");
        }
    }

    if (typeInfo->isPointer())
    {
        EMIT_INST3(context, ByteCodeOp::CompareOpLowerU64, r0, r1, r2);
        return true;
    }

    return Report::internalError(context->node, "emitCompareOpLower, type not native");
}

bool ByteCodeGen::emitCompareOpLower(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto node  = context->node;
    const auto left  = node->firstChild();
    const auto right = node->lastChild();
    SWAG_CHECK(emitCompareOpLower(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGen::emitCompareOpLowerEq(const ByteCodeGenContext* context, const AstNode* left, AstNode* /*right*/, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto typeInfo = TypeManager::concretePtrRefType(left->typeInfo);
    if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
            case NativeTypeKind::S8:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS8, r0, r1, r2);
                return true;
            case NativeTypeKind::S16:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS16, r0, r1, r2);
                return true;
            case NativeTypeKind::S32:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS32, r0, r1, r2);
                return true;
            case NativeTypeKind::S64:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqS64, r0, r1, r2);
                return true;
            case NativeTypeKind::U8:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU8, r0, r1, r2);
                return true;
            case NativeTypeKind::U16:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU16, r0, r1, r2);
                return true;
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU32, r0, r1, r2);
                return true;
            case NativeTypeKind::U64:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, r0, r1, r2);
                return true;
            case NativeTypeKind::F32:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqF32, r0, r1, r2);
                return true;
            case NativeTypeKind::F64:
                EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqF64, r0, r1, r2);
                return true;
            default:
                return Report::internalError(context->node, "emitCompareOpLowerEq, type not supported");
        }
    }

    if (typeInfo->isPointer())
    {
        EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, r0, r1, r2);
        return true;
    }

    return Report::internalError(context->node, "emitCompareOpLowerEq, type not native");
}

bool ByteCodeGen::emitCompareOpLowerEq(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto node  = context->node;
    const auto left  = node->firstChild();
    const auto right = node->lastChild();
    SWAG_CHECK(emitCompareOpLowerEq(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGen::emitCompareOpGreater(const ByteCodeGenContext* context, const AstNode* left, AstNode* /*right*/, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto typeInfo = TypeManager::concretePtrRefType(left->typeInfo);
    if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
            case NativeTypeKind::S8:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterS8, r0, r1, r2);
                return true;
            case NativeTypeKind::S16:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterS16, r0, r1, r2);
                return true;
            case NativeTypeKind::S32:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterS32, r0, r1, r2);
                return true;
            case NativeTypeKind::S64:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterS64, r0, r1, r2);
                return true;
            case NativeTypeKind::U8:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterU8, r0, r1, r2);
                return true;
            case NativeTypeKind::U16:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterU16, r0, r1, r2);
                return true;
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterU32, r0, r1, r2);
                return true;
            case NativeTypeKind::U64:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterU64, r0, r1, r2);
                return true;
            case NativeTypeKind::F32:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF32, r0, r1, r2);
                return true;
            case NativeTypeKind::F64:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterF64, r0, r1, r2);
                return true;
            default:
                return Report::internalError(context->node, "emitCompareOpGreater, type not supported");
        }
    }

    if (typeInfo->isPointer())
    {
        EMIT_INST3(context, ByteCodeOp::CompareOpGreaterU64, r0, r1, r2);
        return true;
    }

    return Report::internalError(context->node, "emitCompareOpGreater, type not native");
}

bool ByteCodeGen::emitCompareOpGreater(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto node  = context->node;
    const auto left  = node->firstChild();
    const auto right = node->lastChild();
    SWAG_CHECK(emitCompareOpGreater(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGen::emitCompareOpGreaterEq(const ByteCodeGenContext* context, const AstNode* left, AstNode* /*right*/, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto typeInfo = TypeManager::concretePtrRefType(left->typeInfo);
    if (typeInfo->isNative())
    {
        switch (typeInfo->nativeType)
        {
            case NativeTypeKind::S8:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS8, r0, r1, r2);
                return true;
            case NativeTypeKind::S16:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS16, r0, r1, r2);
                return true;
            case NativeTypeKind::S32:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS32, r0, r1, r2);
                return true;
            case NativeTypeKind::S64:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqS64, r0, r1, r2);
                return true;
            case NativeTypeKind::U8:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqU8, r0, r1, r2);
                return true;
            case NativeTypeKind::U16:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqU16, r0, r1, r2);
                return true;
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqU32, r0, r1, r2);
                return true;
            case NativeTypeKind::U64:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqU64, r0, r1, r2);
                return true;
            case NativeTypeKind::F32:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqF32, r0, r1, r2);
                return true;
            case NativeTypeKind::F64:
                EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqF64, r0, r1, r2);
                return true;
            default:
                return Report::internalError(context->node, "emitCompareOpGreaterEq, type not supported");
        }
    }

    if (typeInfo->isPointer())
    {
        EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqU64, r0, r1, r2);
        return true;
    }

    return Report::internalError(context->node, "emitCompareOpEqGreater, type not native");
}

bool ByteCodeGen::emitCompareOpGreaterEq(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto node  = context->node;
    const auto left  = node->firstChild();
    const auto right = node->lastChild();
    SWAG_CHECK(emitCompareOpGreaterEq(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGen::emitCompareOp(ByteCodeGenContext* context)
{
    AstNode* node = context->node;

    if (!node->hasSemFlag(SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->firstChild(), TypeManager::concreteType(node->firstChild()->typeInfo), node->firstChild()->typeInfoCast));
        YIELD();
        node->addSemFlag(SEMFLAG_CAST1);
    }

    if (!node->hasSemFlag(SEMFLAG_CAST2))
    {
        SWAG_CHECK(emitCast(context, node->secondChild(), TypeManager::concreteType(node->secondChild()->typeInfo), node->secondChild()->typeInfoCast));
        YIELD();
        node->addSemFlag(SEMFLAG_CAST2);
    }

    // User special function
    if (node->hasSpecialFuncCall())
    {
        SWAG_CHECK(emitUserOp(context));
        YIELD();
        SWAG_CHECK(emitCompareOpPostSpecialFunc(context, node->token.id));
    }
    else
    {
        auto& r0 = node->firstChild()->resultRegisterRc;
        auto& r1 = node->secondChild()->resultRegisterRc;

        const RegisterList r2  = reserveRegisterRC(context);
        node->resultRegisterRc = r2;

        switch (node->token.id)
        {
            case TokenId::SymEqualEqual:
                SWAG_CHECK(emitCompareOpEqual(context, r0, r1, r2));
                break;
            case TokenId::SymExclamEqual:
                SWAG_CHECK(emitCompareOpNotEqual(context, r0, r1, r2));
                break;
            case TokenId::SymLower:
                SWAG_CHECK(emitCompareOpLower(context, r0, r1, r2));
                break;
            case TokenId::SymLowerEqual:
                SWAG_CHECK(emitCompareOpLowerEq(context, r0, r1, r2));
                break;
            case TokenId::SymGreater:
                SWAG_CHECK(emitCompareOpGreater(context, r0, r1, r2));
                break;
            case TokenId::SymGreaterEqual:
                SWAG_CHECK(emitCompareOpGreaterEq(context, r0, r1, r2));
                break;
            case TokenId::SymLowerEqualGreater:
                SWAG_CHECK(emitCompareOp3Way(context, r0, r1, r2));
                break;
            default:
                return Report::internalError(context->node, "emitCompareOpGreater, invalid token op");
        }

        freeRegisterRC(context, r0);
        freeRegisterRC(context, r1);
    }

    return true;
}
