#include "pch.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "SemanticJob.h"
#include "ThreadManager.h"
#include "Ast.h"

bool ByteCodeGenJob::emitBinaryOpPlus(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteReferenceType(typeInfoExpr);

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::BinOpPlusS32, r0, r1, r2);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            emitInstruction(context, ByteCodeOp::BinOpPlusU32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            emitInstruction(context, ByteCodeOp::BinOpPlusS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::BinOpPlusU64, r0, r1, r2);
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::BinOpPlusF32, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::BinOpPlusF64, r0, r1, r2);
            return true;
        default:
            return internalError(context, "emitBinaryOpPlus, type not supported");
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(typeInfo), TypeInfoKind::Pointer);
        auto sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
        {
            auto rt = reserveRegisterRC(context);

            // Be sure that the pointer is on the left side, because ptr = 1 + ptr is possible
            if (context->node->childs[0]->typeInfo->kind == TypeInfoKind::Pointer)
            {
                emitInstruction(context, ByteCodeOp::CopyRBtoRA64, rt, r1);
                emitInstruction(context, ByteCodeOp::Mul64byVB64, rt)->b.u64 = sizeOf;
                emitInstruction(context, ByteCodeOp::IncPointer64, r0, rt, r2);
            }
            else
            {
                emitInstruction(context, ByteCodeOp::CopyRBtoRA64, rt, r0);
                emitInstruction(context, ByteCodeOp::Mul64byVB64, rt)->b.u64 = sizeOf;
                emitInstruction(context, ByteCodeOp::IncPointer64, r1, rt, r2);
            }

            freeRegisterRC(context, rt);
        }
        else
        {
            emitInstruction(context, ByteCodeOp::IncPointer64, r0, r1, r2);
        }

        return true;
    }

    return internalError(context, "emitBinaryOpPlus, invalid native");
}

bool ByteCodeGenJob::emitBinaryOpMinus(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteReferenceType(typeInfoExpr);

    // This is the substract of two pointers if we have a s64 on the left, and a pointer on the right
    if (typeInfo->isNative(NativeTypeKind::Int))
    {
        auto rightTypeInfo = TypeManager::concreteType(node->childs[1]->typeInfo);
        if (rightTypeInfo->kind == TypeInfoKind::Pointer)
        {
            auto rightTypePointer = CastTypeInfo<TypeInfoPointer>(rightTypeInfo, TypeInfoKind::Pointer);
            emitInstruction(context, ByteCodeOp::BinOpMinusS64, r0, r1, r2);
            auto sizeOf = rightTypePointer->pointedType->sizeOf;
            if (sizeOf > 1)
                emitInstruction(context, ByteCodeOp::Div64byVB64, r2)->b.u64 = sizeOf;
            return true;
        }
    }

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::BinOpMinusS32, r0, r1, r2);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            emitInstruction(context, ByteCodeOp::BinOpMinusU32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            emitInstruction(context, ByteCodeOp::BinOpMinusS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::BinOpMinusU64, r0, r1, r2);
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::BinOpMinusF32, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::BinOpMinusF64, r0, r1, r2);
            return true;
        default:
            return internalError(context, "emitBinaryOpMinus, type not supported");
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(typeInfo), TypeInfoKind::Pointer);
        auto sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
        {
            auto rt = reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::CopyRBtoRA64, rt, r1);
            emitInstruction(context, ByteCodeOp::Mul64byVB64, rt)->b.u64 = sizeOf;
            emitInstruction(context, ByteCodeOp::DecPointer64, r0, rt, r2);
            freeRegisterRC(context, rt);
        }
        else
        {
            emitInstruction(context, ByteCodeOp::DecPointer64, r0, r1, r2);
        }

        return true;
    }

    return internalError(context, "emitBinaryOpMinus, invalid native");
}

bool ByteCodeGenJob::emitBinaryOpMul(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteReferenceType(typeInfoExpr);

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBinaryOpMul, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::BinOpMulS32, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        emitInstruction(context, ByteCodeOp::BinOpMulU32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        emitInstruction(context, ByteCodeOp::BinOpMulS64, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::BinOpMulU64, r0, r1, r2);
        return true;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::BinOpMulF32, r0, r1, r2);
        return true;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::BinOpMulF64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitBinaryOpMul, type not supported");
    }
}

bool ByteCodeGenJob::emitBinaryOpDiv(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteReferenceType(typeInfoExpr);

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBinaryOpDiv, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S32:
        emitSafetyDivZero(context, r1, 32);
        emitInstruction(context, ByteCodeOp::BinOpDivS32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        emitSafetyDivZero(context, r1, 64);
        emitInstruction(context, ByteCodeOp::BinOpDivS64, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        emitSafetyDivZero(context, r1, 32);
        emitInstruction(context, ByteCodeOp::BinOpDivU32, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitSafetyDivZero(context, r1, 64);
        emitInstruction(context, ByteCodeOp::BinOpDivU64, r0, r1, r2);
        return true;
    case NativeTypeKind::F32:
        emitSafetyDivZero(context, r1, 32);
        emitInstruction(context, ByteCodeOp::BinOpDivF32, r0, r1, r2);
        return true;
    case NativeTypeKind::F64:
        emitSafetyDivZero(context, r1, 64);
        emitInstruction(context, ByteCodeOp::BinOpDivF64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitBinaryOpDiv, type not supported");
    }
}

bool ByteCodeGenJob::emitBinaryOpModulo(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteReferenceType(typeInfoExpr);

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBinaryOpModulo, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S32:
        emitSafetyDivZero(context, r1, 32);
        emitInstruction(context, ByteCodeOp::BinOpModuloS32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        emitSafetyDivZero(context, r1, 64);
        emitInstruction(context, ByteCodeOp::BinOpModuloS64, r0, r1, r2);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        emitSafetyDivZero(context, r1, 32);
        emitInstruction(context, ByteCodeOp::BinOpModuloU32, r0, r1, r2);
        return true;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitSafetyDivZero(context, r1, 64);
        emitInstruction(context, ByteCodeOp::BinOpModuloU64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitBinaryOpModulo, type not supported");
    }
}

bool ByteCodeGenJob::emitBitmaskAnd(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteReferenceType(node->childs[0]->typeInfo);

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBitmaskAnd, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::BinOpBitmaskAnd8, r0, r1, r2);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::BinOpBitmaskAnd16, r0, r1, r2);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        emitInstruction(context, ByteCodeOp::BinOpBitmaskAnd32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::BinOpBitmaskAnd64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitBitmaskAnd, type not supported");
    }
}

bool ByteCodeGenJob::emitBitmaskOr(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteReferenceType(typeInfoExpr);

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBitmaskOr, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::BinOpBitmaskOr8, r0, r1, r2);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::BinOpBitmaskOr16, r0, r1, r2);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        emitInstruction(context, ByteCodeOp::BinOpBitmaskOr32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::BinOpBitmaskOr64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitBitmaskOr, type not supported");
    }
}

bool ByteCodeGenJob::emitShiftLeft(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteReferenceType(typeInfoExpr);

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitShiftLeft, type not native");

    emitSafetyLeftShift(context, r0, r1, typeInfo);

    auto     opNode     = CastAst<AstOp>(context->node, AstNodeKind::FactorOp);
    uint16_t shiftFlags = 0;
    if (opNode->specFlags & AST_SPEC_OP_SMALL)
        shiftFlags |= BCI_SHIFT_SMALL;

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::BinOpShiftLeftU8, r0, r1, r2)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::BinOpShiftLeftU16, r0, r1, r2)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::BinOpShiftLeftU32, r0, r1, r2)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        emitInstruction(context, ByteCodeOp::BinOpShiftLeftU64, r0, r1, r2)->flags |= shiftFlags;
        return true;

    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::BinOpShiftLeftU8, r0, r1, r2)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::BinOpShiftLeftU16, r0, r1, r2)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        emitInstruction(context, ByteCodeOp::BinOpShiftLeftU32, r0, r1, r2)->flags |= shiftFlags;
        return true;

    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::BinOpShiftLeftU64, r0, r1, r2)->flags |= shiftFlags;
        return true;
    default:
        return internalError(context, "emitShiftLeft, type not supported");
    }
}

bool ByteCodeGenJob::emitShiftRight(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteReferenceType(typeInfoExpr);

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitShiftRight, type not native");

    emitSafetyRightShift(context, r0, r1, typeInfo);

    auto     opNode     = CastAst<AstOp>(context->node, AstNodeKind::FactorOp);
    uint16_t shiftFlags = 0;
    if (opNode->specFlags & AST_SPEC_OP_SMALL)
        shiftFlags |= BCI_SHIFT_SMALL;

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::BinOpShiftRightS8, r0, r1, r2)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::BinOpShiftRightS16, r0, r1, r2)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::BinOpShiftRightS32, r0, r1, r2)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        emitInstruction(context, ByteCodeOp::BinOpShiftRightS64, r0, r1, r2)->flags |= shiftFlags;
        return true;

    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::BinOpShiftRightU8, r0, r1, r2)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::BinOpShiftRightU16, r0, r1, r2)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        emitInstruction(context, ByteCodeOp::BinOpShiftRightU32, r0, r1, r2)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::BinOpShiftRightU64, r0, r1, r2)->flags |= shiftFlags;
        return true;
    default:
        return internalError(context, "emitShiftRight, type not supported");
    }
}

bool ByteCodeGenJob::emitXor(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteReferenceType(typeInfoExpr);

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitXor, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::BinOpXorBool, r0, r1, r2);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        emitInstruction(context, ByteCodeOp::BinOpXorU32, r0, r1, r2);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::BinOpXorU64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitXor, type not supported");
    }
}

bool ByteCodeGenJob::emitLogicalAndAfterLeft(ByteCodeGenContext* context)
{
    auto left    = context->node;
    auto binNode = CastAst<AstBinaryOpNode>(left->parent, AstNodeKind::BinaryOp);

    // We need to cast right now, in case the shortcut is activated
    SWAG_CHECK(emitCast(context, left, TypeManager::concreteType(left->typeInfo), left->castedTypeInfo));
    if (context->result == ContextResult::Pending)
        return true;
    binNode->doneFlags |= AST_DONE_CAST1;

    // If the left expression is false, then we copy the result to the && operator, and we jump right after it
    // (the jump offset will be updated later). That way, we do not evaluate B in 'A && B' if A is false.
    // left->additionalRegisterRC will be used as the result register for the '&&' operation in 'emitBinaryOp'
    ensureCanBeChangedRC(context, left->resultRegisterRC);
    left->additionalRegisterRC     = left->resultRegisterRC;
    left->resultRegisterRC.canFree = false;
    binNode->seekJumpExpression    = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::JumpIfFalse, left->resultRegisterRC);
    return true;
}

bool ByteCodeGenJob::emitLogicalAnd(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto node = CastAst<AstBinaryOpNode>(context->node, AstNodeKind::BinaryOp);

    // Because of the shortcuts, there's no need to actually do a || here, as we are sure that the
    // expression on the left is true
    // Se we just need to propagate the result
    emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r2, r1);

    auto inst   = &context->bc->out[node->seekJumpExpression];
    inst->b.s32 = context->bc->numInstructions - node->seekJumpExpression - 1;
    return true;
}

bool ByteCodeGenJob::emitLogicalOrAfterLeft(ByteCodeGenContext* context)
{
    auto left    = context->node;
    auto binNode = CastAst<AstBinaryOpNode>(left->parent, AstNodeKind::BinaryOp);

    // We need to cast right now, in case the shortcut is activated
    SWAG_CHECK(emitCast(context, left, TypeManager::concreteType(left->typeInfo), left->castedTypeInfo));
    if (context->result == ContextResult::Pending)
        return true;
    binNode->doneFlags |= AST_DONE_CAST1;

    // If the left expression is true, then we copy the result to the || operator, and we jump right after it
    // (the jump offset will be updated later). That way, we do not evaluate B in 'A || B' if B is true.
    // left->additionalRegisterRC will be used as the result register for the '||' operation in 'emitBinaryOp'
    ensureCanBeChangedRC(context, left->resultRegisterRC);
    left->additionalRegisterRC     = left->resultRegisterRC;
    left->resultRegisterRC.canFree = false;
    binNode->seekJumpExpression    = context->bc->numInstructions;
    emitInstruction(context, ByteCodeOp::JumpIfTrue, left->resultRegisterRC);
    return true;
}

bool ByteCodeGenJob::emitLogicalOr(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto node = CastAst<AstBinaryOpNode>(context->node, AstNodeKind::BinaryOp);

    // Because of the shortcuts, there's no need to actually do a && here, as we are sure that the
    // expression on the left is true
    // Se we just need to propagate the result
    emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r2, r1);

    auto inst   = &context->bc->out[node->seekJumpExpression];
    inst->b.s32 = context->bc->numInstructions - node->seekJumpExpression - 1;
    return true;
}

bool ByteCodeGenJob::emitBinaryOp(ByteCodeGenContext* context)
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

    if (!(node->doneFlags & AST_DONE_EMIT_OP))
    {
        auto r0 = node->childs[0]->resultRegisterRC;
        auto r1 = node->childs[1]->resultRegisterRC;

        // User special function
        if (node->hasSpecialFuncCall())
        {
            SWAG_CHECK(emitUserOp(context));
            if (context->result != ContextResult::Done)
                return true;
            node->doneFlags |= AST_DONE_EMIT_OP;
        }
        else
        {
            uint32_t r2 = 0;

            // Register for the binary operation has already been allocated in 'additionalRegisterRC' by the left expression in case of a logical test
            // So we take it as the result register.
            if (node->token.id == TokenId::KwdAnd || node->token.id == TokenId::KwdOr)
            {
                auto front = node->childs[0];
                SWAG_ASSERT(front->additionalRegisterRC.canFree);
                r2 = front->additionalRegisterRC;
                front->additionalRegisterRC.clear();
            }
            else
                r2 = reserveRegisterRC(context);

            node->resultRegisterRC = r2;

            auto typeInfoExpr = node->castedTypeInfo ? node->castedTypeInfo : node->typeInfo;

            switch (node->token.id)
            {
            case TokenId::SymPlus:
                SWAG_CHECK(emitBinaryOpPlus(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymMinus:
                SWAG_CHECK(emitBinaryOpMinus(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymAsterisk:
                SWAG_CHECK(emitBinaryOpMul(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymSlash:
                SWAG_CHECK(emitBinaryOpDiv(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymPercent:
                SWAG_CHECK(emitBinaryOpModulo(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymVertical:
                SWAG_CHECK(emitBitmaskOr(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymAmpersand:
                SWAG_CHECK(emitBitmaskAnd(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymLowerLower:
                SWAG_CHECK(emitShiftLeft(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymGreaterGreater:
                SWAG_CHECK(emitShiftRight(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::SymCircumflex:
                SWAG_CHECK(emitXor(context, typeInfoExpr, r0, r1, r2));
                break;
            case TokenId::KwdAnd:
                emitLogicalAnd(context, r0, r1, r2);
                break;
            case TokenId::KwdOr:
                emitLogicalOr(context, r0, r1, r2);
                break;
            default:
                return internalError(context, "emitBinaryOp, invalid token op");
            }

            freeRegisterRC(context, r0);
            freeRegisterRC(context, r1);
            node->doneFlags |= AST_DONE_EMIT_OP;
        }
    }

    if (!(node->doneFlags & AST_DONE_CAST3))
    {
        SWAG_CHECK(emitCast(context, node, TypeManager::concreteType(node->typeInfo), node->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->doneFlags |= AST_DONE_CAST3;
    }

    return true;
}

bool ByteCodeGenJob::makeInline(ByteCodeGenContext* context, AstFuncDecl* funcDecl, AstNode* identifier)
{
    SWAG_CHECK(SemanticJob::makeInline((JobContext*) context, funcDecl, identifier));

    // Create a semantic job to resolve the inline part, and wait for that to be finished
    context->job->setPending(nullptr, "MAKE_INLINE", funcDecl, nullptr);
    auto inlineNode = identifier->childs.back();
    SWAG_ASSERT(inlineNode->kind == AstNodeKind::Inline);
    auto job = SemanticJob::newJob(context->job->dependentJob, context->sourceFile, inlineNode, false);
    job->addDependentJob(context->job);
    context->job->jobsToAdd.push_back(job);
    return true;
}

bool ByteCodeGenJob::emitUserOp(ByteCodeGenContext* context, AstNode* allParams, AstNode* forNode, bool freeRegisterParams)
{
    AstNode* node = forNode ? forNode : context->node;
    SWAG_ASSERT(node->extension);
    auto symbolOverload = node->extension->resolvedUserOpSymbolOverload;
    SWAG_ASSERT(symbolOverload);
    auto funcDecl = CastAst<AstFuncDecl>(symbolOverload->node, AstNodeKind::FuncDecl);

    // Note: Do not inline a call when evaluation compile time affectation (AST_SEM_EXEC_RET_STACK)
    if (symbolOverload->node->mustInline() && !(node->semFlags & AST_SEM_EXEC_RET_STACK))
    {
        // Expand inline function. Do not expand an inline call inside a function marked as inline.
        // The expansion will be done at the lowest level possible
        // Remember: inline functions are also compiled as non inlined (mostly for compile time execution
        // of calls), and we do not want the call to be inlined twice (one in the normal owner function, and one
        // again after the owner function has been duplicated).
        if (!node->ownerFct || !node->ownerFct->mustInline())
        {
            // Need to wait for function full semantic resolve
            {
                scoped_lock lk(funcDecl->mutex);
                if (!(funcDecl->semFlags & AST_SEM_FULL_RESOLVE))
                {
                    funcDecl->dependentJobs.add(context->job);
                    context->job->setPending(funcDecl->resolvedSymbolName, "EMIT_USER_OP", funcDecl, nullptr);
                    return true;
                }
            }

            if (!(node->doneFlags & AST_DONE_INLINED))
            {
                node->doneFlags |= AST_DONE_INLINED;
                SWAG_CHECK(makeInline(context, funcDecl, node));
                return true;
            }

            if (!(node->doneFlags & AST_DONE_RESOLVE_INLINED))
            {
                node->doneFlags |= AST_DONE_RESOLVE_INLINED;
                auto back = node->childs.back();
                SWAG_ASSERT(back->kind == AstNodeKind::Inline);
                context->job->nodes.push_back(back);
                context->result = ContextResult::NewChilds;
            }

            return true;
        }
    }

    bool foreign = symbolOverload->node->attributeFlags & ATTRIBUTE_FOREIGN;

    // We are less restrictive on type parameters for useop, as type are more in control.
    // Se we could have a needed cast now.
    if (allParams)
    {
        for (auto c : allParams->childs)
        {
            SWAG_CHECK(emitCast(context, c, c->typeInfo, c->castedTypeInfo));
            SWAG_ASSERT(context->result == ContextResult::Done);
        }
    }

    return emitCall(context, allParams ? allParams : node, funcDecl, nullptr, funcDecl->resultRegisterRC, foreign, false, freeRegisterParams);
}
