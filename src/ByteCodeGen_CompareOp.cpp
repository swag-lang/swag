#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCode.h"
#include "ByteCodeGen.h"
#include "Report.h"
#include "TypeManager.h"

bool ByteCodeGen::emitInRange(ByteCodeGenContext* context, AstNode* left, AstNode* right, RegisterList& r0, RegisterList& r2)
{
    const auto rangeNode  = CastAst<AstRange>(right, AstNodeKind::Range);
    auto       low        = rangeNode->expressionLow;
    auto       up         = rangeNode->expressionUp;
    bool       excludeLow = false;
    bool       excludeUp  = rangeNode->specFlags & AstRange::SPECFLAG_EXCLUDE_UP;

    const auto typeInfo = TypeManager::concretePtrRefType(low->typeInfo);
    if (!typeInfo->isNativeIntegerOrRune() && !typeInfo->isNativeFloat())
        return Report::internalError(context->node, "emitInRange, type not supported");

    // Invert test if lower bound is greater than upper bound
    bool orderIsDefined = false;
    if (low->hasComputedValue() && up->hasComputedValue())
    {
        orderIsDefined = true;

        bool swap = false;
        if (typeInfo->isNativeIntegerSigned() && low->computedValue->reg.s64 > up->computedValue->reg.s64)
            swap = true;
        else if (typeInfo->isNativeInteger() && low->computedValue->reg.u64 > up->computedValue->reg.u64)
            swap = true;
        else if (typeInfo->isNative(NativeTypeKind::F32) && low->computedValue->reg.f32 > up->computedValue->reg.f32)
            swap = true;
        else if (typeInfo->isNative(NativeTypeKind::F64) && low->computedValue->reg.f64 > up->computedValue->reg.f64)
            swap = true;
        else if (typeInfo->isRune() && low->computedValue->reg.u32 > up->computedValue->reg.u32)
            swap = true;

        if (swap)
        {
            std::swap(low, up);
            std::swap(excludeLow, excludeUp);
        }
    }

    if (!orderIsDefined)
    {
        return Report::internalError(context->node, "emitInRange, order undefined");
    }

    RegisterList ra, rb;

    // Lower bound
    if (left->hasSpecialFuncCall())
    {
        SWAG_CHECK(emitCompareOpSpecialFunc(context, left, low, r0, low->resultRegisterRC, excludeLow ? TokenId::SymGreater : TokenId::SymGreaterEqual));
        YIELD();
        ra = context->node->resultRegisterRC;
    }
    else if (excludeLow)
    {
        ra = reserveRegisterRC(context);
        SWAG_CHECK(emitCompareOpGreater(context, left, low, r0, low->resultRegisterRC, ra));
    }
    else
    {
        ra = reserveRegisterRC(context);
        SWAG_CHECK(emitCompareOpGreaterEq(context, left, low, r0, low->resultRegisterRC, ra));
    }

    // Upper bound
    if (left->hasSpecialFuncCall())
    {
        SWAG_CHECK(emitCompareOpSpecialFunc(context, left, up, r0, up->resultRegisterRC, excludeUp ? TokenId::SymLower : TokenId::SymLowerEqual));
        YIELD();
        rb = context->node->resultRegisterRC;
    }
    else if (excludeUp)
    {
        rb = reserveRegisterRC(context);
        SWAG_CHECK(emitCompareOpLower(context, left, up, r0, up->resultRegisterRC, rb));
    }
    else
    {
        rb = reserveRegisterRC(context);
        SWAG_CHECK(emitCompareOpLowerEq(context, left, up, r0, up->resultRegisterRC, rb));
    }

    EMIT_INST3(context, ByteCodeOp::CompareOpEqual8, ra, rb, r2);

    freeRegisterRC(context, ra);
    freeRegisterRC(context, rb);
    freeRegisterRC(context, rangeNode->expressionLow);
    freeRegisterRC(context, rangeNode->expressionUp);
    return true;
}

bool ByteCodeGen::emitCompareOpSpecialFunc(ByteCodeGenContext* context, AstNode* left, AstNode* right, RegisterList& r0, RegisterList& r1, TokenId op)
{
    SWAG_ASSERT(left->hasSpecialFuncCall());

    context->allocateTempCallParams();
    context->allParamsTmp->childs.push_back(left);
    context->allParamsTmp->childs.push_back(right);
    left->resultRegisterRC  = r0;
    right->resultRegisterRC = r1;
    SWAG_CHECK(emitUserOp(context, context->allParamsTmp, left, false));
    YIELD();
    SWAG_CHECK(emitCompareOpPostSpecialFunc(context, op));

    return true;
}

bool ByteCodeGen::emitCompareOpPostSpecialFunc(ByteCodeGenContext* context, TokenId op)
{
    const auto node = context->node;
    auto       r2   = node->resultRegisterRC;
    if (node->semFlags & SEMFLAG_INVERSE_PARAMS)
    {
        switch (op)
        {
        case TokenId::SymLowerEqualGreater:
        {
            const auto rt = reserveRegisterRC(context);
            EMIT_INST2(context, ByteCodeOp::NegS32, rt, r2);
            freeRegisterRC(context, r2);
            node->resultRegisterRC = rt;
            break;
        }
        case TokenId::SymExclamEqual:
        {
            const auto rt = reserveRegisterRC(context);
            EMIT_INST2(context, ByteCodeOp::NegBool, rt, r2);
            freeRegisterRC(context, r2);
            node->resultRegisterRC = rt;
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
            node->resultRegisterRC = rt;
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

bool ByteCodeGen::emitCompareOpEqual(ByteCodeGenContext* context, AstNode* left, AstNode* right, RegisterList& r0, RegisterList& r1, RegisterList& r2)
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
        case NativeTypeKind::F32:
            EMIT_INST3(context, ByteCodeOp::CompareOpEqualF32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            EMIT_INST3(context, ByteCodeOp::CompareOpEqualF64, r0, r1, r2);
            return true;

        case NativeTypeKind::String:
            if (right->semFlags & SEMFLAG_TYPE_IS_NULL)
            {
                EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, r0[0], r1[0], r2);
                return true;
            }

            EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r2, r1[1]);
            EMIT_INST4(context, ByteCodeOp::IntrinsicStringCmp, r0[0], r0[1], r1[0], r2);
            return true;

        case NativeTypeKind::Any:
            if (right->semFlags & SEMFLAG_TYPE_IS_NULL)
            {
                EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, r0[0], r1[0], r2);
                return true;
            }

            if (context->node->specFlags & AstBinaryOpNode::SPECFLAG_IMPLICIT_KINDOF)
            {
                SWAG_CHECK(emitKindOf(context, left, leftTypeInfo->kind));
                const auto rflags = reserveRegisterRC(context);
                auto       inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate32, rflags);
                inst->b.u64       = SWAG_COMPARE_STRICT;
                inst              = EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, r0, r1, rflags, r2);
                freeRegisterRC(context, rflags);
                return true;
            }

            break;

        default:
            break;
        }
    }

    if (leftTypeInfo->isPointer())
    {
        // Special case for typeinfos, as this is not safe to just compare pointers.
        // The same typeinfo can be different if defined in two different modules, so we need
        // to make a compare by name too
        if (leftTypeInfo->isPointerToTypeInfo() || rightTypeInfo->isPointerToTypeInfo())
        {
            const auto rflags = reserveRegisterRC(context);
            auto       inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate32, rflags);
            inst->b.u64       = SWAG_COMPARE_STRICT;
            inst              = EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, r0, r1, rflags, r2);
            freeRegisterRC(context, rflags);
            return true;
        }

        // CString compare
        if (leftTypeInfo->flags & TYPEINFO_C_STRING)
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
        if (rightTypeInfo->isPointerNull() || right->semFlags & SEMFLAG_FROM_NULL)
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

        if (context->node->specFlags & AstBinaryOpNode::SPECFLAG_IMPLICIT_KINDOF)
        {
            SWAG_CHECK(emitKindOf(context, left, TypeInfoKind::Interface));
            const auto rflags = reserveRegisterRC(context);
            auto       inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate32, rflags);
            inst->b.u64       = SWAG_COMPARE_STRICT;
            inst              = EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, r0, r1, rflags, r2);
            freeRegisterRC(context, rflags);
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

bool ByteCodeGen::emitCompareOpNotEqual(ByteCodeGenContext* context, AstNode* left, AstNode* right, RegisterList& r0, RegisterList& r1, RegisterList& r2)
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
            if (right->semFlags & SEMFLAG_TYPE_IS_NULL)
            {
                EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual64, r0[0], r1[0], r2);
                return true;
            }
            else
            {
                const auto rt = reserveRegisterRC(context);
                EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, rt, r1[1]);
                EMIT_INST4(context, ByteCodeOp::IntrinsicStringCmp, r0[0], r0[1], r1[0], rt);
                EMIT_INST2(context, ByteCodeOp::NegBool, r2, rt);
                freeRegisterRC(context, rt);
                return true;
            }

        case NativeTypeKind::Any:
            if (right->semFlags & SEMFLAG_TYPE_IS_NULL)
            {
                EMIT_INST3(context, ByteCodeOp::CompareOpNotEqual64, r0[0], r1[0], r2);
                return true;
            }

            if (context->node->specFlags & AstBinaryOpNode::SPECFLAG_IMPLICIT_KINDOF)
            {
                SWAG_CHECK(emitKindOf(context, left, leftTypeInfo->kind));
                const auto rflags = reserveRegisterRC(context);
                const auto rt     = reserveRegisterRC(context);
                auto       inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate32, rflags);
                inst->b.u64       = SWAG_COMPARE_STRICT;
                inst              = EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, r0, r1, rflags, rt);
                EMIT_INST2(context, ByteCodeOp::NegBool, r2, rt);
                freeRegisterRC(context, rflags);
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
        // Special case for typeinfos, as this is not safe to just compare pointers.
        // The same typeinfo can be different if defined in two different modules, so we need
        // to make a compare by name too
        if (leftTypeInfo->isPointerToTypeInfo() || rightTypeInfo->isPointerToTypeInfo())
        {
            const auto rflags = reserveRegisterRC(context);
            const auto rt     = reserveRegisterRC(context);
            auto       inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate32, rflags);
            inst->b.u64       = SWAG_COMPARE_STRICT;
            inst              = EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, r0, r1, rflags, rt);
            EMIT_INST2(context, ByteCodeOp::NegBool, r2, rt);
            freeRegisterRC(context, rflags);
            freeRegisterRC(context, rt);
            return true;
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
        if (rightTypeInfo->isPointerNull() || right->semFlags & SEMFLAG_FROM_NULL)
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

        if (context->node->specFlags & AstBinaryOpNode::SPECFLAG_IMPLICIT_KINDOF)
        {
            SWAG_ASSERT(rightTypeInfo->isPointerToTypeInfo());
            SWAG_CHECK(emitKindOf(context, left, TypeInfoKind::Interface));
            const auto rflags = reserveRegisterRC(context);
            const auto rt     = reserveRegisterRC(context);
            auto       inst   = EMIT_INST1(context, ByteCodeOp::SetImmediate32, rflags);
            inst->b.u64       = SWAG_COMPARE_STRICT;
            inst              = EMIT_INST4(context, ByteCodeOp::IntrinsicTypeCmp, r0, r1, rflags, rt);
            EMIT_INST2(context, ByteCodeOp::NegBool, r2, rt);
            freeRegisterRC(context, rflags);
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

bool ByteCodeGen::emitCompareOpEqual(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, RegisterList& r2)
{
    const auto node  = context->node;
    const auto left  = node->childs.front();
    const auto right = node->childs.back();
    SWAG_CHECK(emitCompareOpEqual(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGen::emitCompareOpNotEqual(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, RegisterList& r2)
{
    const auto node  = context->node;
    const auto left  = node->childs.front();
    const auto right = node->childs.back();
    SWAG_CHECK(emitCompareOpNotEqual(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGen::emitCompareOp3Way(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode*   node     = context->node;
    const auto typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
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
    else if (typeInfo->isPointer())
    {
        EMIT_INST3(context, ByteCodeOp::CompareOp3Way64, r0, r1, r2);
    }
    else
    {
        return Report::internalError(context->node, "emitCompareOp3Way, type not native");
    }

    return true;
}

bool ByteCodeGen::emitCompareOpLower(ByteCodeGenContext* context, AstNode* left, AstNode* right, uint32_t r0, uint32_t r1, uint32_t r2)
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
    else if (typeInfo->isPointer())
    {
        EMIT_INST3(context, ByteCodeOp::CompareOpLowerU64, r0, r1, r2);
    }
    else
    {
        return Report::internalError(context->node, "emitCompareOpLower, type not native");
    }

    return true;
}

bool ByteCodeGen::emitCompareOpLower(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto node  = context->node;
    const auto left  = node->childs.front();
    const auto right = node->childs.back();
    SWAG_CHECK(emitCompareOpLower(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGen::emitCompareOpLowerEq(ByteCodeGenContext* context, AstNode* left, AstNode* right, uint32_t r0, uint32_t r1, uint32_t r2)
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
    else if (typeInfo->isPointer())
    {
        EMIT_INST3(context, ByteCodeOp::CompareOpLowerEqU64, r0, r1, r2);
    }
    else
    {
        return Report::internalError(context->node, "emitCompareOpLowerEq, type not native");
    }

    return true;
}

bool ByteCodeGen::emitCompareOpLowerEq(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto node  = context->node;
    const auto left  = node->childs.front();
    const auto right = node->childs.back();
    SWAG_CHECK(emitCompareOpLowerEq(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGen::emitCompareOpGreater(ByteCodeGenContext* context, AstNode* left, AstNode* right, uint32_t r0, uint32_t r1, uint32_t r2)
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
    else if (typeInfo->isPointer())
    {
        EMIT_INST3(context, ByteCodeOp::CompareOpGreaterU64, r0, r1, r2);
    }
    else
    {
        return Report::internalError(context->node, "emitCompareOpGreater, type not native");
    }

    return true;
}

bool ByteCodeGen::emitCompareOpGreater(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto node  = context->node;
    const auto left  = node->childs.front();
    const auto right = node->childs.back();
    SWAG_CHECK(emitCompareOpGreater(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGen::emitCompareOpGreaterEq(ByteCodeGenContext* context, AstNode* left, AstNode* right, uint32_t r0, uint32_t r1, uint32_t r2)
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
    else if (typeInfo->isPointer())
    {
        EMIT_INST3(context, ByteCodeOp::CompareOpGreaterEqU64, r0, r1, r2);
    }
    else
    {
        return Report::internalError(context->node, "emitCompareOpEqGreater, type not native");
    }

    return true;
}

bool ByteCodeGen::emitCompareOpGreaterEq(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    const auto node  = context->node;
    const auto left  = node->childs.front();
    const auto right = node->childs.back();
    SWAG_CHECK(emitCompareOpGreaterEq(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGen::emitCompareOp(ByteCodeGenContext* context)
{
    AstNode* node = context->node;

    if (!(node->semFlags & SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->childs[0], TypeManager::concreteType(node->childs[0]->typeInfo), node->childs[0]->castedTypeInfo));
        YIELD();
        node->semFlags |= SEMFLAG_CAST1;
    }

    if (!(node->semFlags & SEMFLAG_CAST2))
    {
        SWAG_CHECK(emitCast(context, node->childs[1], TypeManager::concreteType(node->childs[1]->typeInfo), node->childs[1]->castedTypeInfo));
        YIELD();
        node->semFlags |= SEMFLAG_CAST2;
    }

    // User special function
    if (node->hasSpecialFuncCall())
    {
        SWAG_CHECK(emitUserOp(context));
        YIELD();
        SWAG_CHECK(emitCompareOpPostSpecialFunc(context, node->tokenId));
    }
    else
    {
        auto& r0 = node->childs[0]->resultRegisterRC;
        auto& r1 = node->childs[1]->resultRegisterRC;

        RegisterList r2        = reserveRegisterRC(context);
        node->resultRegisterRC = r2;

        switch (node->tokenId)
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
