#include "pch.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "ByteCode.h"
#include "Ast.h"

bool ByteCodeGenJob::emitInRange(ByteCodeGenContext* context, AstNode* left, AstNode* right, RegisterList& r0, RegisterList& r2)
{
    auto rangeNode  = CastAst<AstRange>(right, AstNodeKind::Range);
    auto low        = rangeNode->expressionLow;
    auto up         = rangeNode->expressionUp;
    bool excludeLow = rangeNode->specFlags & AST_SPEC_RANGE_EXCLUDE_LOW;
    bool excludeUp  = rangeNode->specFlags & AST_SPEC_RANGE_EXCLUDE_UP;

    auto typeInfo = TypeManager::concreteReferenceType(low->typeInfo);
    if (!typeInfo->isNativeIntegerOrRune() && !typeInfo->isNativeFloat())
        return context->internalError("emitInRange, type not supported");

    // Invert test if lower bound is greater than upper bound
    bool orderIsDefined = false;
    if (low->flags & AST_VALUE_COMPUTED && up->flags & AST_VALUE_COMPUTED)
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
        else if (typeInfo->isNative(NativeTypeKind::Rune) && low->computedValue->reg.u32 > up->computedValue->reg.u32)
            swap = true;

        if (swap)
        {
            std::swap(low, up);
            std::swap(excludeLow, excludeUp);
        }
    }

    if (!orderIsDefined)
    {
        return context->internalError("emitInRange, order undefined");
    }

    RegisterList ra, rb;

    // Lower bound
    if (left->hasSpecialFuncCall())
    {
        SWAG_CHECK(emitCompareOpSpecialFunc(context, left, low, r0, low->resultRegisterRC, excludeLow ? TokenId::SymGreater : TokenId::SymGreaterEqual));
        if (context->result != ContextResult::Done)
            return true;
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
        if (context->result != ContextResult::Done)
            return true;
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

    emitInstruction(context, ByteCodeOp::CompareOpEqual8, ra, rb, r2);

    freeRegisterRC(context, ra);
    freeRegisterRC(context, rb);
    freeRegisterRC(context, rangeNode->expressionLow);
    freeRegisterRC(context, rangeNode->expressionUp);
    return true;
}

bool ByteCodeGenJob::emitCompareOpSpecialFunc(ByteCodeGenContext* context, AstNode* left, AstNode* right, RegisterList& r0, RegisterList& r1, TokenId op)
{
    SWAG_ASSERT(left->hasSpecialFuncCall());

    auto node = context->node;
    auto job  = context->job;
    if (!job->allParamsTmp)
        job->allParamsTmp = Ast::newFuncCallParams(node->sourceFile, nullptr);
    job->allParamsTmp->childs.clear();
    job->allParamsTmp->childs.push_back(left);
    job->allParamsTmp->childs.push_back(right);
    left->resultRegisterRC  = r0;
    right->resultRegisterRC = r1;
    SWAG_CHECK(emitUserOp(context, job->allParamsTmp, left, false));
    if (context->result != ContextResult::Done)
        return true;
    SWAG_CHECK(emitCompareOpPostSpecialFunc(context, op));

    return true;
}

bool ByteCodeGenJob::emitCompareOpPostSpecialFunc(ByteCodeGenContext* context, TokenId op)
{
    auto node = context->node;
    auto r2   = node->resultRegisterRC;
    if (node->semFlags & AST_SEM_INVERSE_PARAMS)
    {
        switch (op)
        {
        case TokenId::SymLowerEqualGreater:
            emitInstruction(context, ByteCodeOp::NegS32, r2);
            break;
        case TokenId::SymGreater:
            emitInstruction(context, ByteCodeOp::LowerZeroToTrue, r2);
            break;
        case TokenId::SymGreaterEqual:
            emitInstruction(context, ByteCodeOp::LowerEqZeroToTrue, r2);
            break;
        case TokenId::SymLower:
            emitInstruction(context, ByteCodeOp::GreaterZeroToTrue, r2);
            break;
        case TokenId::SymLowerEqual:
            emitInstruction(context, ByteCodeOp::GreaterEqZeroToTrue, r2);
            break;
        case TokenId::SymExclamEqual:
        {
            auto rt = reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::NegBool, rt, r2);
            freeRegisterRC(context, r2);
            node->resultRegisterRC = rt;
            break;
        }
        }
    }
    else
    {
        switch (op)
        {
        case TokenId::SymLower:
            emitInstruction(context, ByteCodeOp::LowerZeroToTrue, r2);
            break;
        case TokenId::SymGreater:
            emitInstruction(context, ByteCodeOp::GreaterZeroToTrue, r2);
            break;
        case TokenId::SymLowerEqual:
            emitInstruction(context, ByteCodeOp::LowerEqZeroToTrue, r2);
            break;
        case TokenId::SymGreaterEqual:
            emitInstruction(context, ByteCodeOp::GreaterEqZeroToTrue, r2);
            break;
        case TokenId::SymExclamEqual:
        {
            auto rt = reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::NegBool, rt, r2);
            freeRegisterRC(context, r2);
            node->resultRegisterRC = rt;
            break;
        }
        }
    }

    return true;
}

bool ByteCodeGenJob::emitCompareOpEqual(ByteCodeGenContext* context, AstNode* left, AstNode* right, RegisterList& r0, RegisterList& r1, RegisterList& r2)
{
    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Native)
    {
        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::CompareOpEqual8, r0, r1, r2);
            return true;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::CompareOpEqual16, r0, r1, r2);
            return true;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::F32:
        case NativeTypeKind::Rune:
            emitInstruction(context, ByteCodeOp::CompareOpEqual32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
            return true;
        case NativeTypeKind::String:
            if (right->semFlags & AST_SEM_TYPE_IS_NULL)
            {
                emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0[0], r1[0], r2);
            }
            else
            {
                emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r2, r1[1]);
                emitInstruction(context, ByteCodeOp::IntrinsicStrCmp, r0[0], r0[1], r1[0], r2);
            }
            return true;
        default:
            return context->internalError("emitCompareOpEqual, type not supported");
        }
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Pointer)
    {
        // Special case for typeinfos, as this is not safe to just compare pointers.
        // The same typeinfo can be different if defined in two different modules, so we need
        // to make a compare by name too
        if (leftTypeInfo->isPointerToTypeInfo() || rightTypeInfo->isPointerToTypeInfo())
        {
            auto rflags = reserveRegisterRC(context);
            auto inst   = emitInstruction(context, ByteCodeOp::SetImmediate32, rflags);
            inst->b.u64 = SWAG_COMPARE_STRICT;
            inst        = emitInstruction(context, ByteCodeOp::IntrinsicTypeCmp, r0, r1, rflags, r2);
            freeRegisterRC(context, rflags);
        }

        // Simple pointer compare
        else
            emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Lambda)
    {
        emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Interface)
    {
        // Just compare pointers. This is enough for now, as we can only compare an interface to 'null'
        emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0[1], r1[1], r2);
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Slice)
    {
        // Just compare pointers. This is enough for now, as we can only compare a slice to 'null'
        emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0[1], r1[1], r2);
    }
    else
    {
        return context->internalError("emitCompareOpEqual, invalid type");
    }

    return true;
}

bool ByteCodeGenJob::emitCompareOpNotEqual(ByteCodeGenContext* context, AstNode* left, AstNode* right, RegisterList& r0, RegisterList& r1, RegisterList& r2)
{
    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Native)
    {
        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::CompareOpNotEqual8, r0, r1, r2);
            return true;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::CompareOpNotEqual16, r0, r1, r2);
            return true;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::F32:
        case NativeTypeKind::Rune:
            emitInstruction(context, ByteCodeOp::CompareOpNotEqual32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::CompareOpNotEqual64, r0, r1, r2);
            return true;
        case NativeTypeKind::String:
            if (right->semFlags & AST_SEM_TYPE_IS_NULL)
            {
                emitInstruction(context, ByteCodeOp::CompareOpNotEqual64, r0[0], r1[0], r2);
            }
            else
            {
                auto rt = reserveRegisterRC(context);
                emitInstruction(context, ByteCodeOp::CopyRBtoRA64, rt, r1[1]);
                emitInstruction(context, ByteCodeOp::IntrinsicStrCmp, r0[0], r0[1], r1[0], rt);
                emitInstruction(context, ByteCodeOp::NegBool, r2, rt);
                freeRegisterRC(context, rt);
            }
            return true;
        default:
            return context->internalError("emitCompareOpNotEqual, type not supported");
        }
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Pointer)
    {
        // Special case for typeinfos, as this is not safe to just compare pointers.
        // The same typeinfo can be different if defined in two different modules, so we need
        // to make a compare by name too
        if (leftTypeInfo->isPointerToTypeInfo() || rightTypeInfo->isPointerToTypeInfo())
        {
            auto rflags = reserveRegisterRC(context);
            auto rt     = reserveRegisterRC(context);
            auto inst   = emitInstruction(context, ByteCodeOp::SetImmediate32, rflags);
            inst->b.u64 = SWAG_COMPARE_STRICT;
            inst        = emitInstruction(context, ByteCodeOp::IntrinsicTypeCmp, r0, r1, rflags, rt);
            emitInstruction(context, ByteCodeOp::NegBool, r2, rt);
            freeRegisterRC(context, rflags);
            freeRegisterRC(context, rt);
        }

        // Simple pointer compare
        else
            emitInstruction(context, ByteCodeOp::CompareOpNotEqual64, r0, r1, r2);
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Lambda)
    {
        emitInstruction(context, ByteCodeOp::CompareOpNotEqual64, r0, r1, r2);
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Interface)
    {
        // Just compare pointers. This is enough for now, as we can only compare an interface to 'null'
        emitInstruction(context, ByteCodeOp::CompareOpNotEqual64, r0[1], r1[1], r2);
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Slice)
    {
        // Just compare pointers. This is enough for now, as we can only compare a slice to 'null'
        emitInstruction(context, ByteCodeOp::CompareOpNotEqual64, r0[1], r1[1], r2);
    }
    else
    {
        return context->internalError("emitCompareOpNotEqual, invalid type");
    }

    return true;
}

bool ByteCodeGenJob::emitCompareOpEqual(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, RegisterList& r2)
{
    auto node  = context->node;
    auto left  = node->childs.front();
    auto right = node->childs.back();
    SWAG_CHECK(emitCompareOpEqual(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGenJob::emitCompareOpNotEqual(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, RegisterList& r2)
{
    auto node  = context->node;
    auto left  = node->childs.front();
    auto right = node->childs.back();
    SWAG_CHECK(emitCompareOpNotEqual(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGenJob::emitCompareOp3Way(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::CompareOp3WayS32, r0, r1, r2);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            emitInstruction(context, ByteCodeOp::CompareOp3WayU32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            emitInstruction(context, ByteCodeOp::CompareOp3WayS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::CompareOp3WayU64, r0, r1, r2);
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::CompareOp3WayF32, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::CompareOp3WayF64, r0, r1, r2);
            return true;
        default:
            return context->internalError("emitCompareOp3Way, type not supported");
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        emitInstruction(context, ByteCodeOp::CompareOp3WayU64, r0, r1, r2);
    }
    else
    {
        return context->internalError("emitCompareOp3Way, type not native");
    }

    return true;
}

bool ByteCodeGenJob::emitCompareOpLower(ByteCodeGenContext* context, AstNode* left, AstNode* right, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteReferenceType(left->typeInfo);
    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::CompareOpLowerS32, r0, r1, r2);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            emitInstruction(context, ByteCodeOp::CompareOpLowerS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU64, r0, r1, r2);
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::CompareOpLowerF32, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::CompareOpLowerF64, r0, r1, r2);
            return true;
        default:
            return context->internalError("emitCompareOpLower, type not supported");
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        emitInstruction(context, ByteCodeOp::CompareOpLowerU64, r0, r1, r2);
    }
    else
    {
        return context->internalError("emitCompareOpLower, type not native");
    }

    return true;
}

bool ByteCodeGenJob::emitCompareOpLower(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto node  = context->node;
    auto left  = node->childs.front();
    auto right = node->childs.back();
    SWAG_CHECK(emitCompareOpLower(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGenJob::emitCompareOpLowerEq(ByteCodeGenContext* context, AstNode* left, AstNode* right, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteReferenceType(left->typeInfo);
    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::CompareOpLowerEqS32, r0, r1, r2);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            emitInstruction(context, ByteCodeOp::CompareOpLowerEqU32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            emitInstruction(context, ByteCodeOp::CompareOpLowerEqS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, r0, r1, r2);
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::CompareOpLowerEqF32, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::CompareOpLowerEqF64, r0, r1, r2);
            return true;
        default:
            return context->internalError("emitCompareOpLowerEq, type not supported");
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        emitInstruction(context, ByteCodeOp::CompareOpLowerEqU64, r0, r1, r2);
    }
    else
    {
        return context->internalError("emitCompareOpLowerEq, type not native");
    }

    return true;
}

bool ByteCodeGenJob::emitCompareOpLowerEq(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto node  = context->node;
    auto left  = node->childs.front();
    auto right = node->childs.back();
    SWAG_CHECK(emitCompareOpLowerEq(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGenJob::emitCompareOpGreater(ByteCodeGenContext* context, AstNode* left, AstNode* right, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteReferenceType(left->typeInfo);
    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterS32, r0, r1, r2);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterU64, r0, r1, r2);
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterF32, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterF64, r0, r1, r2);
            return true;
        default:
            return context->internalError("emitCompareOpGreater, type not supported");
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        emitInstruction(context, ByteCodeOp::CompareOpGreaterU64, r0, r1, r2);
    }
    else
    {
        return context->internalError("emitCompareOpGreater, type not native");
    }

    return true;
}

bool ByteCodeGenJob::emitCompareOpGreater(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto node  = context->node;
    auto left  = node->childs.front();
    auto right = node->childs.back();
    SWAG_CHECK(emitCompareOpGreater(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGenJob::emitCompareOpGreaterEq(ByteCodeGenContext* context, AstNode* left, AstNode* right, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteReferenceType(left->typeInfo);
    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS32, r0, r1, r2);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterEqU32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterEqS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterEqU64, r0, r1, r2);
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterEqF32, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterEqF64, r0, r1, r2);
            return true;
        default:
            return context->internalError("emitCompareOpGreaterEq, type not supported");
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        emitInstruction(context, ByteCodeOp::CompareOpGreaterEqU64, r0, r1, r2);
    }
    else
    {
        return context->internalError("emitCompareOpEqGreater, type not native");
    }

    return true;
}

bool ByteCodeGenJob::emitCompareOpGreaterEq(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto node  = context->node;
    auto left  = node->childs.front();
    auto right = node->childs.back();
    SWAG_CHECK(emitCompareOpGreaterEq(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGenJob::emitCompareOp(ByteCodeGenContext* context)
{
    AstNode* node = context->node;

    if (!(node->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->childs[0], TypeManager::concreteType(node->childs[0]->typeInfo), node->childs[0]->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->doneFlags |= AST_DONE_CAST1;
    }

    if (!(node->doneFlags & AST_DONE_CAST2))
    {
        SWAG_CHECK(emitCast(context, node->childs[1], TypeManager::concreteType(node->childs[1]->typeInfo), node->childs[1]->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->doneFlags |= AST_DONE_CAST2;
    }

    // User special function
    if (node->hasSpecialFuncCall())
    {
        SWAG_CHECK(emitUserOp(context));
        if (context->result != ContextResult::Done)
            return true;
        SWAG_CHECK(emitCompareOpPostSpecialFunc(context, node->token.id));
    }
    else
    {
        auto& r0 = node->childs[0]->resultRegisterRC;
        auto& r1 = node->childs[1]->resultRegisterRC;

        RegisterList r2        = reserveRegisterRC(context);
        node->resultRegisterRC = r2;

        switch (node->token.id)
        {
        case TokenId::SymEqualEqual:
            SWAG_CHECK(emitCompareOpEqual(context, r0, r1, r2));
            break;
        case TokenId::SymExclamEqual:
            SWAG_CHECK(emitCompareOpNotEqual(context, r0, r1, r2));
            break;
        case TokenId::SymLowerEqualGreater:
            SWAG_CHECK(emitCompareOp3Way(context, r0, r1, r2));
            break;
        case TokenId::SymLower:
            SWAG_CHECK(emitCompareOpLower(context, r0, r1, r2));
            break;
        case TokenId::SymGreater:
            SWAG_CHECK(emitCompareOpGreater(context, r0, r1, r2));
            break;
        case TokenId::SymLowerEqual:
            SWAG_CHECK(emitCompareOpLowerEq(context, r0, r1, r2));
            break;
        case TokenId::SymGreaterEqual:
            SWAG_CHECK(emitCompareOpGreaterEq(context, r0, r1, r2));
            break;
        default:
            return context->internalError("emitCompareOpGreater, invalid token op");
        }

        freeRegisterRC(context, r0);
        freeRegisterRC(context, r1);
    }

    return true;
}
